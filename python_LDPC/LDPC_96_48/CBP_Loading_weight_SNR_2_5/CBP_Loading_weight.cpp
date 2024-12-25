#include<iostream>
#include<fstream>
#include<cmath>
#include<ctime>
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include<vector>
#include "random_number_generator.h"
#include "UseFuction.h"

// #define frame_error_lowwer_bound 200 
#define phi(x)  log((exp(x)+1)/(exp(x)-1+1e-14)) //CN update 精簡後的公式，1e-14是避免x=0，導致分母為零
#define Sigmoid(x) 1/(1+exp(-x))

using namespace std;

struct parity_check{
    int n,m,max_col_degree,max_row_degree;
    int *max_col_arr,*max_row_arr;
    int **VN_2_CN_pos; // record 1 pos in each row = record col pos at ith row
    int **CN_2_VN_pos; // record 1 pos in each col = record row pos at ith col
};

void FreeAllH(struct parity_check*H);
void Read_File_H(struct parity_check *H,string& file_name_in);
vector<vector<bool>> Read_File_G(string& file_name);
vector<vector<vector<vector<double>>>> Read_Weight_File(string& file_name,int int_bit,int frac_bit,bool quantize_flag); 
vector<vector<double>> Read_Final_Layer_Data(string& file_name,int int_bit,int frac_bit,bool quantize_flag);
vector<vector<double>> Read_Bias_Data(string& file_name,int int_bit,int frac_bit,bool quantize_flag);
double quantize_generic(float value, int int_bits, int frac_bits);


