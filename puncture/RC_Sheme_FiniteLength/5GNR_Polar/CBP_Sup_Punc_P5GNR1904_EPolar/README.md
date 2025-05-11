# Puncture LDPC
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `H_10_5` Extra LDPC Code 做傳輸。把兩個 `PCM` 合併起來形成一個新的 `PCM` 把疊加的位置給 puncture ， 透過 BP 再把兩組 code 還原。

Puncture method 採用 1-SR 的概念(不是照paper的Alg) 去做 。

[paper](https://ieeexplore.ieee.org/document/6398903)

---
## 檔案

- `PEGReg504x1008.txt` : PayLoad LDPC PCM file
- `H_10_5.txt` : Extra LDPC PCM file
- `PCM_P1008_E10_1SR.txt` : 合併的PCM檔案
- `Pos_PCM_P1008_E10_1SR.txt` : Puncture 的 VN Node (start idx = 1)

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

## 程式碼部分
判斷現在測試是用 Encode 還是 Zero 來測試 
``` c++ =
vector<int> payload_Encode(PayLoad_H.n,0);
vector<int> extra_Encode(Extra_H.n,0);
// 處理 Payload CodeWord 是zero 還是Encode
if(PayLoad_Flag){
      for(int i=0;i<PayLoad_G.size();i++){
            payload_info[i] = random_generation()>0.5?1:0;
      }
      payload_Encode = Vector_Dot_Matrix_Int(payload_info,PayLoad_G);
}
// 處理 Extra CodeWord 是zero 還是Encode
if(Extra_Flag){
      for(int i=0;i<Extra_G.size();i++){
            extra_info[i] = random_generation()>0.5?1:0;
      }
      extra_Encode = Vector_Dot_Matrix_Int(extra_info,Extra_G);
}
// PayLoad xor Extra
for(int i=0;i<PayLoad_H.n;i++){
      if(PayLoad_Flag) transmit_codeword[i] = payload_Encode[i]%2;
      else transmit_codeword[i] = 0;
}
for(int i=0;i<Extra_H.n;i++){
      int xor_pos = punc_map[i];
      if(Extra_Flag) 
            transmit_codeword[xor_pos] = (transmit_codeword[xor_pos] + (extra_Encode[i]%2))%2; 
}
```

計算接收到的 `LLR`，最後在把 puncture的位置設定為0(疊加位置為零)

receiver_LLR = [ PayLoad(0、LLR),Extra(0),superposition(LLR) ] 
``` c++ = 
// Count LLR
for(int i=0;i<PayLoad_H.n;i++){
      // Sum_Product_Algorithm 是 soft decision 所以在調變的時候解
      // BPSK -> {0->1}、{1->-1}
      // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
      double receive_value=(double)(-2*transmit_codeword[i]+1) + sigma*gasdev(); // fixed power
      receiver_LLR[i]=2*receive_value/pow(sigma,2); 
}

// setting puncture bits
for(int i=PayLoad_H.n;i<H.n;i++){
      // Extra bit be punctured
      if(i>=PayLoad_H.n && i<(Extra_H.n+PayLoad_H.n)){ // 1008 ~1017
            receiver_LLR[i] = 0;
      }
      else if(i>=(Extra_H.n+PayLoad_H.n)){ // 1018 ~1027 (疊加的部分)
            int idx = i-(Extra_H.n+PayLoad_H.n);
            int origin_vn_pos = punc_map[idx];
            receiver_LLR[i] = receiver_LLR[origin_vn_pos];   
            receiver_LLR[origin_vn_pos] = 0; // be punctured 
      }
}
```

## 執行程式
可以使用 `Ubuntu` 執行 `makefile`

Linux:
```
make BP
```
