%%
clc;
clear all;
close all;
%%
Payload_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
Extra_file = 'C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt'; % extra data
H_combine_file = 'PCM_P1008_7E15(BCH)_1SR.txt';
Combine_number = 7;
puncture_position_bits_file = ['Pos_', H_combine_file];
Payload_H = readHFromFileByLine(Payload_file);
Extra_H = readHFromFileByLine(Extra_file);
[Payload_H_r,Payload_H_c] = size(Payload_H);
[Extra_H_r,Extra_H_c] = size(Extra_H);
% H_combine = [  P_H    zero1   zero1   zero1  zero1  zero1   zero1  zero1 | zero1  zero1  zero1  zero1.. 
%               zero3     H1    zero2   zero2  zero2  zero2   zero2  zero2 | zero2  zero2  zero2  zero2..  
%               zero3   zero2     H2    zero2  zero2  zero2   zero2  zero2 | zero2  zero2  zero2  zero2..    
%               zero3   zero2   zero2     H3   zero2  zero2   zero2  zero2 | zero2  zero2  zero2  zero2.. 
%               zero3   zero2   zero2   zero2    H4   zero2   zero2  zero2 | zero2  zero2  zero2  zero2.. 
%               zero3   zero2   zero2   zero2   zero2   H5    zero2  zero2 | zero2  zero2  zero2  zero2..   
%               zero3   zero2   zero2   zero2   zero2  zero2   H6    zero2 | zero2  zero2  zero2  zero2..  
%               zero3   zero2   zero2   zero2   zero2  zero2  zero2    H7  | zero2  zero2  zero2  zero2..
%---------------------------------------------------------------------------------------------
%              punc_p1    I1    zero4   zero4   zero4  zero4  zero4  zero4 |   I    zero4  zero4  zero4..            
%              punc_p2  zero4     I2    zero4   zero4  zero4  zero4  zero4 | zero4    I    zero4  zero4..
%              punc_p3  zero4   zero4     I3    zero4  zero4  zero4  zero4 | zero4  zero4    I    zero4..
%              punc_p4  zero4   zero4   zero4     I4   zero4  zero4  zero4 | zero4..
%              punc_p5  zero4   zero4   zero4   zero4   I5    zero4  zero4 | zero4..
%              punc_p6  zero4   zero4   zero4   zero4  zero4   I6    zero4 | zero4..
%              punc_p7  zero4   zero4   zero4   zero4  zero4  zero4    I7  | zero4.. 
%             ] 
zero1 = zeros([Payload_H_r Extra_H_c]);
zero2 = zeros([Extra_H_r Extra_H_c]);
zero3 = zeros([Extra_H_r Payload_H_c]);
zero4 = zeros([Extra_H_c Extra_H_c]);

% punc_pos_bits = oneSR_method(H1,H2_c);
punc_pos_bits = maximize_oneSR_method(Payload_H,Extra_H_c*Combine_number);
H_combine = [];
% compose upper Combine H
for r=1:Combine_number+1 
    if r==1
        muti_zeros = repmat(zero1,1,Combine_number*2);
        H_combine = [H_combine;Payload_H,muti_zeros];
    else
        right_rep = (Combine_number*2 + 1) -  (r); % colsize - r_idx
        left_rep  = (Combine_number*2 + 1) - right_rep - 2; % colsize - Extra_H(1) - zeros3(1) - right_rep
        right_zeros = repmat(zero2,1,right_rep);
        left_zeros  = repmat(zero2,1,left_rep);
        H_combine = [H_combine;zero3,left_zeros,Extra_H,right_zeros];
    end
end
% compose lowwer Combine H
for r=1:Combine_number
    % compose punc_matrix
    punc_mat = zeros(Extra_H_c,Payload_H_c);
    start_idx = (r-1)*Extra_H_c+1;
    end_idx   =  r*Extra_H_c;
    punc_poses = punc_pos_bits(start_idx:end_idx);
    for i=1:size(punc_poses,2)
        punc_pos = punc_poses(i);
        punc_mat(i,punc_pos) = 1;
    end
    right_rep = (Combine_number + 2) -  (r+1) - 1; % colsize - r_idx - I(1) 
    left_rep  = (Combine_number + 2) - right_rep - 3; % colsize - punc_mat(1) - I(2) - right_rep
    right_zeros = repmat(zero4,1,right_rep);
    left_zeros  = repmat(zero4,1,left_rep);
    I = eye(Extra_H_c);
    H_combine  = [H_combine;punc_mat,left_zeros,I,right_zeros,left_zeros,I,right_zeros];
end

% write H_combine to PCM 
writePCM(H_combine,H_combine_file);

% write payload data with puncture bits position 
fileID = fopen(puncture_position_bits_file, 'w');
for pos=punc_pos_bits
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);



%% out matrix excel
% 範例二維矩陣（只有0與1）


% 設定要輸出的 Excel 檔名
filename = 'matrix_output.xlsx';

% 將矩陣寫入 Excel（使用 writematrix，從 A1 開始）
writematrix(H_combine, filename, 'Sheet', 1, 'Range', 'A1');

% 啟用 ActiveX 控制 Excel（僅限 Windows）
excel = actxserver('Excel.Application');
excel.Visible = false;  % 設成 true 可以看到 Excel 操作

% 打開檔案
workbook = excel.Workbooks.Open(fullfile(pwd, filename));
sheet = workbook.Sheets.Item(1);

% 遍歷矩陣，將值為 1 的格子設為填色（黃色）
[rows, cols] = size(H_combine);
for i = 1:rows
    for j = 1:cols
        if H_combine(i, j) == 1
            cell = sheet.Range(getExcelRange(j, i));  % Excel 是列字母+行數字
            cell.Interior.Color = 65535;  % 黃色
        end
    end
end

% 儲存並關閉
workbook.Save();
workbook.Close(false);
excel.Quit();
delete(excel);

disp('矩陣已寫入並標記完成！');

% --- 輔助函式：數字轉 A1 格式（e.g., j=1,i=1 變 A1）
function ref = getExcelRange(col, row)
    letters = '';
    while col > 0
        rem = mod(col-1, 26);
        letters = [char(65+rem), letters];
        col = floor((col-1)/26);
    end
    ref = [letters, num2str(row)];
end