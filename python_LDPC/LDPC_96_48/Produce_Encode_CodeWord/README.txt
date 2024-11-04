Info :
    Reading parity_check_matrix to create SNR_MIN~SNR_MAX Encoded_CodeWord , you can click run.bat to execute.

    Execute Produce_Encode_CodeWord.exe need some parameter(Can edit in run.bat):
        Encode_Folder - Parity_Check_Matrix Folder path
        Encode_matrix_file - Parity_Check_Matrix file name
        Encode_CodeWord_Folder_Path - save the Encode_CodeWord folder path
        Encode_CodeWord_FILE_NAME - save the Encode_CodeWord file name
        Encode_CodeWord_LLR_FILE_NAME - save the Encode_CodeWord(LLR) file name
        FRAME - Each SNR create  Frame Numebr 
        SNR_MIN - Start SNR
        SNR_MAX - End SNR
        SNR_RATIO -SNR_step
        compile_cpp=Produce_Encode_CodeWord.cpp //don't change
        compile_exe=Produce_Encode_CodeWord //don't change

Command:
    .\Produce_Encode_CodeWord ../Generator_Matrix/BCH_63_45_G.txt  ../Encode_CodeWord/BCH_63_45/BCH_63_45_EncodeCodeWord_SNR  ../Encode_CodeWord/BCH_63_45/BCH_63_45_EncodeCodeWord_LLR_SNR 10000 0 7 1