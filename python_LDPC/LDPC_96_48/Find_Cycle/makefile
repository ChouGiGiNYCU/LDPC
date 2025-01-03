CC = g++
TARGET = find_cycle_length
SRCS = find_cycle_length.cpp
H_file_name = H_96_48_2.txt
output_girth_file_name = H_96_48_girth_record.txt
cycle_num_record_file_name = H_96_48_cycle_num_record.txt

.PHONY: all clean

all: clean $(TARGET)
	./$(TARGET) $(H_file_name) $(output_girth_file_name) $(cycle_num_record_file_name)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
