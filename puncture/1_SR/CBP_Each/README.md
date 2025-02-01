# Puncture LDPC

直接對 `PEGReg504x1008` 和 `H_10_5` 做傳統 BP 看效能

---
## 檔案

- `PEGReg504x1008.txt` : LDPC PCM file
- `H_10_5.txt` : LDPC PCM file

---


## 執行程式
可以用cmd 執行或使用 `Ubuntu` 執行 `makefile`

Linux:
```
// 執行 H_10_5 
make BP_E 

// 執行 PEG_504X1008 
make BP_P
```

Cmd : 
``` bash =
g++  CBP.cpp -o  CBP
// 執行 H_10_5
CBP.exe H_10_5.txt CBP_H5x10.csv  None 0 200 1 4 0.2 

// 執行 PEG_504X1008 
CBP.exe PEGReg504x1008.txt CBP_PEGReg504x1008.csv  None 0 200 1 4 0.2 
```