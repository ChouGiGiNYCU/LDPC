CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP
SRCS = CBP.cpp
##########################
SNR_min = 1
SNR_max = 3
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
PCM_file_1 = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
PCM_outfile_1 = CBP_PEG504X1008_it_$(Iteration_Limit)_v1.csv
PCM_file_2 = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_10_5.txt
PCM_outfile_2 = CBP_H10x5_it_$(Iteration_Limit).csv
PCM_file_3 = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_96_48.txt
PCM_outfile_3 = CBP_H96x48_it_$(Iteration_Limit).csv
PCM_file_4 = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_15_7.txt
PCM_outfile_4 = CBP_BCH15x7_it_$(Iteration_Limit).csv
PCM_file_5 = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_63_45.txt
PCM_outfile_5 = CBP_BCH63x45_it_$(Iteration_Limit).csv


############# G setting ###########
GM_file_1 = PEGReg504x1008_G.txt  # 
GM_Encdoe_Flag_1 = 0

GM_file_2 = None
GM_Encdoe_Flag_2 = 0

GM_file_3 = None
GM_Encdoe_Flag_3 = 0

GM_file_4 = None
GM_Encdoe_Flag_4 = 0

GM_file_5 = None
GM_Encdoe_Flag_5 = 0
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP_H10: $(TARGET)
	./$(TARGET) $(PCM_file_2)  $(PCM_outfile_2)  $(GM_file_2) $(GM_Encdoe_Flag_2) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   
BP_H1008: $(TARGET)
	./$(TARGET) $(PCM_file_1)  $(PCM_outfile_1)  $(GM_file_1) $(GM_Encdoe_Flag_1) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   
BP_H96: $(TARGET)
	./$(TARGET) $(PCM_file_3)  $(PCM_outfile_3)  $(GM_file_3) $(GM_Encdoe_Flag_3) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   
BP_BCH15: $(TARGET)
	./$(TARGET) $(PCM_file_4)  $(PCM_outfile_4)  $(GM_file_4) $(GM_Encdoe_Flag_4) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   
BP_BCH63: $(TARGET)
	./$(TARGET) $(PCM_file_5)  $(PCM_outfile_5)  $(GM_file_5) $(GM_Encdoe_Flag_5) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)   

clean:
	rm -f $(TARGET) 
	