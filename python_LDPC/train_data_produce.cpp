#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cmath>
#include "random_number_generator.h"
using namespace std;
int main(int argc,char *argv[]){
    if(argc<9){
        cout << "Please input <SNR_min> <SNR_max> <SNR_ratio> <number> <N> <K> <codeword_length> <filename> " << endl;
        exit(1);
    }
    double SNR_min = atof(argv[1]);
    double SNR_max = atof(argv[2]);
    double SNR_ratio = atof(argv[3]);
    double number = atof(argv[4]);
    double N= atof(argv[5]);
    double K= atof(argv[6]);
    int codeword_length = atof(argv[7]);
    string file_name = argv[8];
    double code_rate = K/N;
    cout << "#####################################" << endl;
    cout << "SNR_min : " << SNR_min << endl;
    cout << "SNR_max : " << SNR_max << endl;
    cout << "SNR_ratio : " << SNR_ratio << endl;
    cout << "number : " << number << endl;
    cout << "codeword_length : " << codeword_length << endl;
    cout << "#####################################" << endl;
    ofstream outFile(file_name+".txt");
    if (!outFile.is_open()) {
        cout << "Output file: " << file_name+".txt" << " cannot be opened." << endl;
		exit(1);
    }
    for(double SNR=SNR_min;SNR<=SNR_max;SNR+=SNR_ratio){
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        for(int i=0;i<number;i++){
            for(int idx=0;idx<codeword_length;idx++){
                // BPSK -> {0->1}、{1->-1}
                double receiver_codeword=-2*0+1+sigma*gasdev();
                double Channel_LLR = receiver_codeword/pow(sigma,2);
                // cout << Channel_LLR << endl;
                outFile << Channel_LLR << " ";
            }
            outFile << endl;
        }
    }
}