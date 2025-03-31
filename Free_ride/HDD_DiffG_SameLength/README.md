# FreeRide-LDPC - HDD
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `H_10_5` Extra LDPC Code 做傳輸。

接收端先做 `Hard-Decision`，再把所有`Extra CodeWord` 的所有可能疊加在上面，最後再把疊加後的結果乘上 `PayLoad PCM`，結果 `parity check sum` 符合最多的代表 `Extra CodeWord` 可能是猜對的，再把 `Extra` 的 `Interference` 給去除掉，最後透過 `BP` 把 `PayLoad` 給解回來。

`Extra` 採用 6 bits(對比H5x10)，並且也使用 `504x1008` 的 `G` 矩陣(跟`Payload`不同)，剩餘其他 `info_bits` 為 0。
----
### Algrithm
![SDD Algorithm](https://github.com/ChouGiGiNYCU/LDPC/tree/main/Free_ride/HDD/img/HDD_Alg.png)


Reference : [Free-Ride paper](https://ieeexplore.ieee.org/document/9584875)

---

## 程式碼部分
`HDD` 部分，把每一種 `Extra Codeword` 的影響去除掉後，去檢查 `Hamming Weight`(1的數量)大小，最小的就是 `Extra CodeWord`。
``` c++ =
vector<int> MinHammingWeight(vector<vector<bool>>& G,const struct parity_check& PayLoad_H, vector<int>& Sup_Encode,vector<int>& sup_pos){
    int info_bits = G.size(); // rowsize
    int min_weight = 1e9;
    int AllPossible = static_cast<int>(pow(2,info_bits));
    vector<int> PayLoad_Encode = Sup_Encode;
    vector<int> Extra_Encode_copy;
    // 找出所有Extra CodeWord 所有組合，且疊加相乘後 Hamming weight 最小的 (代表 minimun number of unsatisfied parity checks)
    for(int i=0;i<AllPossible;i++){
        vector<bool> bin = intToBinaryVector(i,info_bits);
        vector<int> Extra_Encode = Vector_Dot_Matrix_Int(bin,G);

        
        for(int i=0;i<Extra_Encode.size();i++) Extra_Encode[i] = Extra_Encode[i]%2; 
        for(int i=0;i<Extra_Encode.size();i++){
            PayLoad_Encode[i] = (Sup_Encode[sup_pos[i]] + Extra_Encode[i])%2;
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
```