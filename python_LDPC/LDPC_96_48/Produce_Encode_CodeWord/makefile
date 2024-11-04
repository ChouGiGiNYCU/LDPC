CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = Produce_TainSet_ZeroOrEncode
SRCS = Produce_TainSet_ZeroOrEncode.cpp
H_file_name = H_96_48.txt
G_file_name = None
Train_LLR_file_name = H_96_48_Train_signal.txt
Train_CodeWord_file_name = H_96_48_TrainCodeWord.txt
Valid_LLR_file_name = H_96_48_Valid_signal.txt
Valid_CodeWord_file_name = H_96_48_ValidCodeWord.txt
Valid_LLR_file_name_E = H_96_48_ValidLLR_E.txt
Valid_CodeWord_file_name_E = H_96_48_ValidCodeWord_E.txt
# ----------- 
Encode_Flag = 0
SNR_min = 3
SNR_max = 4.5
SNR_ratio = 0.1
# ----------- Produce train data set
batch_size = 30
total_batch_train = 2000
total_batch_valid = 100
 
train:
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread
	./$(TARGET) $(H_file_name)  $(Train_CodeWord_file_name)  $(Train_LLR_file_name)  $(SNR_min) $(SNR_max) $(SNR_ratio) $(total_batch_train) $(batch_size) $(Encode_Flag)
valid_Z:
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread
	./$(TARGET) $(H_file_name)  $(Valid_CodeWord_file_name)  $(Valid_LLR_file_name)  $(SNR_min) $(SNR_max) $(SNR_ratio) $(total_batch_valid) $(batch_size) 0
valid_E:
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread
	./$(TARGET) $(G_file_name)  $(Valid_CodeWord_file_name_E)  $(Valid_LLR_file_name_E)  $(SNR_min) $(SNR_max) $(SNR_ratio) $(total_batch_valid) $(batch_size) 1
clean:
	rm -f $(TARGET1) $(TARGET)
	