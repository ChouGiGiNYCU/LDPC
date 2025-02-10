CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra
SRCS = CBP_punc_PayLoad_Extra.cpp
##########################
SNR_min = 1
SNR_max = 4
SNR_Ratio = 0.25
Iteration_Limit = 50
##########################
payload_PCM = PEGReg504x1008.txt
extra_PCM = H_10_5.txt
Combine_PCM = PCM_P1008_E10_kSR.txt
PayLoad_out_file = Punc_kSR_PayLoad_G_it$(Iteration_Limit).csv
Extra_out_file = Punc_kSR_Extra_G_it$(Iteration_Limit).csv
puncture_pos_file = Pos_PCM_P1008_E10_kSR.txt
############# G setting ###########
payload_Generater_matrix_file = PEGReg504x1008_G.txt 
payload_Encdoe_Flag = 1
extra_Generater_matrix_file = H_10_5_G.txt
extra_Encdoe_Flag = 1
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(payload_PCM) $(extra_PCM) $(puncture_pos_file) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
	
clean:
	rm -f $(TARGET) 
	