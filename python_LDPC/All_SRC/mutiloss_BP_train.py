import torch
from torch.utils.data import TensorDataset, DataLoader, random_split 
import torch.nn as nn 
import torch.optim as optim 
from tqdm import tqdm  # 進度條
import matplotlib.pyplot as plt   # 繪圖
from collections import defaultdict
import numpy as np

from Gaussian_Noise import *
from read_PCM_G import *
from sparse_bp_model import SparseBPNeuralNetwork
from DownLoad_weight import *
################# Some parameter setting #################
H_file_name = r"H_10_5.txt" # write parity_check_matrix to the txt file 
# G_file_name = r"H_10_5_G.txt"
G_file_name = None
Save_model_file_name = r"BPMTL_"
Load_model_file_name = r"BPMTL_17037_train.pth"
DownLoad_Weight_Flag = True
# Load_model_file_name = None
# DownLoad_Weight_Flag = False

learning_ratio = 0.001
EPOCHS = 100000
BATCH_SIZE = 10 
SNR_MIN = 0
SNR_MAX = 3
SNT_RATIO = 1
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
################# Some parameter setting #################
if G_file_name:
    G_matrix = read_G_file(G_file_name)
    G_rowsize = len(G_matrix)
    G_colsize = len(G_matrix[0])

H_matrix = read_H_file(H_file_name)
parity_check_matrix_colsize = len(H_matrix[0])
parity_check_matrix_rowsize = len(H_matrix)
# first hidden layer node number count 1 number in parity_matrix
hidden_layer_node = sum(sum(row) for row in H_matrix)
print("parity_check_matrix_rowsize : ",parity_check_matrix_rowsize)
print("parity_check_matrix_colsize : ",parity_check_matrix_colsize)
print("hidden_layer_node : ",hidden_layer_node)

# define model paramter
input_size = parity_check_matrix_colsize
hidden_layer_node = hidden_layer_node
output_size = parity_check_matrix_colsize

if torch.cuda.is_available():
    print("Now is use GPU to train")
else:
    print("Now is use CPU to train")

##################### Construct CN2VN #####################
CN2VN_pos = []
max_row_degree =0 
for r in range(0,len(H_matrix)):
    tmp=[]
    for c in range(0,len(H_matrix[0])):
        if H_matrix[r][c]==1:
            tmp.append(c)
    max_row_degree = max(max_row_degree,len(tmp))
    CN2VN_pos.append(tmp)
# print(CN2VN_pos)
# CN2VN_pos
print("max_row_degree : ",max_row_degree)
##################### Construct CN2VN #####################

##################### Construct VN2CN #####################
VN2CN=[]
max_col_degree=0
for c in range(0,len(H_matrix[0])):
    tmp =[]
    for r in range(0,len(H_matrix)):
        if H_matrix[r][c]==1:
            tmp.append(r)
    VN2CN.append(tmp)
    max_col_degree = max(max_col_degree,len(tmp))
# print(VN2CN)
print("max_col_degree : ",max_col_degree)
##################### Construct VN2CN #####################

############################################################ Construct Every Layer Mask ############################################################
############################ First layer mask (Input to Cn Update - Vns_to_Cn ) ############################
connect_first_layer = []
dict_update_cn = {}
dict_update_cn_arr = defaultdict(list)
dict_vn_pos = defaultdict(list)
for CN in range(0,len(CN2VN_pos)):
    for c in range(0,len(CN2VN_pos[CN])):
        Vns_to_Cn = CN2VN_pos[CN][0:c] + CN2VN_pos[CN][c+1:len(CN2VN_pos[CN])]
        connect_first_layer.append(Vns_to_Cn)
        dict_update_cn[tuple(Vns_to_Cn)]=(CN2VN_pos[CN][c],CN) # [VNs connect to CN ] - update(CN->CN2VN_pos[r][c]) (VN,CN)
        dict_update_cn_arr[(CN2VN_pos[CN][c],CN)].append(tuple(Vns_to_Cn))
        # print(dict_update_cn[tuple(Vns_to_Cn)], "-",tuple(Vns_to_Cn))

