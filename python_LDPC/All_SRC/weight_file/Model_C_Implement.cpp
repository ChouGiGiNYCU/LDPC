#include<string>
#include<vector>
#include"model_function.h"
#include"Read_PCM.h"
using namespace  std;
#define clamp_min -0.999999
#define clamp_max 0.999999

vector<float> CNLayer_process(vector<float> input,CNUpdateLayer& fc_layer,const float atanh_factor);
vector<float> VNLayer_process(vector<float> input,VNUpdateLayer& fc_layer,const vector<float>& Channel_LLR,const vector<float>& bias_map,const float tanh_factor,bool final_layer=false);

int main(){
    string PCM_file = "H_10_5.txt";
    vector<vector<bool>> H = read_pcm(PCM_file);
    vector<float> bias_map; 
    int hidden_layer_size=0;
    int input_size = H[0].size(); // col
    int output_size = H[0].size();
    for(int c=0;c<H[0].size();c++){
        for(int r=0;r<H.size();r++){
            if(H[r][c]){
                hidden_layer_size++;
                bias_map.push_back(c);
            }
        }
    }
    cout << "input : " << input_size << " | hidden : " << hidden_layer_size << endl;
    vector<float> Channel_LLR(input_size,1); // 用全1測試
    vector<float> output;
    vector<float> input(Channel_LLR.begin(),Channel_LLR.end()); // copy channel LLR

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
    print_vector(input);
    return 0;
}


vector<float> CNLayer_process(vector<float> input,CNUpdateLayer& fc_layer,const float atanh_factor) {
    input = fc_layer.forward(input); // 前向傳播
    clamp(input, clamp_min, clamp_max); // 約束範圍
    atanh(input, atanh_factor); // do 2*atanh(input)
    return input;
}

vector<float> VNLayer_process(vector<float> input,VNUpdateLayer& fc_layer,const vector<float>& Channel_LLR,const vector<float>& bias_map,const float tanh_factor,bool final_layer){
    input = fc_layer.forward(input,Channel_LLR,bias_map);
    if(!final_layer) tanh(input,tanh_factor); // do tanh(0.5 * input)
    return input; 
}
