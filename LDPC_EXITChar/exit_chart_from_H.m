function out = exit_chart_from_H(H, EbN0_dB, punctureMask)
% exit_chart_from_H  由 H 與 SNR (Eb/N0, dB) 畫出 LDPC 的 EXIT 圖
% 用法：
%   exit_chart_from_H(H, EbN0_dB)
%   exit_chart_from_H(H, EbN0_dB, punctureMask)   % punctureMask: [n x 1] logical，1=穿孔
%
% 假設：BPSK over AWGN；LLR 以一致性高斯近似 (GA)；CND 曲線採鏡射繪圖。
% 輸出 out：IA_grid, IE_VND, IE_CND_mirror, lambda, rho, R_est, I_ch_eff

    if nargin < 3, punctureMask = []; end
    H = sparse(double(H~=0));

    % ---- 1) 從 H 取邊視角度數分佈 λ、ρ（irregular 時必須） ----
    [lambda, rho, R_est] = edge_degree_distributions_from_H(H);

    % ---- 2) 通道互資訊（BPSK-AWGN）----
    EbN0 = 10.^(EbN0_dB/10);
    sigma_ch = sqrt(4*EbN0);      % LLR 的標準差（coded bit Eb/N0）
    I_ch = J_fun(sigma_ch);

    % 有穿孔：把被穿孔位視為 I_ch=0（簡潔平均；要更精細可改分組加權）
    if ~isempty(punctureMask)
        n = size(H,2);
        if numel(punctureMask) ~= n
            error('punctureMask 長度應為 %d（變數節點數）', n);
        end
        p = mean(logical(punctureMask(:)));
        I_ch_eff = (1-p)*I_ch;   % 平均化處理
    else
        I_ch_eff = I_ch;
    end

    % ---- 3) 計算 VND / CND 兩條曲線（irregular 混合）----
    IA = linspace(0, 0.999, 1001);
    sA   = Jinv_fun(IA);
    sch  = Jinv_fun(I_ch_eff);
    s1mA = Jinv_fun(1 - IA);

    % VND：IE = sum_d lambda_d * J( sqrt((d-1)*sA^2 + sch^2) )
    IE_VND = zeros(size(IA));
    for d = find(lambda>0)
        IE_VND = IE_VND + lambda(d) * J_fun( sqrt( (d-1)*(sA.^2) + sch.^2 ) );
    end

    % CND：IE = 1 - sum_d rho_d * J( sqrt((d-1)*s1mA^2) )
    IE_CND = zeros(size(IA));
    for d = find(rho>0)
        IE_CND = IE_CND + rho(d) * J_fun( sqrt( (d-1)*(s1mA.^2) ) );
    end
    IE_CND = 1 - IE_CND;

    % 鏡射後的 CND（與 VND 同圖比較）
    x_cnd = IE_CND;      % x = I_Ec
    y_cnd = IA;          % y = I_Ac

    % ---- 4) 畫圖（比照你貼圖的風格）----
    figure('Color','w'); hold on; grid on; box on;
    plot(IA,  IE_VND, 'Color',[.6 .6 .6], 'LineWidth', 2);   % variable nodes（淺）
    plot(x_cnd, y_cnd, 'Color',[.3 .3 .3], 'LineWidth', 2);  % check nodes（深）
    xlim([0 1]); ylim([0 1]); axis square
    xlabel('I_{Av},\; I_{Ec}');
    ylabel('I_{Ev},\; I_{Ac}');
    title(sprintf('EXIT chart  (BPSK-AWGN)   Eb/N0 = %.2f dB,   R_{est} \\approx %.3f', ...
          EbN0_dB, R_est));
    text(0.58,0.78,'variable nodes','Color',[.4 .4 .4]);
    text(0.10,0.30,'check nodes','Color',[.2 .2 .2]);

    % ---- 5) 輸出 ----
    out = struct('IA_grid',IA, 'IE_VND',IE_VND, ...
                 'IE_CND_mirror',[x_cnd; y_cnd], ...
                 'lambda',lambda, 'rho',rho, ...
                 'R_est',R_est, 'I_ch_eff',I_ch_eff);
end

% ========= 附屬函式 =========
function [lambda, rho, R_est] = edge_degree_distributions_from_H(H)
    H = sparse(double(H~=0));
    [m,n] = size(H);
    dv = full(sum(H,1));   % 每個變數節點度數
    dc = full(sum(H,2));   % 每個檢查節點度數
    E  = sum(dv);          % 邊總數
    if E==0, error('H 似乎是零矩陣。'); end

    dv_max = max(dv);  dc_max = max(dc);
    lambda = zeros(1, max(1,dv_max));
    rho    = zeros(1, max(1,dc_max));

    % 邊視角分佈：lambda_d = (d * #VNs of degree d)/E
    for d = 1:dv_max
        lambda(d) = (d * sum(dv==d)) / E;
    end
    for d = 1:dc_max
        rho(d) = (d * sum(dc==d)) / E;
    end

    % 粗估碼率（未扣秩虧）
    R_est = max(0, 1 - m/n);
end

function y = J_fun(sigma)
% 互資訊 I = J(sigma)；兩段式擬合，足夠畫 EXIT
    y = zeros(size(sigma));
    s = sigma;

    idx1 = (s > 0) & (s <= 1.6363);
    idx2 = (s > 1.6363);

    if any(idx1)
        a1 = 1.09542; b1 = 0.214217; c1 = 2.33737;
        y(idx1) = 1 - exp(-a1*s(idx1).^2 - b1*s(idx1) - c1);
    end
    if any(idx2)
        y(idx2) = 1 - exp(-0.5 * s(idx2).^2);
    end
    y = min(max(y,0),1);
end

function s = Jinv_fun(I)
% sigma = J^{-1}(I)；與上面 J_fun 相容的近似
    I = min(max(I,0),1);
    s = zeros(size(I));

    idx0 = (I > 0) & (I < 0.3646);
    idx1 = (I >= 0.3646) & (I < 1);

    if any(idx0)
        a = 1.09542; b = 0.214217; c = 2.33737;
        s(idx0) = ( -b + sqrt(b^2 - 4*a*(c + log(1 - I(idx0)))) ) ./ (2*a);
        s(idx0) = max(s(idx0), 0);
    end
    if any(idx1)
        s(idx1) = sqrt( -2*log( max(1 - I(idx1), eps) ) );
    end
    s(I==0) = 0;
    s(I==1) = 1e6;
end
