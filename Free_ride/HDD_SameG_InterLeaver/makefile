CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_FreeRide_HDD_SameG_InterLeaver
SRCS = CBP_FreeRide_HDD_SameG_InterLeaver.cpp
##########################
SNR_min = 2.25
SNR_max = 3
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
PayLoad_out_file = FreeRide_HDD_SameG_InterLeaver_P1008_ExtraInfo6_PayLoad_it$(Iteration_Limit)_v1.csv
Extra_out_file = FreeRide_HDD_SameG_InterLeaver_P1008_ExtraInfo6_Extra_it$(Iteration_Limit)_v1.csv

############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
payload_Encdoe_Flag = 1
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
extra_Encdoe_Flag = 1
###################################
info_bits = 6

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(payload_PCM) $(PayLoad_out_file) $(extra_PCM) $(Extra_out_file)    $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(info_bits)

clean:
	rm -f $(TARGET) 
	rm -f test
	