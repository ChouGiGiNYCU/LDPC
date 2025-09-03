
% ���w CSV �ɮ׸��|
clear all;
clc;
close all;

CBP_Payload_file = "CBP_H96x48_it_200.csv";
punc_dmin_file   = "Combine_payload_dmin.csv";
punc_nondmin_file = "Combine_payload_No_dmin.csv";

% Ū�� CSV �ɮ�
CBP_Payload = readtable(CBP_Payload_file);
punc_dmin = readtable(punc_dmin_file);

punc_nondmin = readtable(punc_nondmin_file);

% FER
figure();
% plot CBP Payload
semilogy(CBP_Payload.Eb_over_N0, CBP_Payload.frame_error_rate, '-pentagram', ...
    'Color', [0 0.447 0.741],'MarkerFaceColor',[0 0.447 0.741],'LineWidth', 1.5); % �Ĥ@�ռƾ�
hold on; % ø�s�ĤG�ռƾڮɫO�d��e�Ϫ�

semilogy(punc_dmin.Eb_over_N0, punc_dmin.frame_error_rate,'-square', ...
    'Color',[0.8500 0.3250 0.0980],'MarkerFaceColor',[0.8500 0.3250 0.0980], 'LineWidth', 1.5); % �ĤG�ռƾ�
hold on;



semilogy(punc_nondmin.Eb_over_N0, punc_nondmin.frame_error_rate,'-diamond', ...
    'Color',[0.4940 0.1840 0.5560],'MarkerFaceColor',[0.4940 0.1840 0.5560], 'LineWidth', 1.5); % �ĤG�ռƾ�
hold on;


xlabel('SNR');
ylabel('FER');
title('Performance')
legend('CBP Payload96','Origin structure - punc dmin position','Origin structure - punc nondmin position');
grid on;  % �}�Ү�u

% FER
figure();
semilogy(CBP_Payload.Eb_over_N0, CBP_Payload.BER_it_199, '-pentagram', ...
    'Color', [0 0.447 0.741],'MarkerFaceColor',[0 0.447 0.741],'LineWidth', 1.5); % �Ĥ@�ռƾ�
hold on; % ø�s�ĤG�ռƾڮɫO�d��e�Ϫ�

semilogy(punc_dmin.Eb_over_N0, punc_dmin.BER_it_199,'-square', ...
    'Color',[0.8500 0.3250 0.0980],'MarkerFaceColor',[0.8500 0.3250 0.0980], 'LineWidth', 1.5); % �ĤG�ռƾ�
hold on;



semilogy(punc_nondmin.Eb_over_N0, punc_nondmin.BER_it_199,'-diamond', ...
    'Color',[0.4940 0.1840 0.5560],'MarkerFaceColor',[0.4940 0.1840 0.5560], 'LineWidth', 1.5); % �ĤG�ռƾ�
hold on;

xlabel('SNR');
ylabel('BER');
title('Performance')
legend('CBP Payload96','Origin structure - punc dmin position','Origin structure - punc nondmin position');
grid on;  % �}�Ү�u

