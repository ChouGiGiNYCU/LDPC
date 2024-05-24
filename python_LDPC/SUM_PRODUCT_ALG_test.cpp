#include<iostream>
#include<fstream>
#include<cmath>
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include "random_number_generator.h"
// ------------ parameter change here ---------- //
#define iteration_limit 3
// ---------------------------------------------- //
#define phi(x)  log((exp(x)+1)/(exp(x)-1+1e-14)) //CN update 精簡後的公式，1e-14是避免x=0，導致分母為零
#define Sigmoid(x) 1/(1+exp(-x))
struct parity_check{
    int n,m,max_col_degree,max_row_degree;
    int *max_col_arr,*max_row_arr;
    int **VN_2_CN_pos; // record 1 pos in each row = record col pos at ith row
    int **CN_2_VN_pos; // record 1 pos in each col = record row pos at ith col
};

void FreeAllH(struct parity_check*H);
void Read_File(struct parity_check *H,std::string& file_name_in);

int main(int argc,char* argv[]){
    if(argc < 2){
        std::cout << "** Error ---- <.exe file> <input_file_name> <output_file_name>" << std::endl; 
    }
    std::string file_name = argv[1];
    struct parity_check H;
    Read_File(&H,file_name);
    std::cout << "complete read file "<<std::endl;
    std::string file_name_test = argv[2];
    std::ifstream test_file_in(file_name_test + ".txt");
    
    double *receiver_LLR = (double*)malloc(H.n*sizeof(double));
    for(int i=0;i<H.n;i++){
        test_file_in >> receiver_LLR[i];
    }
    double code_rate = (double)(H.n-H.m)/(double)H.n;
    double ** CN_2_VN_LLR = (double**)malloc(sizeof(double*)*H.n);
    for(int i=0;i<H.n;i++) CN_2_VN_LLR[i]=(double*)calloc(H.max_col_arr[i],sizeof(double));
    double ** VN_2_CN_LLR = (double**)malloc(sizeof(double*)*H.m);
    for(int i=0;i<H.m;i++) VN_2_CN_LLR[i]=(double*)calloc(H.max_row_arr[i],sizeof(double));
    double *guess = (double*)malloc(sizeof(double)*H.n);
    
    
    int it=0;
    while(it<iteration_limit){
                
        /* ------- CN update ------- */
        for(int VN=0;VN<H.n;VN++){
            // std::cout << "VN : " << VN << std::endl;
            for(int i=0;i<H.max_col_arr[VN];i++){
                // std::cout << H.max_col_arr[VN] << std::endl;
                int CN = H.CN_2_VN_pos[VN][i];
                // std::cout << "CN : " << CN << std::endl;
                double alpha=1,phi_beta_sum=0;
                for(int j=0;j<H.max_row_arr[CN];j++){
                    int other_VN = H.VN_2_CN_pos[CN][j];
                    if(other_VN == VN) continue; 
                    if(it==0){ //第一次迭帶就是 直接加上剛算好的receiver_LLR
                        phi_beta_sum += phi(abs(receiver_LLR[other_VN]));
                        alpha *= receiver_LLR[other_VN]>0?1:-1;
                    }else{
                        phi_beta_sum += phi(abs(VN_2_CN_LLR[CN][j]));
                        alpha *= VN_2_CN_LLR[CN][j]>0?1:-1;
                    }
                    // std::cout << "j : " << j << std::endl;
                }
                CN_2_VN_LLR[VN][i]= alpha * phi(phi_beta_sum);
            }
        }
        
        /* ------- VN update ------- */
        for(int CN=0;CN<H.m;CN++){
            for(int i=0;i<H.max_row_arr[CN];i++){
                int VN=H.VN_2_CN_pos[CN][i];
                double total_LLR = receiver_LLR[VN];
                for(int j=0;j<H.max_col_arr[VN];j++){
                    int other_CN = H.CN_2_VN_pos[VN][j];
                    if(other_CN == CN) continue;
                    total_LLR += CN_2_VN_LLR[VN][j];
                }
                VN_2_CN_LLR[CN][i] = total_LLR;
            }
        }
        it++;  
    }
    
    /* ------- total LLR ------- */
    for(int VN=0;VN<H.n;VN++){
        guess[VN]=receiver_LLR[VN];
        for(int i=0;i<H.max_col_arr[VN];i++){
            guess[VN]+=CN_2_VN_LLR[VN][i];
        }
        // do the sigmoid
        guess[VN] = Sigmoid(guess[VN]);
    }
    std::cout << "output is :" <<std::endl;
    for(int VN=0;VN<H.n;VN++){
        std::cout << guess[VN] << " " ;
    }
    std::cout << std::endl;
    /* ----------- free all memory ----------- */
    std::cout << "In main Free" << std::endl;
    FreeAllH(&H);
    free(receiver_LLR);
    for(int i=0;i<H.n;i++) free(CN_2_VN_LLR[i]);
    free(CN_2_VN_LLR);
    for(int i=0;i<H.m;i++) free(VN_2_CN_LLR[i]);
    free(VN_2_CN_LLR);
    free(guess);
    
    return 0;
}

