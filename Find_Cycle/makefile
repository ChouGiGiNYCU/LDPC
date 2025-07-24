CC = g++
TARGET = find_cycle_length
SRCS = find_cycle_length.cpp
H_file_name = /mnt/c/Users/USER/Desktop/LDPC/PCM/H_10_5.txt
output_girth_file_name = H_10_5_girth_record.txt
Each_VN_Girth_Record_file = Each_VN_Girth_Record_H10x5.csv


all: clean $(TARGET)
	./$(TARGET) $(H_file_name) $(output_girth_file_name) $(Each_VN_Girth_Record_file)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
