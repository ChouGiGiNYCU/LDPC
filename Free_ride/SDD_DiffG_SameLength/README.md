# FreeRide-LDPC - SDD
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `H_10_5` Extra LDPC Code 做傳輸。透過 `Soft-Decision Decoding` 的方式先解出 `Extra`，再把 `Extra` 的 `Interference` 給去除掉，最後透過 `BP` 把 `PayLoad` 給解回來。

`Extra` 採用 6 bits(對比H5x10)，且使用 `PayLoad` 的 `G`，來做 `Encode`，代表 6 bits 之後的 bits 全零。

----
### Algrithm
![SDD Algorithm](https://github.com/ChouGiGiNYCU/LDPC/tree/main/Free_ride/SDD/img/SDD_Alg.png)
#### Equation 43 - 45
![SDD Algorithm](https://github.com/ChouGiGiNYCU/LDPC/tree/main/Free_ride/SDD/img/SDD_equation.png)

Reference : [Free-Ride paper](https://ieeexplore.ieee.org/document/9584875)

---

## 程式碼部分
`SDD` 部分，把每一種 `Extra Codeword` 的影響去除掉後，去檢查 `check sum` 的 `LLR` 值大小，最大的存下來。
``` c++ =
vector<int> FindLargeLLR(vector<vector<bool>>& G,const struct parity_check& PayLoad_H,const vector<double>& receive_LLR,vector<int>& sup_pos){
    int info_bits = G.size(); // rowsize
    int AllPossible = static_cast<int>(pow(2,info_bits));
    vector<double> receive_LLR_copy = receive_LLR;
    vector<int> Extra_Encode_copy;
    double Large_LLR_sum = DBL_MIN; // double max
    // 找出所有Extra CodeWord 所有組合，且疊加後，去找出最佳的check equations的LLR
    for(int i=0;i<AllPossible;i++){
        // copy the recieve_LLR
        receive_LLR_copy = receive_LLR;
        // find all possible encode codeword
        vector<bool> bin = intToBinaryVector(i,info_bits);
        vector<int> Extra_Encode = Vector_Dot_Matrix_Int(bin,G);
        for(int i=0;i<Extra_Encode.size();i++) Extra_Encode[i] = Extra_Encode[i]%2;
        // do LLR count 
        for(int i=0;i<sup_pos.size();i++){
            int xor_pos =  sup_pos[i];
            receive_LLR_copy[xor_pos] = pow(-1,Extra_Encode[i]) * receive_LLR[xor_pos];
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
            Extra_Encode_copy = Extra_Encode;
        }
        
    }
    return Extra_Encode_copy;
}
```