CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP
SRCS = CBP.cpp
paity_check_matrix_file = H_96_48.txt
out_csv_file = CBP_ReceiveLLR_H_96_48_it_40.csv
############# G setting ###########
Generater_matrix_file = None
Encdoe_Flag = 0
###################################
SNR_min = 0
SNR_max = 12
SNR_Ratio = 0.5
Iteration_Limit = 40

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread
SPA:$(TARGET)
	./$(TARGET) $(paity_check_matrix_file) $(out_csv_file) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
#	./$(TARGET) ../Parity_check_Matrix/BCH_63_45.txt  CBP_Result_Encdoe 0 7 1 5 1 ../Encode_CodeWord/BCH_63_45/BCH_63_45_EncodeCodeWord_SNR_ ../Encode_CodeWord/BCH_63_45/BCH_63_45_EncodeCodeWord_LLR_SNR_

clean:
	rm -f $(TARGET) 
	