
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <float.h>
#include <time.h>
#include <conio.h>
#include <iomanip>
#include <fstream>
#include <istream>
#include <vector>

using namespace std;

#define r 4
#define N (int)pow(2,r)
#define K 8
int Frozen[N]={0};
#include "Polar_FindFB.c"
#define R 0.5
#define snr 0.6

int col=N;
int row=(N-K);

int F[2][2]={1,0,1,1};
int *vn_to_cn_count = new int[col];
int *cn_to_vn_count = new int[row];
int maxi_vn=0;
int maxi_cn=0;

int main(){
    string frozen_bits_position = "frozen_bits_pos.txt";
    string non_frozen_bits_position = "non_frozen_bits_pos.txt";
    ofstream frozen_bits_file(frozen_bits_position);
    ofstream non_frozen_bits_file(non_frozen_bits_position);
    
    B_para_ini2(snr,R);
    B_para();
    double Beta=pow(2,0.25); 
    Beta_expansion(Beta);
    for(int i=0;i<N;i++){
       cout<<Frozen[i]<<" ";
       if(Frozen[i]==1){
            frozen_bits_file << i+1 << " ";
       }else{
            non_frozen_bits_file << i+1 << " ";
       }
    }
    cout << "\n";
    int** G = new int*[N];
    for(int i=0;i<N;++i)
        G[i] = new int[N];
    vector<vector<int>> H(N-K, vector<int>(N, 0));
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            G[i][j]=0;
        }
    }
    for(int i=0;i<N-K;i++){
        for(int j=0;j<N;j++){
            H[i][j]=0;
        }
    }
    for(int i=0;i<2;i++){
        for(int j=0;j<2;j++){
            G[i][j]=F[i][j];
        }
    }
    for(int i=1;i<r;i++){
        for(int j=0;j<pow(2,i);j++){
            for(int k=0;k<pow(2,i);k++){
                 G[(1<<i)+j][k]=G[j][k];
                 G[(1<<i)+j][(1<<i)+k]=G[j][k];
            }
        }
    }
/*
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            cout<<G[i][j]<<"";
        }cout<<endl;
    }//*/
    int f_ind=0;
    for(int i=0;i<N;i++){
        if(Frozen[i]==1){
           for(int j=0;j<N;j++){
               H[f_ind][j]=G[j][i];//cout<<G[k][j]<<" ";

            }//cout<<endl;
             f_ind=f_ind+1;//cout<<f_ind<<endl;
        }
    }
    //*
/*
    for(int j=0;j<N;j++){
        H[0][j]=H[0][j]^H[1][j];

    }
*/
//*
    for(int i=0;i<N-K;i++){
        for(int j=0;j<N;j++){
               cout<<H[i][j]<<" ";
        }
         cout<<endl;
    }//*/




    ofstream out;
    out.open("Dence_PCM.txt",ios::trunc);
    if (!out.is_open()) {
        cout << "Failed to open file";
        return 1;
    }
    out<<col<<" "<<row<<endl;
    for(int i=0;i<col;i++){
        vn_to_cn_count[i]=0;
    }
    for(int i=0;i<row;i++){
        cn_to_vn_count[i]=0;
    }
    for(int i=0;i<col;i++){
        for(int j=0;j<row;j++){
            vn_to_cn_count[i]+=H[j][i];
        }
    }
    maxi_vn=vn_to_cn_count[0];
    for(int i=0;i<col;i++){
        if(vn_to_cn_count[i]>maxi_vn){
           maxi_vn=vn_to_cn_count[i];
        }
    }

    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            cn_to_vn_count[i]+=H[i][j];
        }
    }

    maxi_cn=cn_to_vn_count[0];
    for(int i=0;i<row;i++){
        if(cn_to_vn_count[i]>maxi_cn){
           maxi_cn=cn_to_vn_count[i];
        }
    }

    out<<maxi_vn<<" "<<maxi_cn<<endl;

    for(int i=0;i<col;i++){
        out<<vn_to_cn_count[i]<<" ";
    }
    out<<endl;
    for(int i=0;i<row;i++){
        out<<cn_to_vn_count[i]<< " " ;
    }
    out<<endl;

    for(int i=0;i<col;i++){
        int count = 0;
        for(int j=0;j<row;j++){
            if(H[j][i]==1){
               out<<j+1<< " " ;
               count++;
            }
        }
        while(count<maxi_vn){
            out << 0  << " ";
            count++;
        }
        out<<endl;
    }
    for(int i=0;i<row;i++){
        int count = 0;
        for(int j=0;j<col;j++){
            if(H[i][j]==1){
               out<<j+1<< " ";
               count++;
            }
        }
        while(count<maxi_cn){
            count++;
            out << 0 << " ";
        }
        out<<endl;
    }



    return 0;
}
