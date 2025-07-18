#ifndef BP_FUNCTION
#define BP_FUNCTION
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "UseFuction.h"

using namespace std;

double phi(double x){
    double yy = tanh(x / 2);
    if (fabs(yy) < 1e-14) 
        yy = 1e-14;
    return -log(yy) + 1e-14;
}

void Process_info_bits(boost::dynamic_bitset<>& info_bits,bool Encode_Flag){
    if(Encode_Flag){
        for(int i=0;i<info_bits.size();i++){
            info_bits[i] = random_generation()>0.5?1:0;
        }
    }else{
        info_bits.reset();
    }
}

void Encode_Process(vector<boost::dynamic_bitset<>>& Transpose_G,bool Encode_Flag,boost::dynamic_bitset<>& CodeWord,const boost::dynamic_bitset<>& info_bits){
    if(Encode_Flag){ // 處理 Payload CodeWord 是zero 還是Encode
        GF2_Mat_Vec_Dot(info_bits,Transpose_G,CodeWord);
    }else{
        CodeWord.reset();
    }
    return;
}

void Count_LLR(struct parity_check H,const boost::dynamic_bitset<>& CodeWord,double sigma,vector<double>& LLR){
    for(int i=0;i<H.n;i++){ // Count LLR
        // BPSK -> {0->1}、{1->-1}
        // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
        double receive_value=(double)(-2*CodeWord[i]+1) + sigma*gasdev(); // fixed power
        LLR[i]=2*receive_value/pow(sigma,2); 
    }
}

bool BP_for_Payload(struct parity_check H, double SNR, int iteration_limit,const vector<double> receiver_LLR,const boost::dynamic_bitset<>& CodeWord){

    double ** CN_2_VN_LLR = (double**)malloc(sizeof(double*)*H.n);
    for(int i=0;i<H.n;i++) CN_2_VN_LLR[i]=(double*)calloc(H.max_col_arr[i],sizeof(double));
    double ** VN_2_CN_LLR = (double**)malloc(sizeof(double*)*H.m);
    for(int i=0;i<H.m;i++) VN_2_CN_LLR[i]=(double*)calloc(H.max_row_arr[i],sizeof(double));
    double *Guess_CodeWord = (double*)malloc(sizeof(double)*H.n);
    double *totalLLR = (double*)malloc(H.n*sizeof(double));
    vector<int> payload_Encode(H.n,0); 
    int it=0;
    bool error_syndrome = true;
    for(int i=0;i<H.n;i++) totalLLR[i]=0;
    for(int i=0;i<H.n;i++) for(int j=0;j<H.max_col_arr[i];j++) CN_2_VN_LLR[i][j] = 0;
    for(int i=0;i<H.m;i++) for(int j=0;j<H.max_row_arr[i];j++) VN_2_CN_LLR[i][j] = 0;
    
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
                        double LLR = receiver_LLR[other_VN];
                        phi_beta_sum += phi(abs(LLR));
                        if(LLR==0) alpha *= random_generation()>0.5?1:-1; // random 
                        else alpha *= (LLR>0)?1:-1;
                        
                    }else{
                        double LLR = VN_2_CN_LLR[CN][j];
                        phi_beta_sum += phi(abs(LLR));
                        if(LLR==0) alpha *= random_generation()>0.5?1:-1; // random 
                        else alpha *=(LLR>0)?1:-1;
                        
                    }
                }
                
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
        for(int VN=0;VN<H.n;VN++){
            Guess_CodeWord[VN] = totalLLR[VN]>0?0:1;
        }
            
        /* Determine if it is a syndrome -> guess * H^T = vector(0) */
        error_syndrome = false;
        for(int CN=0;CN<H.m;CN++){
            int count=0;
            for(int i=0;i<H.max_row_arr[CN];i++){
                int VN = H.VN_2_CN_pos[CN][i];
                count+=Guess_CodeWord[VN];
            }
            if(count%2==1){
                error_syndrome = true;
                break;
            }
            error_syndrome = false;
        }
        it++;
    }
    for(int i=0;i<H.n;i++){
        if(Guess_CodeWord[i]!=CodeWord[i]){
            error_syndrome = true;
            break;
        }
    }
    for(int i=0;i<H.n;i++) free(CN_2_VN_LLR[i]);
    free(CN_2_VN_LLR);
    for(int i=0;i<H.m;i++) free(VN_2_CN_LLR[i]);
    free(VN_2_CN_LLR);
    free(Guess_CodeWord);
    
    return error_syndrome==true?false:true;
}


