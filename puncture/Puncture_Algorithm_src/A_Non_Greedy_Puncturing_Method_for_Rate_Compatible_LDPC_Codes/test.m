%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_96_48.txt'; % extra data
H_combine_file = 'PCM_Punc_NonGreedyAlg_Punc96.txt';
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
puncture_bits_num = H2_c;
punc_pos_bits = Puncturing_Algorithm_For_NonGreedy_Method(H1,puncture_bits_num);
%%
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