void FreeAllH(struct parity_check *H){
    std::cout << "Free All element with H." << std::endl;
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

void Read_File(struct parity_check *H,std::string& file_name_in){
    std::ifstream file_in(file_name_in + ".txt");
    if(file_in.fail()){
        std::cout << "** Error ----- " << file_name_in+".txt" << " can't open !!" <<std::endl;
        exit(1);
    }
    
    // struct parity_check *H=(struct parity_check*)malloc(sizeof(parity_check));
    file_in >> H->n >> H->m >> H->max_col_degree >> H->max_row_degree;
    
    std::cout << "Parity_Check_Matrix size  : " << H->m << " x " << H->n << std::endl;
    std::cout << "max_col_degree : " << H->max_col_degree << " | max_row_degree : " << H->max_row_degree << std::endl;
    H->max_col_arr=(int*)malloc(sizeof(int)*H->n);
    H->max_row_arr=(int*)malloc(sizeof(int)*H->m);
    std::cout << "Each col 1's number : ";
    for(int i=0;i<H->n;i++){
        file_in >> H->max_col_arr[i];
        if(H->max_col_arr[i] > H->max_col_degree){
            std::cout << "** Error ----- " << file_name_in << " max_col_array is Error , Please Check!!" << std::endl;
            goto FreeAll;
        }
        std::cout << H->max_col_arr[i] << " ";
    }
    std::cout << std::endl << "Each row 1's number : ";
    for(int i=0;i<H->m;i++){
        file_in >> H->max_row_arr[i];
        if(H->max_row_arr[i] > H->max_row_degree){
            std::cout << "** Error ----- " << file_name_in << " max_row_array is Error , Please Check!!" << std::endl;
            goto FreeAll;
        }
        std::cout << H->max_row_arr[i] << " ";
    }
    std::cout << std::endl;
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
            std::cout << pos << " " ; 
            H->CN_2_VN_pos[i][j]=pos-1;
        }
        std::cout << std::endl;
        if(H->max_col_arr[i]!=H->max_col_degree){
            H->CN_2_VN_pos[i] = (int*)realloc(H->CN_2_VN_pos[i],sizeof(int)*H->max_col_arr[i]);
        }
    }
    for(int i=0;i<H->m;i++){
        int pos;
        for(int j=0;j<H->max_row_degree;j++){
            file_in >> pos;
            std::cout << pos << " " ;
            H->VN_2_CN_pos[i][j]=pos-1;
        }
        std::cout << std::endl;
        if(H->max_row_arr[i]!=H->max_row_degree){
            H->VN_2_CN_pos[i] = (int*)realloc(H->VN_2_CN_pos[i],sizeof(int)*H->max_row_arr[i]);
        }
    }
    file_in.close();
    return;

    FreeAll: 
        std::cout << "In ReadFile Free All element " << std::endl;
        FreeAllH(H);
        file_in.close();
        exit(1);
}