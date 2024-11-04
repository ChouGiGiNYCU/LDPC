#include<fstream>
#include<string>
#include<vector>
using namespace std;
int main(){
    string filename = "H_96_48.txt";
    string outfilename = "H_96_48_Mat.txt";
    ifstream h_file(filename);
    ofstream outfile(outfilename);
    int n,n_sub_k;
    int max_col_degree,max_row_degree;
    h_file>>n>>n_sub_k;
    h_file>>max_col_degree>>max_row_degree;
    
    vector<vector<int>> H(n_sub_k,vector<int>(n,0));
    vector<int> max_col_arr(n,0);
    vector<int> max_row_arr(n_sub_k,0);
    for(int i=0;i<n;i++) h_file >> max_col_arr[i];
    for(int i=0;i<n_sub_k;i++) h_file >> max_row_arr[i];
    //  read 1 postion to H matrix 
    for(int col=0;col<n;col++){
        for(int i=0;i<max_col_degree;i++){
            int row;
            h_file >> row;
            if(row==0) continue;
            else{
                H[row-1][col]=1; // txt idx start is 1  
            }
        }
    }
    h_file.close();
    for(int r=0;r<n_sub_k;r++){
        for(int c=0;c<n;c++){
            outfile<< H[r][c] << " ";
        }
        outfile<<endl;
    }
    return 0;
}