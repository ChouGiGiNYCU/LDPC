%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt'; % extra data
H_combine_file = 'PCM_P1008_E15BCH_Structure_Worst.txt';
puncture_position_bits_file = "Punc_Superpostion_origin_Payload_Worst.txt";
New_puncture_position_bits_file = "Punc_Superpostion_new_Payload_Worst.csv";

H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);
% H_combine = [  H1     zero1       zero2    zero5   zero5 
%               zero3     H2        zero4    zero6   zero6 
%               punc_p  punc_e        I      zero7   zero7
%               puncpb  punc_e      zeor7      I     zero7
%               puncpc   zero7      zero7    zero7     I      
%             ] 
zero1 = zeros([H1_r H2_c]);
zero2 = zeros([H1_r H2_c]);
zero3 = zeros([H2_r H1_c]);
zero4 = zeros([H2_r H2_c]);
zero5 = zeros([H1_r H2_c]);
zero6 = zeros([H2_r H2_c]);
zero7 = zeros([H2_c H2_c]);
%%
% punc_pos_bits = oneSR_method(H1,H2_c);
punc_pos_bits = maximize_oneSR_method(H1,H2_c*3); % idx = 1
% punc_pos_bits_origin = [76 136 196 272 362 465 496 583 682 684 712 756 882 923 932];
% 把找到的punc bits 分成 Payload+Extra 、 New_Struct(e^b) 、 New_Struct(e^b^c)
punc_pos_bits_origin      =  punc_pos_bits(1:H2_c);
punc_pos_bits_New_Struct1 =  punc_pos_bits(H2_c+1:H2_c*2);
punc_pos_bits_New_Struct2 =  punc_pos_bits(H2_c*2+1:end);
%% worst case 
punc_pos_bits = Find_KSR_WorstVN(H1,H2_c*2); % punc 使用 k-SR
% 把找到的punc bits 分成 Payload+Extra 、 New_Struct(e^b)
punc_pos_bits_origin      =  punc_pos_bits(1:H2_c);
punc_pos_bits_New_Struct1 =  punc_pos_bits(H2_c+1:H2_c*2);
% extra 而外的結構(第一個跟payload 做 xor的) 用隨機挑選
punc_pos_bits_New_Struct2 = setdiff(1:H2_c,punc_pos_bits);
idx = randperm(numel(punc_pos_bits_New_Struct2), H2_c); % 隨機不重複地抽 n 個索引
punc_pos_bits_New_Struct2 = punc_pos_bits_New_Struct2(idx);
%%
punc_payload_mat = zeros([H2_c,H1_c]);
for r=1:H2_c
    punc_vn = punc_pos_bits_origin(r);
    punc_payload_mat(r,punc_vn) = 1;
end
punc_extra_mat = eye(H2_c);
I = eye(H2_c);

punc_payload_mat_NewStructure1 = zeros([H2_c,H1_c]);
for r=1:H2_c
    punc_vn = punc_pos_bits_New_Struct1(r);
    punc_payload_mat_NewStructure1(r,punc_vn) = 1;
end

punc_payload_mat_NewStructure2 = zeros([H2_c,H1_c]);
for r=1:H2_c
    punc_vn = punc_pos_bits_New_Struct2(r);
    punc_payload_mat_NewStructure2(r,punc_vn) = 1;
end

H_combine = [[H1,zero1,zero2,zero5,zero5];[zero3,H2,zero4,zero6,zero6]];
H_combine = [H_combine;punc_payload_mat,punc_extra_mat,I,zero7,zero7];
H_combine = [H_combine;punc_payload_mat_NewStructure1,punc_extra_mat,zero7,I,zero7];
H_combine = [H_combine;punc_payload_mat_NewStructure2,zero7,zero7,I,I];

% write H_combine to PCM 
writePCM(H_combine,H_combine_file);

% write payload data with puncture bits position with NewStructure 
T = table(punc_pos_bits_New_Struct1.',punc_pos_bits_New_Struct2.','VariableNames', {'Payloadb_NonPunc(b)', 'Payloadc_Punc(c)'});  % 建立 table
writetable(T, New_puncture_position_bits_file);  % 輸出 csv

fileID = fopen(puncture_position_bits_file, 'w');
for pos=punc_pos_bits_origin
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

