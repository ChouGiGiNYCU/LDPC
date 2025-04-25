CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_Extra_NewStructure
SRCS   = CBP_punc_PayLoad_Extra_NewStructure.cpp
##########################
SNR_min         = 1
SNR_max         = 2.5
SNR_Ratio       = 0.25
Iteration_Limit = 200
##########################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
extra_PCM   = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_96_48.txt
Combine_PCM        = PCM_P1008_E96_Structure.txt
puncture_pos_file  = Punc_Superpostion_origin_Payload.txt
Superpostion_Payload_Extra_NewStructure_pos_file = Punc_Superpostion_new_Payload.csv
PayLoad_out_file = Punc_1SR_P1008_E96_PayLoad_it$(Iteration_Limit)_Structure_Open100.csv
Extra_out_file   = Punc_1SR_P1008_E96_Extra_it$(Iteration_Limit)_Structure_Open100.csv
Super_out_file   = Punc_1SR_P1008_E96_Sup_it$(Iteration_Limit)_Structure_Open100.csv
############# G setting ###########
payload_Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/PEGReg504x1008_G.txt
extra_Generater_matrix_file   = /mnt/c/Users/USER/Desktop/LDPC/GM/H_96_48_G.txt
###################################
Open_Iteration = 100


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(PayLoad_out_file) $(Extra_out_file) $(payload_PCM) $(extra_PCM) $(puncture_pos_file) $(payload_Generater_matrix_file) $(extra_Generater_matrix_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(Open_Iteration) $(Super_out_file)  $(Superpostion_Payload_Extra_NewStructure_pos_file)

clean:
	rm -f $(TARGET) 
	