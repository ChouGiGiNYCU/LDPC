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


%%
clear all;
clc;


% 假設 Excel 檔名為 data.xlsx，且第一列為欄位名稱：SNR, Throughput
Origin_filename   = "C:\Users\USER\Desktop\LDPC\Throughput\Origin\Payload_1008_Extra(BCH63)\throughput.csv";
% Combine_filename  = 'Combine\\PayLoad1008_Extra(BCH15)_originH\\throughtput.csv';  
Enhanced63x30_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(BCH63x30)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced63x30_Origin_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(BCH63x30)_EnhancedH\Troughput.csv";
Enhanced63x36_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(BCH63x36)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced63x36_Origin_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(BCH63x36)_EnhancedH\Troughput.csv";
Enhanced32x16_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(H32x16)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced32x16_Origin_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(H32x16)_EnhancedH\Troughput.csv"
Enhanced63x45_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(BCH63x45)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced63x45_Origin_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(BCH63x45)_EnhancedH\Troughput.csv"

Origin_table  = readtable(Origin_filename);
% Combine_table = readtable(Combine_filename);
Enhanced63x30_table = readtable(Enhanced63x30_filename);
Enhanced63x30_table_Origin = readtable(Enhanced63x30_Origin_filename);
Enhanced63x36_table = readtable(Enhanced63x36_filename);
Enhanced63x36_table_Origin = readtable(Enhanced63x36_Origin_filename);
Enhanced63x45_table = readtable(Enhanced63x45_filename);
Enhanced63x45_table_Origin = readtable(Enhanced63x45_Origin_filename);
Enhanced32x16_table = readtable(Enhanced32x16_filename);
Enhanced32x16_table_Origin = readtable(Enhanced32x16_Origin_filename);

