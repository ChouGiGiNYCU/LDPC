CC = g++
TARGET = find_cycle_length
SRCS = find_cycle_length.cpp
H_file_name = H_96_48.txt
output_girth_file_name = H_96_48_girth_record.txt
cycle_num_record_file_name = H_96_48_cycle_num_record.txt
$(TARGET): $(SRCS)
	$(CC)  $(SRCS) -o  $(TARGET)

all: $(TARGET)
	./$(TARGET) $(H_file_name) $(output_girth_file_name) $(cycle_num_record_file_name)

clean:
	rm -f $(TARGET) 
	
