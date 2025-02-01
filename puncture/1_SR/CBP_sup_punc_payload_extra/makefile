CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra
SRCS = CBP_punc_PayLoad_Extra.cpp
##########################
SNR_min = 1
SNR_max = 4
SNR_Ratio = 0.2
Iteration_Limit = 200
##########################
payload_PCM = PEGReg504x1008.txt
extra_PCM = H_10_5.txt
Combine_PCM = P1008_E10_1SR.txt
PayLoad_out_file = Punc_1SR_PayLoad2_G.csv
Extra_out_file = Punc_1SR_Extra2_G.csv
puncture_pos_file = Pos_P1008_E10_1SR.txt
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
	