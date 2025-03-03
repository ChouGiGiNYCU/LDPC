from collections import defaultdict
from read_PCM_G import *
from Gaussian_Noise import *
import torch
from torch.utils.data import DataLoader, TensorDataset
import numpy as np
import random
##################### Construct CN2VN #####################
def Construct_CN2VN(H_matrix):
    """ 返回cn2vn 的 矩陣，紀錄每一個cn node所連接到的vn nodes"""
    CN2VN_pos = []
    max_row_degree =0 
    for r in range(0,len(H_matrix)):
        tmp=[]
        for c in range(0,len(H_matrix[0])):
            if H_matrix[r][c]==1:
                tmp.append(c)
        max_row_degree = max(max_row_degree,len(tmp))
        CN2VN_pos.append(tmp)
    print("max_row_degree : ",max_row_degree)
    return CN2VN_pos
##################### Construct CN2VN #####################


##################### Construct VN2CN #####################
""" 返回vn2cn 的 矩陣，紀錄每一個vn node所連接到的cn nodes"""
def Construct_VN2CN(H_matrix):
    VN2CN_pos=[]
    max_col_degree=0
    for c in range(0,len(H_matrix[0])):
        tmp =[]
        for r in range(0,len(H_matrix)):
            if H_matrix[r][c]==1:
                tmp.append(r)
        VN2CN_pos.append(tmp)
        max_col_degree = max(max_col_degree,len(tmp))
    print("max_col_degree : ",max_col_degree)
    return VN2CN_pos
##################### Construct VN2CN #####################

############################ First layer mask (Input to Cn Update - Vns_to_Cn ) ############################
def Construct_FLM(CN2VN_pos,parity_check_matrix_colsize,hidden_layer_node):
    """
    建立 First Layer Mask for 對應的H matrix
    """
    connect_first_layer = []
    dict_update_cn = defaultdict()
    neural_num = 0
    for CN in range(0,len(CN2VN_pos)):
        for c in range(0,len(CN2VN_pos[CN])):
            # 建立cn0->vnx ... cn1->vnx ... 
            Vns_to_Cn = CN2VN_pos[CN][0:c] + CN2VN_pos[CN][c+1:len(CN2VN_pos[CN])]
            connect_first_layer.append(Vns_to_Cn)
            dict_update_cn[(CN,CN2VN_pos[CN][c])] = neural_num  # record cn0-> vn x 是第幾的neuron
            neural_num += 1
            # CN -> vnx(CN2VN_pos[CN][c])  所連接到的 vns
    if neural_num != hidden_layer_node:
        ValueError(f"First Layer mask neural number({neural_num}) is not equal to hidden layer number ({hidden_layer_node})")

    # construct first layer mask (CN update)
    first_layer_mask = [[False for i in range(parity_check_matrix_colsize)] for i in range(0,hidden_layer_node)]
    print("layer_mask rowsize is :",len(first_layer_mask)," , colsize : ",len(first_layer_mask[0]))
    for i in range(0,len(connect_first_layer)):
        for num in connect_first_layer[i]:
            first_layer_mask[i][num]=True
    return first_layer_mask,dict_update_cn
############################ First layer mask (Input to Cn Update - Vns_to_Cn ) ############################


############################ Second layer mask (CNs to VNi) ############################
# 這個方法以後可以改， 總之就是有一個紀錄第一層cn0->vn_small ... cn0->vn_big -> cn_big->vn0 -> .. cn_big->vn_big 的順序
# 利用這個順序來建立第二層 vn0->cn_small -> vn0->cn)big -> ....
def Construct_CN2VN_mask(VN2CN_pos,dict_update_cn,hidden_layer_node):
    """
    建立 cn2vn 的mask ， vn update layer 的mask
    """
    # vn0-> cnx .... vnx -> cnx
    dict_update_vn = defaultdict()
    neural_num = 0
    CN2VN_mask_VNUpdate = [[False]*hidden_layer_node for i in range(0,hidden_layer_node)]
    for VN in range(0,len(VN2CN_pos)):
        for idx in range(0,len(VN2CN_pos[VN])):
            Cns_to_Vn = VN2CN_pos[VN][0:idx] + VN2CN_pos[VN][idx+1:len(VN2CN_pos[VN])]
            dict_update_vn[(VN,VN2CN_pos[VN][idx])] = neural_num # ex : [vn0->cn0] : connect cn1->vn0 + cn2->vn0 
            if len(Cns_to_Vn)!=0:
                for prev_cn2vn in Cns_to_Vn:
                    prev_neural_idx = dict_update_cn.get((prev_cn2vn,VN),-1)
                    if prev_neural_idx==-1:
                        ValueError(f"Construct_CN2VN_mask Error , can't find cn{prev_cn2vn}->vn{VN} neural node !!")
                    CN2VN_mask_VNUpdate[neural_num][prev_neural_idx] = True
            neural_num += 1
    if neural_num != hidden_layer_node:
        print(f"First Layer mask neural number({neural_num}) is not equal to hidden layer number ({hidden_layer_node})")
        exit(1)
    return CN2VN_mask_VNUpdate,dict_update_vn
