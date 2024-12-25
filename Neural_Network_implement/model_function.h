#ifndef MODEL_FUNCTION
#define MODEL_FUNCTION
#include<cmath>
#include<iostream>
#include<vector>
#include<cstdlib>
#include<fstream>
#include<string>
using namespace std;

void print_vector(const vector<float>& input){
    for(float i:input){
        cout << " " << i;
    }
    cout << endl;
}

float randomFloat(float min, float max) {
    // static_cast<float> 強制轉型成 float format
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
}

void ReLu(vector<float>& x) {
    for(int i=0;i<x.size();i++){
        x[i] = x[i]>0?x[i]:0;
    }
    return;
}

void sigmoid(vector<float>& x) {
    for(int i=0;i<x.size();i++){
        x[i] = 1.0f / (1.0f + exp(-x[i]));
    }
    return;
}

void tanh(vector<float>& input,const float scale_factor){
    for(int i=0;i<input.size();i++){
        input[i] = tanh(input[i]*scale_factor);
    }
}

void clamp(vector<float>& input,const float& clamp_min,const float& clamp_max){
    for(int i=0;i<input.size();i++){
        input[i] = max(min(input[i],clamp_max),clamp_min);
    }
}

void atanh(vector<float>& input,const float scale_factor){
    for(int i=0;i<input.size();i++){
        input[i] = scale_factor * atanh(input[i]);
    }
}

class Fully_Connect {
private:
    int input_size;
    int output_size;
    string fc_name;
    // 權重和偏置
    vector<vector<float>> weights_input;
    vector<float> bias_input;
    vector<float> neural_output;
public:

    Fully_Connect(const int input_size,const int output_size,string fc_name) {
        this->input_size = input_size;
        this->output_size = output_size;
        this->fc_name = fc_name;
        init_parameter();
        cout << this->fc_name << " Construct success ." << endl;
    }
    
    Fully_Connect(const int input_size,const int output_size,string fc_name,const string& weight_file,const string& bias_file) {
        this->input_size = input_size;
        this->output_size = output_size;
        this->fc_name = fc_name;
        Read_Weight_File(weight_file,bias_file);
        cout << this->fc_name << " Construct success ." << endl;
    }
    ~Fully_Connect(){
        cout << "FC connect is DeConstructor" << endl;
    }
    
    void init_parameter(){
        // 初始化權重和偏置
        for (int i = 0; i < this->input_size; ++i) {
            vector<float> row;
            for (int j = 0; j < output_size; ++j) {
                row.push_back(randomFloat(-1.0f, 1.0f));
            }
            weights_input.push_back(row);
            bias_input.push_back(randomFloat(-1.0f, 1.0f));
        }
    }
    
    void reset_parameters() {
        weights_input.clear();
        bias_input.clear();
        init_parameter(); // 重新初始化
    }

