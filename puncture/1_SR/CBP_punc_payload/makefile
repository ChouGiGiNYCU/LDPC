CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc
SRCS = CBP_punc.cpp
##########################
SNR_min = 1
SNR_max = 4
SNR_Ratio = 0.2
Iteration_Limit = 200
##########################
payload_parity_check_matrix = PEGReg504x1008.txt
payload_out_csv_file = CBP_punc_PEG504X1008_it_$(Iteration_Limit).csv
punc_pos_file = Pos_P1008_E10_1SR.txt
############# G setting ###########
payload_Generater_matrix_file = PEGReg504x1008_G.txt  # 
payload_Encdoe_Flag = 0

###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP: $(TARGET)
	./$(TARGET) $(payload_parity_check_matrix)  $(payload_out_csv_file)  $(payload_Generater_matrix_file) $(payload_Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)  $(punc_pos_file) 

	
clean:
	rm -f $(TARGET) 
	