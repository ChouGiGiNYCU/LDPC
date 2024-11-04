import torch
from torch.utils.data import TensorDataset, DataLoader, random_split
import torch.nn as nn
import torch.optim as optim
from tqdm import tqdm  # 進度條
import matplotlib.pyplot as plt  # 繪圖
from UseFunction import *
from collections import defaultdict
from parity_check_matrix import *
from sparse_bp_model import SparseBPNeuralNetwork
################# Some parameter setting #################
file_name = r"D:\BP_Model\LDPC_128_64\Train_Set\H_128_64.txt" # write parity_check_matrix to the txt file 
train_data_file_name = r"D:\BP_Model\LDPC_128_64\Train_Set\H_128_64_Valid_LLR_2\H_128_64_Train_LLR_2.txt"
train_data_target_file_name = r"D:\BP_Model\LDPC_128_64\Train_Set\H_128_64_Valid_LLR_2\H_128_64_Train_CodeWord_2.txt"
valid_data_file_name = r"D:\BP_Model\LDPC_128_64\Train_Set\H_128_64_Valid_LLR_2\H_128_64_Valid_LLR_2.txt"
valid_data_target_file_name = r"D:\BP_Model\LDPC_128_64\Train_Set\H_128_64_Valid_LLR_2\H_128_64_Valid_CodeWord_2.txt"
Save_model_file_name = "BP_NN2_LDPC_128_64_"
Load_model_file_name = None
Load_setting = False
learning_ratio = 0.001
epochs = 40
batch_size = 128 # (0~5) *50 
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

if(torch.cpu.is_available()):
    print("Now use is CPU to train")
train_data = read_data(train_data_file_name)
print("data size is : {}".format(len(train_data)))
valid_data = read_data(valid_data_file_name)
print("valid_data size is : {}".format(len(valid_data)))

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


########################### Train Set ###########################
train_targets = [[1] * len(train_data[0]) ] * len(train_data) # 根據 train_data 的行數創建對應大小的 targets
valid_targets = [[1] * len(valid_data[0]) ] * len(valid_data) # 根據 valid_data 的行數創建對應大小的 targets
# 創建完整的數據集 data->tensor
train_data_tensor = torch.tensor(train_data, dtype=torch.float32)
valid_data_tensor = torch.tensor(valid_data, dtype=torch.float32)
train_targets_tensor = torch.tensor(train_targets, dtype=torch.float32)
valid_targets_tensor = torch.tensor(valid_targets, dtype=torch.float32)
train_dataset = TensorDataset(train_data_tensor, train_targets_tensor)
valid_dataset =  TensorDataset(valid_data_tensor, valid_targets_tensor)
# 創建 DataLoader
train_dataloader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
valid_dataloader = DataLoader(valid_dataset, batch_size=batch_size, shuffle=True)
print("Number of training samples:", len(train_dataset))
print("Number of validation samples:", len(valid_dataset))
########################### Train Set ###########################

########################### Model Setting ###########################
model = SparseBPNeuralNetwork(input_size,hidden_layer_node,output_size,first_layer_mask,CN2VN_mask_VNUpdate,VN2CN_mask_CNUpdate,CN2VN_mask_output,dot_bias_matrix)
criterion = nn.BCELoss()
optimizer = optim.RMSprop(model.parameters(), lr=learning_ratio,alpha=0.9)
train_losses = []
valid_losses = []
if Load_setting==True:
    model.load_state_dict(torch.load(Load_model_file_name))
########################### Model Setting ###########################
########################### Train Model   ###########################
for epoch in range(epochs):
    model.train()  # train mode
    total_loss = 0
    train_progress = tqdm(train_dataloader, desc=f"Epoch {epoch+1}/{epochs} - Training")
    for inputs, labels in train_progress:
        optimizer.zero_grad()       # clear grad
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
        total_loss += losses.item()
        train_progress.set_postfix(loss=losses.item())
    
    average_train_loss = total_loss / len(train_dataloader)
    if len(train_losses)>0 and average_train_loss<min(train_losses):
        torch.save(model.state_dict(), Save_model_file_name+"{}_train.pth".format(epoch))
    train_losses.append(average_train_loss)
    # 驗證階段
    model.eval()
    total_val_loss = 0
    valid_progress = tqdm(valid_dataloader, desc=f"Epoch {epoch+1}/{epochs} - Validation")
    with torch.no_grad():  # 不計算梯度
        for inputs, labels in valid_progress:
            out5,out4,out3,out2,out1 = model(inputs)
            ###### count multiple loss ######
            loss1 = criterion(out1, labels)
            loss2 = criterion(out2, labels)
            loss3 = criterion(out3, labels)
            loss4 = criterion(out4, labels)
            loss5 = criterion(out5, labels)
            losses = loss1+loss2+loss3+loss4+loss5
            # losses = loss5
            #################################
            total_val_loss += losses.item()
            valid_progress.set_postfix(loss=losses.item())
    
    average_valid_loss = total_val_loss / len(valid_dataloader)
    if len(valid_losses)>0 and average_valid_loss<min(valid_losses):
        torch.save(model.state_dict(), Save_model_file_name+"{}_valid.pth".format(epoch))

    valid_losses.append(average_valid_loss)
    print(f"Epoch {epoch+1}/{epochs} --- Average Train Loss: {average_train_loss:.4f} --- Average Valid Loss: {average_valid_loss:.4f}")
    
    ########################### Save Model  ###########################
    # if epoch%4==0:
    #     torch.save(model.state_dict(), Save_model_file_name+"{}".format(epoch)+".pth")
    ########################### Save Model  ###########################
    

# 打印最終的訓練和驗證損失
# print("Final training loss: {}".format(train_losses[-1]))
# print("Final validation loss: {}".format(valid_losses[-1]))
########################### Train Model   ###########################

########################### Plot Loss   ###########################
# 繪製訓練和驗證損失比較圖
plt.figure(figsize=(10, 5))
plt.plot(train_losses, label='Training Loss')
plt.plot(valid_losses, label='Validation Loss')
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.title('Training and Validation Loss Over Epochs')
plt.legend()
plt.show()
########################### Plot Loss   ###########################

