#ifndef FIND_POS
#define FIND_POS
#include<iostream>
#include<vector>
#include<unordered_map>
#include<set>
#include<string>
#include<fstream>
using namespace  std;

unordered_map<int,vector<int>> Read_File_H(string& H_file){
    ifstream file_in(H_file);
    if(file_in.fail()){
        cout << "** Error ----- " << H_file << " can't open !!" <<endl;
        exit(1);
    }
    unordered_map<int,vector<int>> vn_map; // record each vn connect cn node
    // unordered_map<int,vector<int>> cn_map; // record each cn connect vn node
    vector<int>  col_degree_map,row_degree_map;
    int colsize,rowsize,MaxColDegree,MaxRowDegree;
    
    //read row and col
    file_in >> colsize >> rowsize >> MaxColDegree >> MaxRowDegree;

    // read each col degree
    for(int i=0;i<colsize;i++){
        int col_degree;
        file_in >> col_degree;
        col_degree_map.push_back(col_degree);
        if(col_degree > MaxColDegree){
            cout << "** Error ----- " << H_file << " max_col_array is Error , Please Check!!" << endl;
            exit(1);
        }
    }
    
    // read each row degree
    for(int i=0;i<rowsize;i++){
        int row_degree;
        file_in >> row_degree;
        row_degree_map.push_back(row_degree);

        if(row_degree > MaxRowDegree){
            cout << "** Error ----- " << H_file << " max_row_array is Error , Please Check!!" << endl;
            exit(1);
        }
    }
    
    // read each vn connect to cns
    for(int vn=1;vn<=colsize;vn++){ // PCM檔案VN都是1開始
        int cn;
        for(int cn_num=0;cn_num<col_degree_map[vn];cn_num++){
            file_in >> cn;
            vn_map[vn].push_back(cn);
        }
    }

    file_in.close();
    return vn_map;
}

//利用recursive找各種vn組合
void generateVNSets(int n, int k, int start, vector<int>& currentVNSet,int& ChangeCNBitNum,unordered_map<int,vector<int>>& vn_map ) {
    if (currentVNSet.size() == k) {
        // 當前組合已達到 k 個數，加入結果
        // allSets.push_back(std::set<int>(currentSet.begin(), currentSet.end()));
        
        set<int> connect_cn_nodes;
        for(auto vn:currentVNSet){
            for(int cn:vn_map[vn]){
                connect_cn_nodes.insert(cn);
            }
        }
        if(connect_cn_nodes.size()==ChangeCNBitNum){
            // 可以在這裡增加一些判斷條件，來判斷這組vn set的組合好不好!!
            // 為什麼要每次 做recursive 這裡加上判斷 ， 如果一次找所有vn set組合 RAM會爆炸!!
            /*
                some condition function need to add
            */
            cout << "find a set : " << endl;
            cout << " vn : " ;
            for(int vn:currentVNSet){
                cout << vn <<  " ";
            }
            cout << " | cn : ";
            for(int cn:connect_cn_nodes){
                cout << cn <<  " ";
            }
            cout << endl;
        }
        return;
    }
    for (int i = start; i <= n; ++i) {
        currentVNSet.push_back(i);  // 將數字 i 加入當前組合
        generateVNSets(n, k, i + 1, currentVNSet, ChangeCNBitNum,vn_map);  // 遞迴生成下一層組合
        currentVNSet.pop_back();  // 回溯
    }
}

vector<set<int>> findAllSets(int& start_bit,int& n,int& MaxChangeVNBit,int& ChangeCNBitNum,unordered_map<int,vector<int>>& vn_map) {
    vector<set<int>> allSets;
    for(int k=start_bit;k<=MaxChangeVNBit;k++){
        vector<int> currentVNSet;
        generateVNSets(n, k, 1, currentVNSet, ChangeCNBitNum,vn_map);
    }
    return allSets;
}

void find_extra_vn_node(string& H_file,int& ChangeCNBitNum,int& MaxChangeVNBitNum,int start_bit){
    unordered_map<int,vector<int>> vn_map = Read_File_H(H_file); // record each vn connect cns
    vector<set<int>> CNs_Set; // record flip cn node
    vector<set<int>> VNs_Set; // record need to flip cn node
    int vn_node_num = vn_map.size();
    
    // 各種 vn 的組合先找出來 
    // vector<set<int>> AllVNSets = 
    findAllSets(start_bit,vn_node_num,MaxChangeVNBitNum,ChangeCNBitNum,vn_map);
    // for(auto vns_set:AllVNSets){
    //     set<int> connect_cn_nodes;
    //     for(auto vn:vns_set){
    //         for(int cn:vn_map[vn]){
    //             connect_cn_nodes.insert(cn);
    //         }
    //     }
    //     if(connect_cn_nodes.size()==ChangeCNBitNum){
    //         CNs_Set.push_back(set<int>(connect_cn_nodes.begin(), connect_cn_nodes.end()));
    //         VNs_Set.push_back(set<int>(vns_set.begin(),vns_set.end()));
    //     }
    // }
    // for(int idx=0;idx<CNs_Set.size();idx++){
    //     cout << " vn : " << idx << endl;
    //     for(int vn:VNs_Set[idx]){
    //         cout << vn <<  " ";
    //     }
    //     cout << " | cn : ";
    //     for(int cn:CNs_Set[idx]){
    //         cout << cn <<  " ";
    //     }
    // }
}


#endif