# construct first layer mask (CN update)
first_layer_mask = [[False for i in range(parity_check_matrix_colsize)] for i in range(0,hidden_layer_node)]
print("layer_mask rowsize is :",len(first_layer_mask)," , colsize : ",len(first_layer_mask[0]))
for i in range(0,len(connect_first_layer)):
    for num in connect_first_layer[i]:
        first_layer_mask[i][num]=True
first_layer_mask = first_layer_mask
############################ First layer mask (Input to Cn Update - Vns_to_Cn ) ############################

############################ Second layer mask (CNs to VNi) ############################
# save neural network node position ([connect to VNs]- node_position)
dict_connect_first_layer ={}
for idx,arr in enumerate(connect_first_layer):
    dict_connect_first_layer[tuple(arr)]=idx 
# 找出input到firsy_layer連接中，firsy_layer(VN update)的node哪些是CNx update此VN
for key in sorted(dict_update_cn_arr.keys(),key=lambda x:x[1]):
    VN = key[0]
    for arr in dict_update_cn_arr[key]:
        dict_vn_pos[VN].append(dict_connect_first_layer[arr])
# sort 
sorted_keys = sorted(dict_vn_pos.keys(),key=lambda x:x)
dict_vn_pos_sort = {key: dict_vn_pos[key] for key in sorted_keys}
dict_vn_pos=dict_vn_pos_sort.copy()
# show update key with node
# for key in dict_vn_pos.keys():
    # print(key,"-",dict_vn_pos[key])
# construct CN2VN mask - second layer connect table matrix(mask)
CN2VN_mask_VNUpdate = [[False]*hidden_layer_node for i in range(0,hidden_layer_node)]
idx=0
tmp=[]
for VN in dict_vn_pos.keys():
    if(len(dict_vn_pos[VN])==1):
        idx+=1
        tmp.append([VN2CN[VN][0],VN])
        continue
    for i,node1 in enumerate(dict_vn_pos[VN]):
        for node2 in dict_vn_pos[VN]:
            if node1!=node2:
                CN2VN_mask_VNUpdate[idx][node2]=True
        idx+=1
        tmp.append([VN2CN[VN][i],VN]) # CN,VN
true_indices_per_row = [[idx for idx, val in enumerate(row) if val] for row in CN2VN_mask_VNUpdate]
# for idx,arr in enumerate(true_indices_per_row):
#     print("neourn : {} - {} | VN {} -> CN {}".format(idx, arr, tmp[idx][1], tmp[idx][0]))
############################ Second layer mask (CNs to VNi) ############################

############################ Third layer mask (VNs to CNi) ############################
#constuct third mask of VNs->CNi update (record VN->CN node postion in neural network)
dict_vn_to_cn_node = defaultdict(list)
idx=0
for VN in dict_vn_pos.keys():
    for CN in VN2CN[VN]:
        dict_vn_to_cn_node[(VN,CN)] = idx
        idx+=1
# print("(VN update CN) - Network node idx")
# for key in dict_vn_to_cn_node.keys():
#     print(key,"-",dict_vn_to_cn_node[key])
#construct third mask(VN->CN)
VN2CN_mask_CNUpdate = [[False]*hidden_layer_node for i in range(0,hidden_layer_node)]
# 照著cn0->vn1 -> cn0->vn2 ...順序
idx=0
CN_2_VN_record=[]
for CN in range(0,parity_check_matrix_rowsize):
    #  CN->VN1 update
    for VN1 in  CN2VN_pos[CN]: 
        for VN2 in CN2VN_pos[CN]:
            if VN1!=VN2:
                # print(dict_vn_to_cn_node[(VN2,CN)])
                VN2CN_mask_CNUpdate[idx][dict_vn_to_cn_node[(VN2,CN)]]=True
        idx+=1
        CN_2_VN_record.append([CN,VN1])
