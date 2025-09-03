#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include <boost/dynamic_bitset.hpp>
#include"UseFuction.h"
using namespace std;

vector<boost::dynamic_bitset<>> Read_File_G(string& file_name);
boost::dynamic_bitset<> Int_to_Binary(long long num,size_t bit_length);
int main(int argc,char* argv[]){
    string G_file = argv[1];

    vector<boost::dynamic_bitset<>> Origin_G;
    vector<boost::dynamic_bitset<>> Transpose_G;
    Origin_G = Read_File_G(G_file);
    Transpose_G = Transpose_Matrix(Origin_G); 
    cout << "G file read success!!" << endl;
    cout << "##############################" << "\n";
    size_t info_bits_num = Origin_G.size();
    size_t code_bits_num = Origin_G[0].size();
    
    /*
    int num = 255;
    boost::dynamic_bitset<> info = Int_to_Binary(num,info_bits_num);    
    std::cout << "Binary of " << num << " = " << info << "\n";
    */
    size_t min_col_weight = code_bits_num;
    vector<boost::dynamic_bitset<>> min_weight_codeword;
    cout << " num : " << ((long long)1<<info_bits_num) << endl;

    for(long long num=1;num<(long long)((long long)1<<info_bits_num);num++){
        boost::dynamic_bitset<> info = Int_to_Binary(num,info_bits_num);  
        cout << "info : " << info << endl;
        boost::dynamic_bitset<> codeword(code_bits_num);
        GF2_Mat_Vec_Dot(info,Transpose_G,codeword);
        size_t ones_num = codeword.count();
        if(ones_num<min_col_weight){
            min_col_weight = ones_num;
            min_weight_codeword.clear();
            min_weight_codeword.push_back(codeword);
        }else if(ones_num==min_col_weight){
            min_weight_codeword.push_back(codeword);
        }
    }
    vector<int> map(code_bits_num,0);
    for(auto codeword:min_weight_codeword){
        for(int bit=0;bit<codeword.size();bit++){
            if(codeword[bit]==1){
                map[bit]++;
            }
        }
    }
    for(int i=0;i<code_bits_num;i++){
        if(map[i]) cout << "i : " << i << " |  " << map[i] << "\n"; 
    }
    cout << "min_distance : " << min_col_weight << " " << endl;
    for(int i=0;i<min_weight_codeword.size();i++){
        cout << "codeword : " << min_weight_codeword[i] << endl;
    }
    return 0;
}

boost::dynamic_bitset<> Int_to_Binary(long long num,size_t bit_length){
    boost::dynamic_bitset<> b(bit_length, num);
    return b;
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