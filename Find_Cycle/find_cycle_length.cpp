#include<vector>
#include<iostream>
#include<fstream>
#include<queue>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include <unordered_map>
#include <utility>  
#include<algorithm>
#include<set>
using namespace std;
struct parity_check{
    int n,m,max_col_degree,max_row_degree;
    int *max_col_arr,*max_row_arr;
    int **VN_2_CN_pos; // record 1 pos in each row = record col pos at ith row
    int **CN_2_VN_pos; // record 1 pos in each col = record row pos at ith col
};
void Read_File(struct parity_check *H,string& file_name_in);
void FreeAllH(struct parity_check *H);
int find_girth(struct parity_check *H);
vector<vector<int>> Find_all_same_length_girth(struct parity_check *H,int girth);
bool hasSeen(set<vector<int>>&seen,vector<int>& tmp);
int main(int argc,char* argv[]){
    // queue<vector<int>> myqueue;
    struct parity_check H;
    string file_name = argv[1];
    string output_girth_file_name = argv[2];
    string Each_VN_Girth_Record_file = argv[3];
    ofstream file(output_girth_file_name);
    Read_File(&H,file_name);
    cout << "Read H complete !!" << endl;
    int girth = find_girth(&H);
    cout << "girth length is " << girth << endl;
    vector<vector<int>> All_girth_path = Find_all_same_length_girth(&H,girth);
    vector<vector<int>> cycle_map(H.m,vector<int>(H.n,0)); // CN X VN
    // 把重複的cycle去掉
    set<vector<int>> seen;
    for (auto it = All_girth_path.begin(); it != All_girth_path.end(); ) {
        vector<int> VNs;
        vector<int> CNs;
        for (int i = 0; i < it->size(); i += 2) {
            int VN = (*it)[i];
            int CN = (*it)[i + 1];    
            VNs.push_back(VN);
            CNs.push_back(CN);
        }
        sort(VNs.begin(), VNs.end());
        sort(CNs.begin(), CNs.end());
        
        vector<int> tmp(VNs.begin(), VNs.end());
        for (int CN : CNs) tmp.push_back(CN);
    
        bool seen_flag = hasSeen(seen, tmp);
        if (seen_flag) {
            it = All_girth_path.erase(it); // 安全地刪除並前進
        } else {
            ++it; // 沒刪除就往下一筆
        }
    }
    // 計算每一個variable node 所經過的 girth數量有多少個
    vector<int> VNs_girth_num(H.n,0);
    for(auto& path:All_girth_path){
        for(int i=0;i<path.size();i+=2){
            int VN = path[i];
            VNs_girth_num[VN]++;
        }
    }
    ofstream outfile1(Each_VN_Girth_Record_file);
    outfile1 << "VN , Girth "<<girth<<"\n";
    for(int i=0;i<H.n;i++){
        outfile1 << i << " , " << VNs_girth_num[i] << "\n";
    }

    // 輸出所有cycle path
    for(auto& p:All_girth_path){
        for(int i=0;i<p.size();i++){ // must be even
            if(i%2==0) file << "VN" << p[i] << " -> " ;
            else file << "CN" << p[i] << " -> " ;
        }
        file << "VN" << p[0];
        for(int i=1;i<p.size();i+=2){
            int CN = p[i];
            int VN ;
            if(i<p.size()-1) VN= p[i+1];
            else VN  = p[0];
            cycle_map[CN][VN]++;
        }
        file << "\n";   
    }
    file.close();
    
    return 0;
}

bool hasSeen(set<vector<int>>&seen,vector<int>& tmp) {
    if(seen.count(tmp)) return true;
    seen.insert(tmp);
    return false;
}

