import torch
from torch.utils.data import TensorDataset, DataLoader, random_split
import torch.nn as nn
import torch.optim as optim
from tqdm import tqdm  # 進度條
import matplotlib.pyplot as plt  # 繪圖
from UseFunction import *
from collections import defaultdict
from sparse_bp_model import SparseBPNeuralNetwork
from parity_check_matrix import *

import numpy as np
import csv
import random

# 生成隨機的 True 或 False
random_bool = random.choice([True, False])
################# Some parameter setting #################
file_name ="H_96_48.txt"
################# Some parameter setting #################

# parity_check_matrix = get_parity_check_matrix()
parity_check_matrix = read_H_file(file_name)
parity_check_matrix_colsize = len(parity_check_matrix[0])
parity_check_matrix_rowsize = len(parity_check_matrix)
# first hidden layer node number count 1 number in parity_matrix
hidden_layer_node = sum(sum(row) for row in parity_check_matrix)
print("parity_check_matrix_rowsize : ",parity_check_matrix_rowsize)
print("parity_check_matrix_colsize : ",parity_check_matrix_colsize)
print("hidden_layer_node : ",hidden_layer_node)

# define model paramter
input_size = parity_check_matrix_colsize
hidden_layer_node = hidden_layer_node
output_size = parity_check_matrix_colsize

if torch.cuda.is_available():
    device = torch.device("cuda")
    print(f"GPU is available. Device: {torch.cuda.get_device_name(device)}")
else:
    device = torch.device("cpu")
    print("GPU not available, using CPU instead.")


##################### Construct CN2VN #####################
CN2VN_pos = []
max_row_degree =0 
for r in range(0,len(parity_check_matrix)):
    tmp=[]
    for c in range(0,len(parity_check_matrix[0])):
        if parity_check_matrix[r][c]==1:
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
for c in range(0,len(parity_check_matrix[0])):
    tmp =[]
    for r in range(0,len(parity_check_matrix)):
        if parity_check_matrix[r][c]==1:
            tmp.append(r)
    VN2CN.append(tmp)
    max_col_degree = max(max_col_degree,len(tmp))
# print(VN2CN)
print("max_col_degree : ",max_col_degree)
##################### Construct VN2CN #####################

##################### Write parity_check_matrix to txt  #####################
# Write_txt_File(file_name, parity_check_matrix_colsize, parity_check_matrix_rowsize, max_col_degree, max_row_degree, VN2CN, CN2VN_pos)
##################### Write parity_check_matrix to txt  #####################

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
first_layer_mask = [[False]*parity_check_matrix_colsize for i in range(0,hidden_layer_node)]
print("layer_mask rowsize is :",len(first_layer_mask)," , colsize : ",len(first_layer_mask[0]))
for i in range(0,len(connect_first_layer)):
    for num in connect_first_layer[i]:
        first_layer_mask[i][num]=True
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


########################### Model Setting ###########################
model = SparseBPNeuralNetwork(input_size,hidden_layer_node,output_size,first_layer_mask,CN2VN_mask_VNUpdate,VN2CN_mask_CNUpdate,CN2VN_mask_output,dot_bias_matrix)
########################### Model Setting ###########################

model.eval()
code_rate = (parity_check_matrix_colsize - parity_check_matrix_rowsize)/parity_check_matrix_colsize
receiver_LLR = []
with open("test_LLR.txt",'r') as file:
    data = file.read()
    for value in  data.split():
        receiver_LLR.append(float(value))

############## list to tensor ##############
receiver_LLR_tensor = torch.tensor(receiver_LLR, dtype=torch.float32).unsqueeze(0)
############## list to tensor ##############
with torch.no_grad():
    decode_codeword_tensor = model(receiver_LLR_tensor)

decode_codeword = decode_codeword_tensor[0].squeeze().float().tolist()  # 確保 decode_codeword 是整數列表
print(decode_codeword)



        


