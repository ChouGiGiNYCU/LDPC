# Puncture LDPC
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `H_96_48` Extra LDPC Code 做傳輸。把兩個 `PCM` 合併起來形成一個新的 `PCM` 把疊加的位置給 puncture ，且在`Payload`和`Extra`新增一些連結，讓`Payload`和`Extra`不再像是兩個分開的子圖，而是整合成一個大的`Code`，大的`Code`中一部分是`Payload`/一部分是`Extra`，`Decoding`一樣是透過`BP`。

當 `iteration` 小於 `iteration_limit` 且 `PayLoad_syndrome` 非全零的時候，訊息只在`PayLoad`裡面做傳遞。

Puncture method 採用 1-SR 的概念(不是照paper的Alg) 去做 。

[paper](https://ieeexplore.ieee.org/document/6398903)

---
## 檔案

- `PCM_P1008_E96_Structure_Partial_Combine_50percent.txt` : 合併的`PCM`檔案
- `Table_Superposition_Extra_Payload_50percent.csv` : `Puncture` 的 `VN Node(Superposition)` (start idx = 1)
- `Punc_Superpostion_new_Payload_Structure_Partial_Combine_50percent.csv` : 新的結構中，所連接到的 `Payload bits` (start idx = 1)
- `Table_ExtraTransmitVNs_to_PuncPosPayload_50percent.csv` : 傳送 `Extra bits`，把一些 `Payload bits` 給 `puncture` 掉。

---
## Code Part
`Codeword process`，先處理`Payload CodeWord` -> `Payload CodeWord` ^ `Extra CodeWord` (`Superposition`) -> 新結構的處理
``` c++ =
/*------------ CodeWord process ------------*/
// PayLoad 
for(int i=0;i<PayLoad_H.n;i++){
    transmit_codeword[i] = payload_Encode[i];
}
// PayLoad xor Extra with orgin method
for(int i=0;i<Extra_H.n;i++){
    int punc_xor_pos  = punc_map[i];
    transmit_codeword[punc_xor_pos] = transmit_codeword[punc_xor_pos] ^ extra_Encode[i]; 
}
// Payload xor Extra with NewStructure (Extra ^ P1 ^ P2)
int Extra_vn = 0;
for(auto& pos:New_Structure_map){
    int payload_pos1  = pos.first;
    int payload_pos2  = pos.second;
    transmit_codeword[payload_pos2] = transmit_codeword[payload_pos2] ^ payload_Encode[payload_pos1] ^ extra_Encode[Extra_vn];
    Extra_vn++;
}
```

因為`Puncture node` 的初始LLR值為零，所以`sign` 給一個random 的正負號。
``` c++ =
if(LLR==0) alpha *= random_generation()?1:-1; // random 
```

當條件不成立的時候 訊息不會傳送到 `Extra` 。
``` c++ = 
for(int CN=0;CN<H.m;CN++){
    for(int i=0;i<H.max_row_arr[CN];i++){
        int VN=H.VN_2_CN_pos[CN][i];
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
/* ------- VN update ------- */
for(int CN=0;CN<H.m;CN++){
    for(int i=0;i<H.max_row_arr[CN];i++){
        int VN=H.VN_2_CN_pos[CN][i];
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
```

`Payload` 解對的時候 ， 開始把訊息傳送到`Extra` 。
``` c++ =
if(it<iteration_limit && payload_error_syndrome==false) extra_error_syndrome = true;
```
## 執行程式
可以使用 `Ubuntu` 執行 `makefile`

Linux:
```
make BP
```
