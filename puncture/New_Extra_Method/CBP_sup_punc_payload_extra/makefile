CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra
SRCS = CBP_punc_PayLoad_Extra.cpp
##########################
SNR_min = 0
SNR_max = 2.8
SNR_Ratio = 0.2
Iteration_Limit = 200
##########################
payload_PCM = PEGReg504x1008.txt
extra_PCM = Extra_newPCM_From_PEG504.txt
Combine_PCM = PCM_P1008_E14_1SR.txt
PayLoad_out_file = Punc_1SR_P1008_E14_P_it$(Iteration_Limit)_2.csv
Extra_out_file = Punc_1SR_P1008_E14_E_it$(Iteration_Limit)_2.csv
Info_out_file = Pucn_1SR_P1008_E14_Info_it$(Iteration_Limit)_2.csv
puncture_pos_file = Pos_PCM_P1008_E14_1SR.txt
############# G setting ###########
payload_Generater_matrix_file = PEGReg504x1008_G.txt
extra_Generater_matrix_file = Extra_newG_From_PEG504.txt
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(payload_PCM) $(extra_PCM) $(puncture_pos_file) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Info_out_file)
	
clean:
	rm -f $(TARGET) 
	