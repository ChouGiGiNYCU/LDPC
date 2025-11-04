CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = 5G_RV_flow_Enhanced
SRCS   = 5G_RV_flow_Enhanced.cpp
##########################
SNR_min         = -4.5
SNR_max         = 4
SNR_Ratio       = 0.25
Iteration_Limit = 20
##########################                                                                                                                                                                                         
payload_PCM  = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_1904_616_1288_BG1_Z28_fix.txt
extra_PCM    = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_63_30.txt
Combine_PCM  = PCM_P5G1904_EBCH63x30_EnhanceStruc.txt
Z = 28
Superposition_Payload_Extra_origin_file          = Table_Superposition_Extra_Payload.csv
Superpostion_Payload_Extra_NewStructure_pos_file = ExtraVNs_NewStrcuture.csv
############# G setting ###########
# payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
payload_Generater_matrix_file = None
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_63_30_G.txt
###################################
RV0_pos = 2
RV1_pos = 19
RV2_pos = 35
RV3_pos = 58
Each_Tx_Znum = 30
###################################
Open_Iteration = 10
out_file = Troughput_Hybird_it20_10_10.csv

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(payload_PCM) $(extra_PCM)  $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration)   $(Superposition_Payload_Extra_origin_file)  $(Superpostion_Payload_Extra_NewStructure_pos_file)  $(out_file)  $(Z) $(RV0_pos) $(RV1_pos) $(RV2_pos) $(RV3_pos) $(Each_Tx_Znum)

clean:
	rm -f $(TARGET) 
	