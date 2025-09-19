CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = CBP_punc_PayLoad_test
SRCS = CBP_punc_PayLoad_test.cpp
##########################
SNR_min = -1
SNR_max = 6
SNR_Ratio = 0.5
Iteration_Limit = 200
##########################
Combine_PCM       = H_combine.txt
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_10_5.txt
PayLoad_out_file = Combine_payload.csv

puncture_pos_file = Pos_puncture_1SR.txt
############# G setting ###########
payload_GM = /mnt/c/Users/USER/Desktop/LDPC/GM/H_10_5_G.txt
# payload_Encdoe_Flag = 0

###################################
Open_Iteration = 100
punc_nums = 3

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET) $(Combine_PCM) $(payload_PCM) $(payload_GM) $(PayLoad_out_file) $(puncture_pos_file) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(punc_nums)

clean:
	rm -f $(TARGET) 
	