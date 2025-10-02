%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\BCH_31_16.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_10_5.txt'; % extra data
H_combine_file = 'Origin_P31_E10.txt';
payload_punc_pos = "Pos_payload.txt";
H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);



Already_Punc_VNs = [];
puncture_bits_num = H2_c;
punc_pos_bits = Rate_Compatible_Punctured_With_Short_Block_Lengths(H1,puncture_bits_num,Already_Punc_VNs);
punc_pos_bits = setdiff(punc_pos_bits,Already_Punc_VNs);
payload_punc = punc_pos_bits;
%%
sup_mat = zeros([H2_c H1_c]);
for i=H2_c
    punc_VN = payload_punc(i);
    sup_mat(i,punc_VN)=1;
end

zero1 = zeros([H1_r H2_c]);
zero2 = zeros([H2_r H1_c]);
zero3 = zeros([H2_r H2_c]);
I = eye(H2_c);
H_combine = [[H1,zero1,zero1];[zero2,H2,zero3];[sup_mat,I,I]];
%%

% write H_combine to PCM 
writePCM(H_combine,H_combine_file);




fileID = fopen(payload_punc_pos, 'w');
for pos=payload_punc
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);