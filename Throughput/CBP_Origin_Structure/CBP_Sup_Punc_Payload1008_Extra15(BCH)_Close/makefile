CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra
SRCS = CBP_punc_PayLoad_Extra.cpp
##########################
SNR_min = -2
SNR_max = 0
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/811/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM = /mnt/c/Users/811/Desktop/LDPC/PCM/BCH_15_7.txt
Combine_PCM = PCM_Punc_kSR_Punc15.txt
PayLoad_out_file = Punc_1SR_P1008_E15BCH_PayLoad_it$(Iteration_Limit).csv
Extra_out_file   = Punc_1SR_P1008_E15BCH_Extra_it$(Iteration_Limit).csv
puncture_pos_file = Pos_RV1_puncpos.txt
payload_punc_file = Pos_payload.txt
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/811/Desktop/LDPC/GM/PEGReg504x1008_G.txt
# payload_Encdoe_Flag = 0
extra_Generater_matrix_file = /mnt/c/Users/811/Desktop/LDPC/GM/BCH_15_7_G.txt
# extra_Encdoe_Flag = 0
###################################
Open_Iteration = 100


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(payload_PCM) $(extra_PCM) $(puncture_pos_file) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration) $(payload_punc_file)

clean:
	rm -f $(TARGET) 
	