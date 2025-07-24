#include<vector>
#include<iostream>
#include<fstream>
#include<queue>
#include<string>
#include<unordered_map>
#include<set>
using namespace std;

struct parity_check{
    int n,m,max_col_degree,max_row_degree;
    int *max_col_arr,*max_row_arr;
    int **VN_2_CN_pos; // record 1 pos in each row = record col pos at ith row
    int **CN_2_VN_pos; // record 1 pos in each col = record row pos at ith col
};

void FreeAllH(struct parity_check*H);
void Read_File_H(struct parity_check *H,string& file_name_in);
vector<int> Read_punc_pos(string file,int  extra_nums);
vector<int> find_furthest_vn(struct parity_check *H, int start_vn);

int main(int argc,char* argv[]){

    string PCM_file = argv[1];
    string Punc_vn_file = argv[2];
    int punc_bits = atoi(argv[3]);
    // define H_combine
    struct parity_check H;
    Read_File_H(&H,PCM_file);
    cout << "H_combine file read success!!" << endl;
    // Read Puncture pos
    vector<int> punc_pos_map;
    punc_pos_map = Read_punc_pos(Punc_vn_file,punc_bits);

    for(int idx=0;idx<punc_bits;idx++){
        int root_vn = punc_pos_map[idx];
        vector<int> furthest_vns = find_furthest_vn(&H,root_vn);
        //
        cout << "root : " << root_vn << " | ";
        for(int vn:furthest_vns){
            cout << " " << vn;
        }
        cout << "\n";
    }
    return 0;
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


vector<int> find_furthest_vn(struct parity_check *H, int start_vn) {
    queue<vector<int>> q;
    vector<bool> visited_vn(H->n,false);
    q.push({start_vn});
    visited_vn[start_vn] = true;

    int furthest_vn = start_vn;
    vector<int> last_node;
    while (!q.empty()) {
        vector<int> curr_vn_set = q.front();q.pop();
        last_node = curr_vn_set;
        vector<int> next_vn_set;
        for(auto curr_vn:curr_vn_set){
            for(int i=0;i<H->max_col_arr[curr_vn];i++){ 
                int CN = H->CN_2_VN_pos[curr_vn][i]; // curr_vn 所連接到cn有哪些
                for(int j=0;j<H->max_row_arr[CN];j++){
                    int neighbor_vn =  H->VN_2_CN_pos[CN][j];
                    if(visited_vn[neighbor_vn]==false){ // unvisited vn node 
                        next_vn_set.push_back(neighbor_vn);
                        visited_vn[neighbor_vn] = true;
                    }
                }
            }
        }
        if(next_vn_set.size()!=0) q.push(next_vn_set);
        
    }

    return last_node;
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