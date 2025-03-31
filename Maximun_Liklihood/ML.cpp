#include<iostream>
#include<fstream>
#include<cmath>
#include<vector>
#include <chrono>
#include <ctime>
#include<string>
#include<sstream>
#include<random>
#include <iomanip>  // 引入 iomanip 用於設定輸出格式
#include"random_number_generator.h"

#define Frame_Error_limit 400

using namespace std;

void show_time();
template <typename T>
vector<int> Vector_Dot_Matrix_Int(const vector<T>& vec,const vector<vector<T>>& matrix);
bool random_generation();
vector<bool> intToBinaryVector(int& value,const int& bit_length);

class ParityCheck {
public:
    int n, m;
    int max_col_degree, max_row_degree;
    int *max_col_arr, *max_row_arr;
    int **VN_2_CN_pos; // record 1 pos in each row = record col pos at ith row
    int **CN_2_VN_pos; // record 1 pos in each col = record row pos at ith col

    // Constructor
    ParityCheck() : n(0), m(0), max_col_degree(0), max_row_degree(0),
                    max_col_arr(nullptr), max_row_arr(nullptr),
                    VN_2_CN_pos(nullptr), CN_2_VN_pos(nullptr) {}
    // Destructor
    ~ParityCheck() {
        delete[] max_col_arr;
        delete[] max_row_arr;

        for (int i = 0; i < m; i++) {
            delete[] VN_2_CN_pos[i];
        }
        delete[] VN_2_CN_pos;

        for (int i = 0; i < n; i++) {
            delete[] CN_2_VN_pos[i];
        }
        delete[] CN_2_VN_pos;
    }

    // Function to read from file
    void readFromFile(const string &file_name) {
        ifstream file_in(file_name);
        if (file_in.fail()) {
            cerr << "** Error ----- " << file_name << " can't open !!" << endl;
            exit(1);
        }

        file_in >> n >> m >> max_col_degree >> max_row_degree;
        cout << "Parity_Check_Matrix size  : " << m << " x " << n << endl;
        cout << "max_col_degree : " << max_col_degree << " | max_row_degree : " << max_row_degree << endl;

        // Allocate memory
        max_col_arr = new int[n];
        max_row_arr = new int[m];

        cout << "Each col 1's number : ";
        for(int i = 0; i < n; i++) {
            file_in >> max_col_arr[i];
            if (max_col_arr[i] > max_col_degree) {
                cerr << "** Error ----- " << file_name << " max_col_array is Error , Please Check!!" << endl;
                exit(1);
            }
            cout << max_col_arr[i] << " ";
        }

        cout << "\nEach row 1's number : ";
        for (int i = 0; i < m; i++) {
            file_in >> max_row_arr[i];
            if (max_row_arr[i] > max_row_degree) {
                cerr << "** Error ----- " << file_name << " max_row_array is Error , Please Check!!" << endl;
                exit(1);
            }
            cout << max_row_arr[i] << " ";
        }
        cout << endl;

        // Allocate 2D arrays
        CN_2_VN_pos = new int*[n];
        VN_2_CN_pos = new int*[m];

        for (int i = 0; i < n; i++) {
            CN_2_VN_pos[i] = new int[max_col_degree]();
        }
        for (int i = 0; i < m; i++) {
            VN_2_CN_pos[i] = new int[max_row_degree]();
        }

        // Read CN_2_VN_pos
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < max_col_degree; j++) {
                int pos;
                file_in >> pos;
                CN_2_VN_pos[i][j] = pos - 1;
            }
            if (max_col_arr[i] != max_col_degree) {
                int *new_array = new int[max_col_arr[i]];
                copy(CN_2_VN_pos[i], CN_2_VN_pos[i] + max_col_arr[i], new_array);
                delete[] CN_2_VN_pos[i];
                CN_2_VN_pos[i] = new_array;
            }
        }

        // Read VN_2_CN_pos
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < max_row_degree; j++) {
                int pos;
                file_in >> pos;
                VN_2_CN_pos[i][j] = pos - 1;
            }
            if (max_row_arr[i] != max_row_degree) {
                int *new_array = new int[max_row_arr[i]];
                copy(VN_2_CN_pos[i], VN_2_CN_pos[i] + max_row_arr[i], new_array);
                delete[] VN_2_CN_pos[i];
                VN_2_CN_pos[i] = new_array;
            }
        }

        file_in.close();
    }

    // Function to print matrix info
    void print() const {
        cout << "Parity_Check_Matrix (" << m << " x " << n << ")\n";
        cout << "Max col degree: " << max_col_degree << ", Max row degree: " << max_row_degree << "\n";
        cout << "################\n";
    }
};

