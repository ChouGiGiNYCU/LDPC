# 在 C++ 中運行 BP_Model 訓練好的模型

在 C++ 中加載並執行使用 PyTorch 訓練的模型權重。

## 操作說明

1. **對好layer權重檔案路徑**：
   運行 `Model_C_Implement.cpp` 來讀取的模型權重。並根據你的`iteration`來做調整。
    ``` c++ = 
    // it=5 (mutiloss_BP_train.py)
    string fc0_weight = "fc0.weight.txt",fc0_bias = ""; // CN 
    string fc1_weight = "fc1.weight.txt",fc1_bias = "VnBias_bias_scaling_vectors.0.txt"; // VN
    string fc2_weight = "fc2.weight.txt",fc2_bias = ""; // CN
    string fc3_weight = "fc3.weight.txt",fc3_bias = "VnBias_bias_scaling_vectors.1.txt"; // VN
    string fc4_weight = "fc4.weight.txt",fc4_bias = ""; // CN
    string fc5_weight = "fc5.weight.txt",fc5_bias = "VnBias_bias_scaling_vectors.2.txt"; // VN
    string fc6_weight = "fc6.weight.txt",fc6_bias = ""; // CN
    string fc7_weight = "fc7.weight.txt",fc7_bias = "VnBias_bias_scaling_vectors.3.txt"; // VN
    string fc8_weight = "fc8.weight.txt",fc8_bias = ""; // CN
    string fc9_weight = "fc9.weight.txt",fc9_bias = "VnBias_bias_scaling_vectors.4.txt"; // VN

    CNUpdateLayer fc0(input_size,hidden_layer_size,"CN0",fc0_weight,fc0_bias);
    VNUpdateLayer fc1(hidden_layer_size,hidden_layer_size,input_size,"VN0",fc1_weight,fc1_bias);
    CNUpdateLayer fc2(hidden_layer_size,hidden_layer_size,"CN1",fc2_weight,fc2_bias);
    VNUpdateLayer fc3(hidden_layer_size,hidden_layer_size,input_size,"VN1",fc3_weight,fc3_bias);
    CNUpdateLayer fc4(hidden_layer_size,hidden_layer_size,"CN2",fc4_weight,fc4_bias);
    VNUpdateLayer fc5(hidden_layer_size,hidden_layer_size,input_size,"VN2",fc5_weight,fc5_bias);
    CNUpdateLayer fc6(hidden_layer_size,hidden_layer_size,"CN3",fc6_weight,fc6_bias);
    VNUpdateLayer fc7(hidden_layer_size,hidden_layer_size,input_size,"VN3",fc7_weight,fc7_bias);
    CNUpdateLayer fc8(hidden_layer_size,hidden_layer_size,"CN4",fc8_weight,fc8_bias);
    VNUpdateLayer fc9(hidden_layer_size,output_size,input_size,"VN4",fc9_weight,fc9_bias,true); // final layer
    ```

2. **手動檢驗**：
    根據權重檔案來測試來測試`Model_C_Implement.cpp`是否跟.py 檔案 model 的輸出是否一致(跟`simulation.py`隨機帶一個輸入去測試)。如果一致即可使用c++來跑(沒GPU推薦)。
    ``` c++ = 
    vector<float> Channel_LLR(input_size,1); // 用全1測試
    vector<float> output;
    vector<float> input(Channel_LLR.begin(),Channel_LLR.end()); // copy channel LLR
    //測試
    tanh(input,0.5); // init , do tanh(0.5 * input)
    input = CNLayer_process(input,fc0,2); // layer 0 
    input = VNLayer_process(input,fc1,Channel_LLR,bias_map,0.5); // layer 1
    input = CNLayer_process(input,fc2,2); // layer 2
    input = VNLayer_process(input,fc3,Channel_LLR,bias_map,0.5); // layer 3
    input = CNLayer_process(input,fc4,2); // layer 4
    input = VNLayer_process(input,fc5,Channel_LLR,bias_map,0.5); // layer 5
    input = CNLayer_process(input,fc6,2); // layer 6
    input = VNLayer_process(input,fc7,Channel_LLR,bias_map,0.5); // layer 7
    input = CNLayer_process(input,fc8,2); // layer 8
    input = VNLayer_process(input,fc9,Channel_LLR,bias_map,0.5,true); // layer 9
    sigmoid(input); // out
    print_vector(input); // 檢查輸出
    ``` 

3. **run**：
    請自行更改成BP測performance的code，根據`Model_C_Implement.cpp`來改。

