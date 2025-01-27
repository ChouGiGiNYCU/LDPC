% 指定 CSV 檔案路徑
clear all;
clc;
%%
BP_H1 = 'CBP_Result_H_96_48_it_20.csv';
BP_H2= 'CBP_Result_H2_96_48_it_20.csv';
NMS_H1= 'NMS_Result_H_96_48_it_20.csv';
NMS_H2= 'NMS_Result_H2_96_48_it_20.csv';
BP_Combine_H2 = 'CBP_Combine_H2_96_48_it1_2_it2_2_it_20.csv';
NNBP_Combine_H2 = 'BPNN_96_48_DiffH_NEW.csv';

%%
% 讀取 CSV 檔案
data_BP_H1 = readtable(BP_H1);
data_BP_H2 = readtable(BP_H2);
data_NMS_H1 = readtable(NMS_H1);
data_NMS_H2 = readtable(NMS_H2);
data_BP_Combine_H2 = readtable(BP_Combine_H2);
data_NNBP_Combine_H2 = readtable(NNBP_Combine_H2);
%% FER
figure();
semilogy(data_BP_H1.Eb_over_N0, data_BP_H1.frame_error_rate, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表


semilogy(data_BP_H2.Eb_over_N0, data_BP_H2.frame_error_rate, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(data_NMS_H1.Eb_over_N0, data_NMS_H1.frame_error_rate,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_NMS_H2.Eb_over_N0, data_NMS_H2.frame_error_rate,'--pentagram', ...
    'Color', [0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_BP_Combine_H2.Eb_over_N0, data_BP_Combine_H2.frame_error_rate,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_NNBP_Combine_H2.SNR, data_NNBP_Combine_H2.FER,'--diamond', ...
    'Color', [0.6350 0.0780 0.1840], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('FER');
legend('BP_ H1','BP_ H2','NMS_ H1','NMS_ H2','BP_ Combine','NNBP_ Combine');
% legend('BP_ H1','BP_ H2','NMS_ H1','NMS_ H2','BP_ Combine');
%% BER
figure();
semilogy(data_BP_H1.Eb_over_N0, data_BP_H1.BER_it_19, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(data_BP_H2.Eb_over_N0, data_BP_H2.BER_it_19, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(data_NMS_H1.Eb_over_N0, data_NMS_H1.BER_it_19,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_NMS_H2.Eb_over_N0, data_NMS_H2.BER_it_19,'--square', ...
    'Color',[0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_BP_Combine_H2.Eb_over_N0, data_BP_Combine_H2.BER_it_19,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_NNBP_Combine_H2.SNR, data_NNBP_Combine_H2.BER,'--diamond', ...
    'Color', [0.6350 0.0780 0.1840], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('BER');
legend('BP_ H1','BP_ H2','NMS_ H1','NMS_ H2','BP_ Combine','NNBP_ Combine');
% legend('BP_ H1','BP_ H2','NMS_ H1','NMS_ H2','BP_ Combine');


%% H3
% 指定 CSV 檔案路徑
clear all;
clc;
 
BP_H1 = 'CBP_Result_H_96_48_it_20.csv';
BP_H3= 'CBP_Result_H3_96_48_it_20.csv';
NMS_H1= 'NMS_Result_H_96_48_it_20.csv';
NMS_H3= 'NMS_Result_H3_96_48_it_20.csv';
BP_Combine_H3 = 'CBP_Combine_H3_96_48_it1_2_it2_2_it_20.csv';
% 讀取 CSV 檔案
data_BP_H1 = readtable(BP_H1);
data_BP_H3 = readtable(BP_H3);
data_NMS_H1 = readtable(NMS_H1);
data_NMS_H3 = readtable(NMS_H3);
data_BP_Combine_H3 = readtable(BP_Combine_H3);

%% FER
figure();
semilogy(data_BP_H1.Eb_over_N0, data_BP_H1.frame_error_rate, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表


semilogy(data_BP_H3.Eb_over_N0, data_BP_H3.frame_error_rate, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(data_NMS_H1.Eb_over_N0, data_NMS_H1.frame_error_rate,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_NMS_H3.Eb_over_N0, data_NMS_H3.frame_error_rate,'--pentagram', ...
    'Color', [0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_BP_Combine_H3.Eb_over_N0, data_BP_Combine_H3.frame_error_rate,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('FER');
legend('BP_ H1','BP_ H3','NMS_ H1','NMS_ H3','BP_ Combine');

%% BER
figure();
semilogy(data_BP_H1.Eb_over_N0, data_BP_H1.BER_it_19, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表


semilogy(data_BP_H3.Eb_over_N0, data_BP_H3.BER_it_19, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(data_NMS_H1.Eb_over_N0, data_NMS_H1.BER_it_19,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_NMS_H3.Eb_over_N0, data_NMS_H3.BER_it_19,'--pentagram', ...
    'Color', [0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_BP_Combine_H3.Eb_over_N0, data_BP_Combine_H3.BER_it_19,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('BER');
legend('BP_ H1','BP_ H3','NMS_ H1','NMS_ H3','BP_ Combine');

%% H4
% 指定 CSV 檔案路徑
clear all;
clc;
 
BP_H1 = 'CBP_Result_H_96_48_it_20.csv';
BP_H4= 'CBP_Result_H4_96_48_it_20.csv';
NMS_H1= 'NMS_Result_H_96_48_it_20.csv';
NMS_H4= 'NMS_Result_H4_96_48_it_20.csv';
BP_Combine_H4 = 'CBP_Combine_H4_96_48_it1_2_it2_2_it_20.csv';
% 讀取 CSV 檔案
data_BP_H1 = readtable(BP_H1);
data_BP_H4 = readtable(BP_H4);
data_NMS_H1 = readtable(NMS_H1);
data_NMS_H4 = readtable(NMS_H4);
data_BP_Combine_H4 = readtable(BP_Combine_H4);

%% FER
figure();
semilogy(data_BP_H1.Eb_over_N0, data_BP_H1.frame_error_rate, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表


semilogy(data_BP_H4.Eb_over_N0, data_BP_H4.frame_error_rate, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(data_NMS_H1.Eb_over_N0, data_NMS_H1.frame_error_rate,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_NMS_H4.Eb_over_N0, data_NMS_H4.frame_error_rate,'--pentagram', ...
    'Color', [0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_BP_Combine_H4.Eb_over_N0, data_BP_Combine_H4.frame_error_rate,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('FER');
legend('BP_ H1','BP_ H4','NMS_ H1','NMS_ H4','BP_ Combine');

%% BER
figure();
semilogy(data_BP_H1.Eb_over_N0, data_BP_H1.BER_it_19, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表


semilogy(data_BP_H4.Eb_over_N0, data_BP_H4.BER_it_19, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(data_NMS_H1.Eb_over_N0, data_NMS_H1.BER_it_19,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_NMS_H4.Eb_over_N0, data_NMS_H4.BER_it_19,'--pentagram', ...
    'Color', [0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(data_BP_Combine_H4.Eb_over_N0, data_BP_Combine_H4.BER_it_19,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('BER');
legend('BP_ H1','BP_ H4','NMS_ H1','NMS_ H4','BP_ Combine');

%%
clear all;
clc;


BP_Combine_H2 = 'CBP_Combine_H2_96_48_it1_2_it2_2_it_20.csv';
BP_Combine_H3 = 'CBP_Combine_H3_96_48_it1_2_it2_2_it_20.csv';
BP_Combine_H4 = 'CBP_Combine_H4_96_48_it1_2_it2_2_it_20.csv';
BP_Combine_H2_MODEL =  'BPNN_96_48_DiffH_NEW.csv';
data_BP_Combine_H2 = readtable(BP_Combine_H2);
data_BP_Combine_H3 = readtable(BP_Combine_H3);
data_BP_Combine_H4 = readtable(BP_Combine_H4);
data_BP_Combine_H2_model = readtable(BP_Combine_H2_MODEL);
%FER
figure();
semilogy(data_BP_Combine_H2.Eb_over_N0, data_BP_Combine_H2.frame_error_rate, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_H3.Eb_over_N0, data_BP_Combine_H3.frame_error_rate, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_H4.Eb_over_N0, data_BP_Combine_H4.frame_error_rate,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_H2_model.SNR, data_BP_Combine_H2_model.FER,'--square', ...
    'Color',[0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('FER');
legend('BP_ Combine_ H2','BP_ Combine_ H3','BP_ Combine_ H4','BP_ Combine_ H2_ Model');

%BER
figure();
semilogy(data_BP_Combine_H2.Eb_over_N0, data_BP_Combine_H2.BER_it_19, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_H3.Eb_over_N0, data_BP_Combine_H3.BER_it_19, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_H4.Eb_over_N0, data_BP_Combine_H4.BER_it_19,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_H2_model.SNR, data_BP_Combine_H2_model.BER,'--square', ...
    'Color',[0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('BER');
legend('BP_ Combine_ H2','BP_ Combine_ H3','BP_ Combine_ H4','BP_ Combine_ H2_ Model');

%% Different Iteration
clear all;
clc;


BP_Combine_it2 = 'CBP_Combine_H2_96_48_it1_2_it2_2_it_20.csv';
BP_Combine_it5 = 'CBP_Combine_H2_96_48_it1_5_it2_5_it_20.csv';
BP_Combine_it10 = 'CBP_Combine_H2_96_48_it1_10_it2_10_it_20.csv';
BP_origin = 'CBP_Result_H_96_48_it_20.csv'
data_BP_Combine_it2 = readtable(BP_Combine_it2);
data_BP_Combine_it5 = readtable(BP_Combine_it5);
data_BP_Combine_it10 = readtable(BP_Combine_it10);
data_BP_origin = readtable(BP_origin);
%FER
figure();
semilogy(data_BP_Combine_it2.Eb_over_N0, data_BP_Combine_it2.frame_error_rate, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_it5.Eb_over_N0, data_BP_Combine_it5.frame_error_rate, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_it10.Eb_over_N0, data_BP_Combine_it10.frame_error_rate,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_origin.Eb_over_N0, data_BP_origin.frame_error_rate,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('FER');
legend('BP_ Combine_ H2_ It 2','BP_ Combine_ H2_ It 5','BP_ Combine_ H2_ It 10','BP_ H1');

%BER
figure();
semilogy(data_BP_Combine_it2.Eb_over_N0, data_BP_Combine_it2.BER_it_19, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_it5.Eb_over_N0, data_BP_Combine_it5.BER_it_19, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_Combine_it10.Eb_over_N0, data_BP_Combine_it10.BER_it_19,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據

hold on; % 繪製第二組數據時保留當前圖表
semilogy(data_BP_origin.Eb_over_N0, data_BP_origin.BER_it_19,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據

hold off;
xlabel('SNR');
ylabel('BER');
legend('BP_ Combine_ H2_ It 2','BP_ Combine_ H2_ It 5','BP_ Combine_ H2_ It 10','BP_ H1');
%%
% 指定 CSV 檔案路徑
clear all;


BP_Combine_H3 = 'CBP_Combine_H3_96_48_it_20.csv';
BP_Combine_H3_diffIt = 'CBP_Combine_H3_96_48_it1_10_it2_10.csv';
% 讀取 CSV 檔案
data_BP_Combine_H3 = readtable(BP_Combine_H3);
data_BP_Combine_H3_diffIt = readtable(BP_Combine_H3_diffIt);
%% FER
semilogy(data_BP_Combine_H3.Eb_over_N0, data_BP_Combine_H3.frame_error_rate, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表


semilogy(data_BP_Combine_H3_diffIt.Eb_over_N0, data_BP_Combine_H3_diffIt.frame_error_rate, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold off;
xlabel('SNR');
ylabel('FER');
legend('BP_ Combine_ EachIt-2','BP_ Combine_ EachIt-10');

%% BER
semilogy(data_BP_Combine_H3.Eb_over_N0, data_BP_Combine_H3.BER_it_19, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表


semilogy(data_BP_Combine_H3_diffIt.Eb_over_N0, data_BP_Combine_H3_diffIt.BER_it_19, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold off;

xlabel('SNR');
ylabel('BER');
legend('BP_ Combine_ EachIt-2','BP_ Combine_ EachIt-10');