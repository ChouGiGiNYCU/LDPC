%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_10_5.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\rep_1_3.txt'; % payload data

puncture_position_bits_file = "Pos_puncture_non_dmin.txt";
H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);
% H_combine = [  H1     zero1   zero2 
%               zero3     H2    zero4
%               punc_p  punc_e    I
%             ] 
punc_nums = H2_c;
punc_pos_bits = [1 42 96];
M_mat = zeros(H2_c,H1_c);
for i=1:punc_nums 
    M_mat(i,punc_pos_bits(i))=1;
end
% ans = [H1,                 zeros(H1_r, H2_c), zeros(H1_r, H2_c); zeros(H2_r, H1_c),  H2,                zeros(H2_r, H2_c); M_mat,              eye(H2_c),         eye(H2_c) ];
H_combine = [ H1,                 zeros(H1_r, H2_c), zeros(H1_r, H2_c);
              zeros(H2_r, H1_c),  H2,                zeros(H2_r, H2_c);
              M_mat,              eye(H2_c),         eye(H2_c) ];
%%
% write payload data with puncture bits position 
fileID = fopen(puncture_position_bits_file, 'w');
for pos=punc_pos_bits
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);

writePCM(H_combine,"H_combine_No_dmin.txt");



%%

H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_10_5.txt'; % payload data

puncture_position_bits_file = "Pos_puncture_1SR.txt";
H1 = readHFromFileByLine(H1_file);

[H1_r,H1_c] = size(H1);

% H_combine = [  H1        zero2 
%               
%               punc_p      I
%             ] 
punc_nums = 3;
punc_pos_bits = [1 5 7];
M_mat = zeros(punc_nums,H1_c);
for i=1:punc_nums 
    M_mat(i,punc_pos_bits(i))=1;
end

H_combine = [ H1,      zeros(H1_r, punc_nums);
              
              M_mat,   eye(punc_nums) ];

% write payload data with puncture bits position 
fileID = fopen(puncture_position_bits_file, 'w');
for pos=punc_pos_bits
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);

writePCM(H_combine,"H_combine.txt");