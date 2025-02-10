clear;
clc;
%%
H_filename = "C:\Users\USER\Desktop\LDPC\puncture\1_SR\CBP\H_10_5.txt";
H_filename_2 = "C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt";
H_1 = readHFromFileByLine(H_filename);
% H_2 = readHFromFileByLine(H_filename_2);
[rows,cols]=size(H_1);
H_row_xor_num = 2; % for H2
% H_row_xor_num = 10; % for H3
% H_row_xor_num = 30; % for H4
% H_row_xor_num = 50; % for H5
[N, K, M_ori, M_sys, sysG, sysH, G, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(H_1);
b = mod(G*transpose(H_1),2);
if (sum(sum(b),2)==0)
    disp("G*H_1 is all zero");
end
H_2 = H_1;
op_row = randi([1 rows],1,1);
op_idx = randi([1 rows],1,H_row_xor_num);
% 遍歷每對 op_row 和 op_idx
for i = 1:length(op_idx)
    % 將 op_idx 的行加到 op_row 對應的行
    if op_row~= op_idx(i)
        H_2(op_idx(i), :) = mod(H_2(op_row, :) + H_2(op_idx(i), :),2) ;
    end
end
H_2_girth = findGirth(H_2);
fprintf("h2 girth : %f\n",H_2_girth);
b = mod(G*transpose(H_2),2);
if (sum(sum(b),2)==0)
    disp("G*H_2 is all zero");
end

writePCM(H_2,H_filename_2);