int main(int argc,char* argv[]){
    if(argc < 2){
        cout << "** Error ---- No file in" << endl; 
    }
    string file_name = argv[1];
    string file_name_out = argv[2];
    string G_file_path  = argv[3];
    bool Encode_Flag = atoi(argv[4])==1?true:false;
    int iteration_limit = atoi(argv[5]);
    double SNR_min = atof(argv[6]);
    double SNR_max = atof(argv[7]);
    double SNR_ratio = atof(argv[8]);
    string weight_data_file_name = argv[9];
    string final_layer_data_file_name = argv[10];
    string bias_weight_file_name = argv[11];
    double quantize_int_bit = stod(argv[12]);
    double quantize_frac_bit = stod(argv[13]);
    bool quantize_flag = atoi(argv[14])==1?true:false;
    double frame_error_lowwer_bound = stod(argv[15]);
    cout << "##############################" << endl;
    cout << "file_name : " << file_name << endl;
    cout << "file_name_out : " << file_name_out << endl;
    cout << "G_file_path : " << G_file_path << endl;
    cout << "Encode_Flag : " << Encode_Flag << endl;
    cout << "iteration_limit : " << iteration_limit << endl;
    cout << "SNR_min : " << SNR_min << endl;
    cout << "SNR_max : " << SNR_max << endl;
    cout << "SNR_ratio : " << SNR_ratio << endl;
    cout << "weight_file_name : " << weight_data_file_name << endl;
    cout << "final_layer_weight_file_name : " << final_layer_data_file_name << endl;
    cout << "bias_weight_file_name : " << bias_weight_file_name << endl;
    cout << "quantize_int_bit : " << quantize_int_bit << endl;
    cout << "quantize_frac_bit : " << quantize_frac_bit << endl;
    cout << "quantize_flag : " << quantize_flag << endl;
    cout << "frame_error_lowwer_bound : " << frame_error_lowwer_bound << endl;
    cout << "##############################" << endl;
    // define H
    struct parity_check H;
    Read_File_H(&H,file_name);
    cout << "H file read success!!" << endl;
    
    // define G
    vector<vector<bool>> G;
    if(Encode_Flag){
        G = Read_File_G(G_file_path);
        cout << "G file read success!!" << endl;
    }
    
    vector<vector<vector<vector<double>>>> weight_cn2vn;
    ifstream file; // read weight_file
    file.open(weight_data_file_name); // opening the file
    if (file){ // checking if the file opening was successful, it will only be true ie. file would have been opened, only if the file exists, so indirectly we are checking if the file exists or not by opening it, if opened, then the file exists, else does not exists
        weight_cn2vn = Read_Weight_File(weight_data_file_name,quantize_int_bit,quantize_frac_bit,quantize_flag);
        cout << "Reading weight success!!" << endl;
    }else{
        cout << weight_data_file_name << " File does not exists !" << endl; // printing the error message
    }
    file.close();
    
    vector<vector<double>> final_layer_weight;
    file.open(final_layer_data_file_name); // opening the file
    if (file){ // checking if the file opening was successful, it will only be true ie. file would have been opened, only if the file exists, so indirectly we are checking if the file exists or not by opening it, if opened, then the file exists, else does not exists
        final_layer_weight = Read_Final_Layer_Data(final_layer_data_file_name,quantize_int_bit,quantize_frac_bit,quantize_flag);
        cout << "Reading final layer weight success!!" << endl;
    }else{
        cout << final_layer_data_file_name << " File does not exists !" << endl; // printing the error message
    }
    file.close();

    ofstream outFile(file_name_out);
    if (!outFile.is_open()) {
        cout << "Output file: " << file_name_out  << " cannot be opened." << endl;
		exit(1);
    }
    outFile << "Eb_over_N0" << ", " << "frame_error_rate" << ", ";
    for(int i=0;i<iteration_limit;i++){
        outFile << "BER_it_"+to_string(i) << ", ";
    }
    outFile << endl;
    
    vector<vector<double>> bias_matrix;
    file.open(bias_weight_file_name); // opening the file
    if (file){ // checking if the file opening was successful, it will only be true ie. file would have been opened, only if the file exists, so indirectly we are checking if the file exists or not by opening it, if opened, then the file exists, else does not exists
        bias_matrix = Read_Bias_Data(bias_weight_file_name,quantize_int_bit,quantize_frac_bit,quantize_flag);
        cout << "Read_Bias_Data success!!" << endl;
    }else{
        cout << final_layer_data_file_name << " File does not exists !" << endl; // printing the error message
    }
    file.close();


    double SNR = SNR_min;
    double *transmit_codeword = (double*)calloc(H.n,sizeof(double));
    double *receiver_LLR = (double*)malloc(H.n*sizeof(double));
    double code_rate = (double)(H.n-H.m)/(double)H.n;
    double ** CN_2_VN_LLR = (double**)malloc(sizeof(double*)*H.n);
    for(int i=0;i<H.n;i++) CN_2_VN_LLR[i]=(double*)calloc(H.max_col_arr[i],sizeof(double));
    double ** VN_2_CN_LLR = (double**)malloc(sizeof(double*)*H.m);
    for(int i=0;i<H.m;i++) VN_2_CN_LLR[i]=(double*)calloc(H.max_row_arr[i],sizeof(double));
    double *guess = (double*)malloc(sizeof(double)*H.n);
    double *bit_error_count = (double*)malloc(iteration_limit*sizeof(double));
    vector<bool> information_bit((G.size()));
    vector<int> result;
    clock_t  clk_start,clk_end;
    while(SNR <= SNR_max){
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        // cout <<"SNR : " << SNR << " | sigma : " << sigma << endl;
        double frame_count=0;
        double frame_error=0;
        int it;
        for(int i=0;i<iteration_limit;i++){
            bit_error_count[i]=0;
        }
        clk_start = clock();
        
        while(frame_error < frame_error_lowwer_bound){
            // cout << "SNR : " << SNR << " | frame_count : " << frame_count << " | frame_error : " << frame_error << endl;
            if(Encode_Flag){
                for(int i=0;i<H.n-H.m;i++){
                    information_bit[i] = random_generation()>0.5?1:0;
                }
                result = Vector_Dot_Matrix_Int(information_bit,G);
            }
            for(int i=0;i<H.n;i++){
                if(Encode_Flag) transmit_codeword[i] = (result[i]%2==0)?0:1; // Encode CodeWord
                else transmit_codeword[i] = 0; // all zero
            }
            

            for(int i=0;i<H.n;i++){
                // Sum_Product_Algorithm 是 soft decision 所以在調變的時候解
                // BPSK -> {0->1}、{1->-1}
                double receiver_codeword=(-2*transmit_codeword[i])+1+sigma*gasdev();
                receiver_LLR[i]=(2*receiver_codeword)/pow(sigma,2);
                receiver_LLR[i] = 2;  
            }
            it=0;
            bool error_syndrome = true;
            bool bit_error_flag=false;
            while(it<iteration_limit){
                
                /* ------- CN update ------- */
                for(int VN=0;VN<H.n;VN++){
                    for(int i=0;i<H.max_col_arr[VN];i++){
                        int CN = H.CN_2_VN_pos[VN][i];
                        double total_LLR=1;
                        // double alpha=1,phi_beta_sum=0;
                        for(int j=0;j<H.max_row_arr[CN];j++){
                            int other_VN = H.VN_2_CN_pos[CN][j];
                            if(other_VN == VN) continue; 
                            if(it==0){ //第一次迭帶就是 直接加上剛算好的receiver_LLR
                                // phi_beta_sum += phi(abs(receiver_LLR[other_VN]));
                                // alpha *= receiver_LLR[other_VN]>0?1:-1;
                                total_LLR *= tanh(0.5*(receiver_LLR[other_VN]));
                            }else{
                                total_LLR *= VN_2_CN_LLR[CN][j];
                                // phi_beta_sum += phi(abs(VN_2_CN_LLR[CN][j]));
                                // alpha *= VN_2_CN_LLR[CN][j]>0?1:-1;
                            }
                        }
                        total_LLR = min(0.999999,max(-0.999999,total_LLR)); // range (-1,1)
                        total_LLR = 2*atanh(total_LLR);
                        CN_2_VN_LLR[VN][i] = total_LLR;
                        // CN_2_VN_LLR[VN][i]= alpha * phi(phi_beta_sum);
                    }
                }
                
                /* ------- VN update ------- */
                for(int CN=0;CN<H.m;CN++){
                    for(int i=0;i<H.max_row_arr[CN];i++){
                        int VN=H.VN_2_CN_pos[CN][i];
                        // add bias weight
                        double total_LLR = receiver_LLR[VN]*bias_matrix[it][VN];
                        // double total_LLR = receiver_LLR[VN];
                        for(int j=0;j<H.max_col_arr[VN];j++){
                            int other_CN = H.CN_2_VN_pos[VN][j];
                            if(other_CN == CN) continue;
                            // Method-1 Loading Model
                            if(it==iteration_limit-1) total_LLR += CN_2_VN_LLR[VN][j];
                            else{
                                total_LLR += CN_2_VN_LLR[VN][j]*weight_cn2vn[it][VN][CN];
                                if(weight_cn2vn[it][other_CN][VN][CN]==0.0){
                                    cout << "Error!! CN : " << CN << " VN : " << VN << "it : " << it <<endl;
                                    exit(1);
                                }
                            }
                            // Method-2 Conventional BP
                            // total_LLR += CN_2_VN_LLR[VN][j];
                        }
                        total_LLR = tanh(0.5*total_LLR); 
                        VN_2_CN_LLR[CN][i] = total_LLR;
                    }
                }
                
                it++;
            }
            /* ------- total LLR ------- */
            
            for(int VN=0;VN<H.n;VN++){
                guess[VN]=receiver_LLR[VN]*bias_matrix[it-1][VN];
                for(int i=0;i<H.max_col_arr[VN];i++){
                    int CN = H.CN_2_VN_pos[VN][i];
                    guess[VN]+=CN_2_VN_LLR[VN][i]*final_layer_weight[CN][VN];
                    if(final_layer_weight[CN][VN]==0.0){
                        printf("final layer is zero %d->%d\n",CN,VN);
                    }
                }
                /* ------- make decision ------- */
                cout << guess[VN]<< " ";
                guess[VN]= (guess[VN]<0)?1:0;
            }
            exit(1);
            /* ----- Determine bit error ----- */
            error_syndrome = false;
            bit_error_flag=false;
            for(int VN=0;VN<H.n;VN++){
                if(guess[VN]!=transmit_codeword[VN]){
                    bit_error_count[it-1]++;
                    bit_error_flag=true;
                    error_syndrome = true;
                }
            }
            
            if(bit_error_flag){
                frame_error+=1;
            }
            frame_count++;
        }
        clk_end=clock();
        double clk_duration = (double)(clk_end-clk_start)/CLOCKS_PER_SEC;
        printf("SNR : %.2f  | FER : %.5e  | BER : %.5e(%d) | Time : %.3f | Total frame : %.0f\n",SNR,frame_error/frame_count,bit_error_count[it-1]/((double)H.n*frame_count),it,clk_duration,frame_count);
        outFile << SNR << ", " << frame_error/frame_count << ", ";
        for(int i=0;i<iteration_limit;i++){
            outFile << bit_error_count[i]/((double)H.n*frame_count) << ", ";
        }
        outFile <<endl;

        // cout << "SNR : " << SNR << "| SNR_max : " << SNR_max <<endl;
        if(SNR==SNR_max){
            cout << "over the SNR_max" << endl;
            break;
        }
        else if(SNR+SNR_ratio>SNR_max){
            SNR=SNR_max;
        }else{
            SNR=SNR+SNR_ratio;
        }
        
    }
    outFile.close();
    /* ----------- free all memory ----------- */
    cout << "In main Free" << endl;
    FreeAllH(&H);
    free(transmit_codeword);
    free(receiver_LLR);
    for(int i=0;i<H.n;i++) free(CN_2_VN_LLR[i]);
    free(CN_2_VN_LLR);
    for(int i=0;i<H.m;i++) free(VN_2_CN_LLR[i]);
    free(VN_2_CN_LLR);
    free(guess);
    free(bit_error_count);
    return 0;
}