vector<vector<int>> Find_all_same_length_girth(struct parity_check *H,int girth){
    vector<vector<int>> path;
    queue<vector<int>> q;
    for(int col=0;col<H->n;col++){
        q.push(vector<int>{col}); // each VN Node
    }
    
    while(!q.empty()){
        vector<int> q_top = q.front();
        q.pop();
        if(q_top.size()>girth) continue;
        if(q_top.size()==1){ // let vn  ->  vn-cn
            // cout << "Process size is 1" << endl;
            int curr_vn = q_top.back();
            for(int cn_idx=0;cn_idx<H->max_col_arr[curr_vn];cn_idx++){
                vector<int> temp = q_top;
                temp.emplace_back(H->CN_2_VN_pos[curr_vn][cn_idx]);
                q.push(temp);
            }
        }
        else if(q_top.size()%2==1){ //  VN->CN->VN-> == CN Update
            // cout << "Process size is odd" << endl;
            int size = q_top.size();
            int curr_vn = q_top[size-1];
            int prev_cn = q_top[size-2];
            
            for(int cn_idx=0;cn_idx<H->max_col_arr[curr_vn];cn_idx++){
                vector<int> tmp = q_top;
                if(H->CN_2_VN_pos[curr_vn][cn_idx]==prev_cn) continue;
                else{
                    int curr_cn = H->CN_2_VN_pos[curr_vn][cn_idx];
                    tmp.emplace_back(curr_cn);
                    q.push(tmp);
                }
            }
        }
        else{ // VN->CN-> == VN Update // even find cycle 2、4、6、8
            int size = q_top.size();
            int curr_cn = q_top[size-1];
            int prev_vn = q_top[size-2];
            for(int vn_idx=0;vn_idx<H->max_row_arr[curr_cn];vn_idx++){
                // cout << "Process size is even" << endl;
                vector<int> tmp = q_top;
                if(H->VN_2_CN_pos[curr_cn][vn_idx] == prev_vn) continue;
                else{
                    int curr_vn = H->VN_2_CN_pos[curr_cn][vn_idx];
                    if(curr_vn == tmp.front()){ // find cycle
                        if(tmp.size()==girth){
                            path.emplace_back(tmp);
                            continue;
                        }
                    }
                    tmp.emplace_back(curr_vn);
                    q.push(tmp);
                }
            } 
        }
    }
    return path;
}

int find_girth(struct parity_check *H){
    queue<vector<int>> q;
    for(int col=0;col<H->n;col++){
        q.push(vector<int>{col}); // each VN Node
    }
    
    while(!q.empty()){
        vector<int> q_top = q.front();
        q.pop();
        if(q_top.size()==1){ // let vn  ->  vn-cn
            // cout << "Process size is 1" << endl;
            int curr_vn = q_top.back();
            for(int cn_idx=0;cn_idx<H->max_col_arr[curr_vn];cn_idx++){
                vector<int> temp = q_top;
                temp.emplace_back(H->CN_2_VN_pos[curr_vn][cn_idx]);
                q.push(temp);
            }
        }
        else if(q_top.size()%2==1){ //  VN->CN->VN-> == CN Update
            // cout << "Process size is odd" << endl;
            int size = q_top.size();
            int curr_vn = q_top[size-1];
            int prev_cn = q_top[size-2];
            
            for(int cn_idx=0;cn_idx<H->max_col_arr[curr_vn];cn_idx++){
                vector<int> tmp = q_top;
                if(H->CN_2_VN_pos[curr_vn][cn_idx]==prev_cn) continue;
                else{
                    int curr_cn = H->CN_2_VN_pos[curr_vn][cn_idx];
                    tmp.emplace_back(curr_cn);
                    q.push(tmp);
                }
            }
        }
        else{ // VN->CN-> == VN Update // even find cycle 2、4、6、8
            int size = q_top.size();
            int curr_cn = q_top[size-1];
            int prev_vn = q_top[size-2];
            for(int vn_idx=0;vn_idx<H->max_row_arr[curr_cn];vn_idx++){
                // cout << "Process size is even" << endl;
                vector<int> tmp = q_top;
                if(H->VN_2_CN_pos[curr_cn][vn_idx] == prev_vn) continue;
                else{
                    int curr_vn = H->VN_2_CN_pos[curr_cn][vn_idx];
                    if(curr_vn == tmp.front()){
                        // for(int i=0;i<size;i++){
                        //     if(i%2==0) cout << "VN" << tmp[i] << " -> " ;
                        //     else cout << "CN" << tmp[i] << " -> " ;
                        // }
                        // cout << "VN" << curr_vn << endl;
                        
                        return tmp.size(); // find cycle !!
                    }
                    tmp.emplace_back(curr_vn);
                    q.push(tmp);
                }
            } 
        }
    }
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
void Read_File(struct parity_check *H,string& file_name_in){
    ifstream file_in(file_name_in );
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
            cout << pos << " " ; 
            H->CN_2_VN_pos[i][j]=pos-1;
        }
        cout << endl;
        if(H->max_col_arr[i]!=H->max_col_degree){
            H->CN_2_VN_pos[i] = (int*)realloc(H->CN_2_VN_pos[i],sizeof(int)*H->max_col_arr[i]);
        }
    }
    for(int i=0;i<H->m;i++){
        int pos;
        for(int j=0;j<H->max_row_degree;j++){
            file_in >> pos;
            cout << pos << " " ;
            H->VN_2_CN_pos[i][j]=pos-1;
        }
        cout << endl;
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