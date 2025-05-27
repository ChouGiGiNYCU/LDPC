%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_96_48.txt'; % extra data
H_combine_file = 'PCM_Punc_RCScheme_Punc96.txt';
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
tmp_zero = zeros([H2_c H1_c]);
I = eye(H2_c);
H_combine = [[H1,zero1,zero2];[zero3,H2,zero4]];
H_combine = [H_combine;[tmp_zero,I,I]];
punc_payload_mat = zeros([H2_c,H1_c]);
%%
base_row = H1_r + H2_r;
Already_Punc_VNs = (H1_c+1):(H1_c+H2_c);
rowsize = H1_r;
colsize = H1_c;
payload_punc_pos = [];
for i=1:H2_c
    puncture_bits_num = 1+size(Already_Punc_VNs,2);
    punc_pos_bit = Puncturing_Algorithm_For_Finit_Length(H_combine,puncture_bits_num,Already_Punc_VNs);
    Already_Punc_VNs = union(Already_Punc_VNs,punc_pos_bit);
    H_combine(base_row+i,punc_pos_bit(end)) = 1;
    payload_punc_pos = [payload_punc_pos,punc_pos_bit(end)]
end

%%
% write H_combine to PCM 
writePCM(H_combine,H_combine_file);

% write payload data with puncture bits position 
fileID = fopen(puncture_position_bits_file, 'w');
for pos=payload_punc_pos
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);