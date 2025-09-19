#include<iostream>
#include<fstream>
#include<cmath>
#include<string>
#include "UseFuction.h"
#include "random_number_generator.h"
#include <boost/dynamic_bitset.hpp>


using namespace std;



void FreeAllH(struct parity_check*H);
void Read_File_H(struct parity_check *H,string& file_name_in);
vector<boost::dynamic_bitset<>> Read_File_G(string& file_name);
vector<int> Read_punc_pos(string file,int  extra_nums);
vector<int> Read_Structure_pos(string file,int  extra_nums);
boost::dynamic_bitset<> Int_to_Binary(long long num,size_t bit_length);

int main(int argc,char* argv[]){
    
    string Payload_PCM_file = argv[1];
    string Extra_PCM_file = argv[2];
    string PayLoad_G_file = argv[3];
    string Extra_G_file = argv[4];
    string Superposition_origin_file = argv[5]; // 原本的方法做疊加，Extra/Payload都做punc
    int extra_num = atoi(argv[6]);
    cout << "##############################" << "\n";
    cout << "Payload PCM file : " << Payload_PCM_file << "\n";
    cout << "Extra PCM file : " << Extra_PCM_file << "\n";
    cout << "Payload G file : " << PayLoad_G_file << "\n";
    cout << "Extra G file : " << Extra_G_file << "\n";
    cout << "Superposition_origin_file : " << Superposition_origin_file << "\n";
    cout << "extra_num : " << extra_num << "\n";
    cout << "##############################" << "\n";
    // define PayLoad_H
    struct parity_check PayLoad_H;
    Read_File_H(&PayLoad_H,Payload_PCM_file);
    cout << "PayLoad_H file read success!!" << endl;
    // define Extra_H
    struct parity_check Extra_H;
    Read_File_H(&Extra_H,Extra_PCM_file);
    cout << "Extra_H file read success!!" << endl;
    cout << "##############################" << "\n";
    // Read Superposiotn origin pos
    vector<int> punc_map = Read_punc_pos(Superposition_origin_file,extra_num);
    cout << "Puncture position(Superposition origin) file read success !!" << endl;
    

    // define PayLoad_G
    vector<boost::dynamic_bitset<>> Payload_Origin_G;
    vector<boost::dynamic_bitset<>> Payload_Transpose_G;
    
    Payload_Origin_G = Read_File_G(PayLoad_G_file);
    Payload_Transpose_G = Transpose_Matrix(Payload_Origin_G); 
    cout << "PayLoad_G file read success!!" << endl;
    cout << "##############################" << "\n";
    
    // define Extra_G
    vector<boost::dynamic_bitset<>> Extra_Origin_G;
    vector<boost::dynamic_bitset<>> Extra_Transpose_G;
    
    Extra_Origin_G = Read_File_G(Extra_G_file);
    Extra_Transpose_G = Transpose_Matrix(Extra_Origin_G); 
    cout << "Extra_G file read success!!" << endl;
    cout << "##############################" << "\n";
    

    boost::dynamic_bitset<> Xor_CodeWord(PayLoad_H.n);
    boost::dynamic_bitset<> payload_info(Payload_Origin_G.size());
    boost::dynamic_bitset<> extra_info(Extra_Origin_G.size());
    boost::dynamic_bitset<> payload_Encode(PayLoad_H.n);
    boost::dynamic_bitset<> extra_Encode(Extra_H.n);
    vector<vector<boost::dynamic_bitset<>>> min_weight_codeword;
    int payload_bits_num = Payload_Origin_G.size();
    int extra_bits_num   = Extra_Origin_G.size();
    int min_col_weight = INT_MAX;
    for(long long payload_num=1;payload_num<(1LL)<<payload_bits_num;payload_num++){
        boost::dynamic_bitset<> payload_info = Int_to_Binary(payload_num,payload_bits_num);
        GF2_Mat_Vec_Dot(payload_info,Payload_Transpose_G,payload_Encode); 
        cout << "payload_info : " << payload_info << endl;
        for(long long extra_num=0;extra_num<(1LL)<<extra_bits_num;extra_num++){
            boost::dynamic_bitset<> extra_info = Int_to_Binary(extra_num,extra_bits_num);
            GF2_Mat_Vec_Dot(extra_info,Extra_Transpose_G,extra_Encode);
            for(int i=0;i<PayLoad_H.n;i++) Xor_CodeWord[i] = payload_Encode[i]; // copy
            for(int i=0;i<extra_num;i++){
                Xor_CodeWord[i] = payload_Encode[punc_map[i]] ^ extra_Encode[i];
            }
            size_t ones_num = Xor_CodeWord.count();
            if(ones_num<min_col_weight){
                min_col_weight = ones_num;
                min_weight_codeword.clear();
                min_weight_codeword.push_back({Xor_CodeWord,payload_Encode,extra_Encode});
            }else if(ones_num==min_col_weight){
                min_weight_codeword.push_back({Xor_CodeWord,payload_Encode,extra_Encode});
            }
        }
        
    }
    cout << "Min_Weight : " << min_col_weight << "\n";
    cout << "XorCodeWord nums : " << min_weight_codeword.size() << "\n";
    for(int i=0;i<min_weight_codeword.size();i++){
        cout << "Xor_CodeWord : " << min_weight_codeword[i][0]  << " |   Payload_Encode : " << min_weight_codeword[i][1] << " |   Extra_Encode : " << min_weight_codeword[i][2] << "\n";
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

vector<int> Read_Structure_pos(string file,int  extra_nums){
    ifstream Structure_file(file);
    vector<int> Structure_map;
    for(int i=0;i<extra_nums;i++){
        int pos;
        Structure_file >> pos;
        Structure_map.push_back(pos-1);
    }
    Structure_file.close();
    return Structure_map;
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


boost::dynamic_bitset<> Int_to_Binary(long long num,size_t bit_length){
    boost::dynamic_bitset<> b(bit_length, num);
    return b;
}