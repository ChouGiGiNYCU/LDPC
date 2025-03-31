%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_96_48.txt'; % extra data
H_combine_file = 'PCM_P1008_E96_DualDiagonal_1SR.txt';
puncture_position_bits_file = ['Pos_', H_combine_file];
H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);
% H_combine = [  H1     zero1       zero2 
%               zero3     H2        zero4
%               punc_p  punc_e  dual-diagonal_I
%             ] 
zero1 = zeros([H1_r H2_c]);
zero2 = zeros([H1_r H2_c]);
zero3 = zeros([H2_r H1_c]);
zero4 = zeros([H2_r H2_c]);
H_combine = [[H1,zero1,zero2];[zero3,H2,zero4]];

% punc_pos_bits = oneSR_method(H1,H2_c);
% punc_pos_bits = maximize_oneSR_method(H1,H2_c);
punc_pos_bits = [9 10 28 30 33 34 43 46 56 59 88 93 111 129 134 139 147 158 167 169 181 186 233 238 239 244 262 264 271 277 283 286 290 317 340 351 373 390 391 395 396 399 401 416 451 453 484 489 505 514 537 548 551 563 573 582 603 623 636 639 642 658 666 701 709 720 742 748 757 762 769 775 791 798 810 811 813 817 822 825 837 842 857 862 914 916 921 924 935 939 946 962 969 971 973 1003];
punc_payload_mat = zeros([H2_c,H1_c]);
for r=1:H2_c
    punc_vn = punc_pos_bits(r);
    punc_payload_mat(r,punc_vn) = 1;
end
I = eye(H2_c);
dual_diagonal_I = I + [I(:,2:end),zeros(size(I,1),1)];
H_combine = [H_combine;punc_payload_mat,I,dual_diagonal_I];

% write H_combine to PCM 
writePCM(H_combine,H_combine_file);

% write payload data with puncture bits position 
fileID = fopen(puncture_position_bits_file, 'w');
for pos=punc_pos_bits
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);


%%
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
