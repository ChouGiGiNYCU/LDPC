clear; clc;

%% ====== 讀 H ======
filename = "PCM_P1008_E15BCH_Structure.txt";
% filename = "C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt";
H  = readHFromFileByLine(filename);      % m x n, 0/1
[m, n] = size(H);

%% ====== 指定要 puncture 的欄位（以 1-based column index）======
% 範例：puncture 第 3, 7, 10 欄；請自行修改
punc_idx = [1009:1023];
punc_idx =[];
isPunc = false(1,n);
isPunc(punc_idx) = true;
R_eff = (504+7)/1008;           % 有效碼率
% R_eff = (504)/1008;          
%% ====== 多組 Eb/N0 (dB) ======
EbN0_dB_list = [-0.5, 0, 0.5, 1, 1.5, 2];

%% ====== 基本度數統計與 edge-perspective 分佈 ======
var_deg = sum(H, 1);        % 每個 VN 的度
chk_deg = sum(H, 2);        % 每個 CN 的度
E = sum(var_deg);           % 總邊數 = sum(chk_deg)

max_dv = max(var_deg);
max_dc = max(chk_deg);

% edge-perspective lambda 分離為未 puncture 與 puncture 兩組
lambdaU = zeros(1, max_dv);   % 未 puncture 之邊的權重
lambdaP = zeros(1, max_dv);   % puncture 之邊的權重

for d = 1:max_dv
    cnt_var_d_unp = sum((var_deg == d) & ~isPunc);
    cnt_var_d_p   = sum((var_deg == d) &  isPunc);
    lambdaU(d) = d * cnt_var_d_unp / E;
    lambdaP(d) = d * cnt_var_d_p   / E;
end
% 檢查：sum(lambdaU)+sum(lambdaP) 應為 1（數值誤差容許 1e-12）
% disp(sum(lambdaU)+sum(lambdaP))

% edge-perspective rho
rho = zeros(1, max_dc);
for d = 1:max_dc
    rho(d) = d * sum(chk_deg == d) / E;
end
% 檢查：sum(rho) 應為 1
% disp(sum(rho))

%% ====== J / InvJ：以 σ (標準差) 為自變數 ======
J  = @(sigma) 1 - exp(-0.4527 * sigma.^0.86 + 0.0218);
Jinv = @(I) ((-1/0.4527) * log(1 - I) - 0.0218).^(1/0.86);

%% ====== I_A 範圍 ======
I_A = 0:0.005:1;
I_EC = zeros(size(I_A));  % CND 曲線（與 Eb/N0 無關，畫一次即可）

% 先算一次 CND（固定）
for i = 1:length(I_A)
    sigma_Ac = Jinv(1 - I_A(i));  % 1 - I_A
    EC_sum = 0;
    for d = 1:max_dc
        if rho(d) == 0, continue; end
        sigma_out = sqrt(d-1) * sigma_Ac;
        EC_sum = EC_sum + rho(d) * J(sigma_out);
    end
    I_EC(i) = 1 - EC_sum;
end

%% ====== 畫圖：多條 VND 對應多個 Eb/N0 ======
figure; hold on; grid on; box on;

% 先畫 CND (倒軸)
plot(real(I_EC), I_A, 'k-', 'LineWidth', 2, 'DisplayName', 'CND (inverted)');

% 逐個 Eb/N0 畫 VND
n_punc = nnz(isPunc);
n_tx   = n - n_punc;              % 實際傳送長度
if n_tx <= 0
    error('所有欄位都被 puncture 了，沒有可傳送的符號。');
end


for ebi = 1:numel(EbN0_dB_list)
    EbN0_dB = EbN0_dB_list(ebi);
    EbN0 = 10^(EbN0_dB/10);

    % 通道雜訊（以有效碼率計）
    sigma2_n = 1 / (2 * R_eff * EbN0);  % N0/2 → σ_n^2
    m_ch     = 2 / sigma2_n;            % LLR mean
    sigma_ch = sqrt(2 * m_ch);          % σ = √(2m)

    I_EV = zeros(size(I_A));            % 當前 Eb/N0 的 VND 曲線

    for i = 1:length(I_A)
        sigma_A = Jinv(I_A(i));

        EV_sum = 0;
        for d = 1:max_dv
            if (lambdaU(d) == 0) && (lambdaP(d) == 0), continue; end

            % 未 puncture 的邊：含通道項
            if lambdaU(d) ~= 0
                sigma_out_U = sqrt(sigma_ch^2 + (d-1) * sigma_A^2);
                EV_sum = EV_sum + lambdaU(d) * J(sigma_out_U);
            end
            % puncture 的邊：通道項為 0
            if lambdaP(d) ~= 0
                sigma_out_P = sqrt(       0   + (d-1) * sigma_A^2);
                EV_sum = EV_sum + lambdaP(d) * J(sigma_out_P);
            end
        end
        I_EV(i) = EV_sum;
    end

    plot(I_A, real(I_EV), 'LineWidth', 2, ...
        'DisplayName', sprintf('VND @ Eb/N0 = %.1f dB (R_{eff}=%.3f)', EbN0_dB, R_eff));
end

xlabel('I_A (Input Mutual Information)');
ylabel('I_E (Output Mutual Information)');
title(sprintf('EXIT (puncture %d cols) — R_{eff}=%.3f (基於傳送長度)', n_punc, R_eff));
legend('Location', 'southeast');
axis([0 1 0 1]);

disp('判讀：藍/彩色 VND 與黑色 CND (倒軸) 之間若有開口通道且 VND 在上方，BP 迭代可望收斂。');
