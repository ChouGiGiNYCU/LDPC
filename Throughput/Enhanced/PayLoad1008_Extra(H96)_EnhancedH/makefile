CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = Enhanced_H
SRCS   = Enhanced_H.cpp
##########################
SNR_min         = -2
SNR_max         = 3
SNR_Ratio       = 0.25
Iteration_Limit = 200
##########################
payload_PCM  = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM    = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_96_48.txt
Combine_PCM  = PCM_P1008_EH96_EnhanceStruc_kSR.txt
Superposition_Payload_Extra_origin_file          = Table_Superposition_Extra_Payload.csv
Superpostion_Payload_Extra_NewStructure_pos_file = ExtraVNs_NewStrcuture.csv
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
extra_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/H_96_48_G.txt
###################################
Open_Iteration = 100
out_file = Troughput.csv

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(payload_PCM) $(extra_PCM)  $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration)   $(Superposition_Payload_Extra_origin_file)  $(Superpostion_Payload_Extra_NewStructure_pos_file)  $(out_file) 

clean:
	rm -f $(TARGET) 
	