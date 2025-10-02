function enhanced_exit_suite(H,puncture_idx,extra_idx)
% 三件套主程式：PEXIT + 門檻搜尋 + GA 迭代統計
% ----------------------------------------------------------
% 你要提供/調整的輸入在這裡（最少三項）：
%   H_enh       : 最終(已合併)的 parity-check 矩陣 (m x n, 0/1)
%   is_punctured: 1 x n logical，true 表示該 VN 不傳送（通道訊息=0）
%   vn_group    : 1 x n int，群組標籤（1=payload, 2=extra, 3=merge1, 4=merge2, 5=merge3）
% ----------------------------------------------------------

%% ========= 1) 載入/建立 H_enh、puncture 與群組 =========
% 例：請替換成你的載入方式
% H_enh = readHFromFileByLine("your_final_enhanced_H.txt");  % 若有此函式
% 下面用隨機小例子示意（請換掉）
% rng(1);
% m = 120; n = 150; density = 0.05;
% H_enh = sprand(m,n,density)>0; H_enh = double(mod(H_enh,2));
% % 假設前 100 是 payload、接著 10 是 extra、剩下是 merge nodes
% vn_group = zeros(1,n);
% vn_group(1:100) = 1;           % payload
% vn_group(101:110) = 2;         % extra
% vn_group(vn_group==0) = 3;     % merge1 (只為了完成例子)
% % 假設 puncture：extra + 一部分 payload（請用你的設計替換）
% is_punctured = false(1,n);
% is_punctured(101:110) = true;              % extra 全 puncture（Enhanced 節點場景）
% is_punctured(20:29)   = true;              % 示意：puncture 一些 payload
% % 若是 Partial-Extra，請把直接傳送的 extra 設為 false（非 puncture）

% ====== 通道/掃描參數 ======
EbN0_dB_list = -1.0:0.25:3.0;  % 掃門檻
I_grid = 0:0.005:1;            % EXIT 曲線取樣點
tunnel_margin = 1e-3;          % 判斷 tunnel 開口的最小餘裕
max_iter_ga = 50;              % GA 迭代統計的最大回合

%% ========= 2) 邊觀點分佈（payload/extra/merge 切群） =========
[var_deg, chk_deg, E] = basic_degrees(H_enh);
[lambda_all, lambda_by_group, rho] = edge_perspective_distributions(H_enh, vn_group, var_deg, chk_deg, E);

% 群組清單（方便命名）
groups = struct('id',[1 2 3 4 5], 'name', ["payload","extra","merge1","merge2","merge3"]);

% 有效碼率（以「實際傳送長度」計）
n_tx = sum(~is_punctured);
if n_tx<=0, error('n_tx=0：全 puncture？'); end
R_eff = (n - rank_mod2(H_enh)) / n_tx;   % 粗略估計；如已知 r=m 可用 (n-m)/n_tx

%% ========= 3) 先算一次 CND（與 EbN0 無關） =========
[Jfun, Jinv] = make_J_funcs();
I_EC = cnd_curve(I_grid, rho, Jfun, Jinv);

%% ========= 4) 畫 EXIT：payload/extra/overall 多條 VND，+ CND =========
figure; hold on; grid on; box on;
plot(I_EC, I_grid, 'k-', 'LineWidth', 2, 'DisplayName','CND (inverted)');

