#include<iostream>
#include"UseFuction.h"
#include<vector>
#include"random_number_generator.h"
using namespace std;

vector<vector<bool>> read_file(string& file_name){
    ifstream file(file_name);
    string line;
    vector<string> line_split;
    int idx=0,n,k,max_col;
    vector<vector<bool>> G;
    while (getline(file, line)) {
        line = strip(line);
        // cout << line << endl;
        line_split = split(line,' ');
        if(idx==0){
            if(line_split.size()==2){
                n = stoi(line_split[0]);
                k = stoi(line_split[1]);
                G = vector<vector<bool>>(k,vector<bool>(n,0));
            }else{
                cerr << "First line(n,k) : " << line << " is not n,k !!" << endl;
                exit(1);
            }
        }else if(idx==1){
            if(line_split.size()==1){
                max_col = stoi(line_split[0]);
            }else{
                cerr << "Second line(maxcol) : " << line << " is not max_col !!" << endl;
                exit(1);
            }
        }else if(idx==2){
            idx++;
        }else{
            
            for(string& s:line_split){
                int pos = stoi(s);
                if(pos!=0) G[pos-1][idx-2]=true; // 1
            }
            
        }
        idx++;
    }
    file.close();
    return G;
}

struct parity_check{
    int n,m,max_col_degree,max_row_degree;
    int *max_col_arr,*max_row_arr;
    int **VN_2_CN_pos; // record 1 pos in each row = record col pos at ith row
    int **CN_2_VN_pos; // record 1 pos in each col = record row pos at ith col
};

void FreeAllH(struct parity_check*H);
void Read_File_H(struct parity_check *H,string& file_name_in);

int main(int argc,char *argv[]){
    string  file_name = argv[1];
    string out_file_name_codeword = argv[2];
    string out_file_name_LLR = argv[3];
    double SNR_Min = atof(argv[4]);
    double SNR_Max = atof(argv[5]);
    double SNR_Ratio = atof(argv[6]);
    int total_batch = atoi(argv[7]);
    int batch = atoi(argv[8]);
    bool Encode_Flag = atoi(argv[9])==1?true:false;
    cout << "###############################" << endl;
    cout << "Read file name : " << file_name << endl;
    cout << "Train set file name -CodeWord : " << out_file_name_codeword << endl;
    cout << "Train set file name -LLR : " << out_file_name_LLR << endl;
    cout << "SNR_min : " << SNR_Min << endl;
    cout << "SNR_max : " << SNR_Max << endl;
    cout << "SNR_ratio : " << SNR_Ratio << endl;
    cout << "total_batch : " << total_batch << endl;
    cout << "batch : " << batch << endl;
    cout << "Encode_Flag : " << Encode_Flag << endl;
    cout << "###############################" << endl;
    /*
    if(!path_exists(file_name)){
        throw runtime_error("File don't exist - " + file_name);
    }
    string dir_path = get_directory_path(out_file_name_codeword);
    if (!directory_exists(dir_path)) {
        cout << "Path does not exist. Creating path: " << dir_path <<  endl;
        if (create_directories(dir_path)) {
            cout << "Path created successfully." <<  endl;
        } else {
            cerr << "Failed to create path." <<  endl;
            exit(1);
        }
    } else {
        cout << "Path already exists: " << dir_path <<  endl;
    }
    dir_path = get_directory_path(out_file_name_LLR);
    if (!directory_exists(dir_path)) {
        cout << "Path does not exist. Creating path: " << dir_path <<  endl;
        if (create_directories(dir_path)) {
            cout << "Path created successfully." <<  endl;
        } else {
            cerr << "Failed to create path." <<  endl;
            exit(1);
        }
    } else {
        cout << "Path already exists: " << dir_path <<  endl;
    }
    */
    vector<vector<bool>> G;
    double code_rate;
    struct parity_check H;
    if(Encode_Flag){
        G = read_file(file_name); // define G
        code_rate = (double)G.size() / (double)G[0].size(); // k/n
    }else{
        Read_File_H(&H,file_name); // define H
        code_rate = (double)(H.n-H.m)/(double)H.n;
    }
    cout << "Read File is ok " << endl;
    
    ofstream file_LLR(out_file_name_LLR);
    ofstream file_CodeWord(out_file_name_codeword);
    
    
    
    
    for(int batch_number = 0;batch_number<total_batch;batch_number++){
        for(double SNR=SNR_Min;SNR<=SNR_Max;SNR+=SNR_Ratio){
            double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
            cout << "SNR : " << SNR << " | sigma : " << sigma << endl; 
            for(int batch_number=0;batch_number<batch;batch_number++){
                if(Encode_Flag){
                    // information bit is k 
                    vector<bool> information_bits(G.size(),false);
                    vector<int> Encode_CodeWord;
                    for(int i=0;i<G.size();i++) information_bits[i] = random_generation();
                    Encode_CodeWord = Vector_Dot_Matrix_Int(information_bits,G);
                    for(int i=0;i<Encode_CodeWord.size();i++)  Encode_CodeWord[i]=((Encode_CodeWord[i]&1)==0)?0:1;
                    for(int i=0;i<Encode_CodeWord.size();i++){
                        // BPSK -> {0->1}、{1->-1}
                        double LLR = -2*Encode_CodeWord[i]+1+sigma*gasdev();   
                        LLR = (2*LLR)/pow(sigma,2); 
                        file_LLR << LLR << " ";
                        file_CodeWord << -2*Encode_CodeWord[i]+1 << " ";
                    }
                    file_LLR << endl;
                    file_CodeWord << endl;
                }else{
                    
                    for(int i=0;i<H.n;i++){
                        double LLR = (-2*0)+1+sigma*gasdev(); // ZeroCodeWord
                        // LLR =(2*LLR)/pow(sigma,2);
                        file_LLR << LLR << " ";
                        file_CodeWord << 1 << " ";
                    }
                    
                    file_LLR << endl;
                    file_CodeWord << endl;
                }
            }
        }
    }
    
        
    return 0;
}

void Read_File_H(struct parity_check *H,string& file_name_in){
    ifstream file_in(file_name_in);
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
        cout << "In ReadFile Free All element " << endl;
        FreeAllH(H);
        file_in.close();
        exit(1);
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
