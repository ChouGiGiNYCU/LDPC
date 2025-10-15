CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = 5G_RV_flow_Origin
SRCS = 5G_RV_flow_Origin.cpp
##########################
SNR_min = -4
SNR_max = -1
SNR_Ratio = 0.5
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_1904_616_1288_BG1_Z28_fix.txt
Z = 28
RV0_pos = 2
RV1_pos = 19
RV2_pos = 35
RV3_pos = 58
############# G setting ###########
# payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
payload_Generater_matrix_file = None

###################################
out_file = throughput1.csv

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(payload_PCM)  $(payload_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)  $(RV1_punc_file) $(RV1_punc_nums) $(out_file) $(Z)  $(RV0_pos) $(RV1_pos) $(RV2_pos) $(RV3_pos)

clean:
	rm -f $(TARGET) 
	