filename = "C:\Users\USER\Desktop\LDPC\PCM\H_96_48.txt"
H  = readHFromFileByLine(filename)

ldpc_exit_chart(H)

%%
filename = "PCM_P1008_E15BCH_Structure.txt";
H  = readHFromFileByLine(filename);
puncture_idx = [1009:(1009+14) , (1009+15*2):(1009+15*3-1)];
enhanced_exit_suite_v1(H,puncture_idx,1:1008,1009:1023);