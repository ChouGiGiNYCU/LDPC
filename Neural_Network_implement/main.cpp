#include<string>
#include<vector>
#include"model_function.h"
using namespace  std;

int main(){
    string fc1_weightfile = "fc1.weight.txt";
    string fc1_biasfile =  "fc1.bias.txt";
    string fc2_weightfile = "fc2.weight.txt";
    string fc2_biasfile =  "fc2.bias.txt";
    Fully_Connect FC1(8,3,"fc1",fc1_weightfile,fc1_biasfile);
    Fully_Connect FC2(3,1,"fc2",fc2_weightfile,fc2_biasfile);
    
    return 0;
}