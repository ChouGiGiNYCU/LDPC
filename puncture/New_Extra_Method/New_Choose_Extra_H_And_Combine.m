clear all;
close all;
clc;
% Remmainder : G = [I P] or [P I]
%%  Convert PCM to G
PayLoad_PCM_file =  "PEGReg504x1008.txt";
PayLoad_G_file_new = "Extra_newG_From_PEG504.txt";
PayLoad_PCM_file_new = "Extra_newPCM_From_PEG504.txt";
PayLoad_PCM = readHFromFileByLine(PayLoad_PCM_file);
[PayLoad_PCM_rowsize,PayLoad_PCM_colsize] = size(PayLoad_PCM);

% 選擇Extra bit 要帶的bits數量是多少(這部分可能沒有辦法設定)
% Extra_bits = 6;
% 從 PCM 裡面選擇 n 個 VN node ，且把n個 vn 對應的 cn node 和 cn node 對應的vn node給選起來
choosen_vns = []; 
choosen_cns = [];
choosen_vns_num = 1;
idx = 0;
while idx<choosen_vns_num
    vns_set = [];
    cns_set = [];
    vns_set_length = 1000000;
    for vn=1:PayLoad_PCM_colsize
        if ~isempty(find(choosen_vns==vn))
            disp("vn in choosen_vn")
            continue;
        else
            cns = transpose(find(PayLoad_PCM(:,vn)==1));
            cns_connect_vns = [vn];
        end
        
        for cn=cns
           vns =  find(PayLoad_PCM(cn,:)==1);
           cns_connect_vns = union(cns_connect_vns,vns);
        end
        cns_connect_vns = setdiff(cns_connect_vns,choosen_vns);
        if length(cns_connect_vns)<vns_set_length
            vns_set = cns_connect_vns;
            cns_set = cns;
            vns_set_length = length(cns_connect_vns);
        end
    end
    if isempty(choosen_vns)
        choosen_vns = vns_set;
    else
        choosen_vns = union(choosen_vns,vns_set);
    end
    
    if isempty(choosen_cns)
        choosen_cns = cns_set;
    else
        choosen_cns = union(choosen_cns,cns_set);
    end
    
    idx = idx + 1; 
end
%% 從原本的PCM挑選對應的 VN 和 CN 來產生新的PCM
PayLoad_PCM_new = [];
for cn=choosen_cns
    PayLoad_PCM_new = [PayLoad_PCM_new;PayLoad_PCM(cn,choosen_vns)];
