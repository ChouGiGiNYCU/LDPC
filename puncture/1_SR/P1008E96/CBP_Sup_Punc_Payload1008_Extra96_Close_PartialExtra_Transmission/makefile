CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_Sup_Punc_PartialExtraTransmit
SRCS = CBP_Sup_Punc_PartialExtraTransmit.cpp
##########################
SNR_min = 1
SNR_max = 2.5
SNR_Ratio = 0.25
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_96_48.txt
Combine_PCM      = PCM_P1008_E96_PartialExtraTransmit_1SR.txt
PayLoad_out_file = Punc_1SR_P1008_E96_PayLoad_it$(Iteration_Limit)_CodeRateFix_PartialExtraTransmit.csv
Extra_out_file   = Punc_1SR_P1008_E96_Extra_it$(Iteration_Limit)_CodeRateFix_PartialExtraTransmit.csv
puncture_pos_file = Table_Superposition_Extra_Payload.csv
Table_ExtraTransmit_PayloadPunc_file = Table_ExtraTransmitVNs_to_PuncPosPayload.csv
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
# payload_Encdoe_Flag = 0
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/H_96_48_G.txt
# extra_Encdoe_Flag = 0
###################################
Open_Iteration = 100


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(payload_PCM) $(extra_PCM) $(puncture_pos_file) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration) $(Table_ExtraTransmit_PayloadPunc_file)

clean:
	rm -f $(TARGET) 
	