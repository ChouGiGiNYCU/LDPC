CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP
SRCS = CBP.cpp
paity_check_matrix_file = H_10_5.txt
out_csv_file = CBP_H_10_5_it_5.csv
############# G setting ###########
Generater_matrix_file = None  # H_96_48_G.txt
Encdoe_Flag = 0
###################################
SNR_min = 0
SNR_max = 6
SNR_Ratio = 1
Iteration_Limit = 5

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread
SPA:$(TARGET)
	./$(TARGET) $(paity_check_matrix_file) $(out_csv_file) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)

clean:
	rm -f $(TARGET) 
	