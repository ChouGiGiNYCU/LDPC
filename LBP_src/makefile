CC = g++
CFLAGS = -std=c++11
TARGET = LBP
SRCS = LBP.cpp
##########################
SNR_min = 1
SNR_max = 2.5
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
PCM_file_1 = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
PCM_outfile_1 = LBP_HPEG504x1008_it_200.csv
############# G setting ###########
GM_file_1 = /mnt/c/Users/USER/Desktop/LDPC/GM/H_10_5_G.txt
GM_Encdoe_Flag_1 = 0
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS)  -lboost_system -o  $(TARGET) 

   
LLBP: $(TARGET)
	./$(TARGET) $(PCM_file_1)  $(PCM_outfile_1)  $(GM_file_1) $(GM_Encdoe_Flag_1) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   

	
clean:
	rm -f $(TARGET) 
	