void FreeAllH(struct parity_check *H){
    cout << "Free All element with H." << endl;
    if(H->max_col_arr != NULL) free(H->max_col_arr);
    if(H->max_row_arr != NULL) free(H->max_row_arr);
    if(H->CN_2_VN_pos != NULL) {
        for(int i = 0; i < H->n; i++) {
            if(H->CN_2_VN_pos[i] != NULL) free(H->CN_2_VN_pos[i]);
        }
        free(H->CN_2_VN_pos);
    }
    if(H->VN_2_CN_pos != NULL) {
        for(int i = 0; i < H->m; i++) {
            if(H->VN_2_CN_pos[i] != NULL) free(H->VN_2_CN_pos[i]);
        }
        free(H->VN_2_CN_pos);
    }
}

void Read_File_H(struct parity_check *H,string& file_name_in){
    ifstream file_in(file_name_in);
    if(file_in.fail()){
        cout << "** Error ----- " << file_name_in << " can't open !!" <<endl;
        exit(1);
    }
    
    // struct parity_check *H=(struct parity_check*)malloc(sizeof(parity_check));
    file_in >> H->n >> H->m >> H->max_col_degree >> H->max_row_degree;
    
    cout << "Parity_Check_Matrix size  : " << H->m << " x " << H->n << endl;
    cout << "max_col_degree : " << H->max_col_degree << " | max_row_degree : " << H->max_row_degree << endl;
    H->max_col_arr=(int*)malloc(sizeof(int)*H->n);
    H->max_row_arr=(int*)malloc(sizeof(int)*H->m);
    cout << "Each col 1's number : ";
    for(int i=0;i<H->n;i++){
        file_in >> H->max_col_arr[i];
        if(H->max_col_arr[i] > H->max_col_degree){
            cout << "** Error ----- " << file_name_in << " max_col_array is Error , Please Check!!" << endl;
            goto FreeAll;
        }
        cout << H->max_col_arr[i] << " ";
    }
    cout << endl << "Each row 1's number : ";
    for(int i=0;i<H->m;i++){
        file_in >> H->max_row_arr[i];
        if(H->max_row_arr[i] > H->max_row_degree){
            cout << "** Error ----- " << file_name_in << " max_row_array is Error , Please Check!!" << endl;
            goto FreeAll;
        }
        cout << H->max_row_arr[i] << " ";
    }
    cout << endl;
    H->CN_2_VN_pos=(int**)malloc(sizeof(int*)*H->n);
    H->VN_2_CN_pos=(int**)malloc(sizeof(int*)*H->m);
    for(int i=0;i<H->n;i++){
        H->CN_2_VN_pos[i]=(int*)malloc(H->max_col_degree*sizeof(int));
    }
    for(int i=0;i<H->m;i++){
        H->VN_2_CN_pos[i]=(int*)malloc(H->max_row_degree*sizeof(int));
    }
    for(int i=0;i<H->n;i++){
        int pos;
        for(int j=0;j<H->max_col_degree;j++){
            file_in >> pos;
            H->CN_2_VN_pos[i][j]=pos-1;
        }
        if(H->max_col_arr[i]!=H->max_col_degree){
            H->CN_2_VN_pos[i] = (int*)realloc(H->CN_2_VN_pos[i],sizeof(int)*H->max_col_arr[i]);
        }
    }
    for(int i=0;i<H->m;i++){
        int pos;
        for(int j=0;j<H->max_row_degree;j++){
            file_in >> pos;
            H->VN_2_CN_pos[i][j]=pos-1;
        }
        if(H->max_row_arr[i]!=H->max_row_degree){
            H->VN_2_CN_pos[i] = (int*)realloc(H->VN_2_CN_pos[i],sizeof(int)*H->max_row_arr[i]);
        }
    }
    file_in.close();
    return;

    FreeAll: 
        cout << "In ReadFile Free All element " << endl;
        FreeAllH(H);
        file_in.close();
        exit(1);
}

