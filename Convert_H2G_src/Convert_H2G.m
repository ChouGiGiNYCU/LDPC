clear all;
clc;
H_matrix = [1 1 1 1 1;0 1 1 1 1;0 0 1 1 1;0 0 0 1 1;0 0 0 0 1];
file_name = "C:\Users\USER\Desktop\LDPC\PCM\BCH_15_7.txt";
G_filename = "C:\Users\USER\Desktop\LDPC\GM\BCH_15_7_G.txt";
H_matrix = readHFromFileByLine(file_name);
[N, K, M_ori, M_sys, sysG, sysH, G, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(H_matrix);
writeMatrixToFile(G,G_filename);




