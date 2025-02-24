# Puncture LDPC
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `H_96_48` Extra LDPC Code 做傳輸。把兩個 `PCM` 合併起來形成一個新的 `PCM` 把疊加的位置給 puncture ， 透過 BP 再把兩組 code 還原。

Puncture method 採用 1-SR 的概念(不是照paper的Alg) 去做 。

[paper](https://ieeexplore.ieee.org/document/6398903)

---
## 檔案

- `PCM_P1008_E96_1SR.txt` : 合併的PCM檔案
- `Pos_PCM_P1008_E96_1SR.txt` : Puncture 的 VN Node (start idx = 1)

---
## 問題
由於 `Extra codeword` 全靠外部訊息來判斷 `codeword`  ， 所以容易判斷成非傳送端傳送的 `codeword`，在做 `Early Stop Criteria` (乘上PCM等於零向量)，很容易提提早就結束。導致了`BER`在最後一次的 `iteration` 的錯誤變很少。 

另一方面解釋可以說 `SNR`低的時候，`codeword` 之間的 `dmin` 很小，很容易誤判。

- 解決方式 : 就不要用 `Early Stop Criteria` ，讓解出來的 `codeword` 直接跟傳送的 `codeword` 直接做比對。

``` c++ = 
extra_bit_error_flag=false;
extra_error_syndrome = false;
for(int VN=0;VN<Extra_H.n;VN++){
      if(extra_guess[VN]!=extra_Encode[VN]){
      extra_bit_error_count[it]++;
      extra_bit_error_flag=true;
      extra_error_syndrome = true;
      }
}
```
---


## 執行程式
可以使用 `Ubuntu` 執行 `makefile`

Linux:
```
make BP
```
