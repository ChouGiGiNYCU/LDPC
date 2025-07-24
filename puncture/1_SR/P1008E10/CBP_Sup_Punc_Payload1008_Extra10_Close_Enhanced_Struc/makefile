CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra_NewStructure_PartialExtraTransmit
SRCS   = CBP_punc_PayLoad_Extra_NewStructure_PartialExtraTransmit.cpp
##########################
SNR_min         = 1
SNR_max         = 2.25
SNR_Ratio       = 0.25
Iteration_Limit = 200
##########################
payload_PCM  = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM    = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_10_5.txt
Combine_PCM  = PCM_P1008_E10_EnhanceStruc_50percent.txt
Superposition_Payload_Extra_origin_file          = Table_Superposition_Extra_Payload_50percent.csv
Table_ExtraTransmit_PayloadPunc_file             = Table_ExtraTransmitVNs_to_PuncPosPayload_50percent.csv
Superpostion_Payload_Extra_NewStructure_pos_file = ExtraVNs_NewStrcuture_50percent.csv

PayLoad_out_file = Punc_1SR_P1008_E10_PayLoad_it$(Iteration_Limit)_Enhaced_Struc.csv
Extra_out_file   = Punc_1SR_P1008_E10_Extra_it$(Iteration_Limit)_Enhaced_Struc.csv
Super_out_file   = Punc_1SR_P1008_E10_Sup_it$(Iteration_Limit)_Enhaced_Struc.csv
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
extra_Generater_matrix_file   = /mnt/c/Users/USER/Desktop/LDPC/GM/H_10_5_G.txt
###################################
Open_Iteration = 100


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(Super_out_file) $(payload_PCM) $(extra_PCM)  $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration)   $(Superposition_Payload_Extra_origin_file)  $(Table_ExtraTransmit_PayloadPunc_file) $(Superpostion_Payload_Extra_NewStructure_pos_file)

clean:
	rm -f $(TARGET) 
	