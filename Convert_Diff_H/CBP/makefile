CC = g++
CFLAGS = -Wfatal-errors -std=c++11
####### BP
TARGET1 = CBP
SRCS1 = CBP.cpp
###### NMS
TARGET2 = NMS
SRCS2 = NMS.cpp
#####
TARGET3 = CBP_Combine
SRCS3 = CBP_Combine.cpp
###################################
SNR_min = 0
SNR_max = 6
SNR_Ratio = 0.5
Iteration_Limit = 50
H1_Iteration = 25
H2_Iteration = 25
###### PCM
paity_check_matrix_file1 = H_96_48.txt
paity_check_matrix_file2 = H_96_48_2.txt
paity_check_matrix_file3 = H_96_48_3.txt
paity_check_matrix_file4 = H_96_48_4.txt
####### BP
BP_csv1 = CBP_Result_H_96_48_it_$(Iteration_Limit).csv
BP_csv2 = CBP_Result_H2_96_48_it_$(Iteration_Limit).csv
BP_csv3 = CBP_Result_H3_96_48_it_$(Iteration_Limit).csv
BP_csv4 = CBP_Result_H4_96_48_it_$(Iteration_Limit).csv
###### NMS
NMS_file1 = NMS_Result_H_96_48_it_$(Iteration_Limit).csv
NMS_file2 = NMS_Result_H2_96_48_it_$(Iteration_Limit).csv
NMS_file3 = NMS_Result_H3_96_48_it_$(Iteration_Limit).csv
NMS_file4 = NMS_Result_H4_96_48_it_$(Iteration_Limit).csv
######
Combine2 = CBP_Combine_H2_96_48_it1_$(H1_Iteration)_it2_$(H2_Iteration)_it_$(Iteration_Limit).csv
Combine3 = CBP_Combine_H3_96_48_it1_$(H1_Iteration)_it2_$(H2_Iteration)_it_$(Iteration_Limit).csv
Combine4 = CBP_Combine_H4_96_48_it1_$(H1_Iteration)_it2_$(H2_Iteration)_it_$(Iteration_Limit).csv
Combine_test = CBP_Combine_HH_96_48_it1_$(H1_Iteration)_it2_$(H2_Iteration)_it_$(Iteration_Limit).csv
############# G setting ###########
Generater_matrix_file = None  # H_96_48_G.txt
Encdoe_Flag = 0


$(TARGET1): $(SRCS1)
	$(CC) $(CFLAGS)  $(SRCS1) -o  $(TARGET1) -lpthread

$(TARGET2): $(SRCS2)
	$(CC) $(CFLAGS)  $(SRCS2) -o  $(TARGET2) -lpthread

$(TARGET3): $(SRCS3)
	$(CC) $(CFLAGS)  $(SRCS3) -o  $(TARGET3) -lpthread

BP1:$(TARGET1)
	./$(TARGET1) $(paity_check_matrix_file1) $(BP_csv1) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
BP2:$(TARGET1)
	./$(TARGET1) $(paity_check_matrix_file2) $(BP_csv2) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
BP3:$(TARGET1)
	./$(TARGET1) $(paity_check_matrix_file3) $(BP_csv3) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
BP4:$(TARGET1)
	./$(TARGET1) $(paity_check_matrix_file4) $(BP_csv4) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
BP_test:$(TARGET1)
	./$(TARGET1) H_10_5.txt 2.csv $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)

NMS1:$(TARGET2)
	./$(TARGET2) $(paity_check_matrix_file1) $(NMS_file1) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
NMS2:$(TARGET2)
	./$(TARGET2) $(paity_check_matrix_file2) $(NMS_file2) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
NMS3:$(TARGET2)
	./$(TARGET2) $(paity_check_matrix_file3) $(NMS_file3) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)
NMS4:$(TARGET2)
	./$(TARGET2) $(paity_check_matrix_file4) $(NMS_file4) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio)


combine2:$(TARGET3)
	./$(TARGET3) $(paity_check_matrix_file1) $(paity_check_matrix_file2) $(Combine2) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(H1_Iteration) $(H2_Iteration)

combine3:$(TARGET3)
	./$(TARGET3) $(paity_check_matrix_file1) $(paity_check_matrix_file3) $(Combine3) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(H1_Iteration) $(H2_Iteration)

combine4:$(TARGET3)
	./$(TARGET3) $(paity_check_matrix_file1) $(paity_check_matrix_file4) $(Combine4) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(H1_Iteration) $(H2_Iteration)
combine_test:$(TARGET3)
	./$(TARGET3) $(paity_check_matrix_file1) $(paity_check_matrix_file1) $(Combine_test) $(Generater_matrix_file) $(Encdoe_Flag) $(Iteration_Limit) $(SNR_min) $(SNR_max) $(SNR_Ratio) $(H1_Iteration) $(H2_Iteration)



clean:
	rm -f $(TARGET1) 
	rm -f $(TARGET2)
	rm -f $(TARGET3)
	