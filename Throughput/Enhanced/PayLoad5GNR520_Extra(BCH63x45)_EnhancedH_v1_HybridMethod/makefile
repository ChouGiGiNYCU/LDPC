CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = Enhanced_H
SRCS   = Enhanced_H.cpp
##########################
SNR_min         = -5
SNR_max         = -2.5
SNR_Ratio       = 0.5
Iteration_Limit = 200
##########################                                                                                                                                                                                         
payload_PCM  = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_520_100_420_BG2_Z10_fix.txt
extra_PCM    = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_63_45.txt
Combine_PCM  = PCM_P5G520_EBCH63x45_EnhanceStruc.txt
Z = 10
Superposition_Payload_Extra_origin_file          = Table_Superposition_Extra_Payload.csv
Superpostion_Payload_Extra_NewStructure_pos_file = ExtraVNs_NewStrcuture.csv
############# G setting ###########
# payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
payload_Generater_matrix_file = None
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_63_45_G.txt
###################################
RV1_punc_pos_file = RV1_punc_pos.txt
RV1_punc_nums = 63
###################################
Open_Iteration = 100
out_file = Troughput1.csv

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(payload_PCM) $(extra_PCM)  $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration)   $(Superposition_Payload_Extra_origin_file)  $(Superpostion_Payload_Extra_NewStructure_pos_file)  $(out_file) $(RV1_punc_pos_file) $(RV1_punc_nums) $(Z)

clean:
	rm -f $(TARGET) 
	