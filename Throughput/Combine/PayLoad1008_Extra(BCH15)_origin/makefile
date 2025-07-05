CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = Combine_H_throughput
SRCS = Combine_H_throughput.cpp
##########################
SNR_min = -2
SNR_max = 3
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_15_7.txt
Combine_PCM = PCM_Punc_kSR_Punc7.txt
Xor_Position_file = Pos_PCM_Punc_kSR_Punc15.txt
xor_num_bits = 15
puncture_pos_file = Pos_RV1_puncpos.txt
punc_vn_nums = 7;
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_15_7_G.txt
###################################
Open_Iteration = 100
out_file = throughput.csv

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(payload_PCM) $(extra_PCM) $(Xor_Position_file) $(xor_num_bits) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration) $(puncture_pos_file) $(punc_vn_nums) $(out_file)

clean:
	rm -f $(TARGET) 
	