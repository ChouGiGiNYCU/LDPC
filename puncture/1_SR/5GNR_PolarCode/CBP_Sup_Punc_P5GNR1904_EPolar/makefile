CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra
SRCS = CBP_punc_PayLoad_Extra.cpp
##########################
SNR_min = 1
SNR_max = 2.5
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
Open_Iteration = 100
###################################
payload_PCM       = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_1904_616_1288_BG1_Z28_fix.txt
Z                 = 28
extra_PCM         = Dence_PCM.txt
Combine_PCM       = PCM_5GNR1904_Polar16_1SR.txt
puncture_pos_file = Punc_Pos_PCM_5GNR1904_Polar16_1SR.txt
PayLoad_out_file  = Punc_1SR_P1904_E16_PayLoad_it$(Iteration_Limit)_Open$(Open_Iteration).csv
Extra_out_file    = Punc_1SR_P1904_E16_Extra_it$(Iteration_Limit)_Open$(Open_Iteration).csv
Frozen_bits_file  = frozen_bits_pos.txt
Non_Frozen_bits_file  = non_frozen_bits_pos.txt
############# G setting ###########
payload_Generater_matrix_file = None
# payload_Encdoe_Flag = 0
###################################



$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(payload_PCM) $(extra_PCM) $(puncture_pos_file) $(payload_Generater_matrix_file)  $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration) $(Z) $(Frozen_bits_file) $(Non_Frozen_bits_file) 

clean:
	rm -f $(TARGET) 
	