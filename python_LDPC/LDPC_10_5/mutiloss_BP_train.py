import torch
from torch.utils.data import TensorDataset, DataLoader, random_split 
import torch.nn as nn 
import torch.optim as optim 
from tqdm import tqdm  # 進度條
import matplotlib.pyplot as plt   # 繪圖
from collections import defaultdict
import time
import numpy as np
from function import *
from Gaussian_Noise import *
from read_PCM_G import *
from sparse_bp_model import SparseBPNeuralNetwork
from DownLoad_weight import *


################# Some parameter setting #################
H_file_name = r"H_10_5.txt"   
# G_file_name = r"H_10_5_G.txt"
G_file_name = None
Save_model_file_name = r"NNBP_Best.pth"
# Load_model_file_name = r"BPMTL_17037_train.pth"
# DownLoad_Weight_Flag = True
Load_model_file_name = None
DownLoad_Weight_Flag = False
learning_ratio = 0.001
EPOCHS = 200000
BATCH_SIZE = 20 
SNR_MIN = 1
SNR_MAX = 5
SNT_RATIO = 0.5

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
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

########################### Model Setting ###########################
model = SparseBPNeuralNetwork(input_size,output_size,hidden_layer_node1,FLM_1,CN2VN_mask_VNUpdate1,VN2CN_mask_CNUpdate1,CN2VN_mask_output1,Dot_bias_matrix1,Channel_LLR_mask1,device)
model = model.to(device)
criterion = nn.BCELoss()
optimizer = optim.Adam(model.parameters(), lr=learning_ratio)
train_losses,test_losses = [],[]
if Load_model_file_name:
    model.load_state_dict(torch.load(Load_model_file_name,weights_only=True))
    print(f"Loading the weight file : {Load_model_file_name} - successive !!")
    if DownLoad_Weight_Flag:
        download_weight(model,weight_name="weight",bias_name="bias_scaling_vectors") # down laod model parameter
        exit(1)
########################### Model Setting ###########################

code_rate = (parity_check_matrix_colsize - parity_check_matrix_rowsize)/parity_check_matrix_colsize
############## Create test_set for test model performance ##############
test_loader = Construct_test_set(SNR_MIN=SNR_MIN,SNR_MAX=SNR_MAX,CodeWord_length=parity_check_matrix_colsize,CodeRate=code_rate,Data_set_num=500,BATCH_SIZE=40,G_file=G_file_name)

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
    # test1,test2 =  np.ones(parity_check_matrix_colsize, dtype=int) , np.zeros(parity_check_matrix_colsize, dtype=int)
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
    Predict_CodeWord = model(inputs)
    ###### count multiple loss ######
    loss = criterion(Predict_CodeWord, labels)
    losses = loss
    #################################
    losses.backward()             # backward
    model.apply_sparse_masks()  
    optimizer.step()            # update parameter
    train_loss = losses.item()
    
    # if len(train_losses)>0 and train_loss<min(train_losses):
    #     torch.save(model.state_dict(), Save_model_file_name)
    #     print(f'Epoch [{epoch}] - Loss : [{train_loss}]')
    train_losses.append(train_loss)
    
    ######### test model
    if epoch % 2000 == 0 :
        model.eval() # eval mode
        total_test_loss = 0
        with torch.no_grad():  # 不計算梯度
            for inputs , targets in test_loader:
                inputs = inputs.to(device)
                targets = targets.to(device)
                Predict_CodeWord = model(inputs)
                ###### count multiple loss ######
                loss = criterion(Predict_CodeWord, targets)
                losses = loss
                #################################
                total_test_loss += losses.item()
                
                localtime = time.localtime()
                result = time.strftime("%I:%M %p", localtime)
                
            if len(test_losses)>0 and total_test_loss<min(test_losses):
                torch.save(model.state_dict(), Save_model_file_name)
                print(f'Test - Epoch [{epoch}] - Loss : [{total_test_loss}] - Time : {result}')
            test_losses.append(total_test_loss)

########################### Train Model   ###########################

########################### Plot Loss   ###########################
# 繪製訓練和驗證損失比較圖
plt.figure(figsize=(10, 5))
plt.plot(train_losses, label='Training Loss')
plt.plot(test_losses, label='Validation Loss')
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.title('Training and Validation Loss Over Epochs')
plt.legend()
plt.show()
########################### Plot Loss   ###########################

