clear all;
clc;


% 假設 Excel 檔名為 data.xlsx，且第一列為欄位名稱：SNR, Throughput
Origin_filename   = 'C:\Users\USER\Desktop\LDPC\Throughput\Origin\Payload_1008_Extra(H96)\\throughput1.csv';
% Combine_filename  = 'Combine\\PayLoad1008_Extra(BCH15)_originH\\throughtput.csv';  
Enhanced_filename = 'C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(H96)_EnhancedH\\Troughput.csv';    % Excel 檔案名稱

Origin_table  = readtable(Origin_filename);
% Combine_table = readtable(Combine_filename);
Enhanced_table = readtable(Enhanced_filename);

plot(Origin_table.SNR,Origin_table.Throughput,"-*","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;
% plot(Combine_table.SNR,Combine_table.Throughput,"-square","Color",[0.4940 0.1840 0.5560],"LineWidth",2);
% hold on;
plot(Enhanced_table.SNR,Enhanced_table.Throught,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
legend("Only Payload","Enhanced");
xlabel("SNR");
ylabel("Throughtput")
grid on;


%%
clear all;
clc;


% 假設 Excel 檔名為 data.xlsx，且第一列為欄位名稱：SNR, Throughput
Origin_filename   = "C:\Users\USER\Desktop\LDPC\Throughput\Origin\Payload_1008_Extra(H96)\throughput1.csv";
% Combine_filename  = 'Combine\\PayLoad1008_Extra(BCH15)_originH\\throughtput.csv';  
Enhanced_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(H96)_EnhancedH_v1\Troughput.csv";    % Excel 檔案名稱

Origin_table  = readtable(Origin_filename);
% Combine_table = readtable(Combine_filename);
Enhanced_table = readtable(Enhanced_filename);

plot(Origin_table.SNR,Origin_table.Throughput,"-*","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;
% plot(Combine_table.SNR,Combine_table.Throughput,"-square","Color",[0.4940 0.1840 0.5560],"LineWidth",2);
% hold on;
plot(Enhanced_table.SNR,Enhanced_table.Throught,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
legend("Only Payload","Enhance - Hybrid method");
xlabel("SNR");
ylabel("Throughtput")
grid on;