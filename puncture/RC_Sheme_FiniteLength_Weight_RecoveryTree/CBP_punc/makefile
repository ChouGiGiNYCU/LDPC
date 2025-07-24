CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc
SRCS = CBP_punc.cpp
##########################
SNR_min = 1.75
SNR_max = 3.5
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_parity_check_matrix = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
payload_out_csv_file1 = CBP_Punc_RCScheme_WeightedTree_P1008_It$(Iteration_Limit)_075.csv
punc_pos_file1 = Pos_puncture_Coderate_075.txt
Puncture_bits1 = 336
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt  # 
payload_Encdoe_Flag = 0
###################################

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP: $(TARGET)
	./$(TARGET) $(payload_parity_check_matrix)  $(payload_out_csv_file1)  $(payload_Generater_matrix_file) $(payload_Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)  $(punc_pos_file1) $(Puncture_bits1) 

clean:
	rm -f $(TARGET) 
	