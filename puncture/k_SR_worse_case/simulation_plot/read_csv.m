% 指定 CSV 檔案路徑
clear all;
clc;
%%
Sup_Punc_P_file = 'Punc_1SR_PayLoad2_G.csv';
Sup_Punc_E_file =  'Punc_1SR_Extra2_G.csv';
BP_P_file = 'CBP_PEG504X1008_it_200.csv';
BP_E_file = 'CBP_H5x10_it_200.csv';
BP_Punc_P_E_file = 'CBP_punc_PEG504X1008_it_200_G.csv';
BP_Punc_P_Z_file = 'CBP_punc_PEG504X1008_it_200_Z.csv';
Prev_Sup_P_file = 'CBP_Sup_PEG504X1008_it_200.csv';
Prev_Sup_E_file =  'CBP_Sup_H5x10_it_200.csv';
%%
% 讀取 CSV 檔案
Sup_Punc_P = readtable(Sup_Punc_P_file);
Sup_Punc_E = readtable(Sup_Punc_E_file);
BP_P = readtable(BP_P_file);
BP_E = readtable(BP_E_file);
BP_Punc_P_E = readtable(BP_Punc_P_E_file);
BP_Punc_P_Z = readtable(BP_Punc_P_Z_file);
Prev_Sup_P = readtable(Prev_Sup_P_file);
Prev_Sup_E = readtable(Prev_Sup_E_file);
%% FER
figure();
semilogy(Sup_Punc_P.Eb_over_N0, Sup_Punc_P.frame_error_rate, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表


semilogy(Sup_Punc_E.Eb_over_N0, Sup_Punc_E.frame_error_rate, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(BP_P.Eb_over_N0, BP_P.frame_error_rate,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(BP_E.Eb_over_N0, BP_E.frame_error_rate,'--pentagram', ...
    'Color', [0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(BP_Punc_P_E.Eb_over_N0, BP_Punc_P_E.frame_error_rate,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(BP_Punc_P_Z.Eb_over_N0, BP_Punc_P_Z.frame_error_rate,'--hexagram', ...
    'Color', [0.6350 0.0780 0.1840], 'LineWidth', 1); % 第二組數據

semilogy(Prev_Sup_P.Eb_over_N0, Prev_Sup_P.frame_error_rate,'--^', ...
    'Color', [0.3010 0.7450 0.9330], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(Prev_Sup_E.Eb_over_N0, Prev_Sup_E.frame_error_rate,'--v', ...
    'Color', [0.6350 0.7450 0.1840], 'LineWidth', 1); % 第二組數據

hold off;

xlabel('SNR');
ylabel('FER');
title('Iteration = 200')
legend('Sup_ Punc_ PayLoad','Sup_ Punc_ Extra','BP_ PayLoad','BP_ Extra','BP_ Punc_ PayLoad_ Encode','BP_ Punc_ PayLoad_ Zero','Prev_ Sup_ P','Prev_ Sup_ E');

%% BER
figure();
semilogy(Sup_Punc_P.Eb_over_N0, Sup_Punc_P.BER_it_199, '--o', ...
    'Color', [0 0.447 0.741],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(Sup_Punc_E.Eb_over_N0, Sup_Punc_E.BER_it_199, '--x', ...
    'Color', [0.8500 0.3250 0.0980],'LineWidth', 1); % 第一組數據
hold on; % 繪製第二組數據時保留當前圖表

semilogy(BP_P.Eb_over_N0, BP_P.BER_it_199,'--square', ...
    'Color',[0.9290 0.6940 0.1250], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(BP_E.Eb_over_N0, BP_E.BER_it_199,'--pentagram', ...
    'Color', [0.4940 0.1840 0.5560], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(BP_Punc_P_E.Eb_over_N0, BP_Punc_P_E.BER_it_199,'--*', ...
    'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(BP_Punc_P_Z.Eb_over_N0, BP_Punc_P_Z.BER_it_199,'--hexagram', ...
    'Color', [0.6350 0.0780 0.1840], 'LineWidth', 1); % 第二組數據

semilogy(Prev_Sup_P.Eb_over_N0, Prev_Sup_P.BER_it_199,'--^', ...
    'Color', [0.3010 0.7450 0.9330], 'LineWidth', 1); % 第二組數據
hold on;

semilogy(Prev_Sup_E.Eb_over_N0, Prev_Sup_E.BER_it_199,'--v', ...
    'Color', [0.6350 0.7450 0.1840], 'LineWidth', 1); % 第二組數據
hold off;

xlabel('SNR');
ylabel('BER');
title('Iteration = 200');
legend('Sup_ Punc_ PayLoad','Sup_ Punc_ Extra','BP_ PayLoad','BP_ Extra','BP_ Punc_ PayLoad_ Encode','BP_ Punc_ PayLoad_ Zero','Prev_ Sup_ P','Prev_ Sup_ E');



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