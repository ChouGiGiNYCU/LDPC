CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = Min_Distance_Origin
SRCS   = Min_Distance_Origin.cpp

############# PCM #################
payload_PCM = /mnt/c/Users/USER/Desktop/LDPC/PCM/BCH_31_16.txt
extra_PCM   = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_10_5.txt

############# G setting ###########
payload_GM = /mnt/c/Users/USER/Desktop/LDPC/GM/BCH_31_16_G.txt
extra_GM   = /mnt/c/Users/USER/Desktop/LDPC/GM/H_10_5_G.txt

###################################
Superposition_file = Pos_payload.txt
extra_num = 10

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET)  $(payload_PCM) $(extra_PCM) $(payload_GM) $(extra_GM) $(Superposition_file) $(extra_num)

clean:
	rm -f $(TARGET) 
	