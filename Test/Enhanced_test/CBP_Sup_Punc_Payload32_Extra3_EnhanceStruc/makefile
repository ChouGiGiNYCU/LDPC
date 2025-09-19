CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = Min_Distance_Enhanced
SRCS   = Min_Distance_Enhanced.cpp

############# PCM #################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_31_16.txt
extra_PCM   = /mnt/c/Users/USER/Desktop/LDPC/PCM/rep_1_3.txt

############# G setting ###########
payload_GM = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_31_16_G.txt
extra_GM   = /mnt/c/Users/USER/Desktop/LDPC/GM/rep_1_3_G.txt

###################################
Superposition_file = Table_Superposition_Extra_Payload.csv
Superposition_Enhanced_file = ExtraVNs_NewStrcuture.csv
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET)  $(payload_PCM) $(extra_PCM) $(payload_GM) $(extra_GM) $(Superposition_file) $(Superposition_Enhanced_file)

clean:
	rm -f $(TARGET) 
	