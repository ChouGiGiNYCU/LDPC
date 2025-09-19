#include<iostream>
#include<fstream>
#include<cmath>
#include<ctime>
#include<string>
#include "UseFuction.h"
#include "random_number_generator.h"
#include "BP_function.h"
#include <boost/dynamic_bitset.hpp>


// #define phi(x)  log((exp(x)+1)/(exp(x)-1+1e-14)) //CN update 精簡後的公式，1e-14是避免x=0，導致分母為零
using namespace std;


void FreeAllH(struct parity_check*H);
void Read_File_H(struct parity_check *H,string& file_name_in);
vector<boost::dynamic_bitset<>> Read_File_G(string& file_name);
vector<int> Read_punc_pos(string file,int  extra_nums);

int main(int argc,char* argv[]){
    if(argc < 2){
        cout << "** Error ---- No file in \n" ; 
    }
    int total_frame_count = 1000;
    string H_combine_file = argv[1];
    string Payload_PCM_file = argv[2];
    string Extra_PCM_file = argv[3];
    string PayLoad_G_file = argv[4]==string("None")?"":argv[4];
    bool PayLoad_Flag = PayLoad_G_file==""?false:true;
    string Extra_G_file = argv[5]==string("None")?"":argv[5];
    bool Extra_Flag = PayLoad_G_file==""?false:true;
    int iteration_limit = atoi(argv[6]);
    double SNR_min = atof(argv[7]);
    double SNR_max = atof(argv[8]);
    double SNR_ratio = atof(argv[9]);
    int iteration_open = atoi(argv[10]); // Open_Iteration
    bool iteration_open_flag = iteration_open>0?true:false;
    string Superposition_origin_file = argv[11]; // 原本的方法做疊加，Extra/Payload都做punc
    string NewStructure_file = argv[12];
    string out_file = argv[13];
    string payload_punc_file = argv[14];
    int payload_punc_nums = atoi(argv[15]);
    cout << "##############################" << "\n";
    cout << "H_combine_file : " << H_combine_file << "\n";
    cout << "Payload PCM file : " << Payload_PCM_file << "\n";
    cout << "Extra PCM file : " << Extra_PCM_file << "\n";
    cout << "Payload G file : " << PayLoad_G_file << "\n";
    cout << "PayLoad_Flag : " << PayLoad_Flag << "\n";
    cout << "Extra_G_file : " << Extra_G_file << "\n";
    cout << "Extra_Flag : " << Extra_Flag << "\n";
    cout << "Iteration_limit : " << iteration_limit << "\n";
    cout << "SNR_min : " << SNR_min << "\n";
    cout << "SNR_max : " << SNR_max << "\n";
    cout << "SNR_ratio : " << SNR_ratio << "\n";
    cout << "iteration_open : " << iteration_open << "\n";
    cout << "iteration_open_flag : " << iteration_open_flag << "\n"; 
    cout << "Superposition_origin_file : " << Superposition_origin_file << "\n";
    cout << "payload_punc_file : " << payload_punc_file << "\n";
    cout << "payload_punc_nums : " << payload_punc_nums << "\n";
    cout << "##############################" << "\n";
    
    // define H_combine
    struct parity_check H;
    Read_File_H(&H,H_combine_file);
    cout << "H_combine file read success!!" << endl;
    // define PayLoad_H
    struct parity_check PayLoad_H;
    Read_File_H(&PayLoad_H,Payload_PCM_file);
    cout << "PayLoad_H file read success!!" << endl;
    // define Extra_H
    struct parity_check Extra_H;
    Read_File_H(&Extra_H,Extra_PCM_file);
    cout << "Extra_H file read success!!" << endl;
    cout << "##############################" << "\n";
    //------------------------------------------------
    // Read Superposiotn origin pos
    vector<pair<int,int>> Superposition_origin;
    Superposition_origin = Read_Extra2Pyalod_CSVfile(Superposition_origin_file);
    cout << "Puncture position(Superposition origin) file read success !!" << endl;
    //------------------------------------------------
    // Read Superposiotn with new strucuture pos 
    vector<New_Strcuture> Superpostion_Payload_Extra_NewStructure;
    Superpostion_Payload_Extra_NewStructure = Read_NewStrcuture_CSVfile(NewStructure_file);
    cout << "New_Structure_map position file read success !!" << endl;
    cout << "##############################" << "\n";
    //------------------------------------------------
    vector<int> payload_punc_map; // RV0
    payload_punc_map = Read_punc_pos(payload_punc_file,payload_punc_nums);
    cout << "payload punc position file read success !!" << endl;
    //------------------------------------------------
    // define PayLoad_G
    vector<boost::dynamic_bitset<>> Payload_Origin_G;
    vector<boost::dynamic_bitset<>> Payload_Transpose_G;
    if(PayLoad_Flag){
        Payload_Origin_G = Read_File_G(PayLoad_G_file);
        Payload_Transpose_G = Transpose_Matrix(Payload_Origin_G); 
        cout << "PayLoad_G file read success!!" << endl;
        cout << "##############################" << "\n";
    }

    //------------------------------------------------
    // define Extra_G
    vector<boost::dynamic_bitset<>> Extra_Origin_G;
    vector<boost::dynamic_bitset<>> Extra_Transpose_G;
    if(Extra_Flag){
        Extra_Origin_G = Read_File_G(Extra_G_file);
        Extra_Transpose_G = Transpose_Matrix(Extra_Origin_G); 
        cout << "Extra_G file read success!!" << endl;
        cout << "##############################" << "\n";
    }
    //------------------------------------------------
    // create extra out csv
    ofstream outfile(out_file);
    if (!outfile.is_open()) {
        cout << "Output file: " << out_file  << " cannot be opened." << endl;
		exit(1);
    }
    outfile << "SNR" << "," << "Throught" << "\n";
    //------------------------------------------------
    
    double SNR = SNR_min;
    double code_rate = 1;  
    
    while(SNR<=SNR_max){
        double total_bits = 0, correct_bits = 0;
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        for(double frame_count=0;frame_count<total_frame_count;frame_count++){
            /* Process A with RV0*/
            // decode A version with RV0
            boost::dynamic_bitset<> A_CodeWord(PayLoad_H.n); // payload codeword
            boost::dynamic_bitset<> B_CodeWord(Extra_H.n);   // extra codeword
            boost::dynamic_bitset<> Xor_CodeWord(PayLoad_H.n); 
            boost::dynamic_bitset<> A_info_bits(PayLoad_Flag?Payload_Origin_G.size():(PayLoad_H.n-PayLoad_H.m));
            boost::dynamic_bitset<> B_info_bits(Extra_Flag?Extra_Origin_G.size():(Extra_H.n-Extra_H.m));
            vector<double> A_LLR(H.n,0);
            vector<double> B_LLR(H.n,0);
            vector<double> C_LLR(PayLoad_H.n,0);
            Process_info_bits(A_info_bits,PayLoad_Flag);
            Process_info_bits(B_info_bits,Extra_Flag);
            Encode_Process(Payload_Transpose_G,PayLoad_Flag,A_CodeWord,A_info_bits);
            Encode_Process(Extra_Transpose_G,Extra_Flag,B_CodeWord,B_info_bits);
            for(int i=0;i<A_CodeWord.size();i++) Xor_CodeWord[i] = A_CodeWord[i];
            // PayLoad xor Extra with orgin method
            for(auto& table:Superposition_origin){
                int extra_pos = table.first;
                int payload_pos = table.second;
                Xor_CodeWord[payload_pos] = A_CodeWord[payload_pos] ^ B_CodeWord[extra_pos];
            }
            // Payload xor Extra with NewStructure (Extra ^ P1 ^ P2)
            for(auto& table:Superpostion_Payload_Extra_NewStructure){
                int extra_vn  = table.extra_vn;
                int payload_vn_nonpunc  = table.payload_vn_nonpunc;
                int payload_vn_punc  = table.payload_vn_punc;
                Xor_CodeWord[payload_vn_punc] = A_CodeWord[payload_vn_punc] ^ A_CodeWord[payload_vn_nonpunc] ^ B_CodeWord[extra_vn];
            }
            Count_LLR(PayLoad_H,Xor_CodeWord,sigma,A_LLR);
            for(int i=0;i<PayLoad_H.n;i++) C_LLR[i] = A_LLR[i]; // 第三次直接解payload就好
            for(int i=PayLoad_H.n;i<(PayLoad_H.n+Extra_H.n);i++) A_LLR[i] = 0;
            // 部分Extra 傳送過來的資訊要放上去
            auto pair_iter = Superposition_origin.begin();
            for(int i=(PayLoad_H.n+Extra_H.n);i<(PayLoad_H.n+Extra_H.n*2) && pair_iter!=Superposition_origin.end();i++,pair_iter++){
                int origin_vn_pos = pair_iter->second;
                A_LLR[i] = A_LLR[origin_vn_pos];
                A_LLR[origin_vn_pos] = 0; // be punctured
            }
            for(int i=(PayLoad_H.n+Extra_H.n*2);i<(PayLoad_H.n+Extra_H.n*3);i++) A_LLR[i] = 0; // be punctured
            auto pair_iter1  = Superpostion_Payload_Extra_NewStructure.begin();
            for(int i=(PayLoad_H.n+Extra_H.n*3);i<(PayLoad_H.n+Extra_H.n*4) && pair_iter1!=Superpostion_Payload_Extra_NewStructure.end();i++,pair_iter1++){
                int Payload_punc_pos = pair_iter1->payload_vn_punc;
                A_LLR[i] = A_LLR[Payload_punc_pos];
                A_LLR[Payload_punc_pos] = 0;
            }
            for(int i=0;i<A_LLR.size();i++) B_LLR[i] = A_LLR[i];
            for(int i=0;i<payload_punc_nums;i++){
                A_LLR[payload_punc_map[i]] = 0;
            }
            
            
            bool decode_payload_flag = true,decode_extra_flag = true;
            BP_for_CombineH(H,PayLoad_H,Extra_H,iteration_limit,iteration_open,A_LLR,A_CodeWord,decode_payload_flag,B_CodeWord,decode_extra_flag);
            
            
            total_bits += (PayLoad_H.n-payload_punc_nums); // total bits
            if(decode_payload_flag==true){
                correct_bits += PayLoad_H.n - PayLoad_H.m; // info bits
            }
            if(decode_extra_flag){
                correct_bits += (Extra_H.n - Extra_H.m);
            }
            if(decode_payload_flag && decode_extra_flag){
                continue;
            } 
            
            bool decode_Payload_flag_RV1 = true, decode_Extra_flag_RV1 = true;
            BP_for_CombineH(H,PayLoad_H,Extra_H,iteration_limit,iteration_open,B_LLR,A_CodeWord,decode_Payload_flag_RV1,B_CodeWord,decode_Extra_flag_RV1);
            total_bits += payload_punc_nums;
            if(decode_payload_flag==false && decode_Payload_flag_RV1==true){
                correct_bits += PayLoad_H.n - PayLoad_H.m; // payload info bits + extra info bits
            }
            if(decode_extra_flag==false && decode_Extra_flag_RV1==true){
                correct_bits += Extra_H.n - Extra_H.m;
            }
            if(decode_Payload_flag_RV1==false && decode_Extra_flag_RV1==false){
                bool result = BP_for_Payload(PayLoad_H,iteration_limit,C_LLR,A_CodeWord);
                if(result==true) correct_bits += PayLoad_H.n - PayLoad_H.m;
            }
        }
        cout << "correct_bits : " << correct_bits << " | total_bits : " << total_bits  << "\n";
        double throughput = (correct_bits/total_bits);
        outfile << SNR << ", " << throughput << "\n";
        cout << "SNR : " << SNR << " | " << "Throughput : " << throughput << "\n ";
        cout << "--------------------------------------------\n ";
        SNR += SNR_ratio;
    }
    
    
    
    return 0;
}

