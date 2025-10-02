function out = plotEXIT_from_H(H, EbN0_dB, punctureMask)
% plotEXIT_from_H  畫 LDPC 的 EXIT 圖（VND/CND），輸入 H 矩陣
% 用法：
%   plotEXIT_from_H(H, EbN0_dB)
%   plotEXIT_from_H(H, EbN0_dB, punctureMask)   % punctureMask: [n x 1] logical，1=穿孔
%
% 假設：BPSK over AWGN，LLR 符合一致性高斯近似 (GA)。
% 通道互資訊 I_ch = J(sigma_ch)，sigma_ch = sqrt(4*Eb/N0)（以 coded bit 的 Eb/N0 為準）
%
% out：IA_grid, IE_VND, IE_CND_mirror, lambda, rho, I_ch_eff, R_est

    if nargin < 3, punctureMask = []; end
    H = sparse(double(H~=0));

    [lambda, rho, R_est] = edge_degree_distributions_from_H(H);

    EbN0 = 10.^(EbN0_dB/10);
    sigma_ch = sqrt(4*EbN0);
    I_ch = J_fun(sigma_ch);

    if ~isempty(punctureMask)
        n = size(H,2);
        if numel(punctureMask) ~= n
            error('punctureMask 長度需等於變數節點數 n = size(H,2).');
        end
        p = mean( logical(punctureMask(:)) );
        I_ch_eff = (1-p)*I_ch;   % 穿孔位視為 I_ch=0 的平均化處理
    else
        I_ch_eff = I_ch;
    end

    IA = linspace(0, 0.999, 1001);
    Jinv_IA  = Jinv_fun(IA);
    Jinv_Ich = Jinv_fun(I_ch_eff);

    % VND
    IE_VND = zeros(size(IA));
    degs_v = find(lambda > 0);
    for d = degs_v
        IE_VND = IE_VND + lambda(d) * J_fun( sqrt( (d-1)*(Jinv_IA.^2) + (Jinv_Ich.^2) ) );
    end

    % CND
    Jinv_1mIA = Jinv_fun(1 - IA);
    IE_CND = zeros(size(IA));
    degs_c = find(rho > 0);
    for d = degs_c
        IE_CND = IE_CND + rho(d) * J_fun( sqrt( (d-1) * (Jinv_1mIA.^2) ) );
    end
    IE_CND = 1 - IE_CND;

    % 鏡射（拿來和 VND 對接）
    IE_CND_mirror_x = IE_CND;
    IE_CND_mirror_y = IA;

    figure; hold on; grid on; box on;
    plot(IA, IE_VND, 'LineWidth', 2);
    plot(IE_CND_mirror_x, IE_CND_mirror_y, 'LineWidth', 2);
    xlabel('I_A'); ylabel('I_E / I_A (mirrored)');
    title(sprintf('EXIT (BPSK-AWGN)  Eb/N0 = %.2f dB,  R_{est} \\approx %.3f', EbN0_dB, R_est));
    legend('VND: I_E(I_A)', 'CND: I_A(I_E) (mirrored)', 'Location','SouthEast');
    axis([0 1 0 1]);

    out = struct('IA_grid',IA, 'IE_VND',IE_VND, ...
                 'IE_CND_mirror',[IE_CND_mirror_x; IE_CND_mirror_y], ...
                 'lambda',lambda,'rho',rho,'I_ch_eff',I_ch_eff,'R_est',R_est);
end
