import torch
from torch.utils.data import TensorDataset, DataLoader, random_split
import torch.nn as nn
import torch.optim as optim
from tqdm import tqdm  # 進度條
import matplotlib.pyplot as plt  # 繪圖
from collections import defaultdict
from function import *
from sparse_bp_model import SparseBPNeuralNetwork
from read_PCM_G import *
from Gaussian_Noise import generate_gaussian_noise
import numpy as np
import csv
import random

################# Some parameter setting #################
H_file_name = r"H_10_5.txt"  
Load_model_file_name = r'NNBP_Best.pth'
CSV_file_name = r"BPNN_H10x5_it20.csv"
Load_setting = True
G_file_name = None
Frame_Error_Bound = 400
BATCH_SIZE = 20
SNR_MIN = 0
SNR_MAX = 6
SNR_RATIO = 1
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
# device = torch.device("cpu")
if torch.cuda.is_available(): print(f"GPU is available. Device: {torch.cuda.get_device_name(device)}")
else: print("GPU not available, using CPU instead.")

################# Some parameter setting #################
if G_file_name:
    G_matrix = read_G_file(G_file_name)
    G_rowsize = len(G_matrix)
    G_colsize = len(G_matrix[0])

H_matrix_1 = read_H_file(H_file_name)
parity_check_matrix_colsize = len(H_matrix_1[0])
parity_check_matrix_rowsize = len(H_matrix_1)
# first hidden layer node number count 1 number in parity_matrix
hidden_layer_node1 = sum(sum(row) for row in H_matrix_1)

print("parity_check_matrix_rowsize : ",parity_check_matrix_rowsize)
print("parity_check_matrix_colsize : ",parity_check_matrix_colsize)
print("hidden_layer_node1 : ",hidden_layer_node1)

# define model paramter
input_size = parity_check_matrix_colsize
hidden_layer_node1 = hidden_layer_node1
output_size = parity_check_matrix_colsize



# H1
CN2VN_pos1 = Construct_CN2VN(H_matrix_1)
VN2CN_pos1 = Construct_VN2CN(H_matrix_1)
FLM_1,dict_update_cn1 = Construct_FLM(CN2VN_pos1,parity_check_matrix_colsize,hidden_layer_node1)
CN2VN_mask_VNUpdate1,dict_update_vn1 = Construct_CN2VN_mask(VN2CN_pos1,dict_update_cn1,hidden_layer_node1)
VN2CN_mask_CNUpdate1,dict_update_cn1 = Construct_VN2CN_mask(CN2VN_pos1,dict_update_vn1,hidden_layer_node1)
CN2VN_mask_output1 = Construct_LLM(CN2VN_pos1,hidden_layer_node1,parity_check_matrix_colsize)
Dot_bias_matrix1 = Construct_Dot_Bias_Matrix(VN2CN_pos1,parity_check_matrix_colsize,hidden_layer_node1) # 神經網路裡面的bias=[LLR1 ... LLR_VN]*matrix
Channel_LLR_mask1 = torch.eye(parity_check_matrix_colsize)


########################### Model Setting ###########################
model = SparseBPNeuralNetwork(input_size,output_size,hidden_layer_node1,FLM_1,CN2VN_mask_VNUpdate1,VN2CN_mask_CNUpdate1,CN2VN_mask_output1,Dot_bias_matrix1,Channel_LLR_mask1,device)
model = model.to(device)
if Load_setting == True:
    model.load_state_dict(torch.load(Load_model_file_name, map_location=device, weights_only=True))
    print("model load pth file : {}".format(Load_model_file_name))
########################### Model Setting ###########################
model.eval()
code_rate = (parity_check_matrix_colsize - parity_check_matrix_rowsize)/parity_check_matrix_colsize
# transmit_codeword = [0 for i in parity_check_matrix_colsize]
receiver_LLR = []
Encode_CodeWord = []
BER = []
FER = []
for SNR in np.arange(SNR_MIN,SNR_MAX+1,SNR_RATIO):
    sigma =(1/(2*code_rate*(10**(SNR/10))))**0.5
    # print(f"SNR : {SNR} | sigma : {sigma}")
    FRAME_COUNT = 0
    FRAME_ERROR_COUNT = 0
    BER_COUNT = 0
    while FRAME_ERROR_COUNT < Frame_Error_Bound:
        # print(f"FRAME_ERROR_COUNT : {FRAME_ERROR_COUNT}")
        
        
        receiver_LLR_batch = []
        Encode_CodeWord_batch = []
        for _ in range(BATCH_SIZE):
            receiver_LLR=[]
            ######## produce Encode CodeWord ########
            if G_file_name:
                Information_Bit = np.random.randint(0, 2, size=(1, G_rowsize))  # [0~2) 的隨機值 = 0、1
                Encode_CodeWord = np.dot(Information_Bit,G_matrix) % 2
                Encode_CodeWord = Encode_CodeWord[0].tolist()
            else:
                Encode_CodeWord.clear()
                for i in range(parity_check_matrix_colsize):
                    Encode_CodeWord.append(0)
            ########################################
            ######## Convert to receiver LLR ########
            for i in range(0,parity_check_matrix_colsize):    
                receiver_codeword = -2*Encode_CodeWord[i] + 1 + sigma*generate_gaussian_noise()
                receiver_LLR.append(2*receiver_codeword/(sigma**2))
                # receiver_LLR.append(1) # 測適用
            #########################################
            receiver_LLR_batch.append(receiver_LLR)
            Encode_CodeWord_batch.append(Encode_CodeWord)

        ############## list to tensor ##############
        receiver_LLR_tensor = torch.tensor(receiver_LLR_batch, dtype=torch.float32).to(device)
        ############## list to tensor ##############
        
        with torch.no_grad():
            decode_codeword_tensor = model(receiver_LLR_tensor)
        # print(decode_codeword_tensor)
        decode_codeword_batch = decode_codeword_tensor.squeeze().float().tolist()  # 確保 decode_codeword 是整數列表
        # print(decode_codeword_batch)
        # exit(1)
        for i in range(BATCH_SIZE):
            FRAME_COUNT += 1
            decode_codeword = decode_codeword_batch[i] 
            Encode_CodeWord = Encode_CodeWord_batch[i]
            # print(f"receiver_LLR : {receiver_LLR} | decode_codeword : {decode_codeword}")
            for idx,bit in enumerate(decode_codeword):
                if bit<=0.5:
                    decode_codeword[idx]=1
                else:
                    decode_codeword[idx]=0
            FLAG = False 
            for i in range(len(decode_codeword)):
                if decode_codeword[i]!=Encode_CodeWord[i]:
                    BER_COUNT+=1
                    FLAG=True
            if FLAG == True:
                FRAME_ERROR_COUNT += 1
            

    FER.append(FRAME_ERROR_COUNT/FRAME_COUNT)
    BER.append(BER_COUNT/(parity_check_matrix_colsize*FRAME_COUNT))
    print("SNR:{} | FER : {} | BER : {} | Total_Frame : {}".format(SNR,FER[-1],BER[-1],FRAME_COUNT))


# 開啟輸出的 CSV 檔案
with open(CSV_file_name, 'w', newline='') as csvfile:
    # 建立 CSV 檔寫入器
    writer = csv.writer(csvfile)

    # 寫入一列資料
    writer.writerow(['SNR', 'FER', 'BER'])
    for i,SNR in enumerate(np.arange(SNR_MIN,SNR_MAX+1,SNR_RATIO)):
        writer.writerow([SNR, FER[i], BER[i]])

        



