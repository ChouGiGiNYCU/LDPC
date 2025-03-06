CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_FreeRide_HDD
SRCS = CBP_FreeRide_HDD.cpp
##########################
SNR_min = 1
SNR_max = 4
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_10_5.txt
PayLoad_out_file = FreeRide_HDD_P1008_E10_PayLoad_G_it$(Iteration_Limit).csv
Extra_out_file = FreeRide_HDD_P1008_E10_Extra_G_it$(Iteration_Limit).csv

############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
payload_Encdoe_Flag = 1
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/H_10_5_G.txt
extra_Encdoe_Flag = 1
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(payload_PCM) $(PayLoad_out_file) $(extra_PCM) $(Extra_out_file)    $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)

test:
	g++ test.cpp -o test
	./test	
clean:
	rm -f $(TARGET) 
	rm -f test
	