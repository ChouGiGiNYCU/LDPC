%%
clc;
clear all;
close all;
% 部分 Extra 傳送 (部分 payload 被 Puncture)
% 所有Extra 都跟payload 做 New_Structure 連結
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_1020_330_690_BG1_Z15_fix.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt'; % extra data
H_combine_file = 'PCM_P5G1020_E15BCH_EnhanceStruc_50percent_RCScheme.txt';
puncture_position_bits_outfile = "Table_Superposition_Extra_Payload_50percent.csv"; % 對應payload 、 extra puncture位置(原本的方法)
Transmit_Extra_VNs_table_outfile     = "Table_ExtraTransmitVNs_to_PuncPosPayload_50percent.csv"; % 傳送的extra bits 位置 、不傳送的payload bits file
New_puncture_position_bits_outfile = "ExtraVNs_NewStrcuture_50percent.csv";
% TransmitExtra_New_puncture_position_bits_outfile = "Transmit_ExtraVNs_NewStrcuture_50percent.csv";
Z = 15; % 5GNR setting
H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);
Non_select_VNs = 1:H1_c;
%%  random choose Extra transmit VNs
Extra_Transmit_Ratio = 0.5;
Extra_Transmit_number = floor(Extra_Transmit_Ratio *  H2_c);
Transmit_Extra_VNs = randperm(H2_c,Extra_Transmit_number); % random choose extra vn
Transmit_Extra_VNs = sort(Transmit_Extra_VNs);
non_Transmit_Extra_VNs = setdiff(1:H2_c, Transmit_Extra_VNs);  % 找出剩下沒有傳送的Extra_bits

non_Transmit_Extra_VNs_num = size(non_Transmit_Extra_VNs,2);
I_mat_size = non_Transmit_Extra_VNs_num;
%% find Payload Punc bits
Already_punc_5G = 1:(2*Z);
Payload_punc_bits_num = H2_c * 2 + length(Already_punc_5G) ; % 總共 punc bits 為 Extra(部分Superposition、Extra傳送) + New_Structure2
punc_pos_bits = Puncturing_Algorithm_For_Finit_Length(H1,Payload_punc_bits_num,Already_punc_5G); % start idx = 1
Non_select_VNs = setdiff(Non_select_VNs,punc_pos_bits);

punc_pos_bits = setdiff(punc_pos_bits,Already_punc_5G);
% 把找到的punc bits 分成 Payload+Extra(Superposition)、 Partial Transmit Extra(payload不傳送的) 、 New_Struct(Partial_unTransmit_VNs)
punc_pos_bits_origin      =  punc_pos_bits(1:non_Transmit_Extra_VNs_num); % Superposition
payload_punc_unsend       =  punc_pos_bits(non_Transmit_Extra_VNs_num+1:H2_c); % Partial Transmit Extra(payload不傳送的)
punc_pos_bits_New_Struct2 =  punc_pos_bits(H2_c+1:end); % 被 punc

%% 找跟 non transmit Extra 連接的payload bits(不需要被punc) 但不能跟punc_pos_bits_New_Struct2相鄰
punc_pos_bits_New_Struct1 = []; % 在 New Structure 中 跟Extra 做第一次疊加的 VNs
for r=1:H2_c
    Punc_VN = punc_pos_bits_New_Struct2(r);
    while true
        idx = randi(numel(Non_select_VNs));    % 在 1 到 arr 元素總數間隨機選一個整數
        choose_VN = Non_select_VNs(idx);   % 用線性索引取出該元素
        Nei_CN = transpose(find(H1(:,Punc_VN)==1));
        Nei_CN_VNs_set = [];
        for CN=Nei_CN
            Nei_CN_VNs = find(H1(CN,:)==1);
            Nei_CN_VNs_set = union(Nei_CN_VNs_set,Nei_CN_VNs);
        end
        if any(ismember(Nei_CN_VNs_set,choose_VN))
            continue;
        else
            punc_pos_bits_New_Struct1(end+1) = choose_VN;
            Non_select_VNs = setdiff(Non_select_VNs,choose_VN);
            break;
        end
    end
end
%% define 各個連接的矩陣
sup_payload_mat = zeros([non_Transmit_Extra_VNs_num,H1_c]);
for r=1:non_Transmit_Extra_VNs_num
    punc_vn = punc_pos_bits_origin(r);
    sup_payload_mat(r,punc_vn) = 1;
end

sup_extra_mat = zeros([non_Transmit_Extra_VNs_num,H2_c]);
for r=1:non_Transmit_Extra_VNs_num
    punc_vn = non_Transmit_Extra_VNs(r);
    sup_extra_mat(r,punc_vn) = 1;
end

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

