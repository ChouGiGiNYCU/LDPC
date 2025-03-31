# Puncture LDPC
在 `PEGReg504x1008` PayLoad LDPC Code 上面疊加 `n` 組 `BCH_15_7` Extra BCH Code 做傳輸。把兩個 `PCM` 合併起來形成一個新的 `PCM` 把疊加的位置給 puncture ， 透過 BP 再把兩組 code 還原。

當 `iteration` 小於 `iteration_limit` 且 `PayLoad_syndrome` 非全零的時候，訊息只在`PayLoad`裡面做傳遞。

Puncture method 採用 1-SR 的概念(不是照paper的Alg) 去做 。


Example - 合併後的`PCM`, (n=2) 
```
[Payload_H       0         0       0      0]
[    0        Extra_H      0       0      0]
[    0           0      Extra_H    0      0]
[  Punc1         I         0       I      0]
[  Punc2         0         I       0      I]
```
[paper](https://ieeexplore.ieee.org/document/6398903)

---
## 檔案

- `PCM_P1008_7E15BCH_1SR.txt` : 合併的PCM檔案
- `Pos_PCM_P1008_7E15BCH_1SR.txt` : Puncture 的 VN Node (start idx = 1)

---

## 執行程式
可以使用 `Ubuntu` 執行 `makefile`

Linux:
```
make BP
```
