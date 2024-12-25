# CBP and BP_Model Test

This project implements the LDPC (Low-Density Parity-Check) code for a 10-5 test, utilizing various models and training techniques in the context of error correction. It includes scripts for training, simulation, and processing data related to LDPC codes and BP (belief propagation) algorithms.

## Project Structure

- **`BPMTL_17037_train.pth`**: Pre-trained model weights for training the LDPC code.
- **`CBP.cpp`**: C++ code for the belief propagation algorithm.
- **`DownLoad_weight.py`**: Python script for downloading model weights.
- **`Gaussian_Noise.py`**: Python script for generating Gaussian noise for testing.
- **`H_10_5_G.txt`** and **`H_10_5.txt`**: Matrix files used for LDPC encoding and decoding.
- **`makefile`**: A makefile for building and compiling necessary components.
- **`mutiloss_BP_train.py`**: Python script for training the multi-loss BP model.
- **`random_number_generation.py`**: Python script for generating random numbers (used for simulations).
- **`read_PCM_G.py`**: Python script for reading the PCM G matrix.
- **`simulation.py`**: Python script for running simulations.
- **`sparse_bp_model.py`**: Python script for the sparse BP model implementation.
- **`UseFunction.h`**: Header file for the C++ functions used in the project.

## Requirements

- Python 3.x
- PyTorch
- NumPy
- C++ Compiler (for compiling `CBP.cpp`)
- Make (for using the makefile)
  
Make sure to install the necessary dependencies in Python before running the scripts. You can install them using pip:

```bash
pip install torch 
pip install numpy
```


## 操作說明

1. **根據你的PCM訓練model**：
   運行 `mutiloss_BP_train.py` 來訓練的模型權重。並根據你的`PCM`、`SNR range`、`learning ratio`進行微調。
    ``` python = 
    # 選取你的PCM txt file 
    H_file_name = r"H_10_5.txt"
    ```

2. **運行模擬**：
    使用 `simulation.py` 運行模擬，該模擬將在不同條件下測試 LDPC (PCM) 的 performance。
    ``` python = 
    # 選取預先訓練好的權重
    Load_model_file_name = r'BPMTL_17037_train.pth'
    ```
3. **下載模型參數**:
    運行 `mutiloss_BP_train.py` 來下載模型參數。可丟置 `weight_file` 資料夾裡面。
    ``` python = 
    Load_model_file_name = r"BPMTL_17037_train.pth" # 選取預訓練好的權重
    DownLoad_Weight_Flag = True
    ```

## Conventional BP

1. **編譯 C++ 代碼**：
   如果需要編譯 C++ 代碼，請使用提供的 `makefile` 來編譯或使用 `g++`。
2. **比較performance**：
   比較 `CBP` 和 `model` 的 `BER`、`FER` 效能。