class GeneratorMatrix {
    public:
        int n, k, max_col;
        vector<vector<bool>> G;
        vector<int> max_col_map;
    
        // Constructor
        GeneratorMatrix() : n(0), k(0), max_col(0) {}
    
        // Function to strip spaces (similar to Python strip)
        static string strip(const string& str) {
            size_t first = str.find_first_not_of(" \t");
            if (first == string::npos) return "";
            size_t last = str.find_last_not_of(" \t");
            return str.substr(first, (last - first + 1));
        }
    
        // Function to split string by delimiter
        static vector<string> split(const string& str, char delimiter) {
            vector<string> result;
            stringstream ss(str);
            string token;
            while (getline(ss, token, delimiter)) {
                if (!token.empty()) result.push_back(token);
            }
            return result;
        }
    
        // Function to read generator matrix from file
        void readFromFile(const string& file_name) {
            ifstream file(file_name);
            if (!file.is_open()) {
                cerr << "** Error: Unable to open file " << file_name << " !!" << endl;
                exit(1);
            }
    
            string line;
            vector<string> line_split;
            int idx = 0;
    
            while (getline(file, line)) {
                line = strip(line);
                line_split = split(line, ' ');
    
                if (idx == 0) {  // First line: n, k
                    if (line_split.size() == 2) {
                        n = stoi(line_split[0]);
                        k = stoi(line_split[1]);
                        G.assign(k, vector<bool>(n, false));
                        cout << "G - n : " << n << " | k : " << k << "\n";
                    } else {
                        cerr << "** Error: First line (n,k) is not in correct format! [" << line << "]\n";
                        exit(1);
                    }
                } else if (idx == 1) {  // Second line: max_col
                    if (line_split.size() == 1) {
                        max_col = stoi(line_split[0]);
                        cout << "max col : "  << max_col << "\n";
                    } else {
                        cerr << "** Error: Second line (max_col) is not in correct format! [" << line << "]\n";
                        exit(1);
                    }
                } else if (idx == 2) {  // Third line: max_col_map
                    if (line_split.size() == static_cast<size_t>(n)) {
                        max_col_map.resize(n);
                        for (size_t i = 0; i < line_split.size(); i++) {
                            max_col_map[i] = stoi(line_split[i]);
                        }
                    } else {
                        cerr << "** Error: Third line (each col 1's number) does not match the column size of G !!\n";
                        exit(1);
                    }
                } else {  // Matrix content
                    if (line_split.size() == static_cast<size_t>(max_col_map[idx - 3])) {
                        for (const string& s : line_split) {
                            int pos = stoi(s);
                            int col = idx - 3;
                            if (pos != 0) G[pos - 1][col] = true; // 設定為 1
                        }
                    } else {
                        cerr << "** Error: Each Col 1 position line [" << line << "] length does not match " 
                                  << max_col << " | length: " << line_split.size() << "\n";
                        exit(1);
                    }
                }
                idx++;
            }
            file.close();
        }
    
        // Function to print matrix for debugging
        void print() const {
            cout << "Generator Matrix (" << k << " x " << n << ")\n";
            for (const auto& row : G) {
                for (bool val : row) {
                    cout << (val ? "1 " : "0 ");
                }
                cout << "\n";
            }
            cout << "################\n";
        }
    };

