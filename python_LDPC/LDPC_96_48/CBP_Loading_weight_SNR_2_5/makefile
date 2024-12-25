CC = g++
CFLAGS = -Wfatal-errors -std=c++11
############ File and need change filk name #################
TARGET = CBP_Loading_weight
SRCS = CBP_Loading_weight.cpp
# out_csv_file = CBP_Result_LoadWeight_H_96_48.csv # change name
out_csv_file = CBP_Result_LoadWeight_H_96_48_encode.csv # change name
############ Output file name ##############
paity_check_matrix_file = H_96_48.txt
weight_file_name = H_96_48_weight_cn2vn.txt
bias_weight_file_name = H_96_48_bias.txt
final_layer_weight_file_name = H_96_48_Final_Layer_weight.txt
############# G setting ###########
Generater_matrix_file = H_96_48_G.txt
Encdoe_Flag = 0
###################################
SNR_min = 0
SNR_max = 6
SNR_Ratio = 0.5
frame_error_lowwer_bound = 200
Iteration_Limit = 5
##################################
quantize_int_bit = 2  # 量化 整數bit
quantize_frac_bit = 3 # 量化 小數bit
quantize_flag = 0 # 是否要用量化

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o  $(TARGET) -lpthread

SPA:$(TARGET)
	./$(TARGET) $(paity_check_matrix_file) $(out_csv_file) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(weight_file_name) $(final_layer_weight_file_name) $(bias_weight_file_name) $(quantize_int_bit) $(quantize_frac_bit) $(quantize_flag) $(frame_error_lowwer_bound)

clean:
	rm -f $(TARGET) 