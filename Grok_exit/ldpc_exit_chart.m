function ldpc_exit_chart(H)
% LDPC_EXIT_CHART - 為給定的H矩陣生成EXIT chart
% 輸入: H - LDPC校驗矩陣
% 
% 使用方法:
% H = your_parity_check_matrix;  % 載入你的H矩陣
% ldpc_exit_chart(H);

% 檢查輸入
if nargin < 1
    error('請提供校驗矩陣H作為輸入參數');
end

% 獲取矩陣維度
[M, N] = size(H);
fprintf('校驗矩陣大小: %d x %d\n', M, N);
fprintf('碼長: %d, 校驗位元數: %d\n', N, M);
fprintf('碼率: %.3f\n', (N-M)/N);

% 計算節點度數分佈
variable_degrees = sum(H, 1);  % 每個變數節點的度數
check_degrees = sum(H, 2);     % 每個校驗節點的度數

% 獲取度數分佈
max_var_degree = max(variable_degrees);
max_check_degree = max(check_degrees);

% 計算變數節點和校驗節點的度數分佈
lambda = zeros(1, max_var_degree);  % 變數節點度數分佈
rho = zeros(1, max_check_degree);   % 校驗節點度數分佈

for i = 1:max_var_degree
    lambda(i) = sum(variable_degrees == i) / N;
end

for i = 1:max_check_degree
    rho(i) = sum(check_degrees == i) / M;
end

% 移除零元素
lambda = lambda(lambda > 0);
rho = rho(rho > 0);
var_degrees_used = find(lambda > 0);
check_degrees_used = find(rho > 0);

fprintf('\n變數節點度數分佈:\n');
for i = 1:length(var_degrees_used)
    fprintf('度數 %d: %.4f\n', var_degrees_used(i), lambda(i));
end

fprintf('\n校驗節點度數分佈:\n');
for i = 1:length(check_degrees_used)
    fprintf('度數 %d: %.4f\n', check_degrees_used(i), rho(i));
end

% EXIT chart參數設定
IA_range = 0:0.01:0.999;  % 輸入互資訊範圍
sigma_range = 0.1:0.1:3;  % 雜訊標準差範圍

% 初始化EXIT函數
IE_VN = zeros(size(IA_range));  % 變數節點輸出互資訊
IA_CN = zeros(size(IA_range));  % 校驗節點輸入互資訊
IE_CN = zeros(size(IA_range));  % 校驗節點輸出互資訊

% 計算變數節點EXIT函數
fprintf('\n計算變數節點EXIT函數...\n');
for i = 1:length(IA_range)
    IA = IA_range(i);
    IE_sum = 0;
    
    % 對每個度數的變數節點計算EXIT函數
    for d_idx = 1:length(var_degrees_used)
        d = var_degrees_used(d_idx);
        if d == 1
            % 度數為1的節點沒有來自其他校驗節點的訊息
            IE_d = 0;
        else
            % 使用EXIT函數近似公式
            % 對於度數d的變數節點: IE = 1 - exp(-IA*(d-1))的近似
            if IA == 0
                IE_d = 0;
            else
                % 更精確的EXIT函數計算
                IE_d = 1 - (1 - IA)^(d-1);
            end
        end
        IE_sum = IE_sum + lambda(d_idx) * IE_d;
    end
    IE_VN(i) = IE_sum;
end

% 計算校驗節點EXIT函數
fprintf('計算校驗節點EXIT函數...\n');
for i = 1:length(IA_range)
    IA = IA_range(i);
    IE_sum = 0;
    
    % 對每個度數的校驗節點計算EXIT函數
    for d_idx = 1:length(check_degrees_used)
        d = check_degrees_used(d_idx);
        if d == 1
            IE_d = IA;
        else
            % 校驗節點EXIT函數
            if IA == 1
                IE_d = 1;
            else
                % 使用校驗節點EXIT函數公式
                IE_d = 1 - (1 - IA)^(d-1);
            end
        end
        IE_sum = IE_sum + rho(d_idx) * IE_d;
    end
    IE_CN(i) = IE_sum;
end

% 校驗節點的輸入是變數節點的輸出
IA_CN = IE_VN;

% 繪製EXIT chart
figure('Position', [100, 100, 800, 600]);
hold on;
grid on;

% 繪製變數節點EXIT曲線 (IA_VN vs IE_VN)
plot(IA_range, IE_VN, 'b-', 'LineWidth', 2, 'DisplayName', '變數節點 (VN)');

% 繪製校驗節點EXIT曲線 (IE_CN vs IA_CN，需要翻轉)
plot(IE_CN, IA_range, 'r--', 'LineWidth', 2, 'DisplayName', '校驗節點 (CN)');

% 繪製解碼軌跡（從(0,0)開始的階梯函數）
trajectory_x = [0];
trajectory_y = [0];
current_ia = 0;

for step = 1:100 % 最多20步迭代
    % 變數節點更新
    current_ie_vn = interp1(IA_range, IE_VN, current_ia, 'linear', 'extrap');
    trajectory_x = [trajectory_x, current_ia, current_ie_vn];
    trajectory_y = [trajectory_y, current_ie_vn, current_ie_vn];
    
    % 校驗節點更新
    current_ia = interp1(IE_CN, IA_range, current_ie_vn, 'linear', 'extrap');
    if current_ia >= 0.999 || abs(current_ia - trajectory_x(end)) < 0.001
        break;
    end
    trajectory_x = [trajectory_x, current_ie_vn];
    trajectory_y = [trajectory_y, current_ia];
end

plot(trajectory_x, trajectory_y, 'g-', 'LineWidth', 1.5, 'DisplayName', '解碼軌跡');

% 設定圖表屬性
xlabel('I_A (輸入互資訊)', 'FontSize', 12);
ylabel('I_E (輸出互資訊)', 'FontSize', 12);
title(sprintf('LDPC EXIT Chart (碼長=%d, 碼率=%.3f)', N, (N-M)/N), 'FontSize', 14);
legend('Location', 'southeast');
xlim([0, 1]);
ylim([0, 1]);

% 添加對角線參考
plot([0, 1], [0, 1], 'k:', 'LineWidth', 0.5, 'HandleVisibility', 'off');

% 分析收斂性
convergence_gap = min(IE_VN - interp1(IA_range, IE_CN, IE_VN, 'linear', 'extrap'));
if convergence_gap > 0
    fprintf('\n解碼器可以收斂 (最小間隙: %.4f)\n', convergence_gap);
else
    fprintf('\n解碼器可能無法收斂 (最小間隙: %.4f)\n', convergence_gap);
end

% 顯示閾值資訊
fprintf('解碼閾值分析完成\n');
fprintf('最終軌跡點: (%.4f, %.4f)\n', trajectory_x(end), trajectory_y(end));

hold off;
end

% 使用範例:
% H = your_parity_check_matrix;  % 載入你的H矩陣
% ldpc_exit_chart(H);

% 如果需要測試用的H矩陣，可以使用以下程式碼生成一個簡單的例子:
function H_example = generate_example_H()
    % 生成一個簡單的(7,4)漢明碼H矩陣作為測試
    H_example = [1 1 1 0 1 0 0;
                 1 0 1 1 0 1 0;
                 0 1 1 1 0 0 1];
    fprintf('生成測試用H矩陣 (7,4)漢明碼\n');
end