# Puncture LDPC
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `H_96_48` Extra LDPC Code 做傳輸。把兩個 `PCM` 合併起來形成一個新的 `PCM` 把疊加的位置給 puncture ， 透過 BP 再把兩組 code 還原。

當 `iteration` 小於 `iteration_limit` 且 `PayLoad_syndrome` 非全零的時候，訊息只在`PayLoad`裡面做傳遞。

Puncture method 採用 `paper` 的方式去做。

[paper](https://ieeexplore.ieee.org/document/6365873)

---
## 檔案

- `PCM_Punc_newAlg_Punc96.txt` : 合併的PCM檔案
- `Pos_PCM_Punc_newAlg_Punc96.txt` : Puncture 的 VN Node (start idx = 1)

---
## 問題
由於 `Extra codeword` 全靠外部訊息來判斷 `codeword`  ， 所以容易判斷成非傳送端傳送的 `codeword`，在做 `Early Stop Criteria` (乘上PCM等於零向量)，很容易提提早就結束。導致了`BER`在最後一次的 `iteration` 的錯誤變很少。 

另一方面解釋可以說 `SNR`低的時候，`codeword` 之間的 `dmin` 很小，很容易誤判。

- 解決方式 : 就不要用 `Early Stop Criteria` ，讓解出來的 `codeword` 直接跟傳送的 `codeword` 直接做比對。
---
## Code Part
因為`Puncture node` 的初始LLR值為零，所以`sign` 給一個random 的正負號。

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
