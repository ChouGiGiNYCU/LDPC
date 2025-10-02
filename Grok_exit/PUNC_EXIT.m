function PUNC_EXIT(H,puncture_idx,SNR_dB,rate_tx)
    [m, n] = size(H);
    
    % ====== Puncture mask（超重要）======
    is_punc = false(1, n);
    puncture_idx = puncture_idx(puncture_idx>=1 & puncture_idx<=n); % 防呆
    is_punc(puncture_idx) = true;
    n_punc = sum(is_punc);
    n_tx   = n - n_punc;                     % 實際傳輸長度
    
    % ====== 有效碼率（用 K≈n-m 近似；若你有真 K，請直接代入）======
    % K0       = Payload_inf0 + Extra_info;                        % 近似資訊長度
    % rate_tx  = K0 / n_tx;                    % 實際傳輸時使用的碼率
    
    % ====== 通道參數 (AWGN, BPSK) ======
    % SNR_dB = 1;                               % 你原本的 SNR=1 dB
    EbN0   = 10^(SNR_dB/10);
    sigma2 = 1 / (2 * rate_tx * EbN0);        % N0/2 → 噪聲變異數
    m_ch   = 2 / sigma2;                      % 對稱高斯 LLR: E[LLR]=2/sigma2
    sigma_ch = sqrt(2 * m_ch);                % Var(LLR)=2m → σ=√(2m)
    
    % ====== 由 H 算每個節點/檢查的度與總邊數 ======
    var_deg = sum(H, 1);             % 每個 VN 的度
    chk_deg = sum(H, 2);             % 每個 CN 的度
    E       = sum(var_deg);          % 總邊數（edge-perspective 權重用）
    
    % ====== J 與 InvJ（以 σ 為自變數的近似）======
    J    = @(sigma) 1 - exp(-0.4527 * sigma.^0.86 + 0.0218);
    Jinv = @(I) ((-1/0.4527) * log(max(1e-12, 1 - I)) - 0.0218).^(1/0.86);
    
    % ====== 掃描 I_A 並計算 VND / CND（puncture-aware）======
    I_A  = 0:0.005:1;
    I_EV = zeros(size(I_A));         % VND: I_E vs I_A
    I_EC = zeros(size(I_A));         % CND: I_E vs I_A
    
    % 針對每個 VN 準備其通道 σ（puncture 則為 0）
    sigma_ch_vn = zeros(1, n);
    sigma_ch_vn(~is_punc) = sigma_ch;   % 未被 puncture 的 VN 有通道貢獻
    sigma_ch_vn( is_punc) = 0;          % 被 puncture 的 VN 通道項 = 0
    
    % ---- CND 需要度數分佈的 edge-perspective 權重 rho_e(d) ----
    max_dc = max(chk_deg);
    rho_e  = zeros(1, max_dc);
    for d = 1:max_dc
        rho_e(d) = d * sum(chk_deg == d) / E;
    end
    
    for i = 1:length(I_A)
        % ---- VND（逐 VN、邊數加權）----
        sigma_A = Jinv(I_A(i));
        % 每個 VN 的輸出 MI
        % I_E,vn = J( sqrt( (d_v-1)*sigma_A^2 + sigma_ch_vn^2 ) )
        sigma_out_vn = sqrt( max(0, (var_deg - 1)) .* (sigma_A.^2) + (sigma_ch_vn.^2) );
        I_E_vn       = J(sigma_out_vn);
    
        % 邊數加權：sum_j (d_j/E) * I_E_vn(j)
        I_EV(i) = sum( (var_deg./E) .* I_E_vn );
    
        % ---- CND（與 puncture 無關，照常）----
        sigma_Ac = Jinv(1 - I_A(i));           % 注意 1 - I_A
        EC_sum = 0;
        for d = 1:max_dc
            if rho_e(d) == 0, continue; end
            sigma_out = sqrt(max(0, d-1)) * sigma_Ac;
            EC_sum = EC_sum + rho_e(d) * J(sigma_out);
        end
        I_EC(i) = 1 - EC_sum;
    end
    
    % ====== 繪圖 ======
    hold on; grid on; box on;
    plot(I_A, I_EV, 'b-', 'LineWidth', 2);             % VND curve (puncture-aware)
    plot(real(I_EC), I_A, 'r-', 'LineWidth', 2);             % CND curve (inverted axes)
    xlabel('I_A (Input Mutual Information)');
    ylabel('I_E (Output Mutual Information)');
    title(sprintf('Combine PCM - EXIT Chart (R_tx=%.3f, SNR=%.1f dB, n=%d, n_{tx}=%d)', ...
          rate_tx, SNR_dB, n, n_tx));
    legend('Combine - VND (puncture-aware)', 'Combine - CND (inverted)', 'Location', 'southeast');
    axis([0 1 0 1]);
    
    % disp('判讀：若藍線始終在紅線(倒軸)之上且形成開口通道(tunnel)，BP 迭代可望收斂。');
end