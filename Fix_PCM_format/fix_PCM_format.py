file_name = r"C:\Users\USER\Desktop\LDPC\PCM\H_520_100_420_BG2_Z10.txt"
fileout_name = r"H_520_100_420_BG2_Z10_fix.txt"
with open(file_name,'r') as file :
    each_col_have_one_postion , each_row_have_one_postion = [],[]
    for idx,line in enumerate(file):
        line =line.strip().split()
        if idx==0:
            colsize = int(line[0])
            rowsize = int(line[1])
            print(f"rowsize : {rowsize}  | colsize : {colsize}")
            parity_check_matrix = [[0 for i in range(colsize)] for j in range(rowsize)]
        elif idx==1:
            max_col_one_number = int(line[0])
            max_row_one_number = int(line[1]) 
        elif idx==2 : # col 1 pos in each column
            column_poses = line
        elif idx==3:
            row_poses = line
        elif idx>3 and idx<=(3+colsize):
            tmp = []
            for row_pos in line:
                row_pos = int(row_pos)
                
                if row_pos==0:continue
                else:
                    tmp.append(row_pos)
            if len(tmp)!=max_col_one_number:
                tmp += [0]*(max_col_one_number-len(tmp))
            each_col_have_one_postion.append(tmp)
        else:
            tmp = []
            for col_pos in line:
                col_pos = int(col_pos)
                
                if col_pos==0:continue
                else:
                    tmp.append(col_pos)
            if len(tmp)!=max_row_one_number:
                tmp += [0]*(max_row_one_number-len(tmp))
            each_row_have_one_postion.append(tmp)
            
            
            
            
with open(fileout_name, "w") as f:
    
    f.write(f"{colsize} {rowsize}\n")
    f.write(f"{max_col_one_number} {max_row_one_number}\n")
    s = " ".join(map(str, column_poses))
    f.write(f"{s} \n")
    s = " ".join(map(str, row_poses))
    f.write(f"{s} \n")

    for line in each_col_have_one_postion:
        s = " ".join(map(str, line))
        f.write(f"{s} \n")
    for line in each_row_have_one_postion:
        s = " ".join(map(str, line))
        f.write(f"{s} \n")