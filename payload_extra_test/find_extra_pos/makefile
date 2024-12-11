CLANGE = g++
SOURCE = main.cpp
TARGET = main
H_FILE_TXT = PEGReg504x1008.txt
## init parameter
CN_NUM_CHANGE = 6 
SET_START_NUM = 3
VN_MAX_CHANGE_NUM = 5
# 注意一下自己的PCM 的 DEGREE 設定好的初始值 thanks

all :
	$(CLANGE)  $(SOURCE) -o $(TARGET)
	./$(TARGET) $(H_FILE_TXT) $(CN_NUM_CHANGE) $(VN_MAX_CHANGE_NUM) $(SET_START_NUM)
clean:
	rm -f $(TARGET) 
