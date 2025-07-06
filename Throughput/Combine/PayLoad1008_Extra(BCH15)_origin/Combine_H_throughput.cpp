#include<iostream>
#include<fstream>
#include<cmath>
#include<string>
#include "random_number_generator.h"
#include "UseFuction.h"
#include "BP_function.h"

using namespace std;

void Read_File_H(struct parity_check *H,string& file_name_in);
vector<vector<bool>> Read_File_G(string& file_name);
vector<int> Read_punc_pos(string file,int  extra_nums);
vector<double> ProcessCodeWord_ReturnLLR(vector<vector<bool>>& G,struct parity_check H,bool Encode_Flag,double sigma,vector<int>& CodeWord);
vector<double> ProcessXorCodeWord_ReturnLLR(vector<vector<bool>>& Payload_G,struct parity_check Payload_H,bool Payload_Encode_Flag,double sigma,vector<int>& CodeWordA,vector<int>& CodeWordB,vector<int>& punc_map,vector<int>& xor_map,vector<vector<bool>>& Extra_G,struct parity_check H);

int main(int argc,char* argv[]){
    double total_frame_count = 1e6;
    if(argc < 2){
        cout << "** Error ---- No file in \n" ; 
    }
    string H_combine_file = argv[1];
    string Payload_PCM_file = argv[2];
    string Extra_PCM_file = argv[3];
    string  Xor_Position_file = argv[4];
    int  xor_vn_nums = atoi(argv[5]);
    string PayLoad_G_file = argv[6]==string("None")?"":argv[6];
    bool PayLoad_Flag = PayLoad_G_file==""?false:true;
    string Extra_G_file = argv[7]==string("None")?"":argv[7];
    bool Extra_Flag = Extra_G_file==""?false:true;
    int iteration_limit = atoi(argv[8]);
    double SNR_min = atof(argv[9]);
    double SNR_max = atof(argv[10]);
    double SNR_ratio = atof(argv[11]);
    int iteration_open = atoi(argv[12]); // Open_Iteration
    string RV1_punc_file = argv[13];
    int RV1_punc_nums = atoi(argv[14]);
    string out_file = argv[15];
    cout << "##############################" << "\n";
    cout << "H_combine_file : " << H_combine_file << "\n";
    cout << "Payload PCM file : " << Payload_PCM_file << "\n";
    cout << "Extra PCM file : " << Extra_PCM_file << "\n";
    cout << "Payload G file : " << PayLoad_G_file << "\n";
    cout << "PayLoad_Flag : " << PayLoad_Flag << "\n";
    cout << "Extra G file : " << Extra_G_file << "\n";
    cout << "Extra_Flag : " << Extra_Flag << "\n";
    cout << "Iteration_limit : " << iteration_limit << "\n";
    cout << "SNR_min : " << SNR_min << "\n";
    cout << "SNR_max : " << SNR_max << "\n";
    cout << "SNR_ratio : " << SNR_ratio << "\n";
    cout << "iteration_open : " << iteration_open << "\n";
    cout << "Xor_Position_file : " << Xor_Position_file << "\n";
    cout << "xor_vn_nums : " << xor_vn_nums << "\n";
    cout << "RV1_punc_file : " << RV1_punc_file << "\n";
    cout << "RV1_punc_nums : " << RV1_punc_nums << "\n";
    cout << "out_file : " << out_file << "\n";
    cout << "##############################" << "\n";

    // define H_combine
    struct parity_check H;
    Read_File_H(&H,H_combine_file);
    cout << "H_combine file read success!!" << endl;
    /*-------------------------------------------*/
    // define PayLoad_H
    struct parity_check PayLoad_H;
    Read_File_H(&PayLoad_H,Payload_PCM_file);
    cout << "PayLoad_H file read success!!" << endl;
    /*-------------------------------------------*/
    // define Extra_H
    struct parity_check Extra_H;
    Read_File_H(&Extra_H,Extra_PCM_file);
    cout << "Extra_H file read success!!" << endl;
    /*-------------------------------------------*/
    // Read Puncture pos
    vector<int> RV1_punc_map; // RV0
    RV1_punc_map = Read_punc_pos(RV1_punc_file,RV1_punc_nums);
    cout << "Puncture position file read success !!" << endl;
    // Read Xor pos
    vector<int> xor_map; // RV1
    xor_map = Read_punc_pos(Xor_Position_file,xor_vn_nums);
    cout << "Xor position file read success !!" << endl;
    /*-------------------------------------------*/
     // define PayLoad_G
    vector<vector<bool>> PayLoad_G;
    if(PayLoad_Flag){
        PayLoad_G = Read_File_G(PayLoad_G_file);
        cout << "PayLoad_G file read success!!" << endl;
    }
    // define Extra_G
    vector<vector<bool>> Extra_G;
    if(Extra_Flag){
        Extra_G = Read_File_G(Extra_G_file);
        cout << "Extra_G file read success!!" << endl;
    }

    // create extra out csv
    ofstream outfile(out_file);
    if (!outfile.is_open()) {
        cout << "Output file: " << out_file  << " cannot be opened." << endl;
		exit(1);
    }
    outfile << "SNR" << ", " << "Throughput" << "\n";
    /*-------------------------------------------*/
    double SNR = SNR_min;
    double code_rate = 0.5; // throught
    double total_bits = 0, correct_bits = 0 , sigma = 1;
    while(SNR<=SNR_max){
        for(double frame_count=0;frame_count<total_frame_count;frame_count++){
            /* Process A with RV0*/
            // decode A version with RV0
            vector<int> A_CodeWord(PayLoad_H.n,0);
            vector<double> A_LLR = ProcessCodeWord_ReturnLLR(PayLoad_G,PayLoad_H,PayLoad_Flag,sigma,A_CodeWord); // sigmas =1
            for(int i=0;i<RV1_punc_nums;i++) A_LLR[RV1_punc_map[i]] = 0; // punc 7 position(RV1)

            /* -----------------*/
            bool decode_flag_A = BP_for_Payload(PayLoad_H,SNR,code_rate,iteration_limit,A_LLR);
            total_bits += (PayLoad_H.n-RV1_punc_nums); // total bits
            if(decode_flag_A==true){
                correct_bits += PayLoad_H.n - PayLoad_H.m; // info bits
                continue;
            }
                    
            // decode B version RV0 + A RV1
            vector<int> B_CodeWord(PayLoad_H.n,0);
            vector<double> B_LLR(H.n,0);
            if(PayLoad_Flag){ // 處理 Payload CodeWord 是zero 還是Encode
                vector<bool> info_bits((PayLoad_G.size()));
                for(int i=0;i<PayLoad_G.size();i++){
                    info_bits[i] = random_generation()>0.5?1:0;
                }
                B_CodeWord = Vector_Dot_Matrix_Int(info_bits,PayLoad_G);
                for(int i=0;i<B_CodeWord.size();i++){
                    B_CodeWord[i] = B_CodeWord[i]%2;
                }
            }
            // do A parital CodeWord to Encode(BCH)
            vector<bool> info_bits(RV1_punc_nums,0);
            // cout << "punc_map size : " << punc_map.size() << endl;
            for(int i=0;i<RV1_punc_nums;i++){
                int vn_pos = RV1_punc_map[i];
                info_bits[i] = A_CodeWord[vn_pos];
            }
            vector<int> Extra_CodeWord = Vector_Dot_Matrix_Int(info_bits,Extra_G);
            for(int i=0;i<Extra_CodeWord.size();i++) Extra_CodeWord[i] = Extra_CodeWord[i] % 2;
            // do Xor 
            for(int i=0;i<xor_map.size();i++){
                int xor_vn = xor_map[i];
                B_CodeWord[xor_vn] = B_CodeWord[xor_vn] ^ Extra_CodeWord[i];
            }
            for(int i=0;i<PayLoad_H.n;i++){ // Count LLR
                // BPSK -> {0->1}、{1->-1}
                // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
                double receive_value=(double)(-2*B_CodeWord[i]+1) + 1*gasdev(); // fixed power
                B_LLR[i]=2*receive_value/pow(1,2); 
            }
            vector<double> B_LLR_copy = B_LLR; // copy for later use
            for(int i=0;i<RV1_punc_nums;i++){
                int punc_vn = RV1_punc_map[i]; //  RV1
                B_LLR[punc_vn] = 0; // RV1
            }
            // assign LLR to correct position
            for(int i=0;i<xor_map.size();i++){
                int base_pos = PayLoad_H.n + Extra_H.n + i; 
                B_LLR[base_pos] = B_LLR_copy[xor_map[i]]; // RV1
                B_LLR[xor_map[i]] = 0; // combine H
            }
            bool decode_flag_B = BP_for_CombineH(PayLoad_H,SNR,code_rate,iteration_limit,iteration_open,B_LLR,Extra_H,H);
            total_bits += (PayLoad_H.n-RV1_punc_nums);

            if(decode_flag_B == true){
                correct_bits += PayLoad_H.n - PayLoad_H.m; // info bits
                for(int info_idx=Extra_H.n-RV1_punc_nums,RV1_idx=0;info_idx<Extra_H.n;info_idx++,RV1_idx++){
                    int RV1_punc_vn = RV1_punc_map[RV1_idx]; // A RV1
                    int Xor_vn = xor_map[info_idx];
                    A_LLR[RV1_punc_vn] = B_CodeWord[Xor_vn]==0?B_LLR_copy[Xor_vn]:-1*B_LLR_copy[Xor_vn]; // A RV1
                }
                bool decode_flag_A = BP_for_Payload(PayLoad_H,SNR,code_rate,iteration_limit,A_LLR);
                if(decode_flag_A==true){
                    correct_bits += PayLoad_H.n - PayLoad_H.m; // info bits                    
                }
            }
        }
        double throughput = (correct_bits/total_bits);
        outfile << SNR << ", " << throughput << "\n";
        cout << "SNR : " << SNR << " | " << "Throughput : " << throughput << "% |\n ";
        SNR += SNR_ratio;
    }
}

