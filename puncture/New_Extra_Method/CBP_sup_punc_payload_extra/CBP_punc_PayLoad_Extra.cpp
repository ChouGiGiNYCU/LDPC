#include<iostream>
#include<fstream>
#include<cmath>
#include<ctime>
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include <limits.h>
#include "random_number_generator.h"
#include "UseFuction.h"


// #define frame_error_lowwer_bound 400
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
vector<int> Read_punc_pos(string file,vector<bool>& punc_pos,int  extra_nums);

int main(int argc,char* argv[]){
    double frame_error_lowwer_bound = 200;
    if(argc < 2){
        cout << "** Error ---- No file in" << endl; 
    }
    string H_combine_file = argv[1];
    string PayLoad_out_file = argv[2];
    string Extra_out_file = argv[3];
    string Payload_PCM_file = argv[4];
    string Extra_PCM_file = argv[5];
    string puncture_pos_file = argv[6];
    string PayLoad_G_file = argv[7]==string("None")?"":argv[7];
    bool PayLoad_Flag = PayLoad_G_file==""?false:true;
    string Extra_G_file = argv[8]==string("None")?"":argv[8];
    bool Extra_Flag = Extra_G_file==""?false:true;
    int iteration_limit = atoi(argv[9]);
    double SNR_min = atof(argv[10]);
    double SNR_max = atof(argv[11]);
    double SNR_ratio = atof(argv[12]);
    string Extra_Info_out_file = argv[13];
    
    cout << "##############################" << endl;
    cout << "H_combine_file : " << H_combine_file << endl;
    cout << "PayLoad_out_file : " << PayLoad_out_file << endl;
    cout << "Extra_out_file : " << Extra_out_file << endl;
    cout << "Payload PCM file : " << Payload_PCM_file << endl;
    cout << "Extra PCM file : " << Extra_PCM_file << endl;
    cout << "puncture_pos_file : " << puncture_pos_file << endl;
    cout << "Payload G file : " << PayLoad_G_file << endl;
    cout << "PayLoad_Flag : " << PayLoad_Flag << endl;
    cout << "Extra G file : " << Extra_G_file << endl;
    cout << "Extra_Flag : " << Extra_Flag << endl;
    cout << "Iteration_limit : " << iteration_limit << endl;
    cout << "SNR_min : " << SNR_min << endl;
    cout << "SNR_max : " << SNR_max << endl;
    cout << "SNR_ratio : " << SNR_ratio << endl;
    cout << "Extra_Info_out_file : " << Extra_Info_out_file << endl;
    cout << "##############################" << endl;
    
    // define H_combine
    struct parity_check H;
    Read_File_H(&H,H_combine_file);
    cout << "H_combine file read success!!" << endl;
    // define PayLoad_H
    struct parity_check PayLoad_H;
    Read_File_H(&PayLoad_H,Payload_PCM_file);
    cout << "PayLoad_H file read success!!" << endl;
    // define Extra_H
    struct parity_check Extra_H;
    Read_File_H(&Extra_H,Extra_PCM_file);
    cout << "Extra_H file read success!!" << endl;
    // Read Puncture pos
    vector<bool> punc_pos(H.n,false);
    vector<int> punc_map;
    punc_map = Read_punc_pos(puncture_pos_file,punc_pos,Extra_H.n);
    cout << "Puncture position file read success !!" << endl;
    // define PayLoad_G
    vector<vector<bool>> PayLoad_G;
    if(PayLoad_Flag){
        PayLoad_G = Read_File_G(PayLoad_G_file);
        cout << "PayLoad_G file read success!!" << endl;
    }
    // define Extra_G
    vector<vector<bool>> Extra_G;
    if(Extra_Flag){
        Extra_G = Read_File_G(Extra_G_file);
        cout << "Extra_G file read success!!" << endl;
    }
    // create payload out csv
    ofstream PayLoad_outfile(PayLoad_out_file);
    if (!PayLoad_outfile.is_open()) {
        cout << "Output file: " << PayLoad_out_file  << " cannot be opened." << endl;
		exit(1);
    }
    PayLoad_outfile << "Eb_over_N0" << ", " << "frame_error_rate" << ", ";
    for(int i=0;i<iteration_limit;i++){
        PayLoad_outfile << "BER_it_"+to_string(i) << ", ";
    }
    PayLoad_outfile << endl;

    // create extra codeword out csv
    ofstream Extra_outfile(Extra_out_file);
    if (!Extra_outfile.is_open()) {
        cout << "Output file: " << Extra_out_file  << " cannot be opened." << endl;
		exit(1);
    }
    Extra_outfile << "Eb_over_N0" << ", " << "frame_error_rate" << ", ";
    for(int i=0;i<iteration_limit;i++){
        Extra_outfile << "BER_it_"+to_string(i) << ", ";
    }
    Extra_outfile << endl;
    
    // create extra info out csv
    ofstream Extra_Info_outfile(Extra_Info_out_file);
    if (!Extra_Info_outfile.is_open()) {
        cout << "Output file: " << Extra_Info_out_file  << " cannot be opened." << endl;
		exit(1);
    }
    Extra_Info_outfile << "Eb_over_N0" << ", " << "frame_error_rate" << ", ";
    for(int i=0;i<iteration_limit;i++){
        Extra_Info_outfile << "BER_it_"+to_string(i) << ", ";
    }
    Extra_Info_outfile << endl;
    // Extra bits info length = 11 -> (info=6、fix=5) 後 5 bits 為 info
    int fixed_bits = 5;
    int info_bits = Extra_G.size() - fixed_bits; // 6 
    int parity_check_bits = Extra_G[0].size() - Extra_G.size();
    double SNR = SNR_min;
    double code_rate = (double)(PayLoad_H.n-PayLoad_H.m)/(double)PayLoad_H.n; // 用原本的payload 算 coderate
    
    int *transmit_codeword = (int*)calloc(H.n,sizeof(int));
    double *receiver_LLR = (double*)malloc(H.n*sizeof(double));
    double ** CN_2_VN_LLR = (double**)malloc(sizeof(double*)*H.n);
    for(int i=0;i<H.n;i++) CN_2_VN_LLR[i]=(double*)calloc(H.max_col_arr[i],sizeof(double));
    double ** VN_2_CN_LLR = (double**)malloc(sizeof(double*)*H.m);
    for(int i=0;i<H.m;i++) VN_2_CN_LLR[i]=(double*)calloc(H.max_row_arr[i],sizeof(double));
    double *payload_guess = (double*)malloc(sizeof(double)*PayLoad_H.n);
    double *extra_guess = (double*)malloc(sizeof(double)*Extra_H.n);
    double *extra_info_guess = (double*)malloc(sizeof(double)*Extra_G.size());
    double *payload_bit_error_count = (double*)calloc(iteration_limit,sizeof(double));
    double *extra_bit_error_count = (double*)calloc(iteration_limit,sizeof(double));
    double *extra_Info_bit_error_count = (double*)calloc(iteration_limit,sizeof(double));
    double *totalLLR = (double*)malloc(H.n*sizeof(double));
    vector<bool> payload_info((PayLoad_G.size()));
    vector<bool> extra_info((Extra_G.size()));
    vector<int> payload_Encode(PayLoad_H.n,0);
    vector<int> extra_Encode(Extra_H.n,0);
    
    clock_t  clk_start,clk_end;
    while(SNR <= SNR_max){
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        double frame_count=0;
        double payload_frame_error=0,extra_frame_error=0;
        int it=0;
        for(int i=0;i<iteration_limit;i++) payload_bit_error_count[i]=0;
        for(int i=0;i<iteration_limit;i++) extra_bit_error_count[i]=0;
        for(int i=0;i<iteration_limit;i++) extra_Info_bit_error_count[i]=0;
        clk_start = clock();
        while(payload_frame_error < frame_error_lowwer_bound){
            // 處理 Payload CodeWord 是zero 還是Encode
            if(PayLoad_Flag){
                for(int i=0;i<PayLoad_G.size();i++){
                    payload_info[i] = random_generation()>0.5?1:0;
                }
                payload_Encode = Vector_Dot_Matrix_Int(payload_info,PayLoad_G);
                for(int i=0;i<payload_Encode.size();i++){
                    payload_Encode[i] = payload_Encode[i]%2;
                }
                
            }
            // 處理 Extra CodeWord 是zero 還是Encode
            if(Extra_Flag){
                // [info_bits extra_bits]
                for(int i=0;i<Extra_G.size();i++){
                    extra_info[i] = random_generation()>0.5?1:0;
                }
                // 把 info bits 後面的 fix 成 0 
                for(int i=info_bits;i<Extra_G.size();i++){
                    extra_info[i] = 0;
                }
                extra_Encode = Vector_Dot_Matrix_Int(extra_info,Extra_G);
                for(int i=0;i<extra_Encode.size();i++){
                    extra_Encode[i] = extra_Encode[i]%2;
                }
            }
            
            // PayLoad xor Extra
            for(int i=0;i<PayLoad_H.n;i++){
                if(PayLoad_Flag) transmit_codeword[i] = payload_Encode[i]%2;
                else transmit_codeword[i] = 0;
            }
            for(int i=0;i<Extra_H.n;i++){
                int xor_pos = punc_map[i];
                if(Extra_Flag) 
                    transmit_codeword[xor_pos] = (transmit_codeword[xor_pos] + (extra_Encode[i]%2))%2; 
            }
            
            // [PayLoad_LLR Extra Sup]
            // Count LLR
            for(int i=0;i<PayLoad_H.n;i++){
                // Sum_Product_Algorithm 是 soft decision 所以在調變的時候解
                // BPSK -> {0->1}、{1->-1}
                // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
                double receive_value=(double)(-2*transmit_codeword[i]+1) + sigma*gasdev(); // fixed power
                receiver_LLR[i]=2*receive_value/pow(sigma,2); 
            }
            
            // setting puncture bits
            for(int i=PayLoad_H.n;i<H.n;i++){
                // Extra bit be punctured
                if(i>=PayLoad_H.n && i<(Extra_H.n+PayLoad_H.n-fixed_bits)){ //
                    receiver_LLR[i] = 0; // extra be punctured
                }else if(i>=Extra_H.n+PayLoad_H.n-fixed_bits && i<Extra_H.n+PayLoad_H.n){
                    // 因為 G = [p I] 最後五個bits為fixed
                    // -> CoderWord = [random fixed_bits]
                    receiver_LLR[i] = INT_MAX;  
                }
                else if(i>=(Extra_H.n+PayLoad_H.n)){ // 
                    int idx = i-(Extra_H.n+PayLoad_H.n);
                    int origin_vn_pos = punc_map[idx];
                    receiver_LLR[i] = receiver_LLR[origin_vn_pos];
                    receiver_LLR[origin_vn_pos] = 0; // be punctured 
                }
            }
            
            it=0;
            bool payload_error_syndrome = true;
            bool extra_error_syndrome = true;
            bool payload_bit_error_flag=false;
            bool extra_bit_error_flag=false;
            while(it<iteration_limit && (payload_error_syndrome || extra_error_syndrome)){
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
                                double LLR = receiver_LLR[other_VN];
                                phi_beta_sum += phi(abs(LLR));
                                alpha *= (LLR>=0)?1:-1;
                                //  total_LLR *= tanh(0.5*(receiver_LLR[other_VN])); 
                            }else{
                                double LLR = VN_2_CN_LLR[CN][j];
                                phi_beta_sum += phi(abs(LLR));
                                alpha *=( LLR>=0)?1:-1;
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
                    totalLLR[VN]=receiver_LLR[VN];
                    for(int i=0;i<H.max_col_arr[VN];i++){
                        totalLLR[VN]+=CN_2_VN_LLR[VN][i];
                    }
                }
                
                /* Assgin LLR to PayLoad & guess CodeWord */
                for(int VN=0;VN<PayLoad_H.n;VN++){
                    payload_guess[VN] = totalLLR[VN]>=0?0:1;
                } 
                
                /* Assgin LLR to Extra & guess CodeWord */
                for(int VN=PayLoad_H.n;VN<(PayLoad_H.n+Extra_H.n);VN++){
                    extra_guess[VN-PayLoad_H.n] = totalLLR[VN]>=0?0:1;
                } 
                
                /* Assgin LLR to Extra_info & guess Info_bits */
                // G = [P I]
                for(int VN=parity_check_bits;VN<Extra_H.n;VN++){
                    extra_info_guess[VN] = extra_guess[VN];
                } 
                /* Determine if it is a syndrome -> guess * H^T = vector(0) - PayLoad */
                payload_error_syndrome = false;
                for(int CN=0;CN<PayLoad_H.m;CN++){
                    int count=0;
                    for(int i=0;i<PayLoad_H.max_row_arr[CN];i++){
                        int VN = PayLoad_H.VN_2_CN_pos[CN][i];
                        count+=payload_guess[VN];
                    }
                    if(count%2==1){
                        payload_error_syndrome = true;
                        break;
                    }
                    payload_error_syndrome = false;
                }
                
                /* Determine if it is a syndrome -> guess * H^T = vector(0) - Extra */ 
                extra_error_syndrome = false;
                for(int CN=0;CN<Extra_H.m;CN++){
                    int count=0;
                    for(int i=0;i<Extra_H.max_row_arr[CN];i++){
                        int VN = Extra_H.VN_2_CN_pos[CN][i];
                        count+=extra_guess[VN];
                    }
                    if(count%2==1){
                        extra_error_syndrome = true;
                        break;
                    }
                    extra_error_syndrome = false;
                }
                
                /* ----- Determine PayLoad BER ----- */
                payload_bit_error_flag=false;
                for(int VN=0;VN<PayLoad_H.n;VN++){
                    if(payload_guess[VN]!=payload_Encode[VN]){
                        payload_bit_error_count[it] += 1;
                        payload_bit_error_flag=true;
                        
                    }
                }
                if(it<2){
                    cout << "Prev  it while - " << payload_bit_error_count[0] << " | " << payload_bit_error_count[1] << " | " << payload_bit_error_count[2] << endl;
                }
                /* ----- Determine Extra BER ----- */
                extra_bit_error_flag=false;
                for(int VN=0;VN<Extra_H.n;VN++){
                    if(extra_guess[VN]!=(double)extra_Encode[VN]){
                        extra_bit_error_count[it]+=1;
                        extra_bit_error_flag=true;
                    }
                }
                
                for(int VN=0;VN<info_bits;VN++){
                    if(extra_info_guess[VN]!=(double)extra_info[VN]){
                        extra_Info_bit_error_count[it]+=1;
                    }
                }
                
                if(it<3){
                    cout << "In  it while - " << payload_bit_error_count[0] << " | " << payload_bit_error_count[1] << " | " << payload_bit_error_count[2] << endl;
                }
                it++;
            }
            
            // 如果 syndorme check is ok ，但是codeword bit 有錯，代表解錯codeword ， BER[it+1:iteration_limit] += codeword length
            // if(!payload_error_syndrome && payload_bit_error_flag){
            //     for(int it_idx=it;it_idx<iteration_limit;it_idx++){
            //         payload_bit_error_count[it_idx] += PayLoad_H.n;
            //     }
            // }
            
            // 如果 syndorme check is ok ，但是codeword bit 有錯，代表解錯codeword ， BER[it+1:iteration_limit] += codeword length
            if(!extra_error_syndrome && extra_bit_error_flag){
                for(int it_idx=it;it_idx<iteration_limit;it_idx++){
                    extra_bit_error_count[it_idx] += Extra_H.n;
                    extra_Info_bit_error_count[it_idx] += info_bits;
                }
            }
            if(payload_bit_error_flag){
                payload_frame_error+=1;
            }
            if(extra_bit_error_flag){
                extra_frame_error+=1;
            }
            frame_count++;
            // cout << "In  while - " << payload_bit_error_count[0] << " | " << payload_bit_error_count[1] << endl;
        }

        clk_end=clock();
        double clk_duration = (double)(clk_end-clk_start)/CLOCKS_PER_SEC;
        printf("PayLoad - SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Time : %.3f | Total frame : %.0f\n",SNR,payload_frame_error/frame_count,payload_bit_error_count[iteration_limit-1]/((double)PayLoad_H.n*frame_count),iteration_limit,clk_duration,frame_count);
        printf("Extra   - SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Time : %.3f | Error frame : %.0f | BER_num : %.0f\n",SNR,extra_frame_error/frame_count,extra_bit_error_count[iteration_limit-1]/((double)Extra_H.n*frame_count),iteration_limit,clk_duration,extra_frame_error,extra_bit_error_count[iteration_limit-1]);
        printf("Info    - SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Time : %.3f | Error frame : %.0f | BER_num : %.0f\n",SNR,extra_frame_error/frame_count,extra_Info_bit_error_count[iteration_limit-1]/((double)info_bits*frame_count),iteration_limit,clk_duration,extra_frame_error,extra_Info_bit_error_count[iteration_limit-1]);
        
        // Write PayLoad Performance to csv
        PayLoad_outfile << SNR << ", " << payload_frame_error/frame_count << ", ";
        for(int i=0;i<iteration_limit;i++){
            PayLoad_outfile << payload_bit_error_count[i]/((double)PayLoad_H.n*frame_count) << ", ";
        }
        PayLoad_outfile <<endl;

        // Write Extra Performance to csv
        Extra_outfile << SNR << ", " << extra_frame_error/frame_count << ", ";
        for(int i=0;i<iteration_limit;i++){
            Extra_outfile << extra_bit_error_count[i]/((double)Extra_H.n*frame_count) << ", ";
        }
        Extra_outfile <<endl;
        
        // Write Extra Info Performance to csv 
        Extra_Info_outfile << SNR << ", " << extra_frame_error/frame_count << ", ";
        for(int i=0;i<iteration_limit;i++){
            Extra_Info_outfile << extra_Info_bit_error_count[i]/((double)Extra_G.size()*frame_count) << ", ";
        }
        Extra_Info_outfile <<endl;

        // Change SNR 
        if(SNR==SNR_max) break;
        SNR = min(SNR+SNR_ratio,SNR_max);
        
        // chage error frame bound 
        // if(payload_bit_error_count[iteration_limit-1]/((double)PayLoad_H.n*frame_count) < 1e-4){
        //     frame_error_lowwer_bound /=2;
        // }
        // frame_error_lowwer_bound = fmax(frame_error_lowwer_bound,50);
        
    }
    PayLoad_outfile.close();
    Extra_outfile.close();
    Extra_Info_outfile.close();
    /* ----------- free all memory ----------- */
    cout << "In main Free" << endl;
    FreeAllH(&H);
    free(transmit_codeword);
    free(receiver_LLR);
    for(int i=0;i<H.n;i++) free(CN_2_VN_LLR[i]);
    free(CN_2_VN_LLR);
    for(int i=0;i<H.m;i++) free(VN_2_CN_LLR[i]);
    free(VN_2_CN_LLR);
    free(payload_guess);
    free(extra_guess);
    free(payload_bit_error_count);
    free(extra_bit_error_count);
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
vector<int> Read_punc_pos(string file,vector<bool>& punc_pos,int extra_nums){
    ifstream punc_file(file);
    vector<int> punc_map;
   
    cout <<  "punc pos : " ;
    for(int i=0;i<extra_nums;i++){
        int pos;
        punc_file >> pos;
        cout << " " << pos << endl;
        
        punc_pos[pos-1]=true;
        punc_map.push_back(pos-1);
        
    }
    punc_file.close();
    return punc_map;
}
vector<vector<bool>> Read_File_G(string& file_name){
    ifstream file(file_name);
    string line;
    vector<string> line_split;
    int idx=0,n,k,max_col;
    vector<vector<bool>> G;
    vector<int> max_col_map;
    while (getline(file, line)) {
        line = strip(line);
        // cout << line << endl;
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==2){
                n = stoi(line_split[0]);
                k = stoi(line_split[1]);
                G = vector<vector<bool>>(k,vector<bool>(n,0));
                cout << "G - n : " << n << " | k : " << k << endl;
            }else{
                cerr << "First line(n,k) : " << line << " is not n,k !!" << endl;
                exit(1);
            }
        }else if(idx==1){
            if(line_split.size()==1){
                max_col = stoi(line_split[0]);
                cout << "max col : "  << max_col << endl;
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
                cerr << "Third line(each col 1's number) : " << line << " is not equal G matrix colsize  !!" << endl;
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