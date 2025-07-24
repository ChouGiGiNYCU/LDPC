%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_96_48.txt'; % extra data
H_combine_file = 'PCM_P1008_E96x3_1SR.txt';
puncture_position_bits_file = ['Pos_', H_combine_file];
H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);
% H_combine = [  H1     zero1   zero2 
%               zero3     H2    zero4
%               punc_p  punc_e    I
%             ] 
zero1 = zeros([H1_r H2_c]);
zero2 = zeros([H1_r H2_c]);
zero3 = zeros([H2_r H1_c]);
zero4 = zeros([H2_r H2_c]);
H_combine = [[H1,zero1,zero2];[zero3,H2,zero4]];

% punc_pos_bits = oneSR_method(H1,H2_c);
punc_pos_bits = maximize_oneSR_method(H1,H2_c*3);
%% check  punc_pos_bits 是否都是 1-SR
for Punc_VN=punc_pos_bits 
    Punc_SCNs = transpose(find(H1(:,Punc_VN)==1));
    SCNs_num = 0;
    for SCN=Punc_SCNs
        Nei_SCN_VNs = setdiff(find(H1(SCN,:)==1),Punc_VN);
        if ~any(ismember(punc_pos_bits,Nei_SCN_VNs))
            SCNs_num  = SCNs_num + 1;
        end
    end
    if SCNs_num==0
        disp("find no 1-SR punc vn !!\n");
    end
end
%%
% punc_pos_bits = [51 199 303 403 418 486 624 644 828 882];
punc_payload_mat = zeros([H2_c,H1_c]);
for r=1:H2_c
    punc_vn = punc_pos_bits(r);
    punc_payload_mat(r,punc_vn) = 1;
end
punc_extra_mat = eye(H2_c);
I = eye(H2_c);
H_combine = [H_combine;punc_payload_mat,punc_extra_mat,I];

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
