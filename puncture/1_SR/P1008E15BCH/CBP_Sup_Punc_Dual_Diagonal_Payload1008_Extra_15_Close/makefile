CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra_DualDiagonal
SRCS =CBP_punc_PayLoad_Extra_DualDiagonal.cpp
##########################
SNR_min = 1
SNR_max = 2.75
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM   = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_15_7.txt
Combine_PCM       = PCM_P1008_BCH15_DualDiagonal_1SR.txt
puncture_pos_file = Pos_PCM_P1008_BCH15_DualDiagonal_1SR.txt
PayLoad_out_file = Punc_1SR_P1008_E15_PayLoad_it$(Iteration_Limit)_CodeRateFix_DualDiagonal_Open0.csv
Extra_out_file   = Punc_1SR_P1008_E15_Extra_it$(Iteration_Limit)_CodeRateFix_DualDiagonal_Open0.csv
Super_out_file   = Punc_1SR_P1008_E15_Sup_it$(Iteration_Limit)_CodeRateFix_DualDiagonal_Open0.csv
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_15_7_G.txt
###################################
Open_Iteration = 0


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(payload_PCM) $(extra_PCM) $(puncture_pos_file) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration) $(Super_out_file)

clean:
	rm -f $(TARGET) 
	