function [lambda, rho, R_est] = edge_degree_distributions_from_H(H)
% 由 H 求邊視角度數分佈 λ(x), ρ(x)（用「邊分佈」索引）
    H = sparse(double(H~=0));
    [m,n] = size(H);
    dv = full(sum(H,1));      % variable node degrees
    dc = full(sum(H,2));      % check node degrees
    E  = sum(dv);
    if E==0, error('H 似乎是零矩陣。'); end

    dv_max = max(dv);  dc_max = max(dc);
    lambda = zeros(1, max(1,dv_max));
    rho    = zeros(1, max(1,dc_max));

    for d = 1:dv_max
        lambda(d) = (d * sum(dv==d)) / E;
    end
    for d = 1:dc_max
        rho(d) = (d * sum(dc==d)) / E;
    end

    R_est = max(0, 1 - m/n);  % 粗估碼率（未扣秩虧）
end