int main(int argc,char* argv[]){
    
    string PCM_file = argv[1];
    string G_file = argv[2];
    bool Encode_Flag = G_file==string("None")?false:true; 
    string Performance_file = argv[3]; 
    double SNR_min = atof(argv[4]);
    double SNR_max = atof(argv[5]);
    double SNR_ratio =  atof(argv[6]);

    cout << "##################\n";
    cout << "PCM_file : " << PCM_file << "\n";
    cout << "G_file : " << G_file << "\n";
    cout << "Encode_Flag : " << Encode_Flag << "\n";
    cout << "Performance_file : " << Performance_file << "\n";
    cout << "SNR_min : " << SNR_min << "\n";
    cout << "SNR_max : " << SNR_max << "\n";
    cout << "SNR_ratio : " << SNR_ratio << "\n";
    cout << "##################\n";
    if(Encode_Flag==false){
        cout << "Must Input G Matrix file!!\n";
        exit(1);
    }
    //  define PCM H
    ParityCheck H;
    H.readFromFile(PCM_file);
    
    //define G
    GeneratorMatrix G;
    if(Encode_Flag){
        G.readFromFile(G_file);
        G.print();
    }

    ofstream outFile(Performance_file);
    if (!outFile.is_open()) {
        cout << "Output file: " << Performance_file  << " cannot be opened." << endl;
		exit(1);
    }
    outFile << "Eb_over_N0" << ", " << "FER" << ", " << "BER" << endl;
   

    double SNR = SNR_min;
    double code_rate = static_cast<double>(H.n-H.m)/static_cast<double>(H.n);
    vector<double> Receive_signal(H.n,0);
    vector<bool> Infomation_bits(G.k,false);
    vector<int> Encode_CodeWord(H.n,0);
    
    while(SNR<=SNR_max){
        double BER_count = 0;
        double Frame_count = 0;
        double Frame_Error_count = 0;
        double sigma = sqrt(1/(2*code_rate*pow(10,SNR/10.0)));
        while(Frame_Error_count<Frame_Error_limit){
            if(Encode_Flag){
                // Assgin info bits
                for(int i=0;i<G.k;i++) Infomation_bits[i] = random_generation();
                Encode_CodeWord = Vector_Dot_Matrix_Int(Infomation_bits,G.G);
                for(int i=0;i<H.n;i++) Encode_CodeWord[i] = Encode_CodeWord[i] % 2;
            }else{
                Encode_CodeWord = vector<int>(H.n,0);
            }

            // BPSK & add noise
            for(int i=0;i<H.n;i++){
                // BPSK -> {0->1}、{1->-1}
                // double receiver_codeword=(-2*transmit_codeword[i]+1)/sigma + gasdev(); // power up
                double signal = static_cast<double>(-2*Encode_CodeWord[i]+1) + sigma*gasdev(); // fixed power
                Receive_signal[i] = signal;
            }

            int info_bits = G.G.size(); // rowsize
            int AllPossible = static_cast<int>(pow(2,info_bits));
            double min_distance = 1e9; 
            vector<int> Guess_Encode_CodeWord;
            vector<double> BPSK(H.n,0);
            // ML
            for(int k=0;k<AllPossible;k++){
                double soft_distance = 0;
                vector<bool> info_vector = intToBinaryVector(k,info_bits);
                
                vector<int> Encode_vector = Vector_Dot_Matrix_Int(info_vector,G.G);
                for(int i=0;i<H.n;i++) Encode_vector[i] = Encode_vector[i] % 2;
                for(int i=0;i<H.n;i++) BPSK[i] = static_cast<double>(-2*Encode_vector[i]+1);

                // count soft-distance
                for(int i=0;i<H.n;i++){
                    soft_distance = soft_distance + abs(Receive_signal[i] - BPSK[i]);
                }

                if(min_distance>soft_distance){
                    min_distance = soft_distance;
                    Guess_Encode_CodeWord = Encode_vector;
                }
            }

            // count BER
            bool Bit_Error_Flag = false;
            for(int i=0;i<H.n;i++){
                if(Encode_CodeWord[i]!=Guess_Encode_CodeWord[i]){
                    Bit_Error_Flag = true;
                    BER_count += 1;
                }
            }

            // count FER 
            if(Bit_Error_Flag) Frame_Error_count += 1;
            Frame_count += 1;
        }
        printf("SNR : %.2f | FER : %.5f | BER : %.5f | Total_Frame : %.0f | Time : ",SNR,Frame_Error_count/Frame_count,BER_count/(H.n*Frame_count),Frame_count);
        show_time();
        outFile << SNR << ", " << Frame_Error_count/Frame_count << ", " << BER_count/(H.n*Frame_count) << "\n";
        
        if(SNR==SNR_max) break;
        SNR = min(SNR+SNR_ratio,SNR_max);    
    }

    outFile.close();
    return 0;
}


void show_time(){
    auto now = chrono::system_clock::now();
    // 轉換為 time_t
    time_t now_time = chrono::system_clock::to_time_t(now);
    // 轉換為本地時間
    tm* local_time = localtime(&now_time);
    // 格式化輸出
    cout << local_time->tm_hour << ":" << local_time->tm_min << endl;
}

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

bool random_generation(){
    mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = chrono::high_resolution_clock::now().time_since_epoch().count();
    seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    uniform_real_distribution<double> unif(0, 1);
    // ready to generate random numbers
    double currentRandomNumber = unif(rng);
    return currentRandomNumber>0.5?true:false;
}

// int 轉成binary
vector<bool> intToBinaryVector(int& value,const int& bit_length) {
    vector<bool> binary;
    for (int i = bit_length - 1; i >= 0; i--) {
        int bit = (value >> i) & 1;   // 取每一個bit
        binary.push_back(bit);
    }
    return binary;
}