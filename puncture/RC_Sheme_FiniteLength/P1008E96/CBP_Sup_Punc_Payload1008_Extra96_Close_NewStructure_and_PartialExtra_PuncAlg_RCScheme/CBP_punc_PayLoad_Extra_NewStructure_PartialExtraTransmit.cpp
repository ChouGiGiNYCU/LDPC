#include<iostream>
#include<fstream>
#include<cmath>
#include<ctime>
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include "UseFuction.h"
#include "random_number_generator.h"
#include <boost/dynamic_bitset.hpp>

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
vector<boost::dynamic_bitset<>> Read_File_G(string& file_name);
vector<int> Read_punc_pos(string file,int  extra_nums);
vector<int> Read_Structure_pos(string file,int  extra_nums);

int main(int argc,char* argv[]){
    double frame_error_lowwer_bound = 400;
    if(argc < 2){
        cout << "** Error ---- No file in \n" ; 
    }
    string H_combine_file = argv[1];
    string PayLoad_out_file = argv[2];
    string Extra_out_file = argv[3];
    string Sup_out_file = argv[4]; 
    string Payload_PCM_file = argv[5];
    string Extra_PCM_file = argv[6];
    // string puncture_pos_file = argv[6];
    string PayLoad_G_file = argv[7]==string("None")?"":argv[7];
    bool PayLoad_Flag = PayLoad_G_file==""?false:true;
    string Extra_G_file = argv[8]==string("None")?"":argv[8];
    bool Extra_Flag = Extra_G_file==""?false:true;
    int iteration_limit = atoi(argv[9]);
    double SNR_min = atof(argv[10]);
    double SNR_max = atof(argv[11]);
    double SNR_ratio = atof(argv[12]);
    int iteration_open = atoi(argv[13]); // Open_Iteration
    bool iteration_open_flag = iteration_open>0?true:false;
    string Superposition_origin_file = argv[14]; // 原本的方法做疊加，Extra/Payload都做punc
    string Table_ExtraTransmit_PayloadPunc_file = argv[15]; // 部分extra 傳送，部分payload不傳
    string Superpostion_Payload_Extra_NewStructure_pos_file = argv[16]; // 新的結構 一個被punc過的Extra 會跟 兩個payload bits做結合
    cout << "##############################" << "\n";
    cout << "H_combine_file : " << H_combine_file << "\n";
    cout << "PayLoad_out_file : " << PayLoad_out_file << "\n";
    cout << "Extra_out_file : " << Extra_out_file << "\n";
    cout << "Sup_out_file : " << Sup_out_file << "\n";
    cout << "Payload PCM file : " << Payload_PCM_file << "\n";
    cout << "Extra PCM file : " << Extra_PCM_file << "\n";
    cout << "Payload G file : " << PayLoad_G_file << "\n";
    cout << "PayLoad_Flag : " << PayLoad_Flag << "\n";
    cout << "Extra G file : " << Extra_G_file << "\n";
    cout << "Extra_Flag : " << Extra_Flag << "\n";
    cout << "Iteration_limit : " << iteration_limit << "\n";
    cout << "SNR_min : " << SNR_min << "\n";
    cout << "SNR_max : " << SNR_max << "\n";
    cout << "SNR_ratio : " << SNR_ratio << "\n";
    cout << "iteration_open : " << iteration_open << "\n";
    cout << "iteration_open_flag : " << iteration_open_flag << "\n"; 
    cout << "Superposition_origin_file : " << Superposition_origin_file << "\n";
    cout << "Table_ExtraTransmit_PayloadPunc_file : " << Table_ExtraTransmit_PayloadPunc_file << "\n";
    cout << "Superpostion_Payload_Extra_NewStructure_pos_file : " << Superpostion_Payload_Extra_NewStructure_pos_file << "\n";
    cout << "##############################" << "\n";
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
    cout << "##############################" << "\n";
    // Read Superposiotn origin pos
    vector<pair<int,int>> Superposition_origin;
    Superposition_origin = Read_Extra2Pyalod_CSVfile(Superposition_origin_file);
    cout << "Puncture position(Superposition origin) file read success !!" << endl;
    // Read Superposiotn with new strucuture pos 
    vector<New_Strcuture> Superpostion_Payload_Extra_NewStructure;
    Superpostion_Payload_Extra_NewStructure = Read_NewStrcuture_CSVfile(Superpostion_Payload_Extra_NewStructure_pos_file);
    cout << "New_Structure_map position file read success !!" << endl;
    cout << "##############################" << "\n";
    // Read Partial Extra Transmit Extra Vn and Payload VN  
    vector<pair<int,int>> Table_ExtraTransmit_PayloadPunc;
    Table_ExtraTransmit_PayloadPunc = Read_Extra2Pyalod_CSVfile(Table_ExtraTransmit_PayloadPunc_file);
    cout << "Table_ExtraTransmit_PayloadPunc position file read success !!" << endl;
    cout << "##############################" << "\n";
    // define PayLoad_G
    vector<boost::dynamic_bitset<>> Payload_Origin_G;
    vector<boost::dynamic_bitset<>> Payload_Transpose_G;
    if(PayLoad_Flag){
        Payload_Origin_G = Read_File_G(PayLoad_G_file);
        Payload_Transpose_G = Transpose_Matrix(Payload_Origin_G); 
        cout << "PayLoad_G file read success!!" << endl;
        cout << "##############################" << "\n";
    }
    // define Extra_G
    vector<boost::dynamic_bitset<>> Extra_Origin_G;
    vector<boost::dynamic_bitset<>> Extra_Transpose_G;
    if(Extra_Flag){
        Extra_Origin_G = Read_File_G(Extra_G_file);
        Extra_Transpose_G = Transpose_Matrix(Extra_Origin_G); 
        cout << "Extra_G file read success!!" << endl;
        cout << "##############################" << "\n";
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

    // create extra out csv
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

    // create Sup out csv
    // ofstream Superposition_outfile(Sup_out_file);
    // if (!Superposition_outfile.is_open()) {
    //     cout << "Output file: " << Sup_out_file  << " cannot be opened." << endl;
	// 	exit(1);
    // }
    // Superposition_outfile << "Eb_over_N0" << ", " << "frame_error_rate" << ", ";
    // for(int i=0;i<iteration_limit;i++){
    //     Superposition_outfile << "BER_it_"+to_string(i) << ", ";
    // }
    // Superposition_outfile << endl;
    
    double SNR = SNR_min;
    // ((Payload info) + length(Extra Codeword))/ length(Payload CodeWord) 
    double code_rate = (double)(PayLoad_H.n-PayLoad_H.m+Extra_Origin_G.size())/(double)PayLoad_H.n; 
    cout << "CodeRate : " << code_rate << "\n";
    
    vector<vector<double>> CN_2_VN_LLR(H.n);  // Vector of vectors for CN to VN LLR
    vector<vector<double>> VN_2_CN_LLR(H.m);  // Vector of vectors for VN to CN LLR
    // Initialize each row of the vectors with the appropriate size
    for (int i = 0; i < H.n; i++) {
        CN_2_VN_LLR[i].resize(H.max_col_arr[i], 0.0);  // Resize each row to match max_col_arr[i] and initialize with 0.0
    }
    for (int i = 0; i < H.m; i++) {
        VN_2_CN_LLR[i].resize(H.max_row_arr[i], 0.0);  // Resize each row to match max_row_arr[i] and initialize with 0.0
    }
    
    vector<double> receiver_LLR(H.n,0); 
    vector<double> payload_bit_error_count(iteration_limit,0);
    vector<double> extra_bit_error_count(iteration_limit,0);
    vector<double> superposition_bit_error_count(iteration_limit,0);
    vector<double> totalLLR(H.n,0);

    boost::dynamic_bitset<> transmit_codeword(H.n);
    boost::dynamic_bitset<> payload_info(Payload_Origin_G.size());
    boost::dynamic_bitset<> extra_info(Extra_Origin_G.size());
    boost::dynamic_bitset<> payload_Encode(PayLoad_H.n);
    boost::dynamic_bitset<> extra_Encode(Extra_H.n);
    boost::dynamic_bitset<> payload_guess(PayLoad_H.n);
    boost::dynamic_bitset<> extra_guess(Extra_H.n);
    
    clock_t  clk_start,clk_end;
    int nonTransmit_Extra_Vns_num = Superposition_origin.size();
    while(SNR <= SNR_max){
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        double frame_count=0;
        double payload_frame_error=0,extra_frame_error=0,superposition_frame_error=0;
        int it;
        fill(payload_bit_error_count.begin(),payload_bit_error_count.end(),0.0);
        fill(extra_bit_error_count.begin(),extra_bit_error_count.end(),0.0);
        fill(superposition_bit_error_count.begin(),superposition_bit_error_count.end(),0.0);
        clk_start = clock();
        while(payload_frame_error < frame_error_lowwer_bound){
            // 處理 Payload CodeWord 是zero 還是Encode
            if(PayLoad_Flag){
                for(int i=0;i<Payload_Origin_G.size();i++){
                    payload_info[i] = random_generation();
                }
                GF2_Mat_Vec_Dot(payload_info,Payload_Transpose_G,payload_Encode);
            }else{
                payload_Encode.reset();
            }
            // 處理 Extra CodeWord 是zero 還是Encode
            if(Extra_Flag){
                for(int i=0;i<Extra_Origin_G.size();i++){
                    extra_info[i] = random_generation();
                }
                GF2_Mat_Vec_Dot(extra_info,Extra_Transpose_G,extra_Encode);
            }else{
                extra_Encode.reset();
            }
            /*------------ CodeWord process ------------*/
            // PayLoad 
            for(int i=0;i<PayLoad_H.n;i++){
                transmit_codeword[i] = payload_Encode[i];
            }
            // PayLoad xor Extra with orgin method
            for(auto& table:Superposition_origin){
                int extra_pos = table.first;
                int payload_pos = table.second;
                transmit_codeword[payload_pos] = transmit_codeword[payload_pos] ^ extra_Encode[extra_pos];
            }
            // Transmit extra / payload punc
            for(auto& table:Table_ExtraTransmit_PayloadPunc){
                int extra_pos = table.first;
                int payload_pos = table.second;
                transmit_codeword[payload_pos] =  extra_Encode[extra_pos];
            }
            // Payload xor Extra with NewStructure (Extra ^ P1 ^ P2)
            for(auto& table:Superpostion_Payload_Extra_NewStructure){
                int extra_vn  = table.extra_vn;
                int payload_vn_nonpunc  = table.payload_vn_nonpunc;
                int payload_vn_punc  = table.payload_vn_punc;
                transmit_codeword[payload_vn_punc] = transmit_codeword[payload_vn_punc] ^ payload_Encode[payload_vn_nonpunc] ^ extra_Encode[extra_vn];
            }
            /*------------------------------------------*/
            // Count LLR
            fill(receiver_LLR.begin(),receiver_LLR.end(),0.0);
            for(int i=0;i<PayLoad_H.n;i++){
                // Sum_Product_Algorithm 是 soft decision 所以在調變的時候解
                // BPSK -> {0->1}、{1->-1}
                // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
                double receive_value=static_cast<double>(-2*transmit_codeword[i]+1) + sigma*gasdev(); // fixed power
                receiver_LLR[i]=2*receive_value/pow(sigma,2); 
            }

            /*------------ LLR position swap ------------*/
            // vn consist : [Payload_VNs , Extra_VNs , Superposition_VNs , Extra^Payload1_VNs , Extra^Payload1^Payload2_VNs ]
            // setting puncture bits - Extra_VNs
            for(int i=PayLoad_H.n;i<(PayLoad_H.n+Extra_H.n);i++) receiver_LLR[i] = 0;
             // 部分Extra 傳送過來的資訊要放上去
            for(auto& table:Table_ExtraTransmit_PayloadPunc){
                int extra_vn = table.first;
                int payload_vn = table.second;
                receiver_LLR[PayLoad_H.n+extra_vn] = receiver_LLR[payload_vn]; 
                receiver_LLR[payload_vn] = 0; // 原本的payload被puncture掉
            }
            // setting puncture bits - Superposition_VNs
            auto pair_iter = Superposition_origin.begin();
            for(int i=(PayLoad_H.n+Extra_H.n);i<(PayLoad_H.n+Extra_H.n+nonTransmit_Extra_Vns_num) && pair_iter!=Superposition_origin.end();i++,pair_iter++){
                int origin_vn_pos = pair_iter->second;
                receiver_LLR[i] = receiver_LLR[origin_vn_pos];
                receiver_LLR[origin_vn_pos] = 0; // be punctured
            }
            // setting puncture bits - Extra^Payload1_VNs
            for(int i=(PayLoad_H.n+Extra_H.n+nonTransmit_Extra_Vns_num);i<(PayLoad_H.n+Extra_H.n+nonTransmit_Extra_Vns_num*2);i++) receiver_LLR[i] = 0; // be punctured
            // setting puncture bits - Extra^Payload1^Payload2_VNs
            auto pair_iter1  = Superpostion_Payload_Extra_NewStructure.begin();
            for(int i=(PayLoad_H.n+Extra_H.n+nonTransmit_Extra_Vns_num*2);i<(PayLoad_H.n+Extra_H.n+nonTransmit_Extra_Vns_num*3) && pair_iter1!=Superpostion_Payload_Extra_NewStructure.end();i++,pair_iter1++){
                int Payload_punc_pos = pair_iter1->payload_vn_punc;
                receiver_LLR[i] = receiver_LLR[Payload_punc_pos];
                receiver_LLR[Payload_punc_pos] = 0;
            }
            /*------------------------------------------*/
            
            it=0;
            bool payload_error_syndrome = true;
            bool extra_error_syndrome = true;
            bool payload_bit_error_flag=false;
            bool extra_bit_error_flag=false;
            bool payload_correct_flag = false; // 如果 payload syndrome 是全零，代表 codeword 是個合法的，就開通往extra 傳送 message
            bool superposition_bit_error_flag = false;
            int payload_error_bit = 0 , extra_error_bit=0 , superposition_error_bit=0;
            fill(totalLLR.begin(),totalLLR.end(),0.0);
            for(auto& row : CN_2_VN_LLR)  fill(row.begin(), row.end(), 0.0);
            for(auto& row : VN_2_CN_LLR)  fill(row.begin(), row.end(), 0.0);
            while(it<iteration_limit && (payload_error_syndrome || extra_error_syndrome)){
                payload_error_bit = 0;
                extra_error_bit = 0;
                superposition_error_bit = 0;
                /* ------- CN update ------- */
                for(int VN=0;VN<H.n;VN++){
                    // 不把訊息傳到Extra 那邊
                    if(iteration_open_flag && VN>=PayLoad_H.n && VN<(Extra_H.n+PayLoad_H.n) && (it<iteration_open && payload_correct_flag==false)) continue;
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
                                if(LLR==0) alpha *= random_generation()?1:-1; // random 
                                else alpha *= (LLR>0)?1:-1;
                                
                                // total_LLR *= tanh(0.5*(receiver_LLR[other_VN])); 
                            }else{
                                double LLR = VN_2_CN_LLR[CN][j];
                                phi_beta_sum += phi(abs(LLR));
                                if(LLR==0) alpha *= random_generation()?1:-1; // random 
                                else alpha *=(LLR>0)?1:-1;
                                
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
                        if(iteration_open_flag && VN>=PayLoad_H.n && VN<(Extra_H.n+PayLoad_H.n) && (it<iteration_open && payload_correct_flag==false)) continue;
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
                    payload_guess[VN] = totalLLR[VN]>0?0:1;
                } 
                /* Assgin LLR to Extra & guess CodeWord */
                for(int VN=PayLoad_H.n;VN<(PayLoad_H.n+Extra_H.n);VN++){
                    extra_guess[VN-PayLoad_H.n] = totalLLR[VN]>0?0:1;
                } 
                
                /* Determine if it is a syndrome -> guess * H^T = vector(0) */
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
                
                /* Determine if it is a syndrome -> guess * H^T = vector(0) - PayLoad */ 
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
                if(iteration_open_flag && payload_correct_flag==false && it<iteration_open) extra_error_syndrome = true;
                if(iteration_open_flag && payload_error_syndrome==false) payload_correct_flag = true;
                /* ----- Determine PayLoad BER ----- */
                payload_bit_error_flag=false;
                auto payload_check_result =  payload_guess ^ payload_Encode;
                payload_error_bit = payload_check_result.count();
                if(payload_error_bit>0) payload_bit_error_flag = true;
                payload_bit_error_count[it] += static_cast<double>(payload_error_bit); 
                
                /* ----- Determine Extra BER ----- */
                extra_bit_error_flag=false;
                auto extra_check_result =  extra_guess ^ extra_Encode;
                extra_error_bit = extra_check_result.count();
                if(extra_error_bit>0) extra_bit_error_flag = true;
                extra_bit_error_count[it] += static_cast<double>(extra_error_bit);

                /* ----- Determine Superposition BER ----- */
                // superposition_bit_error_flag=false;
                // for(int idx=0;idx<Extra_H.n;idx++){
                //     int sup_pos = punc_map[idx];
                //     int LLR_pos = PayLoad_H.n+Extra_H.n+idx;
                //     bool superposition_guess = totalLLR[LLR_pos]>0?0:1;
                //     if(superposition_guess^transmit_codeword[sup_pos]==1) superposition_error_bit++; 
                // }
                // auto pair_iter = New_Structure_map.begin(); // a^b^c
                // for(int idx=0;idx<Extra_H.n && pair_iter!=New_Structure_map.end();idx++){
                //     int sup_pos = pair_iter->second;
                //     int LLR_pos = PayLoad_H.n+Extra_H.n*3+idx;
                //     bool superposition_guess = totalLLR[LLR_pos]>0?0:1;
                //     if(superposition_guess^transmit_codeword[sup_pos]==1) superposition_error_bit++; 
                // }
                // if(superposition_error_bit>0) superposition_bit_error_flag = true;
                // superposition_bit_error_count[it] += superposition_error_bit;
                it++;
            }
            // 如果 syndorme check is ok ，但是codeword bit 有錯，代表解錯codeword ， BER[it+1:iteration_limit] += codeword length
            if(!payload_error_syndrome && payload_bit_error_flag){
                for(int it_idx=it;it_idx<iteration_limit;it_idx++){
                    payload_bit_error_count[it_idx] += payload_error_bit;
                }
            }
            // 如果 syndorme check is ok ，但是codeword bit 有錯，代表解錯codeword ， BER[it+1:iteration_limit] += codeword length
            if(!extra_error_syndrome && extra_bit_error_flag){
                for(int it_idx=it;it_idx<iteration_limit;it_idx++){
                    extra_bit_error_count[it_idx] += extra_error_bit;
                }
            }
            if(superposition_bit_error_flag){
                for(int it_idx=it;it_idx<iteration_limit;it_idx++){
                    superposition_bit_error_count[it_idx] += superposition_error_bit;
                }
                superposition_frame_error += 1;
            }
            // Frame count
            if(payload_bit_error_flag){
                payload_frame_error+=1;
            }
            if(extra_bit_error_flag){
                extra_frame_error+=1;
            }
            frame_count++;
        }
        clk_end=clock();
        double clk_duration = (double)(clk_end-clk_start)/CLOCKS_PER_SEC;
        printf("PayLoad - SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Time : %.3f | Total frame : %.0f\n",SNR,payload_frame_error/frame_count,payload_bit_error_count[iteration_limit-1]/((double)PayLoad_H.n*frame_count),it,clk_duration,frame_count);
        printf("Extra   - SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Time : %.3f | Error frame : %.0f | BER_num : %.0f\n",SNR,extra_frame_error/frame_count,extra_bit_error_count[iteration_limit-1]/((double)Extra_H.n*frame_count),it,clk_duration,extra_frame_error,extra_bit_error_count[iteration_limit-1]);
        // printf("Sup     - SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Time : %.3f | Error frame : %.0f | BER_num : %.0f\n",SNR,superposition_frame_error/frame_count,superposition_bit_error_count[iteration_limit-1]/((double)Extra_H.n*frame_count),it,clk_duration,superposition_frame_error,superposition_bit_error_count[iteration_limit-1]);
        
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

        // Write Superposition Performance to csv
        // Superposition_outfile << SNR << ", " << superposition_frame_error/frame_count << ", ";
        // for(int i=0;i<iteration_limit;i++){
        //     Superposition_outfile << superposition_bit_error_count[i]/((double)Extra_H.n*frame_count) << ", ";
        // }
        // Superposition_outfile <<endl;
        
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
    // Superposition_outfile.close();
    /* ----------- free all memory ----------- */
    cout << "In main Free \n";
    FreeAllH(&H);
    return 0;
}

void FreeAllH(struct parity_check *H){
    cout << "Free All element with H. \n";
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
        cout << "** Error ----- " << file_name_in << " can't open !! \n" ;
        exit(1);
    }
    
    // struct parity_check *H=(struct parity_check*)malloc(sizeof(parity_check));
    file_in >> H->n >> H->m >> H->max_col_degree >> H->max_row_degree;
    
    cout << "Parity_Check_Matrix size  : " << H->m << " x " << H->n << endl;
    cout << "max_col_degree : " << H->max_col_degree << " | max_row_degree : " << H->max_row_degree << "\n";
    H->max_col_arr=(int*)malloc(sizeof(int)*H->n);
    H->max_row_arr=(int*)malloc(sizeof(int)*H->m);
    // cout << "Each col 1's number : ";
    for(int i=0;i<H->n;i++){
        file_in >> H->max_col_arr[i];
        if(H->max_col_arr[i] > H->max_col_degree){
            cout << "** Error ----- " << file_name_in << " max_col_array is Error , Please Check!!" << "\n";
            goto FreeAll;
        }
        // cout << H->max_col_arr[i] << " ";
    }
    // cout << endl << "Each row 1's number : ";
    for(int i=0;i<H->m;i++){
        file_in >> H->max_row_arr[i];
        if(H->max_row_arr[i] > H->max_row_degree){
            cout << "** Error ----- " << file_name_in << " max_row_array is Error , Please Check!!" << "\n";
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
        cout << "In ReadFile Free All element \n" ;
        FreeAllH(H);
        file_in.close();
        exit(1);
}
vector<int> Read_punc_pos(string file,int extra_nums){
    ifstream punc_file(file);
    vector<int> punc_map;
    // cout <<  "punc pos : " ;
    for(int i=0;i<extra_nums;i++){
        int pos;
        punc_file >> pos;
        // cout << " " << pos << endl;
        punc_map.push_back(pos-1);
    }
    punc_file.close();
    return punc_map;
}

vector<int> Read_Structure_pos(string file,int  extra_nums){
    ifstream Structure_file(file);
    vector<int> Structure_map;
    for(int i=0;i<extra_nums;i++){
        int pos;
        Structure_file >> pos;
        Structure_map.push_back(pos-1);
    }
    Structure_file.close();
    return Structure_map;
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