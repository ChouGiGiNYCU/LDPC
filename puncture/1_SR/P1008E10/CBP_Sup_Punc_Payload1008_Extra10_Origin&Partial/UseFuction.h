#ifndef UseFuction_H  
#define UseFuction_H  
#include<fstream>
#include <sstream>
#include<random>
#include<vector>
#include<chrono>
#include<iostream>
#include <stdexcept>
#include <boost/dynamic_bitset.hpp>

template <typename T>
std::vector<int> Vector_Dot_Matrix_Int(const std::vector<T>& vec,const std::vector<std::vector<T>>& matrix){
    // 1xk dot kxn = 1xn
    int vec_col = vec.size();
    int mat_row = matrix.size();
    if(vec_col!=mat_row){
        throw std::invalid_argument("-- Error : Vector_Dot_Matrix , Size of vector must be equal to number of rows in the matrix.");
    }
    std::vector<int> result(matrix[0].size(),0);
    for (size_t i = 0; i < vec.size(); ++i) {
        for (size_t j = 0; j < matrix[0].size(); ++j) {
            result[j] += vec[i] * matrix[i][j];
        }
    }
    return result;
}
std::vector<boost::dynamic_bitset<>> Transpose_Matrix(const std::vector<boost::dynamic_bitset<>>& mat) {
    if (mat.empty()) return {};
    size_t rows = mat.size();
    size_t cols = mat[0].size();
    std::vector<boost::dynamic_bitset<>> transposed(cols, boost::dynamic_bitset<>(rows));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (mat[i][j]) {
                transposed[j][i] = 1;
            }
        }
    }
    return transposed;
}
void GF2_Mat_Vec_Dot(const boost::dynamic_bitset<>& vec,const std::vector<boost::dynamic_bitset<>>& mat,boost::dynamic_bitset<>& result) {
    int idx = 0;
    for (const auto& row : mat) {
        // AND 後計算 1 的個數，再 mod 2（即 XOR 累加結果）
        auto and_result = row & vec;
        bool bit = and_result.count() & 1; // 等同於 %2，但更快
        result[idx++] = bit;
    }
    return;
}


template <typename T>
void print_2d_matrix(const std::vector<std::vector<T>>& matrix) {
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            std::cout << " " << matrix[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
std::string strip(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        // 字符串全是空白
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }
    tokens.push_back(s.substr(start));
    return tokens;
}
bool random_generation(){
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    // ready to generate random numbers
    double currentRandomNumber = unif(rng);
    return currentRandomNumber>0.5?true:false;
}


std::vector<std::pair<int,int>> Read_Extra2Pyalod_CSVfile(std::string& file_name){
    std::ifstream file(file_name);
    std::string line;
    std::vector<std::pair<int,int>> Map_Extra2Payload;
    // 讀第一行（標題）並略過
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string vn_str, girth_str;
        // 用逗號分割
        std::getline(ss, vn_str, ',');
        std::getline(ss, girth_str, ',');
        int Extra_vn   = std::stoi(vn_str) - 1; // idx 要變成 0 idx
        int Payload_vn = std::stoi(girth_str) - 1; // idx 要變成 0 idx
        Map_Extra2Payload.emplace_back(Extra_vn,Payload_vn);
    }

    // 印出確認
    // for (auto& node : Map_Extra2Payload) {
    //     int Extra_vn = node.first;
    //     int Payload_vn = node.second;
    //     cout << "Extra_vn : " << Extra_vn << ", Payload_vn : " << Payload_vn << "\n";
    // }

    return Map_Extra2Payload;
}


#endif