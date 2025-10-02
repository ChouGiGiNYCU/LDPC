clear; clc;

% ====== 讀 H ======
% filename = "C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt";
filename = "PCM_P1008_E15BCH_Structure.txt";
puncture_idx = [1009:(1009+14)]
% filename = "C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt";
H  = readHFromFileByLine(filename);      % m x n, 0/1
[m, n] = size(H);
rate = (504+7)/1008;
% rate = (n - m) / n;
% 
% ====== 通道參數 (AWGN, BPSK, unit energy) ======
SNR = 1;
EbN0 = 10^(SNR/10);
sigma2 = 1 / (2 * rate * EbN0);          % 噪聲變異數 N0/2 → σ_n^2
m_ch   = 2 / sigma2;                     % LLR mean for BPSK over AWGN
sigma_ch = sqrt(2 * m_ch);               % 對稱高斯 LLR：Var=2m → σ=√(2m)

% ====== 由 H 算 edge-perspective λ_d, ρ_d ======
var_deg = sum(H, 1);             % 每個 VN 的度
chk_deg = sum(H, 2);             % 每個 CN 的度

max_dv = max(var_deg); 
max_dc = max(chk_deg);

E = sum(var_deg);                % 總邊數，= sum(chk_deg)

lambda_e = zeros(1, max_dv);
rho_e    = zeros(1, max_dc);

for d = 1:max_dv
    lambda_e(d) = d * sum(var_deg == d) / E;
end
for d = 1:max_dc
    rho_e(d) = d * sum(chk_deg == d) / E;
end

% ====== J 與 InvJ：以 sigma (標準差) 為自變數 ======
% 你原先的近似常數我沿用，但請注意這些擬合本來就是針對 σ 的。
J  = @(sigma) 1 - exp(-0.4527 * sigma.^0.86 + 0.0218);
Jinv = @(I) ((-1/0.4527) * log(1 - I) - 0.0218).^(1/0.86);

% ====== 掃描 I_A 並計算 VND / CND ======
I_A = 0:0.005:1;                 % 解析度可自己調
I_EV = zeros(size(I_A));         % VND: I_E vs I_A
I_EC = zeros(size(I_A));         % CND: I_E vs I_A

for i = 1:length(I_A)
    % ---- VND ----
    sigma_A = Jinv(I_A(i));      % 此處的 I_A 以 σ-domain 轉換
    EV_sum = 0;
    for d = 1:max_dv
        if lambda_e(d) == 0, continue; end
        sigma_out = sqrt( sigma_ch^2 + (d-1) * sigma_A^2 );
        EV_sum = EV_sum + lambda_e(d) * J(sigma_out);
    end
    I_EV(i) = EV_sum;

    % ---- CND ----
    sigma_Ac = Jinv(1 - I_A(i)); % 注意 1 - I_A
    EC_sum = 0;
    for d = 1:max_dc
        if rho_e(d) == 0, continue; end
        sigma_out = sqrt( (d-1) ) * sigma_Ac;
        EC_sum = EC_sum + rho_e(d) * J(sigma_out);
    end
    I_EC(i) = 1 - EC_sum;
end

% ====== 繪圖 ======
figure; hold on; grid on; box on;
plot(I_A, I_EV, 'b-', 'LineWidth', 2);             % VND curve
plot(I_EC, I_A, 'r-', 'LineWidth', 2);             % CND curve (inverted axes)
xlabel('I_A (Input Mutual Information)');
ylabel('I_E (Output Mutual Information)');
title(sprintf('EXIT Chart (Rate=%.2f, Eb/N0=%.1f dB)', rate, SNR));
legend('VND', 'CND (inverted)', 'Location', 'southeast');
axis([0 1 0 1]);

disp('判讀：若藍線始終在紅線(倒軸)之上且形成開口通道(tunnel)，BP 迭代可望收斂。');
