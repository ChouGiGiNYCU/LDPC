%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
H2_file = 'C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt'; % extra data
H_combine_file = 'PCM_Punc_kSR_Punc105.txt';
RV1_punc_pos = "Pos_RV1_puncpos.txt";
payload_punc_pos = "Pos_payload.txt";
Combine_number = 7;
H1 = readHFromFileByLine(H1_file);
H2 = readHFromFileByLine(H2_file);
[H1_r,H1_c] = size(H1);
[H2_r,H2_c] = size(H2);
% H_combine = [  H1     zero1   zero2 
%               zero3     H2    zero4
%               punc_p  punc_e    I
%             ] 
zero1 = zeros([H1_r H2_c]);
zero2 = zeros([H2_r H2_c]);
zero3 = zeros([H2_r H1_c]);
zero4 = zeros([H2_c H2_c]);



Already_Punc_VNs = [];
puncture_bits_num = Combine_number*H2_c*2;
punc_pos_bits = Rate_Compatible_Punctured_With_Short_Block_Lengths(H1,puncture_bits_num,Already_Punc_VNs);
punc_pos_bits = setdiff(punc_pos_bits,Already_Punc_VNs);
RV1_punc = punc_pos_bits(1:Combine_number*H2_c);
payload_punc = punc_pos_bits(Combine_number*H2_c+1:end);
%%
H_combine = [];
for r=1:Combine_number+1 
    if r==1
        muti_zeros = repmat(zero1,1,Combine_number*2);
        H_combine = [H_combine;H1,muti_zeros];
    else
        right_rep = (Combine_number*2 + 1) -  (r); % colsize - r_idx
        left_rep  = (Combine_number*2 + 1) - right_rep - 2; % colsize - H2(1) - zeros3(1) - right_rep
        right_zeros = repmat(zero2,1,right_rep);
        left_zeros  = repmat(zero2,1,left_rep);
        H_combine = [H_combine;zero3,left_zeros,H2,right_zeros];
    end
end
% compose lowwer Combine H
for r=1:Combine_number
    % compose punc_matrix
    punc_mat = zeros(H2_c,H1_c);
    start_idx = (r-1)*H2_c+1;
    end_idx   =  r*H2_c;
    punc_poses = punc_pos_bits(start_idx:end_idx);
    for i=1:size(punc_poses,2)
        punc_pos = punc_poses(i);
        punc_mat(i,punc_pos) = 1;
    end
    right_rep = (Combine_number + 2) -  (r+1) - 1; % colsize - r_idx - I(1) 
    left_rep  = (Combine_number + 2) - right_rep - 3; % colsize - punc_mat(1) - I(2) - right_rep
    right_zeros = repmat(zero4,1,right_rep);
    left_zeros  = repmat(zero4,1,left_rep);
    I = eye(H2_c);
    H_combine  = [H_combine;punc_mat,left_zeros,I,right_zeros,left_zeros,I,right_zeros];
end
%%

% write H_combine to PCM 
writePCM(H_combine,H_combine_file);


% write payload data with puncture bits position 
fileID = fopen(RV1_punc_pos, 'w');
for pos=RV1_punc
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);

fileID = fopen(payload_punc_pos, 'w');
for pos=payload_punc
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);