vector<vector<vector<vector<double>>>> Read_Weight_File(string& file_name,int int_bit,int frac_bit,bool quantize_flag){
    ifstream file(file_name);
    string line;
    vector<string> line_split;
    int idx=0,it,vn_num,cn_num;
    vector<vector<vector<vector<double>>>> weight_data ;
    while (getline(file, line)) {
        line = strip(line);
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==3){
                it = stoi(line_split[0]);
                vn_num = stoi(line_split[1]);
                cn_num = stoi(line_split[2]);
                weight_data = vector<vector<vector<vector<double>>>>(it,vector<vector<vector<double>>>(cn_num,vector<vector<double>>(vn_num,vector<double>(cn_num,0))));
            }else{
                cout << "First line(iteration,VN,CN) : " << line << " is not (iteration,VN,CN) !!" << endl;
                exit(1);
            }
        }
        else{
            if(line_split.size()==5){
                it=stoi(line_split[0]);
                int prev_cn = stoi(line_split[1]);
                int prev_vn = stoi(line_split[2]);
                int curr_cn = stoi(line_split[3]);
                double weight = stof(line_split[4]);
                if(quantize_flag) weight_data[it][prev_cn][prev_vn][curr_cn] = quantize_generic(weight,int_bit,frac_bit);
                else weight_data[it][prev_cn][prev_vn][curr_cn] = weight;
                // printf("it : %d | cn : %d | vn : %d | weight :%f\n",it,cn,vn,weight);
            }else{
                cout << idx-1 << "th row have wrong type !!" << endl;
                exit(1); 
            }
        }
        idx++;
    }
    file.close();
    return weight_data;
}

