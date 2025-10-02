CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = ML
SRCS = ML.cpp
##########################
SNR_min = 1
SNR_max = 2.5
SNR_Ratio = 0.25
# Iteration_Limit = 50
##########################
PCM_file = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_15_7.txt
# PCM_outfile = ML_H10x5.csv
############# G setting ###########
GM_file = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_15_7_G.txt
PCM_outfile = ML_BCH_15_7.csv
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

   
M: $(TARGET)
	./$(TARGET) $(PCM_file) $(GM_file)  $(PCM_outfile)  $(SNR_min) $(SNR_max) $(SNR_Ratio)   

	
clean:
	rm -f $(TARGET) 
	