#include<iostream>
#include<fstream>
#include<cmath>
#include<ctime>
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include "UseFuction.h"
#include "random_number_generator.h"




#define frame_error_lowwer_bound 200
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
// vector<vector<bool>> Read_File_G(string& file_name);
vector<boost::dynamic_bitset<>> Read_File_G(string& file_name);

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
    
    vector<boost::dynamic_bitset<>> Origin_G;
    vector<boost::dynamic_bitset<>> Transpose_G;
    if(Encode_Flag){
        Origin_G = Read_File_G(G_file_path);
        Transpose_G = Transpose_Matrix(Origin_G);
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
    double code_rate = (double)(H.n-H.m)/(double)H.n;
    boost::dynamic_bitset<> transmit_codeword(H.n);
    boost::dynamic_bitset<> guess(H.n);
    vector<double> receiver_LLR(H.n,0); 
    vector<double> bit_error_count(iteration_limit,0);

    double **CN_2_VN_LLR = (double**)malloc(sizeof(double*)*H.m);
    for(int i=0;i<H.m;i++) CN_2_VN_LLR[i] = (double*)calloc(H.max_row_arr[i],sizeof(double));
    

    boost::dynamic_bitset<> information_bit(Origin_G.size());
    vector<int> result;

    while(SNR <= SNR_max){
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        
        double frame_count=0;
        double frame_error=0;
        int it;
        for(int i=0;i<iteration_limit;i++){
            bit_error_count[i]=0;
        }
        
        while(frame_error < frame_error_lowwer_bound){
            if(Encode_Flag){
                for(int i=0;i<H.n-H.m;i++){
                    information_bit[i] = random_generation();
                }
                result = GF2_Mat_Vec_Dot(information_bit,Transpose_G);
            }
            for(int i=0;i<H.n;i++){
                if(Encode_Flag) transmit_codeword[i] = result[i]; // Encode CodeWord
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
            int error_bit_count = 0;
            // clean the matrix
            for (int cn = 0; cn < H.m; ++cn)
                for (int e = 0; e < H.max_row_arr[cn]; ++e)
                    CN_2_VN_LLR[cn][e] = 0.0;
            while(it<iteration_limit && error_syndrome){
                error_bit_count = 0;
                /* ------- Each CN Update ------- */
                for(int CN=0;CN<H.m;CN++){
                    double alpha=1,phi_beta_sum=0,tmp_LLR = 0;
                    // count all LLR sum 
                    for(int i=0;i<H.max_row_arr[CN];i++){ // Each connect VNs by CN
                        int VN=H.VN_2_CN_pos[CN][i];
                        double tmp_LLR = receiver_LLR[VN]-CN_2_VN_LLR[CN][i];
                        phi_beta_sum += phi(abs(tmp_LLR));
                        alpha *= tmp_LLR>0?1:-1;
                    }
                    double total_VN2CN_LLR = alpha * phi(phi_beta_sum); // LLR
                    for(int i=0;i<H.max_row_arr[CN];i++){ 
                        int VN = H.VN_2_CN_pos[CN][i];
                        double tmp_LLR = receiver_LLR[VN]-CN_2_VN_LLR[CN][i];
                        double phi_beta_excl = phi_beta_sum - phi(abs(tmp_LLR));
                        double alpha_excl = alpha * ((tmp_LLR>0)?1:-1);
                        // C2V LLR 
                        CN_2_VN_LLR[CN][i] = alpha_excl * phi(phi_beta_excl);
                        receiver_LLR[VN] = tmp_LLR +  CN_2_VN_LLR[CN][i];
                    }
                }
                // cout << "******************** ok ********************\n";

                /* ------- total LLR ------- */
                for(int VN=0;VN<H.n;VN++){
                    double total_LLR = receiver_LLR[VN];
                    /* ------- make decision ------- */
                    guess[VN]= (total_LLR<0)?1:0;
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
                auto check_result = guess ^ transmit_codeword;
                error_bit_count = check_result.count();
                if(error_bit_count>0) bit_error_flag = true;
                bit_error_count[it] += static_cast<double>(error_bit_count);
               
                it++;
            }
            // 如果syndrome是錯的 ber 也要跟著增加
            if(!error_syndrome && bit_error_flag){
                for(int it_idx=it;it_idx<iteration_limit;it_idx++){
                    bit_error_count[it_idx] += error_bit_count;
                }
            }
            if(bit_error_flag){
                frame_error+=1;
            }
            frame_count++;
            
        }
        
        printf("SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Total frame : %.0f\n",SNR,frame_error/frame_count,bit_error_count[iteration_limit-1]/((double)H.n*frame_count),it,frame_count);
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
   
    for(int i=0;i<H.m;i++) free(CN_2_VN_LLR[i]);
    free(CN_2_VN_LLR);
    
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

vector<boost::dynamic_bitset<>> Read_File_G(string& file_name){
    ifstream file(file_name);
    vector<boost::dynamic_bitset<>> G;
    vector<string> line_split;
    string line;
    vector<int> max_col_map;
    int idx = 0, max_col , n ,k;
    while (getline(file, line)) {
        line = strip(line);
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==2){
                n = stoi(line_split[0]); // colsize 
                k = stoi(line_split[1]); // rowsize
                G = vector<boost::dynamic_bitset<>>(k, boost::dynamic_bitset<>(n));
            }else{
                cerr << "First line(n,k) : " << line << " is not n,k !!" << endl;
                exit(1);
            }
        }else if(idx==1){
            if(line_split.size()==1){
                max_col = stoi(line_split[0]); // max_col_degree
            }else{
                cerr << "Second line(maxcol) : " << line << " is not max_col !!" << endl;
                exit(1);
            }
        }else if(idx==2){
            if(line_split.size()==n){
                for(int i=0;i<line_split.size();i++){
                    max_col_map.push_back(stoi(line_split[i]));
                }
            }
            else{
                cerr << "Third line(each col 1's number) : " << line << " is not equal G matrix colsize  !! \n";
                exit(1);
            }
        }
        else{
            if(line_split.size()==max_col_map[idx-3]){
                for(string& s:line_split){
                    int pos = stoi(s);
                    if(pos!=0) G[pos-1][idx-3]=true; // 1
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