thres_found = false;
for ebi = 1:numel(EbN0_dB_list)
    EbN0_dB = EbN0_dB_list(ebi);
    EbN0    = 10^(EbN0_dB/10);
    sigma2_n = 1/(2*R_eff*EbN0);
    m_ch     = 2/sigma2_n;
    sigma_ch = sqrt(2*m_ch);  % 通道 LLR σ（BPSK/AWGN/單位能量）

    % 各群組的通道 σ：puncture → 0；非 puncture → sigma_ch
    sigma_ch_group = zeros(1,numel(groups.id));
    for gi = 1:numel(groups.id)
        mask = (vn_group==groups.id(gi));
        if ~any(mask), sigma_ch_group(gi)=0; continue; end
        % 該群組裡有傳送的節點才有通道訊息
        if any(mask & ~is_punctured)
            sigma_ch_group(gi) = sigma_ch;  % 同一群設同 σ_ch（可依需求細分）
        else
            sigma_ch_group(gi) = 0;
        end
    end

    % 個別群組 VND（重點：extra 子曲線）
    colors = lines(6);
    for gi = 1:numel(groups.id)
        lam = lambda_by_group{gi};    % 該群組的 λ_d（edge-perspective, 已歸一）
        if isempty(lam), continue; end
        I_EV_g = vnd_curve(I_grid, lam, sigma_ch_group(gi), Jfun, Jinv);
        plot(I_grid, I_EV_g, '-', 'LineWidth', 1.5, ...
            'Color', colors(gi,:), 'DisplayName', sprintf('VND-%s @ %.2f dB', groups.name(gi), EbN0_dB));
    end

    % overall VND（用整體 λ_d）
    I_EV_all = vnd_curve(I_grid, lambda_all, sigma_ch, Jfun, Jinv, is_punctured, vn_group);
    ph = plot(I_grid, I_EV_all, 'LineWidth', 2, 'DisplayName', sprintf('VND-all @ %.2f dB', EbN0_dB));
    ph.Color = [0 0.45 0.74]; ph.LineStyle='--';

    % 門檻判斷：overall VND 必須全段高於 CND(倒軸)
    if ~thres_found && tunnel_open(I_grid, I_EV_all, I_EC, tunnel_margin)
        thres_found = true;
        plot(I_grid, I_EV_all, 'LineWidth', 3, 'Color',[0.85 0.33 0.1], ...
            'DisplayName', sprintf('THRESHOLD ≈ %.2f dB', EbN0_dB));
        fprintf('[THRESHOLD] R_eff=%.3f → Eb/N0 ≈ %.2f dB 開始有 tunnel\n', R_eff, EbN0_dB);
    end
end

xlabel('I_A (Input Mutual Information)');
ylabel('I_E (Output Mutual Information)');
title(sprintf('PEXIT curves (R_{eff}=%.3f) — payload/extra/overall + CND', R_eff));
legend('Location','southeast');
axis([0 1 0 1]);

%% ========= 5) GA 迭代統計（在門檻略上方的 SNR） =========
if ~thres_found
    pick_EbN0_dB = EbN0_dB_list(end);   % 若沒找到開口，就取最高 SNR 做示範
else
    pick_EbN0_dB = min(EbN0_dB_list(EbN0_dB_list >= round(pick_recent_threshold(),2)));
end
fprintf('GA 迭代統計使用 Eb/N0 = %.2f dB\n', pick_EbN0_dB);

EbN0    = 10^(pick_EbN0_dB/10);
sigma2_n = 1/(2*R_eff*EbN0);
m_ch     = 2/sigma2_n;
sigma_ch = sqrt(2*m_ch);

% 群組通道 σ
sigma_ch_group = zeros(1,numel(groups.id));
for gi = 1:numel(groups.id)
    mask = (vn_group==groups.id(gi));
    if any(mask & ~is_punctured), sigma_ch_group(gi)=sigma_ch; else, sigma_ch_group(gi)=0; end
end

% 用 edge-perspective 的 density-evolution（GA 版）跟蹤每回合 I（各群組 & overall）
[trace] = ga_iter_stats(lambda_by_group, rho, sigma_ch_group, Jfun, Jinv, max_iter_ga);

% 繪圖：各群組 I_v2c 隨迭代
figure; hold on; grid on; box on;
for gi = 1:numel(groups.id)
    if isempty(lambda_by_group{gi}), continue; end
    plot(0:max_iter_ga, trace.I_v2c_group(:,gi), '-o', 'DisplayName', sprintf('%s', groups.name(gi)));
end
plot(0:max_iter_ga, trace.I_v2c_all, 'k-d', 'LineWidth',1.5, 'DisplayName','overall');
xlabel('iteration');
ylabel('I_{v\to c}');
title(sprintf('GA evolution of I_{v\\to c} @ %.2f dB (R_{eff}=%.3f)', pick_EbN0_dB, R_eff));
legend('Location','southeast');

