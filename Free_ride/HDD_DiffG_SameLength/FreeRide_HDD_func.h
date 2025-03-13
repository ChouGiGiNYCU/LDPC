#ifndef FreeRide_func_H  
#define FreeRide_func_H  
#include<vector>
#include<cmath>
#include "UseFuction.h"
#include "PCM.h"
using namespace std;

vector<int> HardDecision(vector<double>& receive_LLR){
    vector<int> result(receive_LLR.size(),0);
    for(int i=0;i<receive_LLR.size();i++){
        result[i] = receive_LLR[i]>=0?0:1;
    }
    return result;
}
// int 轉成binary
vector<bool> intToBinaryVector(int& value,const int& bit_length) {
    vector<bool> binary;
    for (int i = bit_length - 1; i >= 0; i--) {
        int bit = (value >> i) & 1;   // 取每一個bit
        binary.push_back(bit);
    }
    return binary;
}


vector<int> MinHammingWeight(vector<vector<bool>>& G,const struct parity_check& PayLoad_H, vector<int>& HardDecision_result,const int& extra_info_bits){

    int info_bits = extra_info_bits;
    int min_weight = 1e9;
    int AllPossible = static_cast<int>(pow(2,info_bits));
    vector<int> PayLoad_Encode = HardDecision_result;
    vector<int> Extra_Encode_copy;
    
    // 找出所有Extra CodeWord 所有組合，且疊加相乘後 Hamming weight 最小的 (代表 minimun number of unsatisfied parity checks)
    for(int k=0;k<AllPossible;k++){
        vector<bool> bin = intToBinaryVector(k,info_bits);
        int size = bin.size();
        // info 不足的地方補零
        for(int i=size;i<G.size();i++) bin.push_back(false);
        
        // 產生EncdoeCodeWord
        vector<int> Extra_Encode = Vector_Dot_Matrix_Int(bin,G);
        for(int i=0;i<Extra_Encode.size();i++) Extra_Encode[i] = Extra_Encode[i]%2; 

        // 還原可能的 PayLoad CodeWord
        for(int i=0;i<Extra_Encode.size();i++){
            PayLoad_Encode[i] = (HardDecision_result[i] + Extra_Encode[i])%2;
        }
        // PayLoad_Encode * H
        int unsatisfied_parity_checks = 0;
        for(int CN=0;CN<PayLoad_H.m;CN++){
            int count=0;
            for(int i=0;i<PayLoad_H.max_row_arr[CN];i++){
                int VN = PayLoad_H.VN_2_CN_pos[CN][i];
                count+=PayLoad_Encode[VN];
            }
            if(count%2!=0){
                unsatisfied_parity_checks += 1;
            }
        }
        if(min_weight>unsatisfied_parity_checks){
            // printf("min_weight : %d | unsatisfied_parity_checks : %d | i : %d\n",min_weight,unsatisfied_parity_checks,i);
            min_weight = unsatisfied_parity_checks;
            Extra_Encode_copy = Extra_Encode;
        }
    }
    return Extra_Encode_copy;
}


#endif