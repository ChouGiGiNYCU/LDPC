# Puncture LDPC

在 `PEGReg504x1008` LDPC 上選取幾個 VN 做puncture(LLR=0) ， 來看對於performance的影響。

Puncture method 採用 1-SR 的概念(不是照paper的Alg) 去做 。

[paper](https://ieeexplore.ieee.org/document/6398903)

---
## 檔案

- `PEGReg504x1008.txt` : LDPC PCM file
- `Pos_P1008_E10_1SR.txt` : Puncture 的 VN Node (start idx = 1)

---

## 程式碼部分
讀取 `Pos_P1008_E10_1SR.txt` 然後存在 `punc_map` 裡面

* 注意 pos 要減 1 ( C++ start idx = 0 )
``` c++ =
vector<int> Read_punc_pos(string file,int extra_nums){
    ifstream punc_file(file);
    vector<int> punc_map;  
    for(int i=0;i<extra_nums;i++){
        int pos;
        punc_file >> pos;
        punc_map.push_back(pos-1);
        
    }
    punc_file.close();
    return punc_map;
}
```
做完 `receive_LLR` 的設定後，在對 `punc_pos` 的 `VN Node` 的 `LLR` 設定為 0。
``` c++ = 
// 判斷是否用encode跑測試
if(Encode_Flag){
      for(int i=0;i<H.n-H.m;i++){
         information_bit[i] = random_generation()>0.5?1:0;
      }
      result = Vector_Dot_Matrix_Int(information_bit,G);
}
// 設定傳送的codeword
for(int i=0;i<H.n;i++){
      if(Encode_Flag) transmit_codeword[i] = result[i]%2==0?0:1; // Encode CodeWord
      else transmit_codeword[i] = 0; // all zero
}

// modulation + noise effect
for(int i=0;i<H.n;i++){
      // BPSK -> {0->1}、{1->-1}
      // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
      double receiver_codeword=(-2*transmit_codeword[i]+1) + sigma*gasdev(); // fixed power
      receiver_LLR[i]=2*receiver_codeword/pow(sigma,2); 
}

// do punc
for(int i=0;i<punc_map.size();i++){
      int pos = punc_map[i];
      receiver_LLR[pos] = 0;
}
```

## 執行程式
可以用cmd 執行或使用 `Ubuntu` 執行 `makefile`

Linux:
```
make BP
```

Cmd : 
``` bash =
g++  CBP_punc.cpp -o  CBP_punc

CBP_punc.exe PEGReg504x1008.txt  CBP_PEG504X1008.csv PEGReg504x1008_G.txt  0 200 1 4 0.2  Pos_P1008_E10_1SR.txt
```