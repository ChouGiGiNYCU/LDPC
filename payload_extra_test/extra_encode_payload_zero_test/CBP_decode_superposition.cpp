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
#define total_frame_limit 10000000
#define phi(x)  log((exp(x)+1)/(exp(x)-1+1e-14)) //CN update 精簡後的公式，1e-14是避免x=0，導致分母為零
using namespace std;



void FreeAllH(struct parity_check*H);
void Read_File_H(struct parity_check *H,string& file_name_in);
vector<vector<bool>> Read_File_G(string& file_name);


int main(int argc,char* argv[]){
    if(argc < 12){
        cout << "** Error ---- Check input" << endl; 
    }
    // file name
    string payload_matrix_file = argv[1];
    string payload_matrix_performance = argv[2];
    string extra_matrix_file = argv[3];
    string extra_matrix_performance = argv[4];
    string payload_generative_matrix  = argv[5];
    bool payload_Encode_Flag = atoi(argv[6])==1?true:false;
    string extra_generative_matrix = argv[7];
    bool extra_Encode_Flag = atoi(argv[8])==1?true:false;
    int iteration_limit = atoi(argv[9]);
    double SNR_min = atof(argv[10]);
    double SNR_max = atof(argv[11]);
    double SNR_ratio = atof(argv[12]);
    cout << "##############################" << endl;
    cout << "payload_matrix file  : " << payload_matrix_file << endl;
    cout << "payload_matrix result file : " << payload_matrix_performance << endl;
    cout << "extra_matrix file  : " << extra_matrix_file << endl;
    cout << "extra_matrix result file : " << extra_matrix_performance << endl;
    cout << "payload_generative_matrix : " << payload_generative_matrix << "|  payload_Flag : " << payload_Encode_Flag << endl;
    cout << "extra_generative_matrix : " << extra_generative_matrix << "|  extra_Flag : " << extra_Encode_Flag<< endl;
    // ---------------------
    cout << "iteration_limit : " << iteration_limit << endl;
    cout << "SNR_min : " << SNR_min << endl;
    cout << "SNR_max : " << SNR_max << endl;
    cout << "SNR_ratio : " << SNR_ratio << endl;
    cout << "##############################" << endl;
    
    // define H
    struct parity_check payload_H;
    Read_File_H(&payload_H,payload_matrix_file);
    struct parity_check extra_H;
    Read_File_H(&extra_H,extra_matrix_file);
    cout << "H file read success!!" << endl;

    // define G
    vector<vector<bool>> payload_G;
    vector<vector<bool>> extra_G;
    if(payload_Encode_Flag){
        payload_G = Read_File_G(payload_generative_matrix);
        cout << "payload G file read success!!" << endl;
    }
    if(extra_Encode_Flag){
        extra_G = Read_File_G(extra_generative_matrix);
        print_2d_matrix(extra_G);
        cout << "extra G file read success!!" << endl;
    }

    // output file (performance)
    ofstream payload_outFile(payload_matrix_performance);
    if (!payload_outFile.is_open()) {
        cout << "PayLoad Output file: " << payload_matrix_performance  << " cannot be opened." << endl;
		exit(1);
    }
    
    ofstream extra_outFile(extra_matrix_performance);
    if (!payload_outFile.is_open()) {
        cout << "Extra Output file: " << extra_matrix_performance  << " cannot be opened." << endl;
		exit(1);
    }
    // file init for outfile
    payload_outFile << "Eb_over_N0" << ", " << "frame_error_rate" << ", ";
    for(int i=0;i<iteration_limit;i++){
        payload_outFile << "BER_it_"+to_string(i) << ", ";
    }
    payload_outFile << endl;
    
    extra_outFile << "Eb_over_N0" << ", " << "frame_error_rate" << ", ";
    for(int i=0;i<iteration_limit;i++){
        extra_outFile << "BER_it_"+to_string(i) << ", ";
    }
    extra_outFile << endl;

    // CBP init setting 
    // extra bit 疊加在 payload data 前幾個 bit
    double SNR = SNR_min;
    
    double *payload_transmit_codeword = (double*)calloc(payload_H.n,sizeof(double));
    double *extra_transmit_codeword = (double*)calloc(extra_H.n,sizeof(double));
    double *Transmit_Codeword = (double*)calloc(payload_H.n,sizeof(double));

    double *receiver_LLR = (double*)malloc(payload_H.n*sizeof(double));
    double code_rate = (double)(payload_H.n-payload_H.m)/(double)payload_H.n;
    
    double ** payload_CN_2_VN_LLR = (double**)malloc(sizeof(double*)*payload_H.n);
    for(int i=0;i<payload_H.n;i++) payload_CN_2_VN_LLR[i]=(double*)calloc(payload_H.max_col_arr[i],sizeof(double));
    double ** payload_VN_2_CN_LLR = (double**)malloc(sizeof(double*)*payload_H.m);
    for(int i=0;i<payload_H.m;i++) payload_VN_2_CN_LLR[i]=(double*)calloc(payload_H.max_row_arr[i],sizeof(double));
    
    double ** extra_CN_2_VN_LLR = (double**)malloc(sizeof(double*)*extra_H.n);
    for(int i=0;i<extra_H.n;i++) extra_CN_2_VN_LLR[i]=(double*)calloc(extra_H.max_col_arr[i],sizeof(double));
    double ** extra_VN_2_CN_LLR = (double**)malloc(sizeof(double*)*extra_H.m);
    for(int i=0;i<extra_H.m;i++) extra_VN_2_CN_LLR[i]=(double*)calloc(extra_H.max_row_arr[i],sizeof(double));

    double *payload_guess = (double*)malloc(sizeof(double)*payload_H.n);
    double *extra_guess = (double*)malloc(sizeof(double)*extra_H.n);
    
    double *payload_bit_error_count = (double*)malloc(iteration_limit*sizeof(double));
    double *extra_bit_error_count = (double*)malloc(iteration_limit*sizeof(double));
    
    vector<bool> payload_information_bit((payload_G.size()));
    vector<bool> extra_information_bit((extra_G.size()));
    vector<int> payload_EncodeCodeWord,extra_EncodeCodeWord;
    clock_t  clk_start,clk_end;
    while(SNR <= SNR_max){
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        
        double payload_frame_count=0;
        double payload_frame_error=0;

        double extra_frame_count=0;
        double extra_frame_error=0;
        
        int it;
        for(int i=0;i<iteration_limit;i++){
            payload_bit_error_count[i]=0;
            extra_bit_error_count[i]=0;
        }
        clk_start = clock();
        double total_frame = 0;
        while(payload_frame_error < frame_error_lowwer_bound){
            // 是否要測試encode過後的效能
            // payload encode
            if(payload_Encode_Flag){
                for(int i=0;i<payload_G.size();i++){
                    payload_information_bit[i] = random_generation();
                }
                payload_EncodeCodeWord = Vector_Dot_Matrix_Int(payload_information_bit,payload_G);
                for(int i=0;i<payload_H.n;i++) payload_transmit_codeword[i] = (double)(payload_EncodeCodeWord[i]%2);
            }
            // extra encode
            if(extra_Encode_Flag){
                for(int i=0;i<extra_G.size();i++){
                    extra_information_bit[i] = random_generation();
                }
                extra_EncodeCodeWord = Vector_Dot_Matrix_Int(extra_information_bit,extra_G);
                for(int i=0;i<extra_H.n;i++) extra_transmit_codeword[i] = (double)(extra_EncodeCodeWord[i]%2);
            }
            
            for(int i=0;i<payload_H.n;i++){
                // 兩個都有做encode
                if(extra_Encode_Flag && payload_Encode_Flag){
                    if(i<extra_H.n) Transmit_Codeword[i] = (payload_EncodeCodeWord[i] + extra_EncodeCodeWord[i])%2; // Encode CodeWord
                    else Transmit_Codeword[i] = (payload_EncodeCodeWord[i])%2; // Encode CodeWord
                }
                else if(extra_Encode_Flag){
                    if(i<extra_H.n) Transmit_Codeword[i] = (extra_EncodeCodeWord[i])%2; // Encode CodeWord
                    else Transmit_Codeword[i] = 0; // Encode CodeWord
                }
                else if(payload_Encode_Flag){
                    Transmit_Codeword[i] = (payload_EncodeCodeWord[i])%2;
                }
                else Transmit_Codeword[i] = 0; // all zero
            }
            
            for(int i=0;i<payload_H.n;i++){
                // Sum_Product_Algorithm 是 soft decision 所以在調變的時候解
                // BPSK -> {0->1}、{1->-1}
                // double receiver_codeword=(-2*payload_transmit_codeword[i]+1)/sigma + gasdev(); // power up
                double receiver_codeword=(-2*Transmit_Codeword[i]+1) + sigma*gasdev(); // fixed power(noise reduce)
                receiver_LLR[i]=2*receiver_codeword/pow(sigma,2); 
            }
            
            it=0;
            bool payload_error_syndrome = true;
            bool payload_bit_error_flag=false;

            bool extra_error_syndrome = true;
            bool extra_bit_error_flag=false;

            // 開始做payload 的decode
            while(it<iteration_limit && payload_error_syndrome){          
                /* ------- CN update ------- */
                for(int VN=0;VN<payload_H.n;VN++){
                    for(int i=0;i<payload_H.max_col_arr[VN];i++){
                        int CN = payload_H.CN_2_VN_pos[VN][i];
                        double alpha=1,phi_beta_sum=0;
                        for(int j=0;j<payload_H.max_row_arr[CN];j++){
                            int other_VN = payload_H.VN_2_CN_pos[CN][j];
                            if(other_VN == VN) continue; 
                            if(it==0){ //第一次迭帶就是 直接加上剛算好的receiver_LLR
                                phi_beta_sum += phi(abs(receiver_LLR[other_VN]));
                                alpha *= receiver_LLR[other_VN]>0?1:-1;
                            }else{
                                phi_beta_sum += phi(abs(payload_VN_2_CN_LLR[CN][j]));
                                alpha *= payload_VN_2_CN_LLR[CN][j]>0?1:-1;
                            }
                        }
                        payload_CN_2_VN_LLR[VN][i]= alpha * phi(phi_beta_sum);
                    }
                }
                
                /* ------- VN update ------- */
                for(int CN=0;CN<payload_H.m;CN++){
                    for(int i=0;i<payload_H.max_row_arr[CN];i++){
                        int VN=payload_H.VN_2_CN_pos[CN][i];
                        double total_LLR = receiver_LLR[VN];
                        for(int j=0;j<payload_H.max_col_arr[VN];j++){
                            int other_CN = payload_H.CN_2_VN_pos[VN][j];
                            if(other_CN == CN) continue;
                            total_LLR += payload_CN_2_VN_LLR[VN][j];
                        }
                        payload_VN_2_CN_LLR[CN][i] = total_LLR;
                    }
                }
                
                /* ------- total LLR ------- */
                for(int VN=0;VN<payload_H.n;VN++){
                    payload_guess[VN]=receiver_LLR[VN];
                    for(int i=0;i<payload_H.max_col_arr[VN];i++){
                        payload_guess[VN]+=payload_CN_2_VN_LLR[VN][i];
                    }
                    /* ------- make decision ------- */
                    payload_guess[VN]= (payload_guess[VN]<0)?1:0;
                }
                
                /* Determine if it is a syndrome -> guess * H^T = vector(0) */
                
                for(int CN=0;CN<payload_H.m;CN++){
                    int count=0;
                    for(int i=0;i<payload_H.max_row_arr[CN];i++){
                        int VN = payload_H.VN_2_CN_pos[CN][i];
                        count+=payload_guess[VN];
                    }
                    if(count%2==1){
                        payload_error_syndrome = true;
                        break;
                    }
                    payload_error_syndrome = false;
                }
                
                /* ----- Determine bit error ----- */
                // payload_error_syndrome = false;
                payload_bit_error_flag=false;
                for(int VN=0;VN<payload_H.n;VN++){
                    if(payload_guess[VN]!=payload_transmit_codeword[VN]){
                        payload_bit_error_count[it]++;
                        payload_bit_error_flag=true;
                        // payload_error_syndrome = true;
                    }
                }
                it++;
            }
            if(payload_bit_error_flag){
                payload_frame_error+=1;
            }
            payload_frame_count++;


            // ----------------------------------------------
            it=0;
            // 開始做extra 的 decode
            // for(int i=0;i<extra_H.n;i++){
            //     receiver_LLR[i] = receiver_LLR[i] - (((payload_guess[i]*-2)+1)/pow(sigma,2)); 
            // } // 扣掉 payload 解完碼的結果
            for(int i=0;i<extra_H.n;i++){
                // -1^(payload_guess[i])
                double times =  payload_guess[i]==0?1:-1;
                receiver_LLR[i] = receiver_LLR[i] * times;
            } // 扣掉 payload 解完碼的結果
            while(it<iteration_limit && extra_error_syndrome){          
                /* ------- CN update ------- */
                for(int VN=0;VN<extra_H.n;VN++){
                    for(int i=0;i<extra_H.max_col_arr[VN];i++){
                        int CN = extra_H.CN_2_VN_pos[VN][i];
                        double alpha=1,phi_beta_sum=0;
                        for(int j=0;j<extra_H.max_row_arr[CN];j++){
                            int other_VN = extra_H.VN_2_CN_pos[CN][j];
                            if(other_VN == VN) continue; 
                            if(it==0){ //第一次迭帶就是 直接加上剛算好的receiver_LLR
                                phi_beta_sum += phi(abs(receiver_LLR[other_VN]));
                                alpha *= receiver_LLR[other_VN]>0?1:-1;
                            }else{
                                phi_beta_sum += phi(abs(extra_VN_2_CN_LLR[CN][j]));
                                alpha *= extra_VN_2_CN_LLR[CN][j]>0?1:-1;
                            }
                        }
                        extra_CN_2_VN_LLR[VN][i]= alpha * phi(phi_beta_sum);
                    }
                }
                
                /* ------- VN update ------- */
                for(int CN=0;CN<extra_H.m;CN++){
                    for(int i=0;i<extra_H.max_row_arr[CN];i++){
                        int VN=extra_H.VN_2_CN_pos[CN][i];
                        double total_LLR = receiver_LLR[VN];
                        for(int j=0;j<extra_H.max_col_arr[VN];j++){
                            int other_CN = extra_H.CN_2_VN_pos[VN][j];
                            if(other_CN == CN) continue;
                            total_LLR += extra_CN_2_VN_LLR[VN][j];
                        }
                        extra_VN_2_CN_LLR[CN][i] = total_LLR;
                    }
                }
                
                /* ------- total LLR ------- */
                for(int VN=0;VN<extra_H.n;VN++){
                    extra_guess[VN]=receiver_LLR[VN];
                    for(int i=0;i<extra_H.max_col_arr[VN];i++){
                        extra_guess[VN]+=extra_CN_2_VN_LLR[VN][i];
                    }
                    /* ------- make decision ------- */
                    extra_guess[VN]= (extra_guess[VN]<0)?1:0;
                }
                
                /* Determine if it is a syndrome -> guess * H^T = vector(0) */
                
                for(int CN=0;CN<extra_H.m;CN++){
                    int count=0;
                    for(int i=0;i<extra_H.max_row_arr[CN];i++){
                        int VN = extra_H.VN_2_CN_pos[CN][i];
                        count+=extra_guess[VN];
                    }
                    if(count%2==1){
                        extra_error_syndrome = true;
                        break;
                    }
                    extra_error_syndrome = false;
                }
                
                /* ----- Determine bit error ----- */
                
                extra_bit_error_flag=false;
                for(int VN=0;VN<extra_H.n;VN++){
                    if(extra_guess[VN]!=extra_transmit_codeword[VN]){
                        extra_bit_error_count[it]++;
                        extra_bit_error_flag=true;
                    }
                }
                
                it++;
                
            }
            if(extra_bit_error_flag){
                extra_frame_error+=1;
            }
            extra_frame_count++;
            total_frame++;
        }
        
        clk_end=clock();
        double clk_duration = (double)(clk_end-clk_start)/CLOCKS_PER_SEC;
        printf("Payload Data | SNR : %.2f  | FER : %.5e  | BER : %.5e(%2d) | Time : %6.3f | Total frame : %.0f\n",SNR,payload_frame_error/payload_frame_count,payload_bit_error_count[iteration_limit-1]/((double)payload_H.n*payload_frame_count),iteration_limit,clk_duration,payload_frame_count);
        payload_outFile << SNR << ", " << payload_frame_error/payload_frame_count << ", ";
        for(int i=0;i<iteration_limit;i++){
            payload_outFile << payload_bit_error_count[i]/((double)payload_H.n*payload_frame_count) << ", ";
        }
        payload_outFile <<endl;


        printf("Extra Data   | SNR : %.2f  | FER : %.5e  | BER : %.5e(%2d) | Time : %6.3f | Total frame : %.0f\n",SNR,extra_frame_error/extra_frame_count,extra_bit_error_count[iteration_limit-1]/((double)extra_H.n*extra_frame_count),iteration_limit,clk_duration,extra_frame_count);
        extra_outFile << SNR << ", " << extra_frame_error/extra_frame_count << ", ";
        for(int i=0;i<iteration_limit;i++){
            extra_outFile << extra_bit_error_count[i]/((double)extra_H.n*extra_frame_count) << ", ";
        }
        extra_outFile <<endl;
        
        if(SNR==SNR_max){
            cout << "over the SNR_max" << endl;
            break;
        }
        else if(SNR+SNR_ratio>SNR_max){
            SNR=SNR_max;
        }else{
            SNR=SNR+SNR_ratio;
        }
        
    }
    payload_outFile.close();
    /* ----------- free all memory ----------- */
    cout << "In main Free" << endl;
    FreeAllH(&payload_H);
    free(payload_transmit_codeword);
    free(receiver_LLR);
    for(int i=0;i<payload_H.n;i++) free(payload_CN_2_VN_LLR[i]);
    free(payload_CN_2_VN_LLR);
    for(int i=0;i<payload_H.m;i++) free(payload_VN_2_CN_LLR[i]);
    free(payload_VN_2_CN_LLR);
    free(payload_guess);
    free(payload_bit_error_count);
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
    // cout << "Each col 1's number : ";
    for(int i=0;i<H->n;i++){
        file_in >> H->max_col_arr[i];
        if(H->max_col_arr[i] > H->max_col_degree){
            cout << "** Error ----- " << file_name_in << " max_col_array is Error , Please Check!!" << endl;
            goto FreeAll;
        }
        // cout << H->max_col_arr[i] << " ";
    }
    // cout << endl << "Each row 1's number : ";
    for(int i=0;i<H->m;i++){
        file_in >> H->max_row_arr[i];
        if(H->max_row_arr[i] > H->max_row_degree){
            cout << "** Error ----- " << file_name_in << " max_row_array is Error , Please Check!!" << endl;
            goto FreeAll;
        }
        // cout << H->max_row_arr[i] << " ";
    }
    // cout << endl;
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
            idx++;
            continue;
        }
        else{
            
            for(string& s:line_split){
                int pos = stoi(s);
                if(pos!=0) G[pos-1][idx-3]=true; // 1
            }
            
        }
        idx++;
    }
    file.close();
    return G;
}