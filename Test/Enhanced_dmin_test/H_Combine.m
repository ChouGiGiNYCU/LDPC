%%
clc;
clear all;
close all;
% 部分 Extra 傳送 (部分 payload 被 Puncture)
% 所有Extra 都跟payload 做 New_Structure 連結
%%
H1_file = "C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt"; % payload data
H_combine_file = "New_BCH_Enhanced.txt";
H1 = readHFromFileByLine(H1_file);
[H1_r,H1_c] = size(H1);

%%
% H_combine = [  
%                     H2        zero          zero    zero
%                     I          I            zero    zero
%                     I         zeor            I     zeor
%                    zero       zeor            I       I      
%             ] 
zero1 = zeros([H1_r H1_c]);
zero2 = zeros([H1_c H1_c]);
I = eye(H1_c);

H_combine = [  
                H1,zero1,zero1,zero1;
                I,I,zero2,zero2;
                I,zero2,I,zero2;
                zero2,zero2,I,I;
            
            ];

%% 輸出檔案
writePCM(H_combine,H_combine_file); % write H_combine to PCM 
