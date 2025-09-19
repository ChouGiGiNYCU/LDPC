%% LDPC BP/Min-Sum 實測時間複雜度 vs 邊數 |E|
%  產生 (dv,dc)-regular 隨機 Tanner 圖，量測 I 次迭代的耗時
%  並畫出 log-log 圖與 O(|E|) 參考線，另存圖檔。

rng(1);

% ==== 參數（可自行調整）====
dv = 3;             % 變數節點度數
dc = 6;             % 檢查節點度數
I  = 3;             % 每個案例執行的迭代數
trials = 3;         % 重複測量次數，取平均
E_list = round(logspace(3,5,8));  % 1e3 ~ 1e5 邊，8 個點

times = zeros(size(E_list));
E_eff = zeros(size(E_list));

for k = 1:numel(E_list)
    % --- 產生一張 (dv,dc)-regular 二分圖（configuration model）---
    [edgeVar, edgeCheck, n, m, E] = make_regular_bipartite(dv, dc, E_list(k));
    % 預先建立檢查節點的 incident-edge 索引（cell），方便 Min-Sum 更新
    checkEdges = accumarray(edgeCheck, (1:E)', [m 1], @(x){x});

    % --- 量測：同一張圖跑 trials 次，取平均 ---
    t = zeros(trials,1);
    for t_i = 1:trials
        t(t_i) = run_bp_min_sum_once(edgeVar, checkEdges, n, E, I);
    end
    times(k) = mean(t);
    E_eff(k) = E;

    fprintf('[%2d/%2d] |E|=%7d, time=%.4f s\n', k, numel(E_list), E, times(k));
end

% --- 擬合 log–log 斜率（理想接近 1）---
p = polyfit(log10(E_eff), log10(times), 1);
slope = p(1);

% --- 作圖 ---
figure;
loglog(E_eff, times, 'o-', 'LineWidth', 1.6, 'MarkerSize', 6); hold on;
ref = (times(1)/E_eff(1)) * E_eff;   % O(E) 參考線（同尺度）
loglog(E_eff, ref, '--', 'LineWidth', 1.2);
grid on; box on;
xlabel('|E| (number of edges)');
ylabel(sprintf('Runtime for %d iterations (s)', I));
title(sprintf('LDPC BP (Min-Sum) Runtime vs |E|, dv=%d, dc=%d, slope=%.3f', dv, dc, slope));
legend('Measured','O(|E|) ref','Location','northwest');

% 存圖
exportgraphics(gcf, 'ldpc_bp_time_vs_edges.png', 'Resolution', 200);
disp('圖已輸出：ldpc_bp_time_vs_edges.png');

%% ================== 子函式 ==================

function t = run_bp_min_sum_once(edgeVar, checkEdges, n, E, I)
    % 單次案例：在固定圖上跑 I 次迭代（Min-Sum 更新），回傳總耗時
    Lch  = randn(n,1);   % 通道 LLR（隨機即可，僅測複雜度）
    c2v  = zeros(E,1);   % 檢查->變數 訊息
    v2c  = zeros(E,1);   % 變數->檢查 訊息

    tic;
    for it = 1:I
        % ---- 變數節點更新：v2c = Lch + sum_{c'≠c} c2v ----
        sumC2VperVar = accumarray(edgeVar, c2v, [n 1], @sum, 0);
        v2c = Lch(edgeVar) + (sumC2VperVar(edgeVar) - c2v);

        % ---- 檢查節點更新（Min-Sum；與 SPA 同為每節點 O(deg)）----
        for c = 1:numel(checkEdges)
            edges = checkEdges{c};   % 此檢查節點相鄰的邊索引
            msgs  = v2c(edges);

            % 處理符號：0 視為 +1，避免乘積變 0
            sgn = sign(msgs); sgn(sgn==0) = 1;
            signProd = prod(sgn);

            % 兩小值 trick：對每條邊輸出其餘邊的最小絕對值
            absMsgs = abs(msgs);
            [min1, idxMin] = min(absMsgs);
            abs2 = absMsgs; abs2(idxMin) = inf;
            min2 = min(abs2);

            % 產生輸出：對最小者用 min2，其餘用 min1；符號為除自身外的乘積
            isMin = false(size(msgs)); isMin(idxMin) = true;
            mag   = min1 * ones(size(msgs));
            mag(isMin) = min2;
            out = (signProd .* sgn) .* mag;

            c2v(edges) = out;
        end
    end
    t = toc;
end

function [edgeVar, edgeCheck, n, m, E] = make_regular_bipartite(dv, dc, E_target)
    % 令 E = n*dv = m*dc；選擇最小可行的 n 使 E 不小於目標
    L = dc / gcd(dv, dc);
    n = L * ceil(E_target / (dv * L));  % n 為 L 的倍數
    E = n * dv;
    m = E / dc;

    % configuration model：把 E 個 var-stubs 與 E 個 check-stubs 隨機配對
    var_stubs   = repelem((1:n)', dv, 1);   % E×1
    check_stubs = repelem((1:m)', dc, 1);   % E×1
    perm = randperm(E);
    edgeVar   = var_stubs;
    edgeCheck = check_stubs(perm);
end