    void Read_Weight_File(const string& weight_file,const string& bias_file){
        ifstream WeightFile(weight_file);
        if (!WeightFile.is_open()) {
            cout << this->fc_name  <<" - Failed to open weight file: " << weight_file << endl;
            exit(1);
        }
        ifstream BiasFile(bias_file);
        if (!BiasFile.is_open()) {
            cout << this->fc_name << "- Failed to open bias file: " << bias_file << endl;
            exit(1);
        }
        // parameter 一定是要一行一個參數被讀近來 
        vector<float> weight_tmp,bias_tmp;
        string line;
        while (getline(WeightFile,line)){
            weight_tmp.push_back(stof(line));
        }
        if(weight_tmp.size()!=this->output_size*this->input_size){
            cout << this->fc_name << " - " << weight_file << " | Size is not true , plz check !!"  << endl;
            exit(1);
        }
        while (getline(BiasFile,line)){
            bias_tmp.push_back(stof(line));
        }
        if(bias_tmp.size()!=this->output_size){
            cout << this->fc_name << " - " << bias_file << " | Size is not true , plz check !!" << endl;
            exit(1);
        }
        // assign weight to fc weight、bias vector
        this->weights_input = vector<vector<float>>(this->input_size,vector<float>(this->output_size));
        this->bias_input = vector<float>(this->output_size);
        
        for(int c=0;c<this->output_size;c++){
            for(int r=0;r<this->input_size;r++){
                this->weights_input[r][c] = weight_tmp[(c*this->input_size)+r];
            }
            this->bias_input[c] = bias_tmp[c];
        }
    }
    // 計算output
    const vector<float>& forward(const vector<float>& input) {
        if (input.size() != this->input_size) {
            cout << this->fc_name <<" - Input size mismatch. Expected " << this->input_size << " but got " << input.size() << endl;
            exit(1);  // 可根據需求處理錯誤
        }
        // input size = 1x5 | fc size = 5(input size) x 3(output size)
        this->neural_output = vector<float>(this->output_size,0.0);
        for(int c=0;c<this->output_size;c++){
            for(int r=0;r<this->input_size;r++){
                this->neural_output[c] += weights_input[r][c] * input[r]; 
            }
            this->neural_output[c] += bias_input[c];
        } 
        return this->neural_output;
    }

    void print_parameters() {
        cout << "Weights for layer " << this->fc_name << ":" << endl;
        for (int i = 0; i < this->output_size; ++i) {
            for (int j = 0; j < this->input_size; ++j) {
                cout << "w[" << j << "][" << i << "] = " << weights_input[j][i] << endl;
            }
        }

        cout << "Biases for layer " << this->fc_name << ":" << endl;
        for (int i = 0; i < this->output_size; ++i) {
            cout << "b[" << i << "] = " << bias_input[i] << endl;
        }
    }

    void info() {
        cout << "FC info for " << fc_name << ":" << endl;
        cout << "Input size: " << input_size << endl;
        cout << "Output size: " << output_size << endl;
        cout << "Weights size: " << weights_input.size() << "x" << weights_input[0].size() << endl;
        cout << "Bias size: " << bias_input.size() << endl;
    }

    const vector<vector<float>>& get_weights() const {
        return this->weights_input;
    }

    const vector<float>& get_biases() const {
        return this->bias_input;
    }
};


class VNUpdateLayer {
private:
    int origin_size; 
    int input_size;
    int output_size;
    bool final_layer;
    string fc_name;
    // 權重和偏置
    vector<vector<float>> weights_input;
    vector<float> bias_input;
    vector<float> neural_output;
public:

    VNUpdateLayer(const int input_size,const int output_size,string fc_name) {
        this->input_size = input_size;
        this->output_size = output_size;
        this->fc_name = fc_name;
        init_parameter();
        cout << this->fc_name << " Construct success ." << endl;
    }
    
    VNUpdateLayer(const int input_size,const int output_size,int origin_size,string fc_name,const string& weight_file,const string& bias_file,bool final_layer=false) {
        this->input_size = input_size;
        this->output_size = output_size;
        this->origin_size = origin_size;
        this->fc_name = fc_name;
        this->final_layer = final_layer;
        Read_Weight_File(weight_file,bias_file);
        cout << this->fc_name << " Construct success ." << endl;
    }
    ~VNUpdateLayer(){
        cout << "VNUpdateLayer connect is DeConstructor" << endl;
    }
    
    void init_parameter(){
        // 初始化權重和偏置
        for (int i = 0; i < this->input_size; ++i) {
            vector<float> row;
            for (int j = 0; j < output_size; ++j) {
                row.push_back(randomFloat(-1.0f, 1.0f));
            }
            weights_input.push_back(row);
        }
        for(int i=0;i<this->origin_size;i++) bias_input.push_back(randomFloat(-1.0f, 1.0f));
    }
    
    void reset_parameters() {
        weights_input.clear();
        bias_input.clear();
        init_parameter(); // 重新初始化
    }

