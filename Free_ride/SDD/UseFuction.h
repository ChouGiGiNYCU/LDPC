#ifndef UseFuction_H  
#define UseFuction_H  
#include<fstream>
#include<random>
#include<vector>
#include<iostream>
#include<chrono>
#include <stdexcept>
#include <sstream>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace std;
template <typename T>
vector<int> Vector_Dot_Matrix_Int(const vector<T>& vec,const vector<vector<T>>& matrix){
    // 1xk dot kxn = 1xn
    int vec_col = vec.size();
    int mat_row = matrix.size();
    if(vec_col!=mat_row){
        throw invalid_argument("-- Error : Vector_Dot_Matrix , Size of vector must be equal to number of rows in the matrix.");
    }
    
    vector<int> result(matrix[0].size(),0);
    for (size_t i = 0; i < vec.size(); ++i) {
        for (size_t j = 0; j < matrix[0].size(); ++j) {
            result[j] += vec[i] * matrix[i][j];
        }
    }
    return result;
}

template <typename T>
void print_2d_matrix(const vector<vector<T>>& matrix) {
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            cout << " " << matrix[i][j];
        }
        cout << endl;
    }
    cout << endl;
}
string strip(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == string::npos) {
        // 字符串全是空白
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}
vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);
    while (end != string::npos) {
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
bool path_exists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        // 路徑不存在
        return false;
    } else {
        // 路徑存在
        return true;
    }
}
bool directory_exists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false; // 路徑不存在
    } else if (info.st_mode & S_IFDIR) {
        return true; // 路徑存在且是目錄
    } else {
        return false; // 路徑存在但不是目錄
    }
}

bool create_directory(const std::string& path) {
    #ifdef _WIN32
    return _mkdir(path.c_str()) == 0;
    #else
    return mkdir(path.c_str(), 0755) == 0;
    #endif
}

bool create_directories(const std::string& path) {
    std::istringstream ss(path);
    std::string token;
    std::string current_path;

    while (std::getline(ss, token, '/')) {
        if (!current_path.empty()) {
            current_path += "/";
        }
        current_path += token;
        if (!directory_exists(current_path)) {
            if (!create_directory(current_path)) {
                std::cerr << "Failed to create directory: " << current_path << std::endl;
                return false;
            }
        }
    }
    return true;
}

std::string get_directory_path(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) {
        return path.substr(0, pos);
    }
    return path;
}
#endif