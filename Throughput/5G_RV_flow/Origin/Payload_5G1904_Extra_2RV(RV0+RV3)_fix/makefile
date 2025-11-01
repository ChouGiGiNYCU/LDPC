CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = 5G_RV_flow_Origin
SRCS = 5G_RV_flow_Origin.cpp
##########################
SNR_min = -4
SNR_max = 4.5
SNR_Ratio = 0.5
Iteration_Limit = 20
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_1904_616_1288_BG1_Z28_fix.txt
Z = 28
RV0_pos = 2
RV1_pos = 19
RV2_pos = 35
RV3_pos = 58
Each_Tx_Znum = 30
############# G setting ###########
# payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
payload_Generater_matrix_file = None

###################################
out_file = throughput_it20.csv

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(payload_PCM)  $(payload_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)  $(RV1_punc_file) $(RV1_punc_nums) $(out_file) $(Z)  $(RV0_pos) $(RV1_pos) $(RV2_pos) $(RV3_pos) $(Each_Tx_Znum)

clean:
	rm -f $(TARGET) 
	