void FreeAllH(struct parity_check *H){
    cout << "Free All element with H. \n";
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
            goto FreeAll;
        }
        // cout << H->max_col_arr[i] << " ";
    }
    // cout << endl << "Each row 1's number : ";
    for(int i=0;i<H->m;i++){
        file_in >> H->max_row_arr[i];
        if(H->max_row_arr[i] > H->max_row_degree){
            cout << "** Error ----- " << file_name_in << " max_row_array is Error , Please Check!!" << "\n";
            goto FreeAll;
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

    FreeAll: 
        cout << "In ReadFile Free All element \n" ;
        FreeAllH(H);
        file_in.close();
        exit(1);
}

vector<int> Read_punc_pos(string file,int extra_nums){
    ifstream punc_file(file);
    vector<int> punc_map;
    // cout <<  "punc pos : " ;
    for(int i=0;i<extra_nums;i++){
        int pos;
        punc_file >> pos;
        // cout << " " << pos << endl;
        punc_map.push_back(pos-1);
    }
    punc_file.close();
    return punc_map;
}

vector<boost::dynamic_bitset<>> Read_File_G(string& file_name){
    ifstream file(file_name);
    vector<boost::dynamic_bitset<>> G;
    vector<string> line_split;
    string line;
    vector<int> max_col_map;
    int idx = 0, max_col , n ,k;
    while (getline(file, line)) {
        line = strip(line);
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==2){
                n = stoi(line_split[0]); // colsize 
                k = stoi(line_split[1]); // rowsize
                G = vector<boost::dynamic_bitset<>>(k, boost::dynamic_bitset<>(n));
            }else{
                cerr << "First line(n,k) : " << line << " is not n,k !!" << endl;
                exit(1);
            }
        }else if(idx==1){
            if(line_split.size()==1){
                max_col = stoi(line_split[0]); // max_col_degree
            }else{
                cerr << "Second line(maxcol) : " << line << " is not max_col !!" << endl;
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
                cerr << "Each Col 1 postion line : [" << line << "] postion length is not equal to " << max_col << " | length : " << line_split.size() << endl;
                exit(1); 
            }
        }
        idx++;
    }
    file.close();
    return G;
}