
% 指定 CSV 檔案路徑
clear all;
clc;
close all;
payload_file  = "C:\Users\USER\Desktop\LDPC\Throughput\CBP_Enhanced_Structure\CBP_punc_payload_0bits/CBP_Punc_P1008_E15_it_200_punc0.csv";
Performace_1SR_Payload_file = "C:\Users\USER\Desktop\LDPC\Throughput\CBP_Enhanced_Structure\CBP_Payload1008_Extra15/Punc_P1008_E15_PayLoad_it200_EnhanceStruc.csv";
% Performace_1SR_Extra_file= "C:\Users\USER\Desktop\LDPC\Throughput\CBP_Origin_Structure\CBP_Sup_Punc_Payload1008_Extra15(BCH)_Close\Punc_1SR_P1008_E15BCH_Extra_it200_normal.csv";

% Performace_kSR_Payload_file = "C:\Users\USER\Desktop\LDPC\Throughput\CBP_Origin_Structure\CBP_Sup_Punc_Payload1008_Extra15(BCH)_Close\Punc_1SR_P1008_E15BCH_PayLoad_it200_worst.csv";
% Performace_kSR_Extra_file = "C:\Users\USER\Desktop\LDPC\Throughput\CBP_Origin_Structure\CBP_Sup_Punc_Payload1008_Extra15(BCH)_Close\Punc_1SR_P1008_E15BCH_Extra_it200_worst.csv";

payload = readtable(payload_file);
Performace_1SR_Payload = readtable(Performace_1SR_Payload_file);
% Performace_1SR_Extra   = readtable(Performace_1SR_Extra_file);

% Performace_kSR_Payload = readtable(Performace_kSR_Payload_file);
% Performace_kSR_Extra   = readtable(Performace_kSR_Extra_file);

% FER
figure();

% plot CBP Payload
semilogy(payload.Eb_over_N0, payload.frame_error_rate, '->', ...
    'Color', [0.8500 0.3250 0.0980],'MarkerFaceColor',[0.8500 0.3250 0.0980],'LineWidth', 1.5); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(Performace_1SR_Payload.Eb_over_N0, Performace_1SR_Payload.frame_error_rate, '-pentagram', ...
    'Color', [0 0.447 0.741],'MarkerFaceColor',[0 0.447 0.741],'LineWidth', 1.5); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表
% plot ML_Extra
% semilogy(Performace_1SR_Extra.Eb_over_N0, Performace_1SR_Extra.frame_error_rate, '--pentagram', ...
%     'Color', [0 0.447 0.741],'LineWidth', 1.5); % 第一組數據
% hold on; % 繪製第二組數據時保留當前圖表
% 
% semilogy(Performace_kSR_Payload.Eb_over_N0, Performace_kSR_Payload.frame_error_rate,'-square', ...
%     'Color',[0.8500 0.3250 0.0980],'MarkerFaceColor',[0.8500 0.3250 0.0980], 'LineWidth', 1.5); % 第二組數據
% hold on;
% semilogy(Performace_kSR_Extra.Eb_over_N0, Performace_kSR_Extra.frame_error_rate,'--square', ...
%     'Color',[0.8500 0.3250 0.0980], 'LineWidth', 1.5); % 第二組數據
% hold on;

xlabel('SNR');
ylabel('FER');
title('Performance  punc0bits')
legend("Payload","Enhanced 1-SR Payload");
grid on;  % 開啟格線


legend("Payload","1SR Payload","1SR Extra","Worst Payload","Worst Extra")




% FER
figure();
% plot CBP Payload
semilogy(Performace_1SR_Payload.Eb_over_N0, Performace_1SR_Payload.BER_it_199, '-pentagram', ...
    'Color', [0 0.447 0.741],'MarkerFaceColor',[0 0.447 0.741],'LineWidth', 1.5); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表
% plot ML_Extra
semilogy(Performace_1SR_Extra.Eb_over_N0, Performace_1SR_Extra.BER_it_199, '--pentagram', ...
    'Color', [0 0.447 0.741],'LineWidth', 1.5); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(Performace_kSR_Payload.Eb_over_N0, Performace_kSR_Payload.BER_it_199,'-square', ...
    'Color',[0.8500 0.3250 0.0980],'MarkerFaceColor',[0.8500 0.3250 0.0980], 'LineWidth', 1.5); % 第二組數據
hold on;
semilogy(Performace_kSR_Extra.Eb_over_N0, Performace_kSR_Extra.BER_it_199,'--square', ...
    'Color',[0.8500 0.3250 0.0980], 'LineWidth', 1.5); % 第二組數據
hold on;



xlabel('SNR');
ylabel('BER');
title('Performance')
legend("1SR Payload","1SR Extra","Worst Payload","Worst Extra")
grid on;  % 開啟格線





%%
figure();

% plot CBP Payload
semilogy(payload.Eb_over_N0, payload.BER_it_199, '->', ...
    'Color', [0.8500 0.3250 0.0980],'MarkerFaceColor',[0.8500 0.3250 0.0980],'LineWidth', 1.5); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(Performace_1SR_Payload.Eb_over_N0, Performace_1SR_Payload.BER_it_199, '-pentagram', ...
    'Color', [0 0.447 0.741],'MarkerFaceColor',[0 0.447 0.741],'LineWidth', 1.5); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表
% plot ML_Extra
% semilogy(Performace_1SR_Extra.Eb_over_N0, Performace_1SR_Extra.frame_error_rate, '--pentagram', ...
%     'Color', [0 0.447 0.741],'LineWidth', 1.5); % 第一組數據
% hold on; % 繪製第二組數據時保留當前圖表
% 
% semilogy(Performace_kSR_Payload.Eb_over_N0, Performace_kSR_Payload.frame_error_rate,'-square', ...
%     'Color',[0.8500 0.3250 0.0980],'MarkerFaceColor',[0.8500 0.3250 0.0980], 'LineWidth', 1.5); % 第二組數據
% hold on;
% semilogy(Performace_kSR_Extra.Eb_over_N0, Performace_kSR_Extra.frame_error_rate,'--square', ...
%     'Color',[0.8500 0.3250 0.0980], 'LineWidth', 1.5); % 第二組數據
% hold on;

xlabel('SNR');
ylabel('BER');
title('Performance  punc0bits')
legend("Payload","Enhanced 1-SR Payload");
grid on;  % 開啟格線