void BP_for_CombineH(struct parity_check H,struct parity_check PayLoad_H,struct parity_check Extra_H, int iteration_limit, int iteration_open,const vector<double> receiver_LLR,const boost::dynamic_bitset<>& Payload_CodeWord,bool& decode_payload_flag,const boost::dynamic_bitset<>& Extra_CodeWord,bool& decode_extra_flag){

    double ** CN_2_VN_LLR = (double**)malloc(sizeof(double*)*H.n);
    for(int i=0;i<H.n;i++) CN_2_VN_LLR[i]=(double*)calloc(H.max_col_arr[i],sizeof(double));
    double ** VN_2_CN_LLR = (double**)malloc(sizeof(double*)*H.m);
    for(int i=0;i<H.m;i++) VN_2_CN_LLR[i]=(double*)calloc(H.max_row_arr[i],sizeof(double));
    double *totalLLR = (double*)malloc(H.n*sizeof(double));
    int *Payload_Guess_CodeWord = (int*)malloc(sizeof(int)*PayLoad_H.n);
    int *Extra_Guess_CodeWord = (int*)malloc(sizeof(int)*Extra_H.n);
    
    bool payload_correct_flag = false; // 如果 payload syndrome 是全零，代表 codeword 是個合法的，就開通往extra 傳送 message
    int it=0;
    bool payload_error_syndrome = true,extra_error_syndrome=true;

    for(int i=0;i<H.n;i++) totalLLR[i]=0;
    for(int i=0;i<H.n;i++) for(int j=0;j<H.max_col_arr[i];j++) CN_2_VN_LLR[i][j] = 0;
    for(int i=0;i<H.m;i++) for(int j=0;j<H.max_row_arr[i];j++) VN_2_CN_LLR[i][j] = 0;
    
    while(it<iteration_limit && (payload_error_syndrome || extra_error_syndrome)){
        /* ------- CN update ------- */
        for(int VN=0;VN<H.n;VN++){
            if(VN>=PayLoad_H.n && VN<(Extra_H.n+PayLoad_H.n) && (it<iteration_open && payload_correct_flag==false)) continue;
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
                        if(LLR==0) alpha *= random_generation()>0.5?1:-1; // random 
                        else alpha *= (LLR>0)?1:-1;
                        
                    }else{
                        double LLR = VN_2_CN_LLR[CN][j];
                        phi_beta_sum += phi(abs(LLR));
                        if(LLR==0) alpha *= random_generation()>0.5?1:-1; // random 
                        else alpha *=(LLR>0)?1:-1;
                        
                    }
                }
                
                CN_2_VN_LLR[VN][i]= alpha * phi(phi_beta_sum);
            }
        }
        /* ------- VN update ------- */
        for(int CN=0;CN<H.m;CN++){
            for(int i=0;i<H.max_row_arr[CN];i++){
                int VN=H.VN_2_CN_pos[CN][i];
                // 抑制訊息往上傳(VN->CN)
                if(VN>=PayLoad_H.n && VN<(Extra_H.n+PayLoad_H.n) && (it<iteration_open && payload_correct_flag==false)) continue;
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
            Payload_Guess_CodeWord[VN] = totalLLR[VN]>0?0:1;
        } 
        /* Assgin LLR to Extra & guess CodeWord */
        for(int VN=PayLoad_H.n;VN<(PayLoad_H.n+Extra_H.n);VN++){
            Extra_Guess_CodeWord[VN-PayLoad_H.n] = totalLLR[VN]>0?0:1;
        } 
            
        /* Determine if it is a syndrome -> guess * H^T = vector(0) */
        payload_error_syndrome = false;
        for(int CN=0;CN<PayLoad_H.m;CN++){
            int count=0;
            for(int i=0;i<PayLoad_H.max_row_arr[CN];i++){
                int VN = PayLoad_H.VN_2_CN_pos[CN][i];
                count+=Payload_Guess_CodeWord[VN];
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
                count+=Extra_Guess_CodeWord[VN];
            }
            if(count%2==1){
                extra_error_syndrome = true;
                break;
            }
            extra_error_syndrome = false;
        }
        if(payload_correct_flag==false && it<iteration_open) extra_error_syndrome = true;
        if(payload_error_syndrome==false) payload_correct_flag = true;
        it++;
    }
    for(int vn=0;vn<PayLoad_H.n;vn++){
        if(Payload_CodeWord[vn]!=Payload_Guess_CodeWord[vn]){
            decode_payload_flag = false;
            break;
        }
    }
    for(int vn=0;vn<Extra_H.n;vn++){
        if(Extra_CodeWord[vn]!=Extra_Guess_CodeWord[vn]){
            decode_extra_flag = false;
            break;
        }
    }

    for(int i=0;i<H.n;i++) free(CN_2_VN_LLR[i]);
    free(CN_2_VN_LLR);
    for(int i=0;i<H.m;i++) free(VN_2_CN_LLR[i]);
    free(VN_2_CN_LLR);
    
    return;
}


#endif