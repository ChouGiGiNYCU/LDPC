%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt'; % extra data
H_combine_file = 'PCM_Punc_kSR_Punc15.txt';
RV1_punc_pos = "Pos_RV1_puncpos.txt";
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

Already_Punc_VNs = [];
% Already_Punc_VNs = 1:(2*Z);
% puncture_bits_num = H2_c + length(Already_Punc_VNs);
puncture_bits_num = H2_c;
% puncture_bits_num = H2_c*3;
punc_pos_bits = Rate_Compatible_Punctured_With_Short_Block_Lengths(H1,puncture_bits_num,Already_Punc_VNs);
punc_pos_bits = setdiff(punc_pos_bits,Already_Punc_VNs);
RV1_punc = punc_pos_bits;
%% check  punc_pos_bits 是否都是 1-SR
for Punc_VN=RV1_punc 
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
punc_payload_mat = zeros([H2_c,H1_c]);
for r=1:H2_c
    punc_vn = RV1_punc(r);
    punc_payload_mat(r,punc_vn) = 1;
end

I = eye(H2_c);
H_combine = [H_combine;punc_payload_mat,I,I];

% write H_combine to PCM 
writePCM(H_combine,H_combine_file);


% write payload data with puncture bits position 
fileID = fopen(RV1_punc_pos, 'w');
for pos=RV1_punc
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);