############################ Second layer mask (CNs to VNi) ############################



############################ Third layer mask (VNs to CNi) ############################
def Construct_VN2CN_mask(CN2VN_pos,dict_update_vn,hidden_layer_node):
    #constuct third mask of VNs->CNi update (record VN->CN node postion in neural network)
    #construct third mask(VN->CN)
    VN2CN_mask_CNUpdate = [[False]*hidden_layer_node for i in range(0,hidden_layer_node)]
    dict_update_cn = defaultdict()
    neural_num = 0
    for CN in range(0,len(CN2VN_pos)):
        for c in range(0,len(CN2VN_pos[CN])):
            # 建立cn0->vnx ... cn1->vnx ... 
            Vns_to_Cn = CN2VN_pos[CN][0:c] + CN2VN_pos[CN][c+1:len(CN2VN_pos[CN])]
            dict_update_cn[(CN,CN2VN_pos[CN][c])] = neural_num  # record cn0-> vn x 是第幾的neuron
            if len(Vns_to_Cn)!=0:
                for prev_vn2cn in Vns_to_Cn:
                    prev_neural_idx = dict_update_vn.get((prev_vn2cn,CN),-1)
                    if prev_neural_idx==-1:
                        ValueError(f"Construct_VN2CN_mask Error , can't find vn{prev_vn2cn}->cn{CN} neural node !!")
                    VN2CN_mask_CNUpdate[neural_num][prev_neural_idx] = True
            neural_num += 1
    if neural_num != hidden_layer_node:
        print(f"First Layer mask neural number({neural_num}) is not equal to hidden layer number ({hidden_layer_node})")
        exit(1)
    return VN2CN_mask_CNUpdate,dict_update_cn
    
############################ Third layer mask (VNs to CNi) ############################



############################ Final layer mask (CNs to output) ############################
def Construct_LLM(CN2VN_pos,hidden_layer_node,parity_check_matrix_colsize):
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
    
    # construct fouth final CN update to result
    CN2VN_mask_output = [[False]*hidden_layer_node for i in range(0,parity_check_matrix_colsize)]
    for VN in dict_cn_to_output.keys():
        for node in dict_cn_to_output[VN]:
            CN2VN_mask_output[VN][node] = True
    return CN2VN_mask_output
########################### Final layer mask (CNs to output) ############################
############################################################ Construct Every Layer Mask ############################################################

###########################  Dot_Bias_Matrix ###########################
def Construct_Dot_Bias_Matrix(VN2CN_pos,parity_check_matrix_colsize,hidden_layer_node):
    # 神經網路裡面的bias=[LLR1 ... LLR_VN]*matrix
    dot_bias_matrix=torch.zeros(parity_check_matrix_colsize ,hidden_layer_node)
    idx=0
    for i in range(0,len(VN2CN_pos)):
        for j in range(0,len(VN2CN_pos[i])):
            dot_bias_matrix[i][idx]=1
            idx = idx + 1
    return dot_bias_matrix
###########################  Dot_Bias_Matrix ###########################


###########################  Dot_Bias_Matrix ###########################
def Construct_test_set(SNR_MIN,SNR_MAX,CodeWord_length,CodeRate,Data_set_num,BATCH_SIZE,G_file=None):
    """返回一個test loader 用來評估模型現在的性能"""
    if G_file:
        G_matrix = read_G_file(G_file)
        G_rowsize = len(G_matrix)
        G_colsize = len(G_matrix[0])
    LLRs,EncodeCodeWords = [],[]
    for idx in range(Data_set_num):
        SNR = random.randint(SNR_MIN, SNR_MAX)
        sigma =(1/(2*CodeRate*(10**(SNR/10))))**0.5
        
        if G_file:
            information_bits = np.random.randint(0, 2, size=(1, G_rowsize))  # [0~2) 的隨機值 = 0、1
            EncodeCodeWord = np.dot(information_bits,G_matrix) % 2# (1 x G_rowsize) x (G_matrix)
        else :
            EncodeCodeWord = np.zeros(CodeWord_length, dtype=int)
        LLR = []
        for idx in range(len(EncodeCodeWord)):
            LLR.append(2*((-2*EncodeCodeWord[idx] + 1) + sigma*generate_gaussian_noise())/(sigma**2))
        
        EncodeCodeWord = np.where(EncodeCodeWord == 0, 1, 0)  # EncodeCodeWord=0 -> 1 ,else 0
        
        LLRs.append(LLR)
        EncodeCodeWords.append(EncodeCodeWord)
    # Convert LLRs and EncodeCodeWords to Tensors
    LLRs = np.squeeze(LLRs)
    EncodeCodeWords = np.squeeze(EncodeCodeWords)
    LLRs_tensor = torch.Tensor(np.array(LLRs))
    EncodeCodeWords_tensor = torch.Tensor(np.array(EncodeCodeWords))
    
    # Create a Dataset and DataLoader
    dataset = TensorDataset(LLRs_tensor, EncodeCodeWords_tensor)
    dataloader = DataLoader(dataset, batch_size=BATCH_SIZE, shuffle=False)  
    
    return dataloader