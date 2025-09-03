CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = Origin_H_throughput
SRCS = Origin_H_throughput.cpp
##########################
SNR_min = -2
SNR_max = 3
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
RV1_punc_file = Pos_PCM_P1008_7E15BCH_1SR.txt
RV1_punc_nums = 105
############# G setting ###########
# payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
payload_Generater_matrix_file = None
###################################
out_file = throughput1.csv

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(payload_PCM)  $(payload_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)  $(RV1_punc_file) $(RV1_punc_nums) $(out_file)

clean:
	rm -f $(TARGET) 
	