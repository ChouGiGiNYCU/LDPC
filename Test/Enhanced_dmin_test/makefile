CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP
SRCS = CBP.cpp
##########################
SNR_min = -1
SNR_max = 6
SNR_Ratio = 0.5
Iteration_Limit = 200
##########################
PCM_file = New_BCH_Enhanced.txt
Origin_PCM_file = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_15_7.txt
PCM_outfile = CBP_BCH15x7_it_$(Iteration_Limit)_Enhanced.csv
############# G setting ###########
GM_file = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_15_7_G.txt
# GM_file = None
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

   
BP: $(TARGET)
	./$(TARGET) $(PCM_file) $(Origin_PCM_file) $(PCM_outfile)  $(GM_file)  $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   

	
clean:
	rm -f $(TARGET) 
	