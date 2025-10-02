% MATLAB code for plotting EXIT chart of LDPC code given parity-check matrix H
% Corrected version: Removed transposes, fixed power to linear, adjusted to (d-1), and m_EV = m_ch + lambda_x(m_A)
% Usage: Replace H with your matrix, adjust EbN0_dB and max_deg if needed

clear; clc;

% Example H matrix (replace with your input H, m x n sparse matrix)
% Here, a small (3,6)-regular LDPC example for demo (6x12 matrix, rate 1/2)
% H = [1 1 1 1 0 0 0 0 0 0 0 0;
%      0 0 0 0 1 1 1 1 0 0 0 0;
%      0 0 0 0 0 0 0 0 1 1 1 1;
%      1 0 0 1 1 0 0 1 1 0 0 0;
%      0 1 0 0 0 1 0 0 0 1 0 1;
%      0 0 1 0 0 0 1 0 0 0 1 0];
% filename = "PCM_P1008_E15BCH_Structure.txt";
filename = "C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt";
H  = readHFromFileByLine(filename);
[m, n] = size(H);  % m: checks, n: variables
rate = (n - m) / n;  % Code rate

% Channel parameter: Eb/N0 in dB (adjust for your threshold analysis)
EbN0_dB = 1;  % Example value
sigma2 = 1 / (2 * rate * 10^(EbN0_dB/10));  % Noise variance
m_ch = 2 / sigma2;  % Corrected: mean for AWGN LLR = 2 / sigma^2 (for unit energy BPSK)

% Compute degree distributions from H
max_deg = 20;  % Max degree assumed

% Variable node degrees (lambda, node perspective, then convert to edge via *d)
var_deg = sum(H, 1);  % Degree of each variable node
lambda_coeff = zeros(1, max_deg);
for d = 1:max_deg
    lambda_coeff(d) = sum(var_deg == d) / n;
end
% For VND: lambda_x(m_A) = m_A * (avg_dv - 1)
lambda_x = @(x) x * sum(lambda_coeff(2:end) .* (1:(max_deg-1)));

% Check node degrees (rho)
chk_deg = sum(H, 2);  % Degree of each check node
rho_coeff = zeros(1, max_deg);
for d = 1:max_deg
    rho_coeff(d) = sum(chk_deg == d) / m;
end
% For CND: rho_x(m_A) = m_A * (avg_dc - 1)
rho_x = @(x) x * sum(rho_coeff(2:end) .* (1:(max_deg-1)));

% J function and inverse J (mutual information, Gaussian approx.)
J_approx = @(m) 1 - exp(-0.4527 * m.^0.86 + 0.0218);  % Approximation for speed
invJ_approx = @(I) ( -1/0.4527 * log(1 - I) - 0.0218 ).^(1/0.86);  % Inverse approx

% Use approximation for efficiency
J_func = J_approx;
invJ_func = invJ_approx;

% Compute VND (Variable Node Decoder) EXIT function: I_EV = f(I_A)
I_A = 0:0.01:1;  % Input mutual info range
I_EV = zeros(size(I_A));
for i = 1:length(I_A)
    m_A = invJ_func(I_A(i));  % Mean from I_A
    m_EV = m_ch + lambda_x(m_A);  % Corrected: m_ch + m_A * (avg_dv - 1)
    I_EV(i) = J_func(m_EV);  % Output mutual info
end

% Compute CND (Check Node Decoder) EXIT function: I_EC = f(I_A)
I_EC = zeros(size(I_A));
for i = 1:length(I_A)
    m_A = invJ_func(1 - I_A(i));  % Note the 1 - I_A for CND
    m_EC = rho_x(m_A);  % m_A * (avg_dc - 1)
    I_EC(i) = 1 - J_func(m_EC);  % Complementary
end

% Plot EXIT chart
figure;
plot(I_A, I_EV, 'b-', 'LineWidth', 2);  % VND curve
hold on;
plot(I_EC, I_A, 'r-', 'LineWidth', 2);  % CND curve (inverted axes)
xlabel('I_A (Input Mutual Information)');
ylabel('I_E (Output Mutual Information)');
title(sprintf('EXIT Chart for LDPC Code (Rate=%.2f, Eb/N0=%.1f dB)', rate, EbN0_dB));
legend('VND', 'CND (inverted)', 'Location', 'southeast');
grid on;
axis([0 1 0 1]);

% Check convergence: If VND above CND, it converges
disp('If the blue curve is above the red (inverted), decoding converges.');