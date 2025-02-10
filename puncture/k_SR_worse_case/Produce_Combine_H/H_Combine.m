%%
clc;
clear all;
close all;
%% init parameter
H1_file = 'PEGReg504x1008.txt'; % payload data
H2_file = 'H_10_5.txt'; % extra data
H_combine_file = 'PCM_P1008_E10_1SR.txt';
puncture_position_bits_file = ['Pos_', H_combine_file];
H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);


%% find worse puncture vn node to test performance
% punc_pos_bits = oneSR_method(H1,H2_c);
init_vn = randi([1 H1_c],1,1); % 一開始選擇一個vn node開始
punc_pos_bits = init_vn;
nei_cn = transpose(find(H1(:,init_vn)==1)); % vn 所連接到的cn node
used_cn = nei_cn; 
i=1;

while i<length(nei_cn)
    % let previous puncture vn node without survive check node
    cn = nei_cn(i);
    vn = find(H1(cn,:)==1); 
    if all(ismember(vn,punc_pos_bits)) % 代表這個check node 底下的 vn 都被 puncture 了 (dead check node)
        i = i + 1;
        continue;
    end
    while true
        choose_idx = randperm(length(vn), 1); % 生成不重複的隨機索引
        choose_vn = vn(choose_idx);
        if isempty(intersect(choose_vn , punc_pos_bits))
            punc_pos_bits = [punc_pos_bits,choose_vn];
            connet_cn = transpose(find(H1(:,choose_vn)==1));
            unused_cn = setdiff(connet_cn,used_cn);
            used_cn = union(used_cn,unused_cn);
            nei_cn = [nei_cn,unused_cn];
            break;
        end
        
    end
    i = i + 1;
    if length(punc_pos_bits)==H2_c
        break;
    end
end


%% Compose combine PCM
% H_combine = [  H1     zero1   zero2 
%               zero3     H2    zero4
%               punc_p  punc_e    I
%             ] 
zero1 = zeros([H1_r H2_c]);
zero2 = zeros([H1_r H2_c]);
zero3 = zeros([H2_r H1_c]);
zero4 = zeros([H2_r H2_c]);
H_combine = [[H1,zero1,zero2];[zero3,H2,zero4]];
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