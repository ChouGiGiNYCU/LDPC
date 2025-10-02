filename = "C:\Users\USER\Desktop\LDPC\PCM\H_96_48.txt"
H  = readHFromFileByLine(filename)





EbN0_dB = -1;                 % 想看的 Eb/N0（以 coded bit 為準）
exit_chart_from_H(H, EbN0_dB);   % 基本 EXIT 圖



