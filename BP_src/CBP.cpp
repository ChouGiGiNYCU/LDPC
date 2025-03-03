#include<iostream>
#include<fstream>
#include<cmath>
#include<ctime>
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include "random_number_generator.h"
#include "UseFuction.h"


#define frame_error_lowwer_bound 400
double phi(double x){
    double yy = tanh(x / 2);
    if (fabs(yy) < 1e-14) 
        yy = 1e-14;
    return -log(yy) + 1e-14;
}
// #define phi(x)  log((exp(x)+1)/(exp(x)-1+1e-14)) //CN update 精簡後的公式，1e-14是避免x=0，導致分母為零
using namespace std;

struct parity_check{
    int n,m,max_col_degree,max_row_degree;
    int *max_col_arr,*max_row_arr;
    int **VN_2_CN_pos; // record 1 pos in each row = record col pos at ith row
    int **CN_2_VN_pos; // record 1 pos in each col = record row pos at ith col
};

void FreeAllH(struct parity_check*H);
void Read_File_H(struct parity_check *H,string& file_name_in);
vector<vector<bool>> Read_File_G(string& file_name);


int main(int argc,char* argv[]){
    if(argc < 2){
        cout << "** Error ---- No file in" << endl; 
    }
    string file_name = argv[1];
    string file_name_out = argv[2];
    string G_file_path  = argv[3];
    bool Encode_Flag = atoi(argv[4])==1?true:false;
    int iteration_limit = atoi(argv[5]);
    double SNR_min = atof(argv[6]);
    double SNR_max = atof(argv[7]);
    double SNR_ratio = atof(argv[8]);
    cout << "##############################" << endl;
    cout << "file_name : " << file_name << endl;
    cout << "file_name_out : " << file_name_out << endl;
    cout << "G_file_path : " << G_file_path << endl;
    cout << "Encode_Flag : " << Encode_Flag << endl;
    cout << "iteration_limit : " << iteration_limit << endl;
    cout << "SNR_min : " << SNR_min << endl;
    cout << "SNR_max : " << SNR_max << endl;
    cout << "SNR_ratio : " << SNR_ratio << endl;
    cout << "##############################" << endl;
    // define H
    struct parity_check H;
    Read_File_H(&H,file_name);
    // define G
    cout << "H file read success!!" << endl;
    
    vector<vector<bool>> G;
    if(Encode_Flag){
        G = Read_File_G(G_file_path);
        cout << "G file read success!!" << endl;
    }
    
    
    ofstream outFile(file_name_out);
    if (!outFile.is_open()) {
        cout << "Output file: " << file_name_out  << " cannot be opened." << endl;
		exit(1);
    }
    outFile << "Eb_over_N0" << ", " << "frame_error_rate" << ", ";
    for(int i=0;i<iteration_limit;i++){
        outFile << "BER_it_"+to_string(i) << ", ";
    }
    outFile << endl;
    

    double SNR = SNR_min;
    double *transmit_codeword = (double*)calloc(H.n,sizeof(double));
    
    double *receiver_LLR = (double*)malloc(H.n*sizeof(double));
    double code_rate = (double)(H.n-H.m)/(double)H.n;
    double ** CN_2_VN_LLR = (double**)malloc(sizeof(double*)*H.n);
    for(int i=0;i<H.n;i++) CN_2_VN_LLR[i]=(double*)calloc(H.max_col_arr[i],sizeof(double));
    double ** VN_2_CN_LLR = (double**)malloc(sizeof(double*)*H.m);
    for(int i=0;i<H.m;i++) VN_2_CN_LLR[i]=(double*)calloc(H.max_row_arr[i],sizeof(double));
    double *guess = (double*)malloc(sizeof(double)*H.n);
    double *bit_error_count = (double*)malloc(iteration_limit*sizeof(double));
    vector<bool> information_bit((G.size()));
    vector<int> result;
    clock_t  clk_start,clk_end;
    while(SNR <= SNR_max){
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        // cout <<"SNR : " << SNR << " | sigma : " << sigma << endl;
        // SNR+=1;
        // continue;
        double frame_count=0;
        double frame_error=0;
        int it;
        for(int i=0;i<iteration_limit;i++){
            bit_error_count[i]=0;
        }
        clk_start = clock();
        
        while(frame_error < frame_error_lowwer_bound){
            if(Encode_Flag){
                for(int i=0;i<H.n-H.m;i++){
                    information_bit[i] = random_generation()>0.5?1:0;
                }
                result = Vector_Dot_Matrix_Int(information_bit,G);
            }
            for(int i=0;i<H.n;i++){
                if(Encode_Flag) transmit_codeword[i] = result[i]%2==0?0:1; // Encode CodeWord
                else transmit_codeword[i] = 0; // all zero
            }
            

            for(int i=0;i<H.n;i++){
                // Sum_Product_Algorithm 是 soft decision 所以在調變的時候解
                // BPSK -> {0->1}、{1->-1}
                // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
                double receiver_codeword=(-2*transmit_codeword[i]+1) + sigma*gasdev(); // fixed power
                receiver_LLR[i]=2*receiver_codeword/pow(sigma,2); 
            }
            it=0;
            bool error_syndrome = true;
            bool bit_error_flag=false;
            while(it<iteration_limit && error_syndrome){
                
                /* ------- CN update ------- */
                for(int VN=0;VN<H.n;VN++){
                    for(int i=0;i<H.max_col_arr[VN];i++){
                        int CN = H.CN_2_VN_pos[VN][i];
                        double alpha=1,phi_beta_sum=0;
                        double total_LLR = 1;
                        for(int j=0;j<H.max_row_arr[CN];j++){
                            int other_VN = H.VN_2_CN_pos[CN][j];
                            if(other_VN == VN) continue; 
                            if(it==0){ //第一次迭帶就是 直接加上剛算好的receiver_LLR
                                phi_beta_sum += phi(abs(receiver_LLR[other_VN]));
                                alpha *= receiver_LLR[other_VN]>0?1:-1;
                                //  total_LLR *= tanh(0.5*(receiver_LLR[other_VN])); 
                            }else{
                                phi_beta_sum += phi(abs(VN_2_CN_LLR[CN][j]));
                                alpha *= VN_2_CN_LLR[CN][j]>0?1:-1;
                                // total_LLR *=tanh(0.5*VN_2_CN_LLR[CN][j]) ;
                            }
                        }
                        // total_LLR = 2*atanh(total_LLR);
                        // CN_2_VN_LLR[VN][i] = total_LLR;
                        CN_2_VN_LLR[VN][i]= alpha * phi(phi_beta_sum);
                    }
                }

                /* ------- VN update ------- */
                for(int CN=0;CN<H.m;CN++){
                    for(int i=0;i<H.max_row_arr[CN];i++){
                        int VN=H.VN_2_CN_pos[CN][i];
                        double total_LLR = receiver_LLR[VN];
                        for(int j=0;j<H.max_col_arr[VN];j++){
                            int other_CN = H.CN_2_VN_pos[VN][j];
                            if(other_CN == CN) continue;
                            total_LLR += CN_2_VN_LLR[VN][j];
                        }
                        VN_2_CN_LLR[CN][i] = total_LLR;
                    }
                }
                
                /* ------- total LLR ------- */
                for(int VN=0;VN<H.n;VN++){
                    guess[VN]=receiver_LLR[VN];
                    for(int i=0;i<H.max_col_arr[VN];i++){
                        guess[VN]+=CN_2_VN_LLR[VN][i];
                    }
                    /* ------- make decision ------- */
                    guess[VN]= (guess[VN]<0)?1:0;
                }
                
                /* Early Stop - Determine if it is a syndrome -> guess * H^T = vector(0) */
                error_syndrome = false;
                for(int CN=0;CN<H.m;CN++){
                    int count=0;
                    for(int i=0;i<H.max_row_arr[CN];i++){
                        int VN = H.VN_2_CN_pos[CN][i];
                        count+=guess[VN];
                    }
                    if(count%2==1){
                        error_syndrome = true;
                        break;
                    }
                    error_syndrome = false;
                }
                
                /* ----- Determine bit error ----- */
                
                bit_error_flag=false;
                for(int VN=0;VN<H.n;VN++){
                    if(guess[VN]!=transmit_codeword[VN]){
                        bit_error_count[it]++;
                        bit_error_flag=true;
                    }
                }
                it++;
            }
            // 如果syndrome是錯的 ber 也要跟著增加
            if(!error_syndrome && bit_error_flag){
                for(int it_idx=it;it_idx<iteration_limit;it_idx++){
                    bit_error_count[it_idx] += H.n;
                }
            }
            if(bit_error_flag){
                frame_error+=1;
            }
            frame_count++;
        }
        clk_end=clock();
        double clk_duration = (double)(clk_end-clk_start)/CLOCKS_PER_SEC;
        printf("SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Time : %.3f | Total frame : %.0f\n",SNR,frame_error/frame_count,bit_error_count[it-1]/((double)H.n*frame_count),it,clk_duration,frame_count);
        outFile << SNR << ", " << frame_error/frame_count << ", ";
        for(int i=0;i<iteration_limit;i++){
            outFile << bit_error_count[i]/((double)H.n*frame_count) << ", ";
        }
        outFile <<endl;
        
        if(SNR==SNR_max) break;
        SNR = min(SNR+SNR_ratio,SNR_max);
    }
    outFile.close();
    /* ----------- free all memory ----------- */
    cout << "In main Free" << endl;
    FreeAllH(&H);
    free(transmit_codeword);
    free(receiver_LLR);
    for(int i=0;i<H.n;i++) free(CN_2_VN_LLR[i]);
    free(CN_2_VN_LLR);
    for(int i=0;i<H.m;i++) free(VN_2_CN_LLR[i]);
    free(VN_2_CN_LLR);
    free(guess);
    free(bit_error_count);
    return 0;
}

