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
Combine_PCM = PCM_Punc_kSR_Punc105.txt
RV1_punc_file = Pos_RV1_puncpos.txt
RV1_punc_nums = 105
payload_punc_file = Pos_payload.txt
payload_punc_nums = 105
############# G setting ###########
# payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
payload_Generater_matrix_file = None
# extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_15_7_G.txt
extra_Generater_matrix_file = None
###################################
Open_Iteration = 100
out_file = throughput_test.csv
Extra_PCM = 7
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(payload_PCM) $(extra_PCM) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration) $(RV1_punc_file) $(RV1_punc_nums) $(payload_punc_file) $(payload_punc_nums) $(out_file) $(Extra_PCM)

clean:
	rm -f $(TARGET) 
	