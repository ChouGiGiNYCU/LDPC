#ifndef FreeRide_SDD_func_H  
#define FreeRide_SDD_func_H  
#include<vector>
#include<cmath>
#include<float.h>
#include "UseFuction.h"
#include "PCM.h"
using namespace std;
vector<int> interleaver(const vector<int>& CodeWord, int div) {
    int n = CodeWord.size();
    vector<int> result(n, -1);
    
    int i = 0; // 用於填充 result 的索引
    for (int start_pos = 0; start_pos < div; start_pos++) {
        for (int idx = start_pos; idx < n; idx += div) {
            result[i++] = CodeWord[idx]; 
        }
    }
    return result;
}
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


vector<int> FindLargeLLR(vector<vector<bool>>& G,const struct parity_check& PayLoad_H,const vector<double>& receive_LLR,const int& info_length,const int& InterLeaver_col){
    int info_bits = info_length; // rowsize
    int AllPossible = static_cast<int>(pow(2,info_bits));
    vector<double> receive_LLR_copy = receive_LLR;
    vector<int> Extra_Encode_copy;
    double Large_LLR_sum = DBL_MIN; // double min
    // 找出所有Extra CodeWord 所有組合，且疊加後，去找出最佳的check equations的LLR
    for(int k=0;k<AllPossible;k++){
        // copy the recieve_LLR
        receive_LLR_copy = receive_LLR;
        // find all possible encode codeword
        vector<bool> bin = intToBinaryVector(k,info_bits);
        for(int i=bin.size();i<G.size();i++) bin.push_back(false);

        vector<int> Extra_Encode = Vector_Dot_Matrix_Int(bin,G);
        for(int i=0;i<Extra_Encode.size();i++) Extra_Encode[i] = Extra_Encode[i]%2;
        vector<int> interleaver_result = interleaver(Extra_Encode,InterLeaver_col);
        
        // do LLR count 
        for(int VN=0;VN<receive_LLR_copy.size();VN++){
            receive_LLR_copy[VN] = pow(-1,interleaver_result[VN]) * receive_LLR[VN];
        } 
        // count all check equations sum
        double total_LLR_sum = 0;
        /* ------- count check equations LLR ------- */
        for(int CN=0;CN<PayLoad_H.m;CN++){
            double total_LLR = 1;
            for(int i=0;i<PayLoad_H.max_row_arr[CN];i++){
                int VN = PayLoad_H.VN_2_CN_pos[CN][i];
                total_LLR *= tanh(0.5*(receive_LLR_copy[VN])); 
            }
            total_LLR = 2*atanh(total_LLR);
            total_LLR_sum += total_LLR; 
        }

        // record large LLR_sum 
        if(Large_LLR_sum<total_LLR_sum){
            Large_LLR_sum = total_LLR_sum;
            Extra_Encode_copy = interleaver_result;
        }
        
    }
    
    return Extra_Encode_copy;
}


#endif