end
% 產生新的 PCM 對應的 G
[N, K, M_ori, M_sys, sysG, sysH, PayLoad_G_new, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(PayLoad_PCM_new);


% check new_H 跟 new_G相乘是否等於 zero matrix
if size(PayLoad_G_new, 2) ~= size(PayLoad_PCM_new, 2)
    error('PayLoad_G_new 矩陣維度不匹配 PayLoad_PCM_new，無法相乘');
    exit;
end
m = mod(PayLoad_G_new * transpose(PayLoad_PCM_new),2);
if sum(m(:))==0
    disp("PayLoad_G_new x PayLoad_PCM_new^T is zero matrix , test pass !!");
else
    error("PayLoad_G_new x PayLoad_PCM_new^T is not zero matrix , please check !!");
    exit;
end

writeGToFile(PayLoad_G_new,PayLoad_G_file_new);
writePCM(PayLoad_PCM_new,PayLoad_PCM_file_new);
%% inverse G
inv_G = inv(PayLoad_G_new)
%%
% 從 PayLoad_G 選擇 Extra_bits'th 個 rows , 並且把選到的 rows 有 1 的 col 給抓出來，組成一個新的 PayLoad_G_new
% choosen_rows = randperm(PayLoad_G_rowsize,Extra_bits);
% 
% cols_set = [];
% for r=choosen_rows
%     cols = find(PayLoad_G(r,:)==1);
%     cols_set = union(cols_set,cols);
% end
% 
% % 產生新的PayLoad_G 
% PayLoad_G_new = [];
% for r=choosen_rows
%     row = PayLoad_G(r,cols_set);
%     PayLoad_G_new = [PayLoad_G_new;row];
% end
% 
% % 把PayLoad_G_new 對應的 PayLoad_PCM_new 也給產生出來
% % G x H^T = zero_matrix，所以把 PayLoad_PCM  cols_set 裡面的 col 選出來
% PayLoad_PCM_new = PayLoad_PCM(:,cols_set); 
% 
% % check new_H 跟 new_G相乘是否等於 zero matrix
% if size(PayLoad_G_new, 2) ~= size(PayLoad_PCM_new, 2)
%     error('PayLoad_G_new 矩陣維度不匹配 PayLoad_PCM_new，無法相乘');
%     exit;
% end
% m = mod(PayLoad_G_new * transpose(PayLoad_PCM_new),2);
% if sum(m(:))==0
%     disp("PayLoad_G_new x PayLoad_PCM_new^T is zero matrix , test pass !!");
% else
%     error("PayLoad_G_new x PayLoad_PCM_new^T is not zero matrix , please check !!");
%     exit;
% end
% 
% writeGToFile(PayLoad_G_new,PayLoad_G_file_new);
% writePCM(PayLoad_PCM_new,PayLoad_PCM_file_new);

%% 把原本的 PayLoad_PCM 跟 PayLoad_PCM_new 做合併
[PayLoad_PCM_new_rowsize,PayLoad_PCM_new_colsize] = size(PayLoad_PCM_new);
H_combine_file = ['PCM_P',num2str(PayLoad_PCM_colsize),'_E',num2str(PayLoad_PCM_new_colsize), '_1SR.txt'];
puncture_position_bits_file = ['Pos_', H_combine_file];


% H_combine = [  P     zero1   zero2 
%               zero3     E    zero4
%               punc_p  punc_e    I
%             ] 

zero1 = zeros([PayLoad_PCM_rowsize,PayLoad_PCM_new_colsize]);
zero2 = zeros([PayLoad_PCM_rowsize,PayLoad_PCM_new_colsize]);
zero3 = zeros([PayLoad_PCM_new_rowsize,PayLoad_PCM_colsize]);
zero4 = zeros([PayLoad_PCM_new_rowsize,PayLoad_PCM_new_colsize]);
H_combine = [[PayLoad_PCM,zero1,zero2];[zero3,PayLoad_PCM_new,zero4]];
punc_pos_bits = maximize_oneSR_method(PayLoad_PCM,PayLoad_PCM_new_colsize);

punc_payload_mat = zeros([PayLoad_PCM_new_colsize,PayLoad_PCM_colsize]);
for r=1:PayLoad_PCM_new_colsize
    punc_vn = punc_pos_bits(r);
    punc_payload_mat(r,punc_vn) = 1;
end
punc_extra_mat = eye(PayLoad_PCM_new_colsize);
I = eye(PayLoad_PCM_new_colsize);
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


%% test H2G
clear all;
close all;
clc;
puncture_col = [1 6 9];
H = [1 1 1 1 0 0 0 0 0 0;1 0 0 0 1 1 1 0 0 0;0 1 0 0 1 0 0 1 1 0;0 0 1 0 0 1 0 1 0 1;0 0 0 1 0 0 1 0 1 1];
[rows,cols]= size(H);
cols = setdiff(1:cols,puncture_col);
H_test = H(:,cols);
% 產生新的 PCM 對應的 G
[N, K, M_ori, M_sys, sysG, sysH, G_test, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(H_test);
[N, K, M_ori, M_sys, sysG, sysH, G, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(H);

random_info = ones( 1, size(G_test,1));
random_codeword = mod(random_info * G_test,2);
insert_indices = puncture_col; % 要插入的位置
% 先调整 vec 的长度，确保有足够的空间来插入元素
random_codeword = [random_codeword(1:insert_indices(1)-1), 0, random_codeword(insert_indices(1):end)];

% 插入其余的元素
for i = 2:length(insert_indices)
    % 对于每个插入位置，调整 vec 的长度并插入新元素
    random_codeword = [random_codeword(1:insert_indices(i)-1), 0, random_codeword(insert_indices(i):end)];
end

mod(random_codeword * transpose(H),2)

while true
    info = randi([0, 1], 1, size(G,1));
    codeword = mod(info*G,2);
    if all(codeword==random_codeword)
        break;
    end
end
%%
