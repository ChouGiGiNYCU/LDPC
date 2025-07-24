import torch
from torch.utils.data import TensorDataset, DataLoader, random_split 
import torch.nn as nn 
import torch.optim as optim 
from tqdm import tqdm  # 進度條
import matplotlib.pyplot as plt   # 繪圖
from collections import defaultdict
import numpy as np
from function import *
from Gaussian_Noise import *
from read_PCM_G import *
from sparse_bp_model import SparseBPNeuralNetwork
from DownLoad_weight import *
################# Some parameter setting #################
H_file_name = r"H_96_48.txt"  
H_file_name_2 = r"H_96_48_2.txt"  
# G_file_name = r"H_10_5_G.txt"
G_file_name = None
Save_model_file_name = r"NNBP_Best.pth"
# Load_model_file_name = r"BPMTL_17037_train.pth"
# DownLoad_Weight_Flag = True
Load_model_file_name = None
DownLoad_Weight_Flag = False
learning_ratio = 0.001
EPOCHS = 100000
BATCH_SIZE = 20 
SNR_MIN = 2
SNR_MAX = 5
SNT_RATIO = 0.5

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
################# Some parameter setting #################
if G_file_name:
    G_matrix = read_G_file(G_file_name)
    G_rowsize = len(G_matrix)
    G_colsize = len(G_matrix[0])

H_matrix_1 = read_H_file(H_file_name)
H_matrix_2 = read_H_file(H_file_name_2)
parity_check_matrix_colsize = len(H_matrix_1[0])
parity_check_matrix_rowsize = len(H_matrix_1)
# first hidden layer node number count 1 number in parity_matrix
hidden_layer_node1 = sum(sum(row) for row in H_matrix_1)
hidden_layer_node2 = sum(sum(row) for row in H_matrix_2)
print("parity_check_matrix_rowsize : ",parity_check_matrix_rowsize)
print("parity_check_matrix_colsize : ",parity_check_matrix_colsize)
print("hidden_layer_node1 : ",hidden_layer_node1)
print("hidden_layer_node2 : ",hidden_layer_node2)

# define model paramter
input_size = parity_check_matrix_colsize
hidden_layer_node1 = hidden_layer_node1
hidden_layer_node2 = hidden_layer_node2
output_size = parity_check_matrix_colsize

if torch.cuda.is_available(): print("Now is use GPU to train")
else: print("Now is use CPU to train")

# H1
CN2VN_pos1 = Construct_CN2VN(H_matrix_1)
VN2CN_pos1 = Construct_VN2CN(H_matrix_1)
FLM_1,dict_update_cn1 = Construct_FLM(CN2VN_pos1,parity_check_matrix_colsize,hidden_layer_node1)
CN2VN_mask_VNUpdate1,dict_update_vn1 = Construct_CN2VN_mask(VN2CN_pos1,dict_update_cn1,hidden_layer_node1)
VN2CN_mask_CNUpdate1,dict_update_cn1 = Construct_VN2CN_mask(CN2VN_pos1,dict_update_vn1,hidden_layer_node1)
CN2VN_mask_output1 = Construct_LLM(CN2VN_pos1,hidden_layer_node1,parity_check_matrix_colsize)
Dot_bias_matrix1 = Construct_Dot_Bias_Matrix(VN2CN_pos1,parity_check_matrix_colsize,hidden_layer_node1) # 神經網路裡面的bias=[LLR1 ... LLR_VN]*matrix
Channel_LLR_mask1 = torch.eye(parity_check_matrix_colsize)
# H2
CN2VN_pos2 = Construct_CN2VN(H_matrix_2)
VN2CN_pos2 = Construct_VN2CN(H_matrix_2)
FLM_2,dict_update_cn2 = Construct_FLM(CN2VN_pos2,parity_check_matrix_colsize,hidden_layer_node2)
CN2VN_mask_VNUpdate2,dict_update_vn2 = Construct_CN2VN_mask(VN2CN_pos2,dict_update_cn2,hidden_layer_node2)
VN2CN_mask_CNUpdate2,dict_update_cn2 = Construct_VN2CN_mask(CN2VN_pos2,dict_update_vn2,hidden_layer_node2)
CN2VN_mask_output2 = Construct_LLM(CN2VN_pos2,hidden_layer_node2,parity_check_matrix_colsize)
Dot_bias_matrix2 = Construct_Dot_Bias_Matrix(VN2CN_pos2,parity_check_matrix_colsize,hidden_layer_node2) # 神經網路裡面的bias=[LLR1 ... LLR_VN]*matrix
Channel_LLR_mask2 = torch.eye(parity_check_matrix_colsize)

