CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc
SRCS = CBP_punc.cpp
##########################
SNR_min = 1
SNR_max = 3
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_parity_check_matrix = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
payload_out_csv_file1 = CBP_Punc_P1008_E96_it_$(Iteration_Limit).csv
punc_pos_file1 = Pos_PCM_P1008_E96_1SR.txt
payload_out_csv_file2 = CBP_Punc_P1008_E10_it_$(Iteration_Limit).csv
punc_pos_file2 = Pos_PCM_P1008_E10_1SR.txt

############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt  # 
payload_Encdoe_Flag = 0
###################################
Puncture_bits1 = 96
Puncture_bits2 = 10

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP_P96: $(TARGET)
	./$(TARGET) $(payload_parity_check_matrix)  $(payload_out_csv_file1)  $(payload_Generater_matrix_file) $(payload_Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)  $(punc_pos_file1) $(Puncture_bits1) 

BP_P10: $(TARGET)
	./$(TARGET) $(payload_parity_check_matrix)  $(payload_out_csv_file2)  $(payload_Generater_matrix_file) $(payload_Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)  $(punc_pos_file2) $(Puncture_bits2) 
	
clean:
	rm -f $(TARGET) 
	