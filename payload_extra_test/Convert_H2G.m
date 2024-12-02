%%
filename = "PEGReg504x1008_Mat.txt";
H = readMatrixFromFileByLine(filename);
filename = "PEGReg504x1008_G.txt";
[N, K, M_ori, M_sys, sysG, sysH, G, PCM ]= f_GaussJordanE_gf2_HPIform_TFR(H);
writeMatrixToFile(G,filename)