vector<vector<double>> Read_Final_Layer_Data(string& file_name,int int_bit,int frac_bit,bool quantize_flag){
    ifstream file(file_name);
    string line;
    vector<string> line_split;
    int idx=0,vn_num,cn_num;
    vector<vector<double>> final_layer_weight_data;
    while (getline(file, line)) {
        line = strip(line);
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==2){
                vn_num = stoi(line_split[0]);
                cn_num = stoi(line_split[1]);
                final_layer_weight_data = vector<vector<double>>(cn_num,vector<double>(vn_num,0));
            }else{
                cout << "First line(iteration,VN,CN) : " << line << " is not (iteration,VN,CN) !!" << endl;
                exit(1);
            }
        }
        else{
            if(line_split.size()==3){
                int cn=stoi(line_split[0]);
                int vn=stoi(line_split[1]);
                double weight = stof(line_split[2]);
                if(quantize_flag) final_layer_weight_data[cn][vn] = quantize_generic(weight,int_bit,frac_bit);
                else final_layer_weight_data[cn][vn] = weight;
                // cout << "cn : " << cn << " | vn : " << vn << "| weight : " << final_layer_weight_data[cn][vn] << endl;
            }else{
                cout << idx-1 << "th row have wrong type !!" << endl;
                exit(1); 
            }
        }
        idx++;
    }
    file.close();
    return final_layer_weight_data;
}

