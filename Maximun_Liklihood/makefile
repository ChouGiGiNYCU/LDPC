CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = ML
SRCS = ML.cpp
##########################
SNR_min = 1
SNR_max = 3
SNR_Ratio = 0.5
# Iteration_Limit = 50
##########################
PCM_file = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_10_5.txt
PCM_outfile = ML_H10x5.csv
############# G setting ###########
GM_file = /mnt/c/Users/USER/Desktop/LDPC/GM/H_10_5_G.txt
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

   
M: $(TARGET)
	./$(TARGET) $(PCM_file) $(GM_file)  $(PCM_outfile)  $(SNR_min) $(SNR_max) $(SNR_Ratio)   

	
clean:
	rm -f $(TARGET) 
	