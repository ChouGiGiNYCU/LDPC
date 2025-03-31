CC = g++
CFLAGS = -std=c++11
TARGET = CBP
SRCS = CBP.cpp
##########################
SNR_min = 1
SNR_max = 4
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
PCM_file_1 = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_10_5.txt
PCM_outfile_1 = CBP_H5x10_it_$(Iteration_Limit).csv
############# G setting ###########
GM_file_1 = /mnt/c/Users/USER/Desktop/LDPC/GM/H_10_5_G.txt
GM_Encdoe_Flag_1 = 1
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS)  -lboost_system -o  $(TARGET) 

   
BP: $(TARGET)
	./$(TARGET) $(PCM_file_1)  $(PCM_outfile_1)  $(GM_file_1) $(GM_Encdoe_Flag_1) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   

	
clean:
	rm -f $(TARGET) 
	