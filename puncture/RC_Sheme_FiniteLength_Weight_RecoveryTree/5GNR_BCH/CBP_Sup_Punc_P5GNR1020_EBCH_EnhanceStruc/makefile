CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra_NewStructure_PartialExtraTransmit
SRCS   = CBP_punc_PayLoad_Extra_NewStructure_PartialExtraTransmit.cpp
##########################
SNR_min         = 1
SNR_max         = 2
SNR_Ratio       = 0.25
Iteration_Limit = 200
##########################
payload_PCM  = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_1020_330_690_BG1_Z15_fix.txt
Z            = 15
extra_PCM    = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_15_7.txt
Combine_PCM  = PCM_P5G1020_E15BCH_EnhanceStruc_50percent_RCScheme_WeightTree.txt
Superposition_Payload_Extra_origin_file          = Table_Superposition_Extra_Payload_50percent.csv
Table_ExtraTransmit_PayloadPunc_file             = Table_ExtraTransmitVNs_to_PuncPosPayload_50percent.csv
Superpostion_Payload_Extra_NewStructure_pos_file = ExtraVNs_NewStrcuture_50percent.csv

PayLoad_out_file = Punc_RCSchemeWeight_P5G1020_E15BCH_PayLoad_it$(Iteration_Limit)_Enhanced.csv
Extra_out_file   = Punc_RCSchemeWeight_P5G1020_E15BCH_Extra_it$(Iteration_Limit)_Enhanced.csv
Super_out_file   = Punc_RCSchemeWeight_P5G1020_E15BCH_Sup_it$(Iteration_Limit)_Enhanced.csv
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/H_1020_330_690_BG1_Z15_fix_G.txt
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_15_7_G.txt
###################################
Open_Iteration = 100


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(Super_out_file) $(payload_PCM) $(extra_PCM)  $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration)   $(Superposition_Payload_Extra_origin_file)  $(Table_ExtraTransmit_PayloadPunc_file) $(Superpostion_Payload_Extra_NewStructure_pos_file) $(Z) 

clean:
	rm -f $(TARGET) 
	