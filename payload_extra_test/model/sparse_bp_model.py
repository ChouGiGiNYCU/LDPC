import torch
import torch.nn as nn
class Mutiloss_Final_Layer(nn.Module):
    def __init__(self, in_features, out_features, layer_mask,device):
        super(Mutiloss_Final_Layer, self).__init__()
        self.weight = nn.Parameter(torch.Tensor(out_features, in_features))
        self.bias = nn.Parameter(torch.Tensor(out_features))
        self.layer_mask = torch.tensor(layer_mask, dtype=torch.float32)  # Convert to tensor
        self.reset_parameters()
        self.to(device)
    def reset_parameters(self):
        nn.init.ones_(self.weight)
        nn.init.zeros_(self.bias)
        self.bias.requires_grad = False  # 保持bias不参与梯度更新
        self.weight.requires_grad = False
        if self.layer_mask is not None:
            layer_mask_device = self.layer_mask.to(self.weight.device)
            self.weight.data *= layer_mask_device
            # self.weight.data *= self.layer_mask
    def forward(self, x):
        return torch.matmul(x, self.weight.t()) + self.bias
     

class VnUpdateLayer(nn.Module): 
    def __init__(self, in_features, out_features, layer_mask, device):
        super(VnUpdateLayer, self).__init__()
        # 不要在这里调用 .to(device)
        self.weight = nn.Parameter(torch.Tensor(out_features, in_features))
        self.bias = nn.Parameter(torch.Tensor(out_features))
        self.layer_mask = torch.tensor(layer_mask, dtype=torch.float32)
        self.reset_parameters()
        self.to(device)  # 在这里移动整个层到设备上
    def reset_parameters(self):
        # nn.init.ones_(self.weight)
        nn.init.xavier_uniform_(self.weight)
        self.weight.data *= self.layer_mask
        nn.init.zeros_(self.bias)
        self.bias.requires_grad = False  # 保持bias不参与梯度更新
    def forward(self, x):
        return torch.matmul(x, self.weight.t())
    def apply_sparse_mask(self):
        if self.weight.grad is not None:
            layer_mask_device = self.layer_mask.to(self.weight.grad.device)
            self.weight.grad.data *= layer_mask_device
            # self.weight.grad.data *= self.layer_mask

# CN update Layer (Dont need update paramter or weight)
class CnUpdateLayer(nn.Module): # apply dot
    def __init__(self, in_features, out_features,layer_mask,device):
        super(CnUpdateLayer, self).__init__()
        self.weight = nn.Parameter(torch.Tensor(out_features, in_features))
        self.bias = nn.Parameter(torch.Tensor(out_features))
        self.out_features = out_features
        self.layer_mask = torch.tensor(layer_mask, dtype=torch.float32)  # Convert to tensor
          # 将 weight 的 requires_grad 设置为 False
        self.reset_parameters()
        self.to(device)
    def reset_parameters(self):
        nn.init.ones_(self.weight) # 初始化權重 
        nn.init.zeros_(self.bias)
        self.bias.requires_grad = False
        self.weight.requires_grad = False
        if self.layer_mask is not None:
            layer_mask_device = self.layer_mask.to(self.weight.device)
            self.weight.data *= layer_mask_device
            # self.weight.data *= self.layer_mask

    def forward(self, x):
        # 应用掩码并处理每个输出神经元
        batch_size = x.size(0)
        output = torch.zeros(batch_size, self.out_features, device=x.device)
        for i in range(self.out_features):
            # 每個nerual 的mask
            mask = self.layer_mask[i].to(x.device)
            x_masked = x * mask  # 掩码应用于输入数据
            # 將0替換成1，避免乘起來是零
            x_nonzero = torch.where(x_masked != 0, x_masked, torch.ones_like(x_masked))
            # 計算連乘法
            product = torch.prod(x_nonzero, dim=1)
            # 检查全零行，将这些行的乘积结果置为1，避免影响后续的双曲正切反函数
            non_zero_prod = torch.where((x_masked != 0).any(dim=1), product, torch.zeros_like(product))
            # 计算双曲正切反函数并乘以2
            output[:, i] = non_zero_prod
        
        return output
    def apply_sparse_mask(self):
        pass
        
            
