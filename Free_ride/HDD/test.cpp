#include<vector>
#include<iostream>
#include"FreeRide_func.h"
using namespace std;

int main(){
    vector<double> t{1,2,3,4,5,-1};
    vector<int> HardDecsion_test = HardDecision(t);
    for(int i=0;i<HardDecsion_test.size();i++){
        cout << HardDecsion_test[i] << " ";
    }
    return 0;
}