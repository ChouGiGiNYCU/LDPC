%%
clear all;
clc;
close all;
%%
Payload_PCM_file = "C:\Users\USER\Desktop\LDPC\PCM\H_1008_504.txt";
Extra_PCM_file   = "C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt";
Combine_PCM_file = "C:\Users\USER\Desktop\LDPC\Throughput\Enhanced\PayLoad1008_Extra(BCH15)_EnhancedH_v1\PCM_P1008_EH15_EnhanceStruc_kSR.txt";
iteration = 1:5:200;

Payload_PCM = readHFromFileByLine(Payload_PCM_file);
Extra_PCM   = readHFromFileByLine(Extra_PCM_file);
Combine_PCM = readHFromFileByLine(Combine_PCM_file);

Payload_edges = sum(sum(Payload_PCM));
Extra_edges   = sum(sum(Extra_PCM));
Combine_edges = sum(sum(Combine_PCM));

Extra_info_bits = size(Extra_PCM,2) - size(Extra_PCM,1);
%% BP time complexity ~= it*O(|E|) edges數量呈 linearly

Free_Ride_complexity = zeros([1,numel(iteration)]);
Enhanced_complexity  = zeros([1,numel(iteration)]);

for idx=1:numel(iteration)
    Free_Ride_complexity(idx) = pow2(Extra_info_bits) + iteration(idx)*Payload_edges;
    Enhanced_complexity(idx)  = iteration(idx)*Combine_edges;
end

%%
plot(iteration,Free_Ride_complexity,"-*","Color",[0.8500 0.3250 0.0980],"LineWidth",2);
hold on;

plot(iteration,Enhanced_complexity,"->","Color",[0.6350 0.0780 0.1840],"LineWidth",2);
hold on;

title("Time Complexity Compare")
legend("Free Ride","Enhanced Matrix");
xlabel("Iteration");
ylabel("Complexity");
grid on;