plot(Origin_table.SNR,Origin_table.Throughput,"-*","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;

plot(Enhanced63x30_table.SNR,Enhanced63x30_table.Throught,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
hold on;
plot(Enhanced63x30_table_Origin.SNR,Enhanced63x30_table_Origin.Throught,"-->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
hold on;

plot(Enhanced63x36_table.SNR,Enhanced63x36_table.Throught,"-<","Color",[1 0 1],"LineWidth",2);
hold on;
plot(Enhanced63x36_table_Origin.SNR,Enhanced63x36_table_Origin.Throught,"--<","Color",[1 0 1],"LineWidth",2);
hold on;

plot(Enhanced63x45_table.SNR,Enhanced63x45_table.Throught,"->","Color",[0 1 0],"LineWidth",2);
hold on;
plot(Enhanced63x45_table_Origin.SNR,Enhanced63x45_table_Origin.Throught,"-->","Color",[0 1 0],"LineWidth",2);
hold on;

plot(Enhanced32x16_table.SNR,Enhanced32x16_table.Throught,"-^","Color",[0 0 1],"LineWidth",2);
hold on;
plot(Enhanced32x16_table_Origin.SNR,Enhanced32x16_table_Origin.Throught,"--^","Color",[0 0 1],"LineWidth",2);
hold on;

legend("Only Payload","Enhance(BCH63X30) - Hybrid method","Enhance(BCH63X30) - Origin method","Enhance(BCH63X36) - Hybrid method","Enhance(BCH63X36) - Origin method","Enhance(BCH63X45) - Hybrid method","Enhance(BCH63X45) - Origin method","Enhance(H32X16) - Hybrid method","Enhance(H32X16) - Origin method");
title("Payload PEG1008 - Extra 63")
% legend("Only Payload","Enhance(BCH63X30) - Hybrid method","Enhance(BCH63X36) - Hybrid method","Enhance(BCH63X45) - Hybrid method","Enhance(H32X16) - Hybrid method");
xlabel("SNR");
ylabel("Throughtput")
grid on;


%%
%%
clear all;
clc;


% 假設 Excel 檔名為 data.xlsx，且第一列為欄位名稱：SNR, Throughput
Origin_filename   = "C:\Users\USER\Desktop\LDPC\Throughput\Origin\Payload_2640_Extra(H96)\throughput.csv";
% Combine_filename  = 'Combine\\PayLoad1008_Extra(BCH15)_originH\\throughtput.csv';  
Enhanced_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad2640_Extra(H96)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱

Origin_table  = readtable(Origin_filename);
% Combine_table = readtable(Combine_filename);
Enhanced_table = readtable(Enhanced_filename);

plot(Origin_table.SNR,Origin_table.Throughput,"-*","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;
% plot(Combine_table.SNR,Combine_table.Throughput,"-square","Color",[0.4940 0.1840 0.5560],"LineWidth",2);
% hold on;
plot(Enhanced_table.SNR,Enhanced_table.Throught,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
legend("Only Payload","Enhance(H48X96) - Hybrid method");
xlabel("SNR");
ylabel("Throughtput");
title("Payload-2640 Extra-96")
grid on;
%%
clear all;
clc;


% 假設 Excel 檔名為 data.xlsx，且第一列為欄位名稱：SNR, Throughput
Origin_filename   = "C:\Users\USER\Desktop\LDPC\Throughput\Origin\Payload_816_Extra(H96)\throughput.csv";
% Combine_filename  = 'Combine\\PayLoad1008_Extra(BCH15)_originH\\throughtput.csv';  
Enhanced_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad816_Extra(H96)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱

Origin_table  = readtable(Origin_filename);
% Combine_table = readtable(Combine_filename);
Enhanced_table = readtable(Enhanced_filename);

plot(Origin_table.SNR,Origin_table.Throughput,"-*","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;
% plot(Combine_table.SNR,Combine_table.Throughput,"-square","Color",[0.4940 0.1840 0.5560],"LineWidth",2);
% hold on;
plot(Enhanced_table.SNR,Enhanced_table.Throught,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
legend("Only Payload","Enhance(H48X96) - Hybrid method");
xlabel("SNR");
ylabel("Throughtput");
title("Payload-816 Extra-96")
grid on;

%%
%%
clear all;
clc;


% 假設 Excel 檔名為 data.xlsx，且第一列為欄位名稱：SNR, Throughput
Origin_filename   = "C:\Users\USER\Desktop\LDPC\Throughput\Origin\Payload_816_Extra(BCH63)\throughput.csv";
% Combine_filename  = 'Combine\\PayLoad1008_Extra(BCH15)_originH\\throughtput.csv';  
Enhanced63x30_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad816_Extra(BCH63x30)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced63x36_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad816_Extra(BCH63x36)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
% Enhanced32x16_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(H32x16)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced63x45_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad816_Extra(BCH63x45)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱

Origin_table  = readtable(Origin_filename);
% Combine_table = readtable(Combine_filename);
Enhanced63x30_table = readtable(Enhanced63x30_filename);
Enhanced63x36_table = readtable(Enhanced63x36_filename);
Enhanced63x45_table = readtable(Enhanced63x45_filename);
% Enhanced32x16_table = readtable(Enhanced32x16_filename);
% 
plot(Origin_table.SNR,Origin_table.Throughput,"-*","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;

plot(Enhanced63x30_table.SNR,Enhanced63x30_table.Throught,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
hold on;

plot(Enhanced63x36_table.SNR,Enhanced63x36_table.Throught,"-<","Color",[1 0 1],"LineWidth",2);
hold on;

plot(Enhanced63x45_table.SNR,Enhanced63x45_table.Throught,"->","Color",[0 1 0],"LineWidth",2);
hold on;

% plot(Enhanced32x16_table.SNR,Enhanced32x16_table.Throught,"-^","Color",[0 0 1],"LineWidth",2);
% hold on;


% legend("Only Payload","Enhance(BCH63X30) - Hybrid method","Enhance(BCH63X36) - Hybrid method","Enhance(BCH63X45) - Hybrid method","Enhance(H32X16) - Hybrid method");
legend("Only Payload","Enhance(BCH63X30) - Hybrid method","Enhance(BCH63X36) - Hybrid method","Enhance(BCH63X45) - Hybrid method");
title("Payload816")
xlabel("SNR");
ylabel("Throughtput")
grid on;

%%
clear all;
clc;


% 假設 Excel 檔名為 data.xlsx，且第一列為欄位名稱：SNR, Throughput
Origin_filename   = "C:\Users\USER\Desktop\LDPC\Throughput\Origin\Payload_816_Extra(BCH63)\throughput.csv";
% Combine_filename  = 'Combine\\PayLoad1008_Extra(BCH15)_originH\\throughtput.csv';  
Enhanced63x30_RV63_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad816_Extra(BCH63x30)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced63x30_RV100_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad816_Extra(BCH63x30)_RV1(100)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced63x30_RV120_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad816_Extra(BCH63x30)_RV1(120)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced63x30_RV150_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad816_Extra(BCH63x30)_RV1(150)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱

Origin_table  = readtable(Origin_filename);
% Combine_table = readtable(Combine_filename);
Enhanced63x30_RV63_table = readtable(Enhanced63x30_RV63_filename);
Enhanced63x30_RV100_table = readtable(Enhanced63x30_RV100_filename);
Enhanced63x30_RV120_table = readtable(Enhanced63x30_RV120_filename);
Enhanced63x30_RV150_table = readtable(Enhanced63x30_RV150_filename);
% Enhanced32x16_table = readtable(Enhanced32x16_filename);
% 
plot(Origin_table.SNR,Origin_table.Throughput,"-*","Color",[0 0 0],"LineWidth",2);
hold on;

plot(Enhanced63x30_RV63_table.SNR,Enhanced63x30_RV63_table.Throught,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
hold on;

plot(Enhanced63x30_RV100_table.SNR,Enhanced63x30_RV100_table.Throught,"-<","Color",[0.4660 0.6740 0.1880],"LineWidth",2);
hold on;

plot(Enhanced63x30_RV120_table.SNR,Enhanced63x30_RV120_table.Throught,"-v","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;

plot(Enhanced63x30_RV150_table.SNR,Enhanced63x30_RV150_table.Throught,"->","Color",[0.4940 0.1840 0.5560],"LineWidth",2);
hold on;

% plot(Enhanced32x16_table.SNR,Enhanced32x16_table.Throught,"-^","Color",[0 0 1],"LineWidth",2);
% hold on;


legend("Only Payload","Enhance(BCH63X30) - RV0(63) - Hybrid method","Enhance(BCH63X30) - RV0(100) - Hybrid method","Enhance(BCH63X30) - RV0(120) - Hybrid method","Enhance(BCH63X30) - RV0(150) - Hybrid method");
title("Payload816 with different puncturing number in RV0")
xlabel("SNR");
ylabel("Throughtput")
grid on;


%%
clear all;
clc;


% 假設 Excel 檔名為 data.xlsx，且第一列為欄位名稱：SNR, Throughput
Origin_filename   = "C:\Users\USER\Desktop\LDPC\Throughput\Origin\Payload_5G1020_Extra(BCH63)\throughput.csv";
% Combine_filename  = 'Combine\\PayLoad1008_Extra(BCH15)_originH\\throughtput.csv';  
Enhanced_5G1020_BCH63x30_Hybrid_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad5GNR_Extra(BCH63x30)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced_5G1020_BCH63x30_filename ="C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad5GNR_Extra(BCH63x30)_EnhancedH_v1\Troughput.csv";
Enhanced_5G1020_BCH63x36_Hybrid_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad5GNR_Extra(BCH63x36)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced_5G1020_BCH63x36_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad5GNR_Extra(BCH63x36)_EnhancedH_v1\Troughput.csv";
Enhanced_5G1020_BCH63x45_Hybrid_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad5GNR_Extra(BCH63x45)_EnhancedH_v1_HybridMethod\Troughput.csv";    % Excel 檔案名稱
Enhanced_5G1020_BCH63x45_filename = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad5GNR_Extra(BCH63x45)_EnhancedH_v1\Troughput.csv";    % Excel 檔案名稱

Origin_table  = readtable(Origin_filename);
% Combine_table = readtable(Combine_filename);
Enhanced_5G1020_BCH63x30_Hybrid = readtable(Enhanced_5G1020_BCH63x30_Hybrid_filename);
Enhanced_5G1020_BCH63x30 = readtable(Enhanced_5G1020_BCH63x30_filename);
Enhanced_5G1020_BCH63x36_Hybrid = readtable(Enhanced_5G1020_BCH63x36_Hybrid_filename);
Enhanced_5G1020_BCH63x36 = readtable(Enhanced_5G1020_BCH63x36_filename);
Enhanced_5G1020_BCH63x45_Hybrid = readtable(Enhanced_5G1020_BCH63x45_Hybrid_filename);
Enhanced_5G1020_BCH63x45 = readtable(Enhanced_5G1020_BCH63x45_filename);
% Enhanced32x16_table = readtable(Enhanced32x16_filename);
% 
plot(Origin_table.SNR,Origin_table.Throughput,"-*","Color",[0 0 0],"LineWidth",2);
hold on;

plot(Enhanced_5G1020_BCH63x30_Hybrid.SNR,Enhanced_5G1020_BCH63x30_Hybrid.Throught,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
hold on;
plot(Enhanced_5G1020_BCH63x30.SNR,Enhanced_5G1020_BCH63x30.Throught,"-->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
hold on;

plot(Enhanced_5G1020_BCH63x36_Hybrid.SNR,Enhanced_5G1020_BCH63x36_Hybrid.Throught,"-<","Color",[0.4660 0.6740 0.1880],"LineWidth",2);
hold on;
plot(Enhanced_5G1020_BCH63x36.SNR,Enhanced_5G1020_BCH63x36.Throught,"--<","Color",[0.4660 0.6740 0.1880],"LineWidth",2);
hold on;

plot(Enhanced_5G1020_BCH63x45_Hybrid.SNR,Enhanced_5G1020_BCH63x45_Hybrid.Throught,"-v","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;
plot(Enhanced_5G1020_BCH63x45.SNR,Enhanced_5G1020_BCH63x45.Throught,"--v","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;



legend("Only Payload","Enhance(BCH63X30) - Hybrid method","Enhance(BCH63X30)","Enhance(BCH63X36) - Hybrid method","Enhance(BCH63X36)","Enhance(BCH63X45) - Hybrid method","Enhance(BCH63X45)");
title("Payload 5GNR-1020 ")
xlabel("SNR");
ylabel("Throughtput")
grid on;