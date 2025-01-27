CC = g++
CFLAGS = -Wfatal-errors -std=c++11
############ File #################
TARGET = SUM_PRODUCT_ALG_test
SRCS = SUM_PRODUCT_ALG_test.cpp
############ Output file name ##############
paity_check_matrix_file = H_10_5.txt
test_file = test_LLR.txt


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

SPA:$(TARGET)
	./$(TARGET) $(paity_check_matrix_file) $(test_file) 

clean:
	rm -f $(TARGET) 