########################### Model Setting ###########################
model = SparseBPNeuralNetwork(input_size,output_size,hidden_layer_node1,FLM_1,CN2VN_mask_VNUpdate1,VN2CN_mask_CNUpdate1,CN2VN_mask_output1,Dot_bias_matrix1,Channel_LLR_mask1,hidden_layer_node2,FLM_2,CN2VN_mask_VNUpdate2,VN2CN_mask_CNUpdate2,CN2VN_mask_output2,Dot_bias_matrix2,Channel_LLR_mask2,device)
model = model.to(device)
criterion = nn.BCELoss()
optimizer = optim.Adam(model.parameters(), lr=learning_ratio)
train_losses = []
if Load_model_file_name:
    model.load_state_dict(torch.load(Load_model_file_name,weights_only=True))
    if DownLoad_Weight_Flag:
        download_weight(model,weight_name="weight",bias_name="bias_scaling_vectors") # down laod model parameter
        exit(1)
########################### Model Setting ###########################

code_rate = (parity_check_matrix_colsize - parity_check_matrix_rowsize)/parity_check_matrix_colsize
########################### Train Model   ###########################
for epoch in range(EPOCHS):
    model.train()  # train mode
    LLRs,EncodeCodeWords = [],[]
    ############ 產生train data set
    for SNR in np.arange(SNR_MIN,SNR_MAX,SNT_RATIO):
        sigma =(1/(2*code_rate*(10**(SNR/10))))**0.5
        for i in range(BATCH_SIZE):
            if G_file_name:
                information_bits = np.random.randint(0, 2, size=(1, G_rowsize))  # [0~2) 的隨機值 = 0、1
                EncodeCodeWord = np.dot(information_bits,G_matrix) % 2# (1 x G_rowsize) x (G_matrix)
            else :
                EncodeCodeWord = np.zeros(parity_check_matrix_colsize, dtype=int)
            LLR = []
            for idx in range(len(EncodeCodeWord)):
                LLR.append(2*((-2*EncodeCodeWord[idx] + 1) + sigma*generate_gaussian_noise())/(sigma**2))

            EncodeCodeWord = np.where(EncodeCodeWord == 0, 1, 0)  # EncodeCodeWord=0 -> 1 ,else 0
            LLRs.append(LLR)
            EncodeCodeWords.append(EncodeCodeWord)
    # test1,test2 =  np.ones(parity_check_matrix_colsize, dtype=int)*1.1 , np.zeros(parity_check_matrix_colsize, dtype=int)
    # LLRs.append(test1)
    # EncodeCodeWords.append(test2)
    # LLRs.append(test1)
    # EncodeCodeWords.append(test2)
    LLRs = np.squeeze(LLRs)
    EncodeCodeWords = np.squeeze(EncodeCodeWords)
    LLRs = torch.Tensor(np.array(LLRs)).to(device)
    EncodeCodeWords = torch.Tensor(np.array(EncodeCodeWords)).to(device)
    ############
    optimizer.zero_grad()       # clear grad
    inputs = LLRs
    labels = EncodeCodeWords
    out10,out9,out8,out7,out6,out5,out4,out3,out2,out1 = model(inputs)
    ###### count multiple loss ######
    loss1 = criterion(out1, labels)
    loss2 = criterion(out2, labels)
    loss3 = criterion(out3, labels)
    loss4 = criterion(out4, labels)
    loss5 = criterion(out5, labels)
    loss6 = criterion(out6, labels)
    loss7 = criterion(out7, labels)
    loss8 = criterion(out8, labels)
    loss9 = criterion(out9, labels)
    loss10 = criterion(out10, labels)
    losses = loss1+loss2+loss3+loss4+loss5+loss6+loss7+loss8+loss9+loss10
    #################################
    losses.backward()             # backward
    model.apply_sparse_masks()  
    optimizer.step()            # update parameter
    train_loss = losses.item()
    
    if len(train_losses)>0 and train_loss<min(train_losses):
        torch.save(model.state_dict(), Save_model_file_name)
        print(f'Epoch [{epoch}] - Loss : [{train_loss}]')
    train_losses.append(train_loss)
    

########################### Train Model   ###########################

########################### Plot Loss   ###########################
# 繪製訓練和驗證損失比較圖
plt.figure(figsize=(10, 5))
plt.plot(train_losses, label='Training Loss')
# plt.plot(valid_losses, label='Validation Loss')
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.title('Training and Validation Loss Over Epochs')
plt.legend()
plt.show()
########################### Plot Loss   ###########################