    void Read_Weight_File(const string& weight_file,const string& bias_file){
        ifstream WeightFile(weight_file);
        if (!WeightFile.is_open()) {
            cout << this->fc_name  <<" - Failed to open weight file: " << weight_file << endl;
            exit(1);
        }
        // parameter 一定是要一行一個參數被讀近來 
        vector<float> weight_tmp;
        string line;
        while (getline(WeightFile,line)){
            weight_tmp.push_back(stof(line));
        }
        if(weight_tmp.size()!=this->output_size*this->input_size){
            cout << this->fc_name << " - " << weight_file << " | Size is not true , plz check !!" << endl;
            exit(1);
        }
        // assign weight to fc weight、bias vector
        this->weights_input = vector<vector<float>>(this->input_size,vector<float>(this->output_size));
        for(int c=0;c<this->output_size;c++){
            for(int r=0;r<this->input_size;r++){
                this->weights_input[r][c] = weight_tmp[(c*this->input_size)+r];
            }
        }

        ifstream BiasFile(bias_file);
        if (!BiasFile.is_open()) {
            cout << this->fc_name  <<" - Failed to open bias file: " << bias_file << endl;
            exit(1);
        }
        while (getline(BiasFile,line)){
            bias_input.push_back(stof(line));
        }
        if(bias_input.size()!=this->origin_size){
            cout << this->fc_name << " - " << bias_file << " | Size is not true , plz check !!" << endl;
        }
    }
    // 計算output
    const vector<float>& forward(const vector<float>& input,const vector<float>& channel_LLR,const vector<float>& bias_map) {
        if (input.size() != this->input_size) {
            cout << this->fc_name <<" - Input size mismatch. Expected " << this->input_size << " but got " << input.size() << endl;
            exit(1);  // 可根據需求處理錯誤
        }
        // input size = 1x5 | fc size = 5(input size) x 3(output size)
        this->neural_output = vector<float>(this->output_size,0.0);
        for(int c=0;c<this->output_size;c++){
            for(int r=0;r<this->input_size;r++){
                this->neural_output[c] += weights_input[r][c] * input[r]; 
            }
            
            if(!this->final_layer) this->neural_output[c] += bias_input[bias_map[c]]*channel_LLR[bias_map[c]];
            else                   this->neural_output[c] += channel_LLR[c]*bias_input[c];
        } 
        return this->neural_output;
    }

    void print_parameters() {
        cout << "Weights for layer " << this->fc_name << ":" << endl;
        for (int i = 0; i < this->output_size; ++i) {
            for (int j = 0; j < this->input_size; ++j) {
                cout << "w[" << j << "][" << i << "] = " << weights_input[j][i] << endl;
            }
        }

        cout << "Biases for layer " << this->fc_name << ":" << endl;
        for (int i = 0; i < this->output_size; ++i) {
            cout << "b[" << i << "] = " << bias_input[i] << endl;
        }
    }

    void info() {
        cout << "FC info for " << fc_name << ":" << endl;
        cout << "Input size: " << input_size << endl;
        cout << "Output size: " << output_size << endl;
        cout << "Weights size: " << weights_input.size() << "x" << weights_input[0].size() << endl;
        cout << "Bias size: " << bias_input.size() << endl;
    }

    const vector<vector<float>>& get_weights() const {
        return this->weights_input;
    }

    const vector<float>& get_biases() const {
        return this->bias_input;
    }
};

class CNUpdateLayer {
private:
    int input_size;
    int output_size;
    string fc_name;
    // 權重和偏置
    vector<vector<float>> weights_input;
    vector<float> bias_input;
    vector<float> neural_output;
public:

    CNUpdateLayer(const int input_size,const int output_size,string fc_name) {
        this->input_size = input_size;
        this->output_size = output_size;
        this->fc_name = fc_name;
        init_parameter();
        cout << this->fc_name << " Construct success ." << endl;
    }
    
