# Puncture LDPC
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `H_10_5` Extra LDPC Code 做傳輸。把兩個 `PCM` 合併起來形成一個新的 `PCM` 把疊加的位置給 puncture ， 透過 BP 再把兩組 code 還原。

Puncture method 採用 k-SR ， 彩是希望有一個worse case 做比較的。

---
## 檔案

- `PEGReg504x1008.txt` : PayLoad LDPC PCM file
- `H_10_5.txt` : Extra LDPC PCM file
- `PCM_P1008_E10_kSR.txt` : 合併的PCM檔案
- `Pos_PCM_P1008_E10_kSR.txt` : Puncture 的 VN Node (start idx = 1)

---

## 執行程式
可以使用 `Ubuntu` 執行 `makefile`

Linux:
```
make BP
```