% 可選：把 I 轉 σ 或 LLR 統計（示意：輸出等效 σ）
figure; hold on; grid on; box on;
for gi = 1:numel(groups.id)
    if isempty(lambda_by_group{gi}), continue; end
    sigma_eq = Jinv(trace.I_v2c_group(:,gi));   % 等效 σ（可近似代表 |LLR| 成長）
    plot(0:max_iter_ga, sigma_eq, '-o', 'DisplayName', sprintf('%s', groups.name(gi)));
end
xlabel('iteration'); ylabel('\sigma_{eq} (from J^{-1}(I))');
title('Equivalent LLR-\sigma across iterations (per group)');
legend('Location','southeast');

%% ========= 輔助小函式（內嵌） =========
    function Eb = pick_recent_threshold()
        % 從圖例字串抓最後印出的 THRESHOLD 值；若你想更嚴謹可把閾值存成變數
        Eb = EbN0_dB_list(1);
        % 此處簡化：直接回傳目前 ebi 所在的 EbN0_dB
        Eb = pick_EbN0_dB; %#ok<NASGU>
    end
end

%% ========== 基礎工具們 ==========

function [var_deg, chk_deg, E] = basic_degrees(H)
var_deg = full(sum(H,1));
chk_deg = full(sum(H,2));
E = sum(var_deg);
end

function [lambda_all, lambda_by_group, rho] = edge_perspective_distributions(H, vn_group, var_deg, chk_deg, E)
max_dv = max(var_deg); max_dc = max(chk_deg);
lambda_all = zeros(1,max_dv);
rho = zeros(1,max_dc);

% overall λ_d
for d=1:max_dv
    lambda_all(d) = d * sum(var_deg==d) / E;
end

% group-wise λ_d（每個群的邊權）
groups = unique(vn_group);
lambda_by_group = cell(1, max(groups));
for gi = 1:max(groups)
    mask = (vn_group==gi);
    if ~any(mask), lambda_by_group{gi} = []; continue; end
    lam = zeros(1,max_dv);
    E_g = sum(var_deg(mask));  % 該群邊數
    if E_g==0, lambda_by_group{gi}=[]; continue; end
    for d=1:max_dv
        lam(d) = d * sum((var_deg==d) & mask) / E_g;
    end
    lambda_by_group{gi} = lam;
end

% ρ_d
for d=1:max_dc
    rho(d) = d * sum(chk_deg==d) / E;
end
end

function r = rank_mod2(H)
% 估算 GF(2) 秩（簡單版高斯消去；大型矩陣可替換更快實作）
H = mod(H,2);
[m,n] = size(H);
r = 0; c = 1;
for i=1:m
    % 找到當前列從 c 開始的首個 1
    pivot = find(H(i:end,c:end),1);
    while isempty(pivot)
        c = c+1; if c>n, break; end
        pivot = find(H(i:end,c:end),1);
    end
    if c>n, break; end
    [pi,pj] = ind2sub([m-i+1, n-c+1], pivot);
    pi = pi + i - 1; pj = pj + c - 1;
    % 交換
    tmp = H(i,:); H(i,:) = H(pi,:); H(pi,:) = tmp;
    tmp = H(:,c); H(:,c) = H(:,pj); H(:,pj) = tmp;
    % 消去
    for rr = [1:i-1, i+1:m]
        if H(rr,c)==1
            H(rr,:) = xor(H(rr,:), H(i,:));
        end
    end
    r = r+1; c = c+1;
    if c>n, break; end
end
end

function [Jfun, Jinv] = make_J_funcs()
% J / J^{-1} 以 σ 為自變數（常見近似）
Jfun  = @(sigma) 1 - exp(-0.4527 * sigma.^0.86 + 0.0218);
Jinv  = @(I) ((-1/0.4527) * log(1 - I) - 0.0218).^(1/0.86);
end

function I_EC = cnd_curve(I_grid, rho, J, Jinv)
I_EC = zeros(size(I_grid));
for i=1:numel(I_grid)
    sigma_Ac = Jinv(1 - I_grid(i));
    EC_sum = 0;
    for d=1:numel(rho)
        if rho(d)==0, continue; end
        sigma_out = sqrt(d-1) * sigma_Ac;
        EC_sum = EC_sum + rho(d) * J(sigma_out);
    end
    I_EC(i) = 1 - EC_sum;
