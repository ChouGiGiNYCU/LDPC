# Puncture LDPC
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `H_96_48` Extra LDPC Code 做傳輸。把兩個 `PCM` 合併起來形成一個新的 `PCM` 把疊加的位置給 puncture ， 透過 BP 再把兩組 code 還原。

新的部分:
1 - 部分的 `Extra bits` 傳送出去，相對的把部分的 `Payload bits` 不傳。
2 - 剩下的 `Extra bits` 和 `Payload bits` 疊加在一起去傳送。

當 `iteration` 小於 `iteration_limit` 且 `PayLoad_syndrome` 非全零的時候，訊息只在`PayLoad`裡面做傳遞。

Puncture method 採用 1-SR 的概念(不是照paper的Alg) 去做 。

[paper](https://ieeexplore.ieee.org/document/6398903)

---
## 檔案

- `PCM_P1008_E96_PartialExtraTransmit_1SR_22_Random.txt` : 合併的PCM檔案
- `Table_Superposition_Extra_Payload_22_Random.csv` :  剩下的 `Extra bits` 和 `Payload bits` 疊加在一起去傳送的VNs (start idx = 1)
- `Table_ExtraTransmitVNs_to_PuncPosPayload_22_Random.csv` : 部分的 `Extra bits` 傳送出去，相對的把部分的 `Payload bits` 不傳的對照表 (start idx = 1)
---
## Code Part
因為`Puncture node` 的初始LLR值為零，所以`sign` 給一個random 的正負號。
``` c++ = 
if(LLR==0) alpha *= random_generation()>0.5?1:-1; // random 
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
