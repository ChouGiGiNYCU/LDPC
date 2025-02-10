CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP
SRCS = CBP.cpp
##########################
SNR_min = 1
SNR_max = 4
SNR_Ratio = 0.2
Iteration_Limit = 200
##########################
payload_parity_check_matrix = PEGReg504x1008.txt
payload_out_csv_file = CBP_PEG504X1008_it_$(Iteration_Limit).csv
extra_paity_check_matrix_file = H_10_5.txt
extra_out_csv_file = CBP_H5x10_it_$(Iteration_Limit).csv
extra_paity_check_matrix_file2 = H_96_48.txt
extra_out_csv_file2 = CBP_H96x48_it_$(Iteration_Limit).csv
############# G setting ###########
payload_Generater_matrix_file = PEGReg504x1008_G.txt  # 
payload_Encdoe_Flag = 0

extra_Generater_matrix_file = None
extra_Encdoe_Flag = 0

extra_Generater_matrix_file2 = None
extra_Encdoe_Flag2 = 0
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP_E10: $(TARGET)
	./$(TARGET) $(extra_paity_check_matrix_file)  $(extra_out_csv_file)  $(extra_Generater_matrix_file) $(extra_Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   
BP_P1008: $(TARGET)
	./$(TARGET) $(payload_parity_check_matrix)  $(payload_out_csv_file)  $(payload_Generater_matrix_file) $(payload_Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   
BP_E96: $(TARGET)
	./$(TARGET) $(extra_paity_check_matrix_file2)  $(extra_out_csv_file2)  $(extra_Generater_matrix_file2) $(extra_Encdoe_Flag2) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   

	
clean:
	rm -f $(TARGET) 
	