vector<double> ProcessCodeWord_ReturnLLR(vector<vector<bool>>& G,struct parity_check H,bool Encode_Flag,double sigma,vector<int>& CodeWord){
    vector<double> LLR(H.n,0); 
    if(Encode_Flag){ // 處理 Payload CodeWord 是zero 還是Encode
        vector<bool> info_bits((G.size()));
        for(int i=0;i<G.size();i++){
            info_bits[i] = random_generation()>0.5?1:0;
        }
        CodeWord = Vector_Dot_Matrix_Int(info_bits,G);
        for(int i=0;i<CodeWord.size();i++){
            CodeWord[i] = CodeWord[i]%2;
        }
    }
    for(int i=0;i<H.n;i++){ // Count LLR
        // BPSK -> {0->1}、{1->-1}
        // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
        double receive_value=(double)(-2*CodeWord[i]+1) + sigma*gasdev(); // fixed power
        LLR[i]=2*receive_value/pow(sigma,2); 
    }
    return LLR;
}


void Read_File_H(struct parity_check *H,string& file_name_in){
    ifstream file_in(file_name_in);
    if(file_in.fail()){
        cout << "** Error ----- " << file_name_in << " can't open !! \n" ;
        exit(1);
    }
    
    // struct parity_check *H=(struct parity_check*)malloc(sizeof(parity_check));
    file_in >> H->n >> H->m >> H->max_col_degree >> H->max_row_degree;
    
    cout << "Parity_Check_Matrix size  : " << H->m << " x " << H->n << endl;
    cout << "max_col_degree : " << H->max_col_degree << " | max_row_degree : " << H->max_row_degree << "\n";
    H->max_col_arr=(int*)malloc(sizeof(int)*H->n);
    H->max_row_arr=(int*)malloc(sizeof(int)*H->m);
    // cout << "Each col 1's number : ";
    for(int i=0;i<H->n;i++){
        file_in >> H->max_col_arr[i];
        if(H->max_col_arr[i] > H->max_col_degree){
            cout << "** Error ----- " << file_name_in << " max_col_array is Error , Please Check!!" << "\n";
            exit(1);
        }
        // cout << H->max_col_arr[i] << " ";
    }
    // cout << endl << "Each row 1's number : ";
    for(int i=0;i<H->m;i++){
        file_in >> H->max_row_arr[i];
        if(H->max_row_arr[i] > H->max_row_degree){
            cout << "** Error ----- " << file_name_in << " max_row_array is Error , Please Check!!" << "\n";
            exit(1);
        }
        // cout << H->max_row_arr[i] << " ";
    }
    // cout << endl;
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
}

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

