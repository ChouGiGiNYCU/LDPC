%%
clc;
clear all;
close all;
%%
PayloadH_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
ExtraH_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_10_5.txt'; % extra data
H_combine_file = 'PCM_P1008_H10_Strcuture_1SR.txt';
New_structure_vn_file = ['Strcut_Pos_',H_combine_file];
puncture_position_bits_file = ['Punc_Pos_', H_combine_file];
PayloadH = read_parity_check(PayloadH_file);
ExtraH   = read_parity_check(ExtraH_file);

% H_combine = [  PayloadH     zero1         zero2 
%                 zero3       ExtraH        zero4
%                 punc_p      punc_e          I
%             ] 
zero1 = zeros([PayloadH.m ExtraH.n]);
zero2 = zeros([PayloadH.m ExtraH.n]);
zero3 = zeros([ExtraH.m PayloadH.n]);
zero4 = zeros([ExtraH.m ExtraH.n]);
H_combine = [[PayloadH.mat,zero1,zero2];[zero3,ExtraH.mat,zero4]];

% punc_pos_bits = oneSR_method(H1,H2_c);
% punc_pos_bits = maximize_oneSR_method(PayloadH,ExtraH.n);
punc_pos_bits = [51 199 303 403 418 486 624 644 828 882]; % idx start = 1
used_vns = punc_pos_bits; % idx start = 1
New_structure_vns = [];
for vn=punc_pos_bits
    idx0_vn = vn-1; % function 
    furthest_vns = find_furthest_vn(PayloadH,idx0_vn) + 1;
    choose_vn = random_unique_pick(furthest_vns,used_vns);
    used_vns = [used_vns,choose_vn];
    New_structure_vns = [New_structure_vns,choose_vn];
    fprintf('root : %d | ', vn);
    fprintf('%d ', furthest_vns);
    fprintf('\n');
end

punc_payload_mat = zeros([ExtraH.n PayloadH.n]);

for r=1:ExtraH.n
    punc_vn = punc_pos_bits(r);
    New_structure_vn = New_structure_vns(r);
    punc_payload_mat(r,punc_vn) = 1;
    punc_payload_mat(r,New_structure_vn) = 1;
end
I = eye(ExtraH.n);
% dual_diagonal_I = I + [I(:,2:end),zeros(size(I,1),1)];
H_combine = [H_combine;punc_payload_mat,I,I];

% write H_combine to PCM 
writePCM(H_combine,H_combine_file);

% write payload data with puncture bits position 
fileID = fopen(puncture_position_bits_file, 'w');
for pos=punc_pos_bits
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);

% write payload data with puncture bits position 
fileID = fopen(New_structure_vn_file, 'w');
for pos=New_structure_vns
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

