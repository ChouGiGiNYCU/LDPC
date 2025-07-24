def read_H_file(file_name):
    curr_col=0
    with open(file_name,'r') as file :
        for idx,line in enumerate(file):
            line =line.strip().split()
            if idx==0:
                colsize = int(line[0])
                rowsize = int(line[1])
                print(f"rowsize : {rowsize}  | colsize : {colsize}")
                parity_check_matrix = [[0 for i in range(colsize)] for j in range(rowsize)]
            elif idx==1:
                max_col_one_number = int(line[0])
                max_col_row_number = int(line[1]) 
            elif idx==2 : # col 1 pos in each column
                column_pos = line
            elif idx==3:
                row_pos = line
            else:
                for row_pos in line:
                    row_pos = int(row_pos)
                    if row_pos==0:continue
                    else:
                        parity_check_matrix[row_pos-1][curr_col] = 1
                curr_col+=1
                
                if curr_col >= colsize:
                    break

    return parity_check_matrix


def read_G_file(file_name):
    curr_col = 0
    try:
        with open(file_name,'r') as file :
            for idx,line in enumerate(file):
                line =line.strip().split()
                if idx==0:
                    colsize = int(line[0])
                    rowsize = int(line[1])
                    print(f"G rowsize : {rowsize}  | colsize : {colsize}")
                    G_matrix = [[0 for i in range(colsize)] for j in range(rowsize)]
                elif idx==1:
                    max_col_one_number = int(line[0])
                elif idx==2 : # col 1 pos in each column
                    column_pos = line
                else:
                    for row_pos in line:
                        row_pos = int(row_pos)
                        if row_pos==0:continue
                        else:
                            G_matrix[row_pos-1][curr_col] = 1
                    curr_col+=1
    except:
        EOFError(f"Can't open {file_name} !!")
    return G_matrix