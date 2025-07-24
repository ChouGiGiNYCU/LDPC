CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_decode_superposition
SRCS = CBP_decode_superposition.cpp
###################################
SNR_min = 1
SNR_max = 2.6
SNR_Ratio = 0.2
Iteration_Limit = 200
###################################
payload_parity_check_matrix = PEGReg504x1008.txt
payload_out_csv_file = CBP_Sup_PEG504X1008_it_$(Iteration_Limit).csv

extra_paity_check_matrix_file = H_10_5.txt
extra_out_csv_file = CBP_Sup_H5x10_it_$(Iteration_Limit).csv
############# G setting ###########
payload_Generater_matrix_file = PEGReg504x1008_G.txt  # 
payload_Encdoe_Flag = 1

extra_Generater_matrix_file = H_10_5_G.txt
extra_Encdoe_Flag = 1
###################################

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread
SPA:$(TARGET)
	./$(TARGET) $(payload_parity_check_matrix) $(payload_out_csv_file) $(extra_paity_check_matrix_file) $(extra_out_csv_file) $(payload_Generater_matrix_file)  $(payload_Encdoe_Flag) $(extra_Generater_matrix_file) $(extra_Encdoe_Flag)  $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)

SPA_H_10_5: 
	$(CC) $(CFLAGS)  CBP.cpp -o  CBP
	./CBP H_10_5.txt  H_10_5_performance.csv  None 0 10 0 10 1   
SPA_H_1008_504: 
	$(CC) $(CFLAGS)  CBP.cpp -o  CBP
	./CBP PEGReg504x1008.txt  H_1008_504_performance.csv  None 0 10 0 5 1   

	
clean:
	rm -f $(TARGET) 
	