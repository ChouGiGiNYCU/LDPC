CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra
SRCS = CBP_punc_PayLoad_Extra.cpp
##########################
SNR_min = -1
SNR_max = 6
SNR_Ratio = 0.5
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_96_48.txt
extra_PCM   = /mnt/c/Users/USER/Desktop/LDPC/PCM/rep_1_3.txt
Combine_PCM       = H_combine_wirh_dmin.txt
puncture_pos_file = Pos_puncture.txt
PayLoad_out_file = Combine_payload_dmin.csv
Extra_out_file   = Combine_extra_dmin.csv

############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/H_96_48_G.txt
# payload_Encdoe_Flag = 0
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/rep_1_3_G.txt
# extra_Encdoe_Flag = 0
###################################
Open_Iteration = 100


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(payload_PCM) $(extra_PCM) $(puncture_pos_file) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration)

clean:
	rm -f $(TARGET) 
	