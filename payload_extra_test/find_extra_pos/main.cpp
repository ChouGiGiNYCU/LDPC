#include<iostream>
#include"find_extra_bit_pos.h"
using namespace std;
int main(int argc,char* argv[]){
    string H_filename = argv[1];
    int CNChange = stoi(argv[2]);
    int maxVNChange = stoi(argv[3]);
    int start_bit = stoi(argv[4]);
    find_extra_vn_node(H_filename,CNChange,maxVNChange,start_bit);
    return 0;
}