    CNUpdateLayer(const int input_size,const int output_size,string fc_name,const string& weight_file,const string& bias_file) {
        this->input_size = input_size;
        this->output_size = output_size;
        this->fc_name = fc_name;
        Read_Weight_File(weight_file); // cn 沒有 bias 權重
        this->bias_input = vector<float>(output_size,0);
        cout << this->fc_name << " Construct success ." << endl;
    }
    ~CNUpdateLayer(){
        cout << "CNUpdateLayer connect is DeConstructor" << endl;
    }
    
    void init_parameter(){
        // 初始化權重和偏置
        for (int i = 0; i < this->input_size; ++i) {
            vector<float> row;
            for (int j = 0; j < output_size; ++j) {
                row.push_back(randomFloat(-1.0f, 1.0f));
            }
            weights_input.push_back(row);
            bias_input.push_back(randomFloat(-1.0f, 1.0f));
        }
    }
    
    void reset_parameters() {
        weights_input.clear();
        bias_input.clear();
        init_parameter(); // 重新初始化
    }

    void Read_Weight_File(const string& weight_file){
        ifstream WeightFile(weight_file);
        if (!WeightFile.is_open()) {
            cout << this->fc_name  <<" - Failed to open weight file: " << weight_file << endl;
            exit(1);
        }

        // parameter 一定是要一行一個參數被讀近來 
        vector<float> weight_tmp;
        string line;
        while (getline(WeightFile,line)){
            weight_tmp.push_back(stof(line));
        }
        if(weight_tmp.size()!=this->output_size*this->input_size){
            cout << this->fc_name << " - " << weight_file << " | Size is not true , plz check !!" << endl;
            exit(1);
        }
        
        // assign weight to fc weight
        this->weights_input = vector<vector<float>>(this->input_size,vector<float>(this->output_size));
        
        for(int c=0;c<this->output_size;c++){
            for(int r=0;r<this->input_size;r++){
                this->weights_input[r][c] = weight_tmp[(c*this->input_size)+r];
            }
        }
    }
    // 計算output
    vector<float> forward(const vector<float>& input) {
        if (input.size() != this->input_size) {
            cout << this->fc_name <<" - Input size mismatch. Expected " << this->input_size << " but got " << input.size() << endl;
            exit(1);  // 可根據需求處理錯誤
        }
        // input size = 1x5 | fc size = 5(input size) x 3(output size)
        this->neural_output = vector<float>(this->output_size,1.0f);
        int cnt=0;
        for(int c=0;c<this->output_size;c++){
            this->neural_output[c] = 1.0;
            for(int r=0;r<this->input_size;r++){
                if(weights_input[r][c]!=0.0){
                    this->neural_output[c] = (this->neural_output[c] * weights_input[r][c] * input[r]);
                }
                
            }
        } 
        return this->neural_output;
    }

    void print_parameters() {
        cout << "Weights for layer " << this->fc_name << ":" << endl;
        for (int i = 0; i < this->output_size; ++i) {
            for (int j = 0; j < this->input_size; ++j) {
                cout << "w[" << j << "][" << i << "] = " << weights_input[j][i] << endl;
            }
        }

        cout << "Biases for layer " << this->fc_name << ":" << endl;
        for (int i = 0; i < this->output_size; ++i) {
            cout << "b[" << i << "] = " << bias_input[i] << endl;
        }
    }

    void info() {
        cout << "FC info for " << fc_name << ":" << endl;
        cout << "Input size: " << input_size << endl;
        cout << "Output size: " << output_size << endl;
        cout << "Weights size: " << weights_input.size() << "x" << weights_input[0].size() << endl;
        cout << "Bias size: " << bias_input.size() << endl;
    }

    const vector<vector<float>>& get_weights() const {
        return this->weights_input;
    }

    const vector<float>& get_biases() const {
        return this->bias_input;
    }
};

#endif