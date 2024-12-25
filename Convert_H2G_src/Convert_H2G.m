clear;
clc;
H_filename = "H_10_5.txt";
G_filename = "H_10_5_G.txt"
H_matrix = readHFromFileByLine(H_filename);
[N, K, M_ori, M_sys, sysG, sysH, G, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(H_matrix);
writeMatrixToFile(G,G_filename)
