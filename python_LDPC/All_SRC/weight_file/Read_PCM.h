#ifndef READ_PCM
#define READ_PCM
#include<string>
#include<vector>
#include<fstream>
using namespace std;

// get h matrix 的樣子
vector<vector<bool>> read_pcm(const string H_filename){
    ifstream f(H_filename);
    int cols,rows,max_cols_degree,max_rows_degree;
    f>>cols>>rows>>max_cols_degree>>max_rows_degree;
    vector<vector<bool>> H(rows,vector<bool>(cols,false));
    vector<int> cols_degree(cols,0),rows_degree(rows,0);
    for(int i=0;i<cols;i++) f>>cols_degree[i]; 
    for(int i=0;i<rows;i++) f>>rows_degree[i]; 
    // assign 1 to h matrix
    for(int c=0;c<cols;c++){
        for(int j=0;j<cols_degree[c];j++){
            int row;
            f>>row;
            H[row-1][c]=true;
        }
    }
    // check 
    for(int r=0;r<rows;r++){
        for(int j=0;j<rows_degree[r];j++){
            int col;
            f>>col;
            if(H[r][col-1]!=true){
                cout << "Read PCM驗證中有錯誤, plz check!!" << endl;
                exit(1);
            }
        }
    }
    cout << "PCM read success , check is ok!!" << endl;
    return H;
}

#endif