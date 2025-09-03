CC     = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = min_distance_col
SRCS   = min_distance_col.cpp

############# G setting ###########
Generater_matrix_file = /mnt/c/Users/USER/Desktop/LDPC/GM/H_10_5_G.txt


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

BP:$(TARGET)
	./$(TARGET)  $(Generater_matrix_file)

clean:
	rm -f $(TARGET) 
	