class SparseBPNeuralNetwork(nn.Module):
    def __init__(self, input_size, output_size,hidden_size1,first_layer_mask1,CN2VN_mask_VNUpdate1,VN2CN_mask_CNUpdate1,CN2VN_mask_output1,Dot_bias_matrix1,Channel_LLR_mask1,hidden_size2,first_layer_mask2,CN2VN_mask_VNUpdate2,VN2CN_mask_CNUpdate2,CN2VN_mask_output2,Dot_bias_matrix2,Channel_LLR_mask2,device):
        super(SparseBPNeuralNetwork, self).__init__()
        # self.count_loss_fc = Mutiloss_Final_Layer(hidden_size,output_size,CN2VN_mask_output,device).to(device) # count loss for each hidden layer(VnUpdateLayer)
        self.bias_matrix1 = Dot_bias_matrix1.to(device)
        self.bias_matrix2 = Dot_bias_matrix2.to(device)
        self.Channel_LLR_mask1 = Channel_LLR_mask1.to(device)
        self.Channel_LLR_mask2 = Channel_LLR_mask2.to(device)
        # H1 - 1
        self.fc0 = CnUpdateLayer(input_size, hidden_size1,first_layer_mask1,device).to(device) # CN update
        self.fc1 = VnUpdateLayer(hidden_size1, hidden_size1,CN2VN_mask_VNUpdate1,device).to(device) # VN update (i=odd)
        self.fc2 = CnUpdateLayer(hidden_size1, hidden_size1,VN2CN_mask_CNUpdate1,device).to(device) # CN update (i=even)
        self.fc3 = VnUpdateLayer(hidden_size1, output_size,CN2VN_mask_output1,device).to(device) # Total LLR 
        # H2 - 1
        self.fc4 = CnUpdateLayer(input_size, hidden_size2,first_layer_mask2,device).to(device) # CN update
        self.fc5 = VnUpdateLayer(hidden_size2, hidden_size2,CN2VN_mask_VNUpdate2,device).to(device) # VN update (i=odd)
        self.fc6 = CnUpdateLayer(hidden_size2, hidden_size2,VN2CN_mask_CNUpdate2,device).to(device) # CN update (i=even)
        self.fc7 = VnUpdateLayer(hidden_size2, output_size,CN2VN_mask_output2,device).to(device) # Total LLR 
        # H1 - 2
        self.fc8 = CnUpdateLayer(input_size, hidden_size1,first_layer_mask1,device).to(device) # CN update
        self.fc9 = VnUpdateLayer(hidden_size1, hidden_size1,CN2VN_mask_VNUpdate1,device).to(device) # VN update (i=odd)
        self.fc10 = CnUpdateLayer(hidden_size1, hidden_size1,VN2CN_mask_CNUpdate1,device).to(device) # CN update (i=even)
        self.fc11 = VnUpdateLayer(hidden_size1, output_size,CN2VN_mask_output1,device).to(device) # Total LLR 
        # H2 - 2
        self.fc12 = CnUpdateLayer(input_size, hidden_size2,first_layer_mask2,device).to(device) # CN update
        self.fc13 = VnUpdateLayer(hidden_size2, hidden_size2,CN2VN_mask_VNUpdate2,device).to(device) # VN update (i=odd)
        self.fc14 = CnUpdateLayer(hidden_size2, hidden_size2,VN2CN_mask_CNUpdate2,device).to(device) # CN update (i=even)
        self.fc15 = VnUpdateLayer(hidden_size2, output_size,CN2VN_mask_output2,device).to(device) # Total LLR 
        # H1 - 3
        self.fc16 = CnUpdateLayer(input_size, hidden_size1,first_layer_mask1,device).to(device) # CN update
        self.fc17 = VnUpdateLayer(hidden_size1, hidden_size1,CN2VN_mask_VNUpdate1,device).to(device) # VN update (i=odd)
        self.fc18 = CnUpdateLayer(hidden_size1, hidden_size1,VN2CN_mask_CNUpdate1,device).to(device) # CN update (i=even)
        self.fc19 = VnUpdateLayer(hidden_size1, output_size,CN2VN_mask_output1,device).to(device) # Total LLR 
        # H2 - 3
        self.fc20 = CnUpdateLayer(input_size, hidden_size2,first_layer_mask2,device).to(device) # CN update
        self.fc21 = VnUpdateLayer(hidden_size2, hidden_size2,CN2VN_mask_VNUpdate2,device).to(device) # VN update (i=odd)
        self.fc22 = CnUpdateLayer(hidden_size2, hidden_size2,VN2CN_mask_CNUpdate2,device).to(device) # CN update (i=even)
        self.fc23 = VnUpdateLayer(hidden_size2, output_size,CN2VN_mask_output2,device).to(device) # Total LLR 
        # H1 - 4
        self.fc24 = CnUpdateLayer(input_size, hidden_size1,first_layer_mask1,device).to(device) # CN update
        self.fc25 = VnUpdateLayer(hidden_size1, hidden_size1,CN2VN_mask_VNUpdate1,device).to(device) # VN update (i=odd)
        self.fc26 = CnUpdateLayer(hidden_size1, hidden_size1,VN2CN_mask_CNUpdate1,device).to(device) # CN update (i=even)
        self.fc27 = VnUpdateLayer(hidden_size1, output_size,CN2VN_mask_output1,device).to(device) # Total LLR 
        # H2 - 4
        self.fc28 = CnUpdateLayer(input_size, hidden_size2,first_layer_mask2,device).to(device) # CN update
        self.fc29 = VnUpdateLayer(hidden_size2, hidden_size2,CN2VN_mask_VNUpdate2,device).to(device) # VN update (i=odd)
        self.fc30 = CnUpdateLayer(hidden_size2, hidden_size2,VN2CN_mask_CNUpdate2,device).to(device) # CN update (i=even)
        self.fc31 = VnUpdateLayer(hidden_size2, output_size,CN2VN_mask_output2,device).to(device) # Total LLR 
        # H1 - 5
        self.fc32 = CnUpdateLayer(input_size, hidden_size1,first_layer_mask1,device).to(device) # CN update
        self.fc33 = VnUpdateLayer(hidden_size1, hidden_size1,CN2VN_mask_VNUpdate1,device).to(device) # VN update (i=odd)
        self.fc34 = CnUpdateLayer(hidden_size1, hidden_size1,VN2CN_mask_CNUpdate1,device).to(device) # CN update (i=even)
        self.fc35 = VnUpdateLayer(hidden_size1, output_size,CN2VN_mask_output1,device).to(device) # Total LLR 
        # H2 - 5
        self.fc36 = CnUpdateLayer(input_size, hidden_size2,first_layer_mask2,device).to(device) # CN update
        self.fc37 = VnUpdateLayer(hidden_size2, hidden_size2,CN2VN_mask_VNUpdate2,device).to(device) # VN update (i=odd)
        self.fc38 = CnUpdateLayer(hidden_size2, hidden_size2,VN2CN_mask_CNUpdate2,device).to(device) # CN update (i=even)
        self.fc39 = VnUpdateLayer(hidden_size2, output_size,CN2VN_mask_output2,device).to(device) # Total LLR 

        # self.fc3 = VnUpdateLayer(hidden_size, hidden_size,CN2VN_mask_VNUpdate,device).to(device)# VN update (i=odd)
        # self.fc4 = CnUpdateLayer(hidden_size, hidden_size,VN2CN_mask_CNUpdate,device).to(device) # CN update (i=even)
        # self.fc5 = VnUpdateLayer(hidden_size, hidden_size,CN2VN_mask_VNUpdate,device).to(device) # VN update (i=odd)
        # self.fc6 = CnUpdateLayer(hidden_size, hidden_size,VN2CN_mask_CNUpdate,device).to(device) # CN update (i=even)
        # self.fc7 = VnUpdateLayer(hidden_size, hidden_size,CN2VN_mask_VNUpdate,device).to(device) # VN update (i=odd)
        # self.fc8 = CnUpdateLayer(hidden_size, hidden_size,VN2CN_mask_CNUpdate,device).to(device) # CN update (i=even)
        # self.fc9 = VnUpdateLayer(hidden_size, output_size,CN2VN_mask_output,device).to(device) # Total LLR 
        
        self.sigmoid = nn.Sigmoid()
        self.tanh = nn.Tanh()
        self.bias_scaling_vectors = nn.ParameterList([
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H1-1
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H2-1
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H1-2
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H2-2
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H1-3
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H2-3
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H1-4
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H2-4
            nn.Parameter(torch.eye(input_size), requires_grad=True), #H1-5
            nn.Parameter(torch.eye(input_size), requires_grad=True)  #H2-5
        ]).to(device)

    def forward(self, x):
        
        ##########################################################
        # H1-1
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc0(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc1(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[0]),self.bias_matrix1))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc2(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc3(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[0])
        totalLLR1 = self.sigmoid(x.clone()) 
        ##########################################################
        # H2-1
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc4(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc5(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[1]),self.bias_matrix2))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc6(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc7(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[1])
        totalLLR2 = self.sigmoid(x.clone()) 
        ##########################################################
        # H1 - 2
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc8(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc9(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[2]),self.bias_matrix1))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc10(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc11(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[2])
        totalLLR3 = self.sigmoid(x.clone()) 
        ##########################################################
        # H2-2
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc12(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc13(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[3]),self.bias_matrix2))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc14(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc15(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[3])
        totalLLR4 = self.sigmoid(x.clone()) 
        ##########################################################
        # H1 - 3
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc16(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc17(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[4]),self.bias_matrix1))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc18(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc19(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[4])
        totalLLR5 = self.sigmoid(x.clone()) 
        ##########################################################
        # H2 - 3
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc20(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc21(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[5]),self.bias_matrix2))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc22(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc23(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[5])
        totalLLR6 = self.sigmoid(x.clone()) 
        ##########################################################
        # H1 - 4
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc24(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc25(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[6]),self.bias_matrix1))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc26(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc27(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[6])
        totalLLR7 = self.sigmoid(x.clone()) 
        ##########################################################
        # H2 - 4
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc28(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc29(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[7]),self.bias_matrix2))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc30(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc31(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[7])
        totalLLR8 = self.sigmoid(x.clone()) 
        ##########################################################
        # H1 - 5
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc32(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc33(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[8]),self.bias_matrix1))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc34(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc35(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[8])
        totalLLR9 = self.sigmoid(x.clone()) 
        ##########################################################
        # H2 - 5
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc36(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc37(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[9]),self.bias_matrix2))) 
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc38(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # total LLR (i=3)
        x = self.fc39(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[9])
        totalLLR10 = self.sigmoid(x)
        ##########################################################
        return totalLLR10,totalLLR9,totalLLR8,totalLLR7,totalLLR6,totalLLR5,totalLLR4,totalLLR3,totalLLR2,totalLLR1

    def apply_sparse_masks(self):
        self.fc1.apply_sparse_mask()
        self.fc3.apply_sparse_mask()
        self.fc5.apply_sparse_mask()
        self.fc7.apply_sparse_mask()
        self.fc9.apply_sparse_mask()
        self.fc11.apply_sparse_mask()
        self.fc13.apply_sparse_mask()
        self.fc15.apply_sparse_mask()
        self.fc17.apply_sparse_mask()
        self.fc19.apply_sparse_mask()
        self.fc21.apply_sparse_mask()
        self.fc23.apply_sparse_mask()
        self.fc25.apply_sparse_mask()
        self.fc27.apply_sparse_mask()
        self.fc29.apply_sparse_mask()
        self.fc31.apply_sparse_mask()
        self.fc33.apply_sparse_mask()
        self.fc35.apply_sparse_mask()
        self.fc37.apply_sparse_mask()
        self.fc39.apply_sparse_mask()

        if self.bias_scaling_vectors[0].grad is not None: self.bias_scaling_vectors[0].grad.data *= self.Channel_LLR_mask1
        if self.bias_scaling_vectors[1].grad is not None: self.bias_scaling_vectors[1].grad.data *= self.Channel_LLR_mask2
        if self.bias_scaling_vectors[2].grad is not None: self.bias_scaling_vectors[2].grad.data *= self.Channel_LLR_mask1
        if self.bias_scaling_vectors[3].grad is not None: self.bias_scaling_vectors[3].grad.data *= self.Channel_LLR_mask2
        if self.bias_scaling_vectors[4].grad is not None: self.bias_scaling_vectors[4].grad.data *= self.Channel_LLR_mask1
        if self.bias_scaling_vectors[5].grad is not None: self.bias_scaling_vectors[5].grad.data *= self.Channel_LLR_mask2
        if self.bias_scaling_vectors[6].grad is not None: self.bias_scaling_vectors[6].grad.data *= self.Channel_LLR_mask1
        if self.bias_scaling_vectors[7].grad is not None: self.bias_scaling_vectors[7].grad.data *= self.Channel_LLR_mask2
        if self.bias_scaling_vectors[8].grad is not None: self.bias_scaling_vectors[8].grad.data *= self.Channel_LLR_mask1
        if self.bias_scaling_vectors[9].grad is not None: self.bias_scaling_vectors[9].grad.data *= self.Channel_LLR_mask2
############################################## Model #############################################   