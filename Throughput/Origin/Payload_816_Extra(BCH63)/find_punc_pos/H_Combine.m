%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\H_816_272.txt'; % payload data

puncture_position_bits_file = "RV1_punc_pos_file.txt";
H1 = readHFromFileByLine(H1_file);

[H1_r,H1_c] = size(H1);

punc_pos_bits_num = 63;
% punc_pos_bits = oneSR_method(H1,H2_c);
punc_pos_bits = Rate_Compatible_Punctured_With_Short_Block_Lengths(H1,punc_pos_bits_num,[]);
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