vector<vector<bool>> Read_File_G(string& file_name){
    ifstream file(file_name);
    string line;
    vector<string> line_split;
    int idx=0,n,k,max_col;
    vector<vector<bool>> G;
    vector<int> max_col_map;
    while (getline(file, line)) {
        line = strip(line);
        // cout << line << endl;
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==2){
                n = stoi(line_split[0]);
                k = stoi(line_split[1]);
                G = vector<vector<bool>>(k,vector<bool>(n,0));
                cout << "G - n : " << n << " | k : " << k << "\n";
            }else{
                cerr << "First line(n,k) : " << line << " is not n,k !! \n";
                exit(1);
            }
        }else if(idx==1){
            if(line_split.size()==1){
                max_col = stoi(line_split[0]);
                cout << "max col : "  << max_col << "\n";
            }else{
                cerr << "Second line(maxcol) : " << line << " is not max_col !! \n" ;
                exit(1);
            }
        }else if(idx==2){
            if(line_split.size()==n){
                for(int i=0;i<line_split.size();i++){
                    max_col_map.push_back(stoi(line_split[i]));
                }
            }
            else{
                cerr << "Third line(each col 1's number) : " << line << " is not equal G matrix colsize  !! \n";
                exit(1);
            }
        }
        else{
            if(line_split.size()==max_col_map[idx-3]){
                for(string& s:line_split){
                    int pos = stoi(s);
                    if(pos!=0) G[pos-1][idx-3]=true; // 1
                }
            }else{
                cerr << "Each Col 1 postion line : [" << line << "] postion length is not equal to " << max_col << " | length : " << line_split.size() << "\n";
                exit(1); 
            }
        }
        idx++;
    }
    file.close();
    return G;
}