true_indices_per_row = [[idx for idx, val in enumerate(row) if val] for row in VN2CN_mask_CNUpdate]
# for idx,arr in enumerate(true_indices_per_row):
#     # print("node : ",idx," - ",arr,"| CN",CN_2_VN_record[idx][0],"->","VN",CN_2_VN_record[idx][1])
#     print("neourn : {} - {} | CN {} -> VN {}".format(idx, arr, CN_2_VN_record[idx][0], CN_2_VN_record[idx][1]))
############################ Third layer mask (VNs to CNi) ############################

############################ Final layer mask (CNs to output) ############################
# construct fourth mask of  CN ->VN + channel LLR = output
dict_cn_to_output = defaultdict(list)
idx=0
for CN in range(0,len(CN2VN_pos)):
    for VN in CN2VN_pos[CN]:
        dict_cn_to_output[VN].append(idx)
        idx+=1
# sort 
sorted_keys = sorted(dict_cn_to_output.keys(),key=lambda x:x)
dict_cn_to_output_sort = {key: dict_cn_to_output[key] for key in sorted_keys}
dict_cn_to_output=dict_cn_to_output_sort.copy()
# for key in dict_cn_to_output.keys():
#     print(key,"-",dict_cn_to_output[key]) # 跟第一層一樣
# construct fouth final CN update to result
CN2VN_mask_output = [[False]*hidden_layer_node for i in range(0,parity_check_matrix_colsize)]
for VN in range(0,len(dict_cn_to_output)):
    for node in dict_cn_to_output[VN]:
        CN2VN_mask_output[VN][node] = True
    # print(CN2VN_mask_output[VN])
    
true_indices_per_row = [[idx for idx, val in enumerate(row) if val] for row in CN2VN_mask_output]
# for idx,arr in enumerate(true_indices_per_row):
#     print("neourn : {} - {} ".format(idx, arr))
############################ Final layer mask (CNs to output) ############################
############################################################ Construct Every Layer Mask ############################################################

###########################  Dot_Bias_Matrix ###########################
# 神經網路裡面的bias=[LLR1 ... LLR_VN]*matrix
dot_bias_matrix=torch.zeros(parity_check_matrix_colsize ,hidden_layer_node)
idx=0
for i in range(0,len(VN2CN)):
    for j in range(0,len(VN2CN[i])):
        dot_bias_matrix[i][idx]=1
        idx = idx + 1
# for i in dot_bias_matrix:
#     print(i)
###########################  Dot_Bias_Matrix ###########################
Channel_LLR_mask = torch.eye(parity_check_matrix_colsize)
# print(Channel_LLR_mask)


########################### Model Setting ###########################
model = SparseBPNeuralNetwork(input_size,hidden_layer_node,output_size,first_layer_mask,CN2VN_mask_VNUpdate,VN2CN_mask_CNUpdate,CN2VN_mask_output,dot_bias_matrix,Channel_LLR_mask,device)
model = model.to(device)
criterion = nn.BCELoss()
optimizer = optim.RMSprop(model.parameters(), lr=learning_ratio,alpha=0.9)
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
    
    LLRs = np.squeeze(LLRs)
    EncodeCodeWords = np.squeeze(EncodeCodeWords)
    LLRs = torch.Tensor(np.array(LLRs)).to(device)
    EncodeCodeWords = torch.Tensor(np.array(EncodeCodeWords)).to(device)
    ############
    optimizer.zero_grad()       # clear grad
    inputs = LLRs
    labels = EncodeCodeWords
    out5,out4,out3,out2,out1 = model(inputs)
    ###### count multiple loss ######
    loss1 = criterion(out1, labels)
    loss2 = criterion(out2, labels)
    loss3 = criterion(out3, labels)
    loss4 = criterion(out4, labels)
    loss5 = criterion(out5, labels)
    losses = loss1+loss2+loss3+loss4+loss5
    #################################
    losses.backward()             # backward
    model.apply_sparse_masks()  
    optimizer.step()            # update parameter
    train_loss = losses.item()
    
    if len(train_losses)>0 and train_loss<min(train_losses):
        torch.save(model.state_dict(), Save_model_file_name+"{}_train.pth".format(epoch))
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