I = eye(non_Transmit_Extra_VNs_num);
%%
% H_combine = [  H1     zero1       zero2                     zero4   zero4 
%               zero3     H2        zero5                     zero6   zero6 
%                sup_p   sup_e        I(unsend extra vn)      zero7   zero7
%               puncs1  punc_e      zeor9                       I     zeor8
%               puncs2  zero10      zeor9                       I       I      
%             ] 
zero1 = zeros([H1_r H2_c]);
zero2 = zeros([H1_r non_Transmit_Extra_VNs_num]);
zero3 = zeros([H2_r H1_c]);
zero4 = zeros([H1_r H2_c]);

zero5 = zeros([H2_r non_Transmit_Extra_VNs_num]);
zero6 = zeros([H2_r H2_c]);
zero7 = zeros([non_Transmit_Extra_VNs_num H2_c]);

zero8 = zeros([H2_c H2_c]);
zero9 = zeros([H2_c non_Transmit_Extra_VNs_num]);
zero10 = zeros([H2_c H2_c]);
I_sup = eye(non_Transmit_Extra_VNs_num);
I_New_Strcture = eye(H2_c);
H_combine = [  
                H1 ,                                 zero1 , zero2 , zero4 , zero4;
                zero3 ,                              H2    , zero5 , zero6 , zero6;
                sup_payload_mat ,            sup_extra_mat , I_sup , zero7 , zero7;
                punc_payload_mat_NewStructure1 , I_New_Strcture , zero9 , I_New_Strcture , zero8;
                punc_payload_mat_NewStructure2 , zero10 , zero9    , I_New_Strcture ,I_New_Strcture;
            
            ];

%% 輸出檔案
writePCM(H_combine,H_combine_file); % write H_combine to PCM 

% write payload data with puncture bits position with Superposition
T = table(non_Transmit_Extra_VNs.',punc_pos_bits_origin.','VariableNames', {'Extra_VNs', 'Payload_VNs'});  % 建立 table
writetable(T, puncture_position_bits_outfile);  % 輸出 csv

% write payload data with NonTransmit Extra bits puncture bits position with NewStructure 
T = table([1:H2_c].',punc_pos_bits_New_Struct1.',punc_pos_bits_New_Struct2.','VariableNames', {'Extra bits','Payloadb_NonPunc(b)', 'Payloadc_Punc(c)'});  % 建立 table
writetable(T, New_puncture_position_bits_outfile);  % 輸出 csv

% write payload data with Transmit Extra bits puncture bits position with NewStructure 
% T = table(Transmit_Extra_VNs.',punc_pos_bits_New_Struct.',punc_pos_bits_New_Struct2.','VariableNames', {'Extra bits','Payloadb_NonPunc(b)', 'Payloadc_Punc(c)'});  % 建立 table
% writetable(T, NonTransmitExtra_New_puncture_position_bits_outfile);  % 輸出 csv


% Extra 傳送 payload不傳送的 各個bits
T = table(Transmit_Extra_VNs.',payload_punc_unsend.','VariableNames', {'Extra_VNs', 'Payload_VNs'});  % 建立 table
writetable(T, Transmit_Extra_VNs_table_outfile);  % 輸出 csv

%%
% 範例二維矩陣（只有0與1）
% 
% 
% % 設定要輸出的 Excel 檔名
% filename = 'matrix_output.xlsx';
% 
% % 將矩陣寫入 Excel（使用 writematrix，從 A1 開始）
% writematrix(H_combine, filename, 'Sheet', 1, 'Range', 'A1');
% 
% % 啟用 ActiveX 控制 Excel（僅限 Windows）
% excel = actxserver('Excel.Application');
% excel.Visible = false;  % 設成 true 可以看到 Excel 操作
% 
% % 打開檔案
% workbook = excel.Workbooks.Open(fullfile(pwd, filename));
% sheet = workbook.Sheets.Item(1);
% 
% % 遍歷矩陣，將值為 1 的格子設為填色（黃色）
% [rows, cols] = size(H_combine);
% for i = 1:rows
%     for j = 1:cols
%         if H_combine(i, j) == 1
%             cell = sheet.Range(getExcelRange(j, i));  % Excel 是列字母+行數字
%             cell.Interior.Color = 65535;  % 黃色
%         end
%     end
% end
% 
% % 儲存並關閉
% workbook.Save();
% workbook.Close(false);
% excel.Quit();
% delete(excel);
% 
% disp('矩陣已寫入並標記完成！');
% 
% % --- 輔助函式：數字轉 A1 格式（e.g., j=1,i=1 變 A1）
% function ref = getExcelRange(col, row)
%     letters = '';
%     while col > 0
%         rem = mod(col-1, 26);
%         letters = [char(65+rem), letters];
%         col = floor((col-1)/26);
%     end
%     ref = [letters, num2str(row)];
% end