vector<vector<bool>> Read_File_G(string& file_name){
    ifstream file(file_name);
    string line;
    vector<string> line_split;
    int idx=0,n,k,max_col;
    vector<vector<bool>> G;
    while (getline(file, line)) {
        line = strip(line);
        // cout << line << endl;
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==2){
                n = stoi(line_split[0]);
                k = stoi(line_split[1]);
                G = vector<vector<bool>>(k,vector<bool>(n,0));
            }else{
                cout << "First line(n,k) : " << line << " is not n,k !!" << endl;
                exit(1);
            }
        }else if(idx==1){
            if(line_split.size()==1){
                max_col = stoi(line_split[0]);
            }else{
                cout << "Second line(maxcol) : " << line << " is not max_col !!" << endl;
                exit(1);
            }
        }else if(idx==2){
            continue;
        }
        else{
            if(line_split.size()==max_col){
                for(string& s:line_split){
                    int pos = stoi(s);
                    if(pos!=0) G[pos-1][idx-2]=true; // 1
                }
            }else{
                cout << "Each Col 1 postion line : [" << line << "] postion length is not equal to " << max_col << " | length : " << line_split.size() << endl;
                exit(1); 
            }
        }
        idx++;
    }
    file.close();
    return G;
}

vector<vector<double>> Read_Bias_Data(string& file_name,int int_bit,int frac_bit,bool quantize_flag){
    ifstream file(file_name);
    vector<vector<double>> bias_matrix;
    string line;
    vector<string> line_split;
    int idx=0;
    while (getline(file, line)) {
        line = strip(line);
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==3){
                int it = stoi(line_split[0]);
                int vn_num = stoi(line_split[1]);
                int cn_num = stoi(line_split[2]);
                bias_matrix = vector<vector<double>>(it,vector<double>(vn_num,0));
            }else{
                cout << "First line(iteration,VN_number,CN_numer) : " << line << " is  wrong type !!" << endl;
                exit(1);
            }
        }
        else{
            if(line_split.size()==3){
                int it = stoi(line_split[0]);
                int vn = stoi(line_split[1]);
                double bias_weight = stof(line_split[2]);
                if(quantize_flag) bias_matrix[it][vn] = quantize_generic(bias_weight,int_bit,frac_bit);
                else bias_matrix[it][vn] = bias_weight;
            }else{
                cout << "Error Type in read (iteration vn bias_weight) !!  Error line : " << idx  << endl;
                exit(1); 
            }
        }
        idx++;
    }
    file.close();
    return bias_matrix;
}

double quantize_generic(float value, int int_bits, int frac_bits) {
    // 計算整數和小數部分的範圍，考慮符號位
    int integer_range = (1 << (int_bits - 1)) - 1;   // 正數整數部分的最大值，去掉一個符號位
    int fractional_levels = (1 << frac_bits);  // 小數部分有多少個級別

    // 計算最大和最小範圍值
    float max_value = (float)integer_range + (float)(fractional_levels - 1) / fractional_levels;
    float min_value = -(float)(integer_range + 1); // 最小值為負的整數範圍

    // 確保輸入值在最小和最大範圍內
    if (value < min_value) {
        value = min_value;
        return value;
    } else if (value > max_value) {
        value = max_value;
        return value;
    }

    // 計算量化級數的間距
    float step_size = 1.0 / fractional_levels;
    // 計算最接近的量化值
    double quantized_value = round(value / step_size) * (double)step_size;

    return quantized_value;
}