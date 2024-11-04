%%
filename = "H_96_48_Mat.txt";
H = readMatrixFromFileByLine(filename);
filename = "H_96_48_G.txt";
[N, K, M_ori, M_sys, sysG, sysH, G, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(H);
writeMatrixToFile(G,filename)


