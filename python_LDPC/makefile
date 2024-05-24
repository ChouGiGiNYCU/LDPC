CC = g++
CFLAGS = -Wfatal-errors -std=c++11
TARGET = train_data_produce
SRCS = train_data_produce.cpp
TARGET1 = SUM_PRODUCT_ALG_test
SRCS1 = SUM_PRODUCT_ALG_test.cpp
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET)-lpthread
$(TARGET1): $(SRCS1)
	$(CC) $(CFLAGS) $(SRCS1)  -o  $(TARGET1) -lpthread
run0: $(TARGET)
	./$(TARGET) 0 5 0.1 10000 15 11 15 train_data_set
run1: $(TARGET1)
	./$(TARGET1) BCH_15_11 test
clean:
	rm -f $(TARGET) 