void FreeAllH(struct parity_check *H){
    cout << "Free All element with H." << endl;
    if(H->max_col_arr != NULL) free(H->max_col_arr);
    if(H->max_row_arr != NULL) free(H->max_row_arr);
    if(H->CN_2_VN_pos != NULL) {
        for(int i = 0; i < H->n; i++) {
            if(H->CN_2_VN_pos[i] != NULL) free(H->CN_2_VN_pos[i]);
        }
        free(H->CN_2_VN_pos);
    }
    if(H->VN_2_CN_pos != NULL) {
        for(int i = 0; i < H->m; i++) {
            if(H->VN_2_CN_pos[i] != NULL) free(H->VN_2_CN_pos[i]);
        }
        free(H->VN_2_CN_pos);
    }
}

void Read_File_H(struct parity_check *H,string& file_name_in){
    ifstream file_in(file_name_in);
    if(file_in.fail()){
        cout << "** Error ----- " << file_name_in << " can't open !!" <<endl;
        exit(1);
    }
    
    // struct parity_check *H=(struct parity_check*)malloc(sizeof(parity_check));
    file_in >> H->n >> H->m >> H->max_col_degree >> H->max_row_degree;
    
    cout << "Parity_Check_Matrix size  : " << H->m << " x " << H->n << endl;
    cout << "max_col_degree : " << H->max_col_degree << " | max_row_degree : " << H->max_row_degree << endl;
    H->max_col_arr=(int*)malloc(sizeof(int)*H->n);
    H->max_row_arr=(int*)malloc(sizeof(int)*H->m);
    cout << "Each col 1's number : ";
    for(int i=0;i<H->n;i++){
        file_in >> H->max_col_arr[i];
        if(H->max_col_arr[i] > H->max_col_degree){
            cout << "** Error ----- " << file_name_in << " max_col_array is Error , Please Check!!" << endl;
            goto FreeAll;
        }
        cout << H->max_col_arr[i] << " ";
    }
    cout << endl << "Each row 1's number : ";
    for(int i=0;i<H->m;i++){
        file_in >> H->max_row_arr[i];
        if(H->max_row_arr[i] > H->max_row_degree){
            cout << "** Error ----- " << file_name_in << " max_row_array is Error , Please Check!!" << endl;
            goto FreeAll;
        }
        cout << H->max_row_arr[i] << " ";
    }
    cout << endl;
    H->CN_2_VN_pos=(int**)malloc(sizeof(int*)*H->n);
    H->VN_2_CN_pos=(int**)malloc(sizeof(int*)*H->m);
    for(int i=0;i<H->n;i++){
        H->CN_2_VN_pos[i]=(int*)malloc(H->max_col_degree*sizeof(int));
    }
    for(int i=0;i<H->m;i++){
        H->VN_2_CN_pos[i]=(int*)malloc(H->max_row_degree*sizeof(int));
    }
    for(int i=0;i<H->n;i++){
        int pos;
        for(int j=0;j<H->max_col_degree;j++){
            file_in >> pos;
            H->CN_2_VN_pos[i][j]=pos-1;
        }
        if(H->max_col_arr[i]!=H->max_col_degree){
            H->CN_2_VN_pos[i] = (int*)realloc(H->CN_2_VN_pos[i],sizeof(int)*H->max_col_arr[i]);
        }
    }
    for(int i=0;i<H->m;i++){
        int pos;
        for(int j=0;j<H->max_row_degree;j++){
            file_in >> pos;
            H->VN_2_CN_pos[i][j]=pos-1;
        }
        if(H->max_row_arr[i]!=H->max_row_degree){
            H->VN_2_CN_pos[i] = (int*)realloc(H->VN_2_CN_pos[i],sizeof(int)*H->max_row_arr[i]);
        }
    }
    file_in.close();
    return;

    FreeAll: 
        cout << "In ReadFile Free All element " << endl;
        FreeAllH(H);
        file_in.close();
        exit(1);
}

vector<vector<bool>> Read_File_G(string& file_name){
    ifstream file(file_name);
    string line;
    vector<string> line_split;
    int idx=0,n,k,max_col;
    vector<vector<bool>> G;
    while (getline(file, line)) {
        line = strip(line);
        // cout << line << endl;
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==2){
                n = stoi(line_split[0]);
                k = stoi(line_split[1]);
                G = vector<vector<bool>>(k,vector<bool>(n,0));
            }else{
                cerr << "First line(n,k) : " << line << " is not n,k !!" << endl;
                exit(1);
            }
        }else if(idx==1){
            if(line_split.size()==1){
                max_col = stoi(line_split[0]);
            }else{
                cerr << "Second line(maxcol) : " << line << " is not max_col !!" << endl;
                exit(1);
            }
        }else if(idx==2){
            continue;
        }
        else{
            if(line_split.size()==max_col){
                for(string& s:line_split){
                    int pos = stoi(s);
                    if(pos!=0) G[pos-1][idx-2]=true; // 1
                }
            }else{
                cerr << "Each Col 1 postion line : [" << line << "] postion length is not equal to " << max_col << " | length : " << line_split.size() << endl;
                exit(1); 
            }
        }
        idx++;
    }
    file.close();
    return G;
}