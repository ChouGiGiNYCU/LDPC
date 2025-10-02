CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP
SRCS = CBP.cpp
##########################
SNR_min = 1
SNR_max = 2
SNR_Ratio = 0.5
Iteration_Limit = 200
##########################
PCM_file_1 = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_1904_616_1288_BG1_Z28_fix.txt
PCM_outfile_1 = CBP_H_1904_616_1288_BG1_Z28_it_$(Iteration_Limit).csv
############# G setting ###########
GM_file_1 = /mnt/c/Users/USER/Desktop/LDPC/GM/None
GM_Encdoe_Flag_1 = 0
###################################
Z = 28

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

   
BP: $(TARGET)
	./$(TARGET) $(PCM_file_1)  $(PCM_outfile_1)  $(GM_file_1) $(GM_Encdoe_Flag_1) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Z) 

	
clean:
	rm -f $(TARGET) 
	