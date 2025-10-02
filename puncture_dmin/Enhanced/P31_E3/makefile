CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = Min_Distance_Enhance
SRCS   = Min_Distance_Enhance.cpp

##############################
GM  = PCM_P31_EH3_EnhanceStruc_kSR_G.txt
punc_pos_file = punc_pos.txt
punc_cnt = 12
###################################


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET)  $(GM) $(punc_pos_file) $(punc_cnt)

clean:
	rm -f $(TARGET) 
	