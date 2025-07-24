%%
clc;
clear all;
close all;
%%
H1_file = 'C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt'; % payload data
target_coderate = 0.75;
puncture_position_bits_file = "Pos_puncture_Coderate_075.txt";
H1 = readHFromFileByLine(H1_file);

[H1_r,H1_c] = size(H1);
% H_combine = [  H1     zero1   zero2 
%               zero3     H2    zero4
%               punc_p  punc_e    I
%             ] 
punc_pos_bits_num = ceil(((H1_c * target_coderate) - H1_r ) / target_coderate);
% punc_pos_bits = oneSR_method(H1,H2_c);
punc_pos_bits = Puncturing_Algorithm_For_Finit_Length(H1,punc_pos_bits_num,[]);
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

