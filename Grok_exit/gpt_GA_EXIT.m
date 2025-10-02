clear; clc;


% ====== 讀 H ======
filename = "PCM_P1008_E15BCH_Structure.txt";
Payload_length = 1008;
Extra_length   = 15;
Payload_inf0 = 504;
Extra_info = 7;
% 依你當前設計的 puncture 位置
puncture_idx = [21,25,47,57,116,134,164,178,183,187,234,246,284,299,330, ...
                745 ,759,774 ,801 ,806 ,837 ,843 ,878 ,883 ,887 ,920 ,929 ,938 ,939,976 ,
                Payload_length+1:(Payload_length+Extra_length), ...
                Payload_length+Extra_length*2+1:(Payload_length+Extra_length*3)];
rate = (Extra_info+Payload_inf0)/Payload_length
H  = readHFromFileByLine(filename);      % m x n, 0/1
SNR =10;
figure;
PUNC_EXIT(H,puncture_idx,SNR,rate);

figure;
filename = "PCM_P1008_E15BCH_Structure.txt";
rate=(Extra_info+Payload_inf0)/Payload_length;
H  = readHFromFileByLine(filename);      % m x n, 0/1
EXIT(H,SNR,rate);

figure;
rate = 7/15;
filename = "C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt";
H  = readHFromFileByLine(filename);      % m x n, 0/1
EXIT(H,SNR,rate);
