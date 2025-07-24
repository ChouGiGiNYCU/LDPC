CC = g++
TARGET = find_cycle_length
SRCS = find_cycle_length.cpp
H_file_name = /mnt/c/Users/USER/Desktop/LDPC/PCM/PEGReg504x1008.txt
output_girth_file_name = PEGReg504x1008_girth_record.txt
Each_VN_Girth_Record_file = Each_VN_Girth_Record_PEGReg504x1008.csv
find_cycle_length = 10

all: clean $(TARGET)
	./$(TARGET) $(H_file_name) $(output_girth_file_name) $(Each_VN_Girth_Record_file) $(find_cycle_length)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
