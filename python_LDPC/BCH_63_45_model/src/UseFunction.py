def Write_txt_File(file_name, parity_check_matrix_colsize, parity_check_matrix_rowsize, max_col_degree, max_row_degree, VN2CN, CN2VN_pos):
    with open(file_name, "w") as txt_file:
        txt_file.write("{} {} \n".format(parity_check_matrix_colsize ,parity_check_matrix_rowsize))
        txt_file.write("{} {} \n".format(max_col_degree, max_row_degree))
        for i in range(len(VN2CN)):
            txt_file.write("{} ".format(len(VN2CN[i])))
        txt_file.write("\n")
        for i in range(len(CN2VN_pos)):
            txt_file.write("{} ".format(len(CN2VN_pos[i])))
        txt_file.write("\n")
        for i in range(len(VN2CN)):
            for j in range(len(VN2CN[i])):
                txt_file.write("{} ".format(VN2CN[i][j]+1))
            if len(VN2CN[i]) != max_col_degree:
                for j in range(max_col_degree - len(VN2CN[i])):
                    txt_file.write("{} ".format(0))
            txt_file.write("\n")
        for i in range(len(CN2VN_pos)):
            for j in range(len(CN2VN_pos[i])):
                txt_file.write("{} ".format(CN2VN_pos[i][j]+1))
            if len(CN2VN_pos[i]) != max_row_degree:
                for j in range(max_row_degree - len(CN2VN_pos[i])):
                    txt_file.write("{} ".format(0))
            txt_file.write("\n")
        txt_file.write("\n")

def read_data(file_path):
    data = []
    with open(file_path, 'r') as file:
        for line in file:
            # 將每行的數值轉換成浮點數
            numbers = list(map(float, line.strip().split()))
            data.append(numbers)
    return data
