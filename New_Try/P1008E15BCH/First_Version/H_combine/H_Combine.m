%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt'; % extra data
H_combine_file = 'PCM_P1008_E15BCH_NewTry.txt';
puncture_position_bits_outfile = "table_ExtraTransmit_PayloadPunc.csv";
H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);

%% pre-process Extra each degree

Extra_VN2CN_pos = {};
for VN=1:H2_c
    Extra_Nei_CNs = transpose(find(H2(:,VN)==1)); 
    Extra_VN2CN_pos{VN} = Extra_Nei_CNs;
end

Extra_CN2VN_pos = {};
for CN=1:H2_r
    Extra_Nei_VNs = find(H2(CN,:)==1); 
    Extra_CN2VN_pos{CN} = Extra_Nei_VNs;
end
%%
Payload_VN2CN_pos = {};
for VN=1:H1_c
    Payload_Nei_CNs = transpose(find(H1(:,VN)==1)); 
    Payload_VN2CN_pos{VN} = Payload_Nei_CNs;
end

Payload_CN2VN_pos = {};
for CN=1:H1_r
    Payload_Nei_VNs = find(H1(CN,:)==1); 
    Payload_CN2VN_pos{CN} = Payload_Nei_VNs;
end
%%
% H_combine = [  H1     zero1        
%               zero3     H2 
%             ] 
zero1 = zeros([H1_r H2_c]);
zero3 = zeros([H2_r H1_c]);
H_combine = [[H1,zero1];[zero3,H2]];
%% find Payload Punc bits
Already_punc_vn = [];
Payload_punc_bits_num = 1;
Extra_unused_VNs = 1:H2_c;
Extra_unused_CNs = 1:H2_r;
Sup_Extra = [];
SCN = [-1];
while true
    Punc_Payload_VN = Rate_Compatible_Punctured_With_Short_Block_Lengths(H1,1,Already_punc_vn); % start idx = 1
    fprintf("Extra_unused_CNs num: %d\n",length(Extra_unused_CNs));
    if isempty(Extra_unused_CNs)
        break;
    end
    min_VN_deg = 1e9;
    choose_Extra_VN = -1;
    for Extra_VN=Extra_unused_VNs  
        if length(Extra_VN2CN_pos{Extra_VN})<min_VN_deg && length(intersect(Extra_unused_CNs,Extra_VN2CN_pos{Extra_VN}))>0
            choose_Extra_VN = Extra_VN; 
            min_VN_deg = length(Extra_VN2CN_pos{Extra_VN});
        end
    end
    if choose_Extra_VN~=-1
        SCN = union(SCN,Payload_VN2CN_pos{Punc_Payload_VN});
        for Extra_CN=Extra_VN2CN_pos{choose_Extra_VN}
            H_combine(H1_r+Extra_CN,Punc_Payload_VN)=1;
        end
        Extra_unused_CNs = setdiff(Extra_unused_CNs,Extra_VN2CN_pos{choose_Extra_VN});
        Extra_unused_VNs = setdiff(Extra_unused_VNs,choose_Extra_VN);
        Sup_Extra = [Sup_Extra,choose_Extra_VN];
        Already_punc_vn = [Already_punc_vn,Punc_Payload_VN];
    end

end


%% 輸出檔案
writePCM(H_combine,H_combine_file); % write H_combine to PCM 

% write payload data with puncture bits position with Superposition
T = table(Sup_Extra.',Already_punc_vn.','VariableNames', {'Transmit_Extra_VNs', 'Punc_Payload_VNs'});  % 建立 table
writetable(T, puncture_position_bits_outfile);  % 輸出 csv


%%

% % 範例二維矩陣（只有0與1）
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
% 
