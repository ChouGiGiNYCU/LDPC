%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_1020_330_690_BG1_Z15_fix.txt'; % payload data
Z = 15;
RV1_num = 63;
puncture_position_bits_file = "RV1_punc_pos_file.txt";
H1 = readHFromFileByLine(H1_file);

[H1_r,H1_c] = size(H1);

punc_pos_bits_num = RV1_num + 2*Z;
Already_punc_VNs = 1:(2*Z);
punc_pos_bits = Rate_Compatible_Punctured_With_Short_Block_Lengths(H1,punc_pos_bits_num,Already_punc_VNs);
punc_pos_bits = setdiff(punc_pos_bits,Already_punc_VNs);
%% check  punc_pos_bits 是否都是 1-SR
% for Punc_VN=punc_pos_bits 
%     Punc_SCNs = transpose(find(H1(:,Punc_VN)==1));
%     SCNs_num = 0;
%     for SCN=Punc_SCNs
%         Nei_SCN_VNs = setdiff(find(H1(SCN,:)==1),Punc_VN);
%         if ~any(ismember(punc_pos_bits,Nei_SCN_VNs))
%             SCNs_num  = SCNs_num + 1;
%         end
%     end
%     if SCNs_num==0
%         disp("find no 1-SR punc vn !!\n");
%     end
% end
%%
% write payload data with puncture bits position 
fileID = fopen(puncture_position_bits_file, 'w');
for pos=punc_pos_bits
    fprintf(fileID, '%d ', pos);
end
fprintf(fileID, '\n');
fclose(fileID);

