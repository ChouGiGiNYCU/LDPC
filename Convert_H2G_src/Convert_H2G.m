clear all;
clc;
H_matrix = [0 1 1 0 0 0;1 0 1 1 0 0;1 0 0 1 1 0;0 0 0 0 1 1];
file_name = "C:\Users\USER\Desktop\LDPC\PCM\PEGReg504x1008.txt";
G_filename = "C:\Users\USER\Desktop\LDPC\GM\PEGReg504x1008.txt";
H_matrix = readHFromFileByLine(file_name);
[N, K, M_ori, M_sys, sysG, sysH, G, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(H_matrix);
writeMatrixToFile(G,G_filename);

if mod(sum(sum(G*H_matrix.')),2)==0
    disp("G * H^T is zero matrix !!");
end

