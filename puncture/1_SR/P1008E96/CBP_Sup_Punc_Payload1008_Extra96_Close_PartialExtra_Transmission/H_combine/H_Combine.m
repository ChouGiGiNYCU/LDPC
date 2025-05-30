%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_96_48.txt'; % extra data
Each_VNs_Girth_num_file = "Each_VN_Girth_Record_H96x48.csv";
H_combine_file = 'PCM_P1008_E96_PartialExtraTransmit_1SR_77.txt';
puncture_position_bits_file = "Table_Superposition_Extra_Payload_77.csv"; % 對應payload 、 extra puncture位置(原本的方法)
Transmit_Extra_VNs_table_file     = "Table_ExtraTransmitVNs_to_PuncPosPayload_77.csv"; % 傳送的extra bits 位置 、不傳送的payload bits

H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);
%% 讀取每個VN經過girth的數量多寡 來決定傳送的 Extra transmit VNs
Extra_Transmit_Ratio = 0.5;
% Extra_Transmit_number = floor(Extra_Transmit_Ratio *  H2_c);
Extra_Transmit_number = 77;
% 把Extra VN 分成 Transmit and Superposition 
% read Each_VNs_Girth_num_file 
Each_VNs_Girth_num = readtable(Each_VNs_Girth_num_file);
vn = Each_VNs_Girth_num.VN; 
girth_num = Each_VNs_Girth_num.Girth;
% 找出 girth 較多的 VNs
[~, idx] = sort(girth_num, 'descend'); % 根據 girth_node 的值做遞減排序
vn_sorted = vn(idx); % 排序後的 vn 與 girth_node
girth_sorted = girth_num(idx);
% 要傳送的部分 Extra VNs
Transmit_Extra_VNs = vn_sorted(1:Extra_Transmit_number).' + 1; % idx =1
remaining_Extra_VN = setdiff(1:H2_c, Transmit_Extra_VNs);  % 找出剩下的部分
%%  random choose Extra transmit VNs
Extra_Transmit_Ratio = 0.5;
Extra_Transmit_number = floor(Extra_Transmit_Ratio *  H2_c);
Transmit_Extra_VNs = randperm(H2_c,Extra_Transmit_number); % random choose
remaining_Extra_VN = setdiff(1:H2_c, Transmit_Extra_VNs);  % 找出剩下的部分
%% Extra transmit non 1-SR node 
NonTrasmit_VNs = maximize_oneSR_method(H2,40); % idx = 1
Extra_Transmit_number = H2_c - size(NonTrasmit_VNs,2);
Transmit_Extra_VNs = setdiff(1:H2_c, NonTrasmit_VNs);  % 找出剩下的部分
remaining_Extra_VN = NonTrasmit_VNs; % 不傳的VNs要跟payload 做結合
%%
Punc_p_r = H2_c-Extra_Transmit_number;
% H_combine = [  H1     zero1       zero2 
%               zero3     H2        zero4
%               punc_p  punc_e        I
%             ] 
zero1 = zeros([H1_r H2_c]);
zero2 = zeros([H1_r Punc_p_r]);
zero3 = zeros([H2_r H1_c]);
zero4 = zeros([H2_r Punc_p_r]);
H_combine = [[H1,zero1,zero2];[zero3,H2,zero4]];

% punc_pos_bits = oneSR_method(H1,H2_c);
% punc_pos_bits = maximize_oneSR_method(H1,H2_c); % idx = 1
punc_pos_bits_origin = [9 10 28 30 33 34 43 46 56 59 88 93 111 129 134 139 147 158 167 169 181 186 233 238 239 244 262 264 271 277 283 286 290 317 340 351 373 390 391 395 396 399 401 416 451 453 484 489 505 514 537 548 551 563 573 582 603 623 636 639 642 658 666 701 709 720 742 748 757 762 769 775 791 798 810 811 813 817 822 825 837 842 857 862 914 916 921 924 935 939 946 962 969 971 973 1003]; 

remove_idx = randperm(length(punc_pos_bits_origin), Extra_Transmit_number); % 隨機選擇 num 個 index 要刪掉
Extra_Transmit_OnPayload_vn = punc_pos_bits_origin(remove_idx);
punc_pos_bits_origin(remove_idx) = [];


punc_payload_mat = zeros([Punc_p_r,H1_c]);
for r=1:Punc_p_r
    punc_vn = punc_pos_bits_origin(r);
    punc_payload_mat(r,punc_vn) = 1;
end
punc_extra_mat = zeros([Punc_p_r H2_c]);

for r=1:Punc_p_r
    remain_vn = remaining_Extra_VN(r);
    punc_extra_mat(r,remain_vn) = 1;
end
I = eye(Punc_p_r);
H_combine = [H_combine;punc_payload_mat,punc_extra_mat,I];

% write H_combine to PCM 
writePCM(H_combine,H_combine_file);

% write payload data with puncture bits position 
T = table(remaining_Extra_VN.',punc_pos_bits_origin.','VariableNames', {'Extra_VNs', 'Payload_VNs'});  % 建立 table
writetable(T, puncture_position_bits_file);  % 輸出 csv
% fileID = fopen(puncture_position_bits_file, 'w');
% for pos=punc_pos_bits_origin
%     fprintf(fileID, '%d ', pos);
% end
% fprintf(fileID, '\n');
% fclose(fileID);


T = table(Transmit_Extra_VNs.',Extra_Transmit_OnPayload_vn.','VariableNames', {'Extra_VNs', 'Payload_VNs'});  % 建立 table
writetable(T, Transmit_Extra_VNs_table_file);  % 輸出 csv

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
