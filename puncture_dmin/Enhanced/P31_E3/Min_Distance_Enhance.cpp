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
vector<int> Read_punc_pos(string file,int  extra_cnt);
vector<int> Read_Structure_pos(string file,int  extra_cnt);
boost::dynamic_bitset<> Int_to_Binary(long long num,size_t bit_length);

int main(int argc,char* argv[]){
    
    string Enhanced_G_file = argv[1];
    string punc_pos_file = argv[2];
    int punc_cnt = atoi(argv[3]);
    cout << "##############################" << "\n";
    cout << "Enhanced_G_file : " << Enhanced_G_file << "\n";
    cout << "punc_pos_file : " << punc_pos_file << "\n";
    cout << "punc_cnt : " << punc_cnt << "\n";
    cout << "##############################" << "\n";
    
    // define PayLoad_G
    vector<boost::dynamic_bitset<>> Enhanced_G;
    vector<boost::dynamic_bitset<>> Enhanced_Transpose_G;
    
    Enhanced_G = Read_File_G(Enhanced_G_file);
    Enhanced_Transpose_G = Transpose_Matrix(Enhanced_G); 
    cout << "PayLoad_G file read success!!" << endl;
    cout << "##############################" << "\n";
    // =================================================
    vector<int> punc_map = Read_punc_pos(punc_pos_file,punc_cnt);
    cout << "punc pos file read success!!" << endl;
    cout << "##############################" << "\n"; 
    // =================================================
    boost::dynamic_bitset<> CodeWord(Enhanced_G[0].size());
    boost::dynamic_bitset<> puncture_CodeWord(Enhanced_G[0].size()-punc_cnt);
    
    vector<boost::dynamic_bitset<>> min_weight_codeword;
    long long info_size = Enhanced_G.size();
    long long code_size = Enhanced_G[0].size();
    int min_col_weight = INT_MAX;
    for(long long num=1;num<(1LL)<<Enhanced_G.size();num++){
        boost::dynamic_bitset<> info = Int_to_Binary(num,info_size);
        GF2_Mat_Vec_Dot(info,Enhanced_Transpose_G,CodeWord); 
        cout << "info : " << info << "| codeword : " << CodeWord << endl;
        for(int i=0,idx=0,j=0;i<code_size;i++){
            if(idx<punc_cnt && punc_map[idx]==i){
                idx++;
                continue;
            } 
            else{
                puncture_CodeWord[j] = CodeWord[i];
                j++;
            }
        }
        size_t ones_num = puncture_CodeWord.count();
        if(ones_num<min_col_weight){
            min_col_weight = ones_num;
            min_weight_codeword.clear();
            min_weight_codeword.push_back(puncture_CodeWord);
        }else if(ones_num==min_col_weight){
            min_weight_codeword.push_back(puncture_CodeWord);
        }
        
    }
    cout << "Min_Weight : " << min_col_weight << "\n";
    cout << "punc CodeWord nums : " << min_weight_codeword.size() << "\n";
    for(int i=0;i<min_weight_codeword.size();i++){
        cout << "punc CodeWord : " << min_weight_codeword[i] << "\n";
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
vector<int> Read_punc_pos(string file,int extra_cnt){
    ifstream punc_file(file);
    vector<int> punc_map;
    // cout <<  "punc pos : " ;
    for(int i=0;i<extra_cnt;i++){
        int pos;
        punc_file >> pos;
        // cout << " " << pos << endl;
        punc_map.push_back(pos-1);
    }
    punc_file.close();
    return punc_map;
}

vector<int> Read_Structure_pos(string file,int  extra_cnt){
    ifstream Structure_file(file);
    vector<int> Structure_map;
    for(int i=0;i<extra_cnt;i++){
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