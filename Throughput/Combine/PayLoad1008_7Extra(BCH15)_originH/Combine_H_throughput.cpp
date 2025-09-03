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
void Encode_Process(const vector<vector<bool>>& G,const struct parity_check H,bool Encode_Flag,vector<int>& CodeWord);
vector<double> LLR_Process(const struct parity_check H,const vector<int>& CodeWord,double sigma);

int main(int argc,char* argv[]){
    double total_frame_count = 100;
    if(argc < 2){
        cout << "** Error ---- No file in \n" ; 
    }
    string H_combine_file = argv[1];
    string Payload_PCM_file = argv[2];
    string Extra_PCM_file = argv[3];
    string PayLoad_G_file = argv[4]==string("None")?"":argv[4];
    bool PayLoad_Flag = PayLoad_G_file==""?false:true;
    string Extra_G_file = argv[5]==string("None")?"":argv[5];
    bool Extra_Flag = Extra_G_file==""?false:true;
    int iteration_limit = atoi(argv[6]);
    double SNR_min = atof(argv[7]);
    double SNR_max = atof(argv[8]);
    double SNR_ratio = atof(argv[9]);
    int iteration_open = atoi(argv[10]); // Open_Iteration
    string RV1_punc_file = argv[11];
    int RV1_punc_nums = atoi(argv[12]);
    string payload_punc_file = argv[13];
    int payload_punc_nums = atoi(argv[14]);
    string out_file = argv[15];
    int Extra_PCM_num = atoi(argv[16]);
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
    cout << "RV1_punc_file : " << RV1_punc_file << "\n";
    cout << "RV1_punc_nums : " << RV1_punc_nums << "\n";
    cout << "payload_punc_file : " << payload_punc_file << "\n";
    cout << "payload_punc_nums : " << payload_punc_nums << "\n";
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
    /*-------------------------------------------*/
    vector<int> payload_punc_map; // RV0
    payload_punc_map = Read_punc_pos(payload_punc_file,payload_punc_nums);
    cout << "payload punc position file read success !!" << endl;
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
    double code_rate = 1; // throught
    while(SNR<=SNR_max){
        double total_bits = 0 , correct_bits = 0;
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        int to_RV1 = 0;
        for(double frame_count=0;frame_count<total_frame_count;frame_count++){
            /* Process A with RV0*/
            // decode A version with RV0
            vector<int> A_CodeWord;
            vector<int> B_CodeWord;
            vector<int> Xor_CodeWord(PayLoad_H.n);
            Encode_Process(PayLoad_G,PayLoad_H,PayLoad_Flag,A_CodeWord);
            Encode_Process(Extra_G,Extra_H,Extra_Flag,B_CodeWord);
            
            if(A_CodeWord.size()!=PayLoad_H.n){
                cout << "Error -- A_CodeWord size is not equal to Payload_H.n\n";
                exit(1);
            }

            if(B_CodeWord.size()!=Extra_H.n){
                cout << "Error -- B_CodeWord size is not equal to Extra_H.n\n";
                exit(1);
            }
            for(int i=0;i<PayLoad_H.n;i++) Xor_CodeWord[i] = A_CodeWord[i];
            for(int i=0;i<RV1_punc_nums;i++){
                int pos = RV1_punc_map[i];
                Xor_CodeWord[pos] = A_CodeWord[pos] ^ B_CodeWord[i]; 
            }
            
            vector<double> A_LLR = LLR_Process(PayLoad_H,Xor_CodeWord,sigma); // RV1要傳送的LLR
            // cout << "A_LLR size : " << A_LLR.size() << endl;
            vector<double> B_LLR(H.n,0);
            for(int i=0;i<(Extra_H.n*Extra_PCM_num*2);i++) A_LLR.push_back(0); 
            if(A_LLR.size()!=H.n){
                printf("[ERROR] - A_LLR size : %ld \n",A_LLR.size());
                exit(1);
            }
            for(int i=0;i<Extra_H.n*Extra_PCM_num;i++) {
                int origin_pos = RV1_punc_map[i];
                int super_pos  = PayLoad_H.n + Extra_H.n*Extra_PCM_num + i;
                A_LLR[super_pos] = A_LLR[origin_pos];  
                A_LLR[origin_pos] = 0; //  Superposition position
            }
            for(int i=0;i<H.n;i++) B_LLR[i] = A_LLR[i];
            for(int i=0;i<payload_punc_nums;i++) A_LLR[payload_punc_map[i]] = 0;
            /* -----------------*/
            bool decode_Payload_flag = true;
            vector<bool> decode_Extra_flag(7,true);
            BP_for_CombineH(H,PayLoad_H,Extra_H,iteration_limit,iteration_open,A_LLR,A_CodeWord,decode_Payload_flag,B_CodeWord,decode_Extra_flag,Extra_PCM_num);
            total_bits += (PayLoad_H.n-payload_punc_nums); // total bits
            if(decode_Payload_flag==true){
                correct_bits += (PayLoad_H.n - PayLoad_H.m); // info bits
            }
            bool all_extra_ok = true;
            for(int i=0;i<Extra_PCM_num;i++){
                if(decode_Extra_flag[i]==true){
                    correct_bits += (Extra_H.n - Extra_H.m);
                }else{
                    all_extra_ok = false;
                }
            }
            
            if(decode_Payload_flag && all_extra_ok){
                continue;
            } 
            // decode payload + Extra with Combine H architecture
            /* Assign LLR to correct position*/
            to_RV1 += 1;
            bool decode_Payload_flag_RV1 = true;
            vector<bool> decode_Extra_flag_RV1(Extra_PCM_num,true);
            BP_for_CombineH(H,PayLoad_H,Extra_H,iteration_limit,iteration_open,B_LLR,A_CodeWord,decode_Payload_flag_RV1,B_CodeWord,decode_Extra_flag_RV1,Extra_PCM_num);
            total_bits += payload_punc_nums;
            if(decode_Payload_flag==false && decode_Payload_flag_RV1==true){
                correct_bits += PayLoad_H.n - PayLoad_H.m; // payload info bits + extra info bits
            }
            for(int i=0;i<Extra_PCM_num;i++){
                if(decode_Extra_flag[i]==false && decode_Extra_flag_RV1[i]==true){
                    correct_bits += (Extra_H.n - Extra_H.m);
                }
            }
        }
        cout << "correct_bits : " << correct_bits << " | total_bits : " << total_bits  << "\n";
        cout << "RV1_num : " << to_RV1 << "\n";
        double throughput = (correct_bits/total_bits);
        outfile << SNR << ", " << throughput << "\n";
        cout << "SNR : " << SNR << " | " << "Throughput : " << throughput << "\n ";
        cout << "--------------------------------------------\n ";
        SNR += SNR_ratio;
    }
}

 vector<double> LLR_Process(const struct parity_check H,const vector<int>& CodeWord,double sigma){
    vector<double> LLR(H.n,0); 
    for(int i=0;i<H.n;i++){ // Count LLR
        // BPSK -> {0->1}、{1->-1}
        // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
        double receive_value=(double)(-2*CodeWord[i]+1) + sigma*gasdev(); // fixed power
        LLR[i]=2*receive_value/pow(sigma,2); 
    }
    return LLR;
}

void Encode_Process(const vector<vector<bool>>& G,const struct parity_check H,bool Encode_Flag,vector<int>& CodeWord){
    if(Encode_Flag){ // 處理 Payload CodeWord 是zero 還是Encode
        vector<bool> info_bits(G.size());
        for(int i=0;i<G.size();i++){
            info_bits[i] = random_generation()>0.5?1:0;
        }
        CodeWord = Vector_Dot_Matrix_Int(info_bits,G);
        for(int i=0;i<CodeWord.size();i++){
            CodeWord[i] = CodeWord[i]%2;
        }
    }else{
        CodeWord.assign(H.n, 0);
    }
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