end
end

function I_EV = vnd_curve(I_grid, lambda_d, sigma_ch, J, Jinv, is_punc, vn_group)
% 單一群的 VND（lambda_d 為該群 edge-persp. 分佈），puncture → sigma_ch=0
% 若給了 is_punc/vn_group，會以「該群仍有傳送者」則採用 sigma_ch，否則 0。
if nargin>=6 && ~isempty(is_punc) && ~isempty(vn_group)
    % 此處已在主程式處理群組通道 σ，故略過
end

I_EV = zeros(size(I_grid));
for i=1:numel(I_grid)
    sigma_A = Jinv(I_grid(i));
    EV_sum = 0;
    for d=1:numel(lambda_d)
        if lambda_d(d)==0, continue; end
        sigma_out = sqrt(sigma_ch^2 + (d-1)*sigma_A^2);
        EV_sum = EV_sum + lambda_d(d) * J(sigma_out);
    end
    I_EV(i) = EV_sum;
end
end

function ok = tunnel_open(I_grid, I_EV, I_EC, margin)
% 判斷 overall VND 是否全段高於 CND(倒軸)（保留一點 margin）
ok = all(I_EV >= (interp1(I_grid, I_EC, I_grid) + margin));
end

function trace = ga_iter_stats(lambda_by_group, rho, sigma_ch_group, J, Jinv, max_iter)
% 基於 edge-perspective 的簡化 GA density evolution：
% - 同一群的邊使用相同 λ_d，並共享同一通道 σ_ch
% - I_c2v 的更新只依 rho（與群無關）
G = numel(lambda_by_group);
I_v2c_group = zeros(max_iter+1, G);
I_c2v       = zeros(max_iter+1, 1);

% 初始：v->c (iter 0)
for gi=1:G
    lam = lambda_by_group{gi};
    if isempty(lam), continue; end
    sigma_out0 = sigma_ch_group(gi);
    I_v2c_group(1,gi) = J(sigma_out0);  % 初次只靠通道（puncture→0）
end

% overall v->c (iter 0)：以各群邊數比例（E_g / sum E_g）加權
w = zeros(1,G);
for gi=1:G
    lam = lambda_by_group{gi};
    if isempty(lam), w(gi)=0; else, w(gi)=sum(lam); end % lam 已經 sum=1；這裡僅做是否存在
end
w = w / sum(w + (sum(w)==0));  % 正規化防 NaN
I_v2c_all = zeros(max_iter+1,1);
I_v2c_all(1) = sum(w .* I_v2c_group(1,:));

% 迭代
for t=1:max_iter
    % c <- v 聚合（與群無關）
    % 先把上一回合 v->c 視為 I_A，更新 CND 輸出 I_c2v
    IA = I_v2c_all(t);
    sigma_Ac = Jinv(1 - IA);
    EC_sum = 0;
    for d=1:numel(rho)
        if rho(d)==0, continue; end
        sigma_out = sqrt(d-1) * sigma_Ac;
        EC_sum = EC_sum + rho(d) * J(sigma_out);
    end
    I_c2v(t+1) = 1 - EC_sum;

    % group-wise v <- c 合併 + 通道
    for gi=1:G
        lam = lambda_by_group{gi};
        if isempty(lam), continue; end
        EV = 0;
        sigma_A = Jinv(I_c2v(t+1));
        for d=1:numel(lam)
            if lam(d)==0, continue; end
            sigma_out = sqrt(sigma_ch_group(gi)^2 + (d-1)*sigma_A^2);
            EV = EV + lam(d) * J(sigma_out);
        end
        I_v2c_group(t+1,gi) = EV;
    end
    I_v2c_all(t+1) = sum(w .* I_v2c_group(t+1,:));
end

trace.I_v2c_group = I_v2c_group;
trace.I_v2c_all   = I_v2c_all;
trace.I_c2v       = I_c2v;
end
