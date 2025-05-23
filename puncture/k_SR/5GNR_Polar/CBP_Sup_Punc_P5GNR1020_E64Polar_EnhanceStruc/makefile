CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra_NewStructure_PartialExtraTransmit
SRCS   = CBP_punc_PayLoad_Extra_NewStructure_PartialExtraTransmit.cpp
##########################
SNR_min         = 2
SNR_max         = 2.5
SNR_Ratio       = 0.25
Iteration_Limit = 200
##########################
payload_PCM  = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_1020_330_690_BG1_Z15_fix.txt
Z            = 15
extra_PCM    = Polar64_Dence_PCM.txt
Combine_PCM  = PCM_P5G1020_E64Polr_EnhanceStruc_50percent_kSR.txt
Superposition_Payload_Extra_origin_file          = Table_Superposition_Extra_Payload_50percent.csv
Table_ExtraTransmit_PayloadPunc_file             = Table_ExtraTransmitVNs_to_PuncPosPayload_50percent.csv
Superpostion_Payload_Extra_NewStructure_pos_file = ExtraVNs_NewStrcuture_50percent.csv

PayLoad_out_file = Punc_kSR_P5G1020_E64Polar_PayLoad_it$(Iteration_Limit)_Enhance_Open100.csv
Extra_out_file   = Punc_kSR_P5G1020_E64Polar_Extra_it$(Iteration_Limit)_Enhance_Open100.csv
Super_out_file   = Punc_kSR_P5G1020_E64Polar_Sup_it$(Iteration_Limit)_Enhance_Open100.csv
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/H_1020_330_690_BG1_Z15_fix_G.txt

Frozen_bits_file      = Polar64_Frozen_Bits_pos.txt
Non_Frozen_bits_file  = Polar64_non_frozen_bits_pos.txt
###################################
Open_Iteration = 100


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(Super_out_file) $(payload_PCM) $(extra_PCM)  $(payload_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration)   $(Superposition_Payload_Extra_origin_file)  $(Table_ExtraTransmit_PayloadPunc_file) $(Superpostion_Payload_Extra_NewStructure_pos_file) $(Z) $(Frozen_bits_file) $(Non_Frozen_bits_file)

clean:
	rm -f $(TARGET) 
	