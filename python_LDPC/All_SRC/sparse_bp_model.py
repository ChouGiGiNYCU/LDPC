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
            # 应用每个输出神经元的掩码
            mask = self.layer_mask[i].to(x.device)
            x_masked = x * mask  # 掩码应用于输入数据
            # 将零值替换为一，以避免影响乘积
            x_nonzero = torch.where(x_masked != 0, x_masked, torch.ones_like(x_masked))
            # 计算掩码后数据的乘积
            product = torch.prod(x_nonzero, dim=1)
            # 检查全零行，将这些行的乘积结果置为1，避免影响后续的双曲正切反函数
            non_zero_prod = torch.where((x_masked != 0).any(dim=1), product, torch.zeros_like(product))
            # 计算双曲正切反函数并乘以2
            output[:, i] = non_zero_prod
        
        return output
    def apply_sparse_mask(self):
        pass
        
            
class SparseBPNeuralNetwork(nn.Module):
    def __init__(self, input_size, hidden_size, output_size,first_layer_mask,CN2VN_mask_VNUpdate,VN2CN_mask_CNUpdate,CN2VN_mask_output,dot_bias_matrix,Channel_LLR_mask,device):
        super(SparseBPNeuralNetwork, self).__init__()
        self.count_loss_fc = Mutiloss_Final_Layer(hidden_size,output_size,CN2VN_mask_output,device).to(device) # count loss for each hidden layer(VnUpdateLayer)
        self.fc0 = CnUpdateLayer(input_size, hidden_size,first_layer_mask,device).to(device) # CN update
        self.fc1 = VnUpdateLayer(hidden_size, hidden_size,CN2VN_mask_VNUpdate,device).to(device) # VN update (i=odd)
        self.fc2 = CnUpdateLayer(hidden_size, hidden_size,VN2CN_mask_CNUpdate,device).to(device) # CN update (i=even)
        self.fc3 = VnUpdateLayer(hidden_size, hidden_size,CN2VN_mask_VNUpdate,device).to(device)# VN update (i=odd)
        self.fc4 = CnUpdateLayer(hidden_size, hidden_size,VN2CN_mask_CNUpdate,device).to(device) # CN update (i=even)
        self.fc5 = VnUpdateLayer(hidden_size, hidden_size,CN2VN_mask_VNUpdate,device).to(device) # VN update (i=odd)
        self.fc6 = CnUpdateLayer(hidden_size, hidden_size,VN2CN_mask_CNUpdate,device).to(device) # CN update (i=even)
        self.fc7 = VnUpdateLayer(hidden_size, hidden_size,CN2VN_mask_VNUpdate,device).to(device) # VN update (i=odd)
        self.fc8 = CnUpdateLayer(hidden_size, hidden_size,VN2CN_mask_CNUpdate,device).to(device) # CN update (i=even)
        self.fc9 = VnUpdateLayer(hidden_size, output_size,CN2VN_mask_output,device).to(device) # Total LLR 
        self.bias_matrix = dot_bias_matrix
        self.Channel_LLR_mask = Channel_LLR_mask.to(device)
        self.bias_matrix = self.bias_matrix.to(device)
        self.sigmoid = nn.Sigmoid()
        self.tanh = nn.Tanh()
        self.bias_scaling_vectors = nn.ParameterList([
            nn.Parameter(torch.eye(input_size), requires_grad=True),
            nn.Parameter(torch.eye(input_size), requires_grad=True),
            nn.Parameter(torch.eye(input_size), requires_grad=True),
            nn.Parameter(torch.eye(input_size), requires_grad=True),
            nn.Parameter(torch.eye(output_size), requires_grad=True)
        ]).to(device)

    def forward(self, x):
        # first layer of input to CN update
        Channel_LLR = x.clone()
        Channel_LLR = Channel_LLR.to(x.device)
        ##########################################################
        # input to CNUpdate (i=0)
        x = self.tanh(0.5 * x)
        x = torch.clamp(self.fc0(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # print(x)
        out1 = self.sigmoid(self.count_loss_fc(x)+Channel_LLR)
        ##########################################################
        # CN->VN VNUpdate (i=1)
        x = self.tanh(0.5*(self.fc1(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[0]),self.bias_matrix))) 
        # print(x)
        ##########################################################
        # VN->CN CNUpdate (i=2)
        x = torch.clamp(self.fc2(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # print(x)
        out2 = self.sigmoid(self.count_loss_fc(x)+Channel_LLR)
        ##########################################################
        # CN->VN VNUpdate (i=3)
        x = self.tanh(0.5*(self.fc3(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[1]),self.bias_matrix)))
        # print(x)
        ##########################################################
        # VN->CN CNUpdate (i=4)
        x = torch.clamp(self.fc4(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # print(x)
        out3 = self.sigmoid(self.count_loss_fc(x)+Channel_LLR)
        ##########################################################
        # CN->VN VNUpdate (i=5)
        x = self.tanh(0.5*(self.fc5(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[2]),self.bias_matrix))) 
        # print(x)
        ##########################################################
        # VN->CN CNUpdate (i=6)
        x = torch.clamp(self.fc6(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # print(x)
        out4 = self.sigmoid(self.count_loss_fc(x)+Channel_LLR)
        ##########################################################
        # CN->VN VNUpdate (i=7)
        x = self.tanh(0.5*(self.fc7(x)+torch.matmul(torch.matmul(Channel_LLR,self.bias_scaling_vectors[3]),self.bias_matrix))) 
        # print(x)
        ##########################################################
        # VN->CN CNUpdate (i=8)
        x = torch.clamp(self.fc8(x), min=-0.999999, max=0.999999)
        x = 2 * torch.atanh(x)
        # print(x)
        ##########################################################
        # output VN (i=9)
        x = self.fc9(x)+torch.matmul(Channel_LLR,self.bias_scaling_vectors[4])
        out5 = self.sigmoid(x)
        return out5,out4,out3,out2,out1

    def apply_sparse_masks(self):
        self.fc1.apply_sparse_mask()
        self.fc3.apply_sparse_mask()
        self.fc5.apply_sparse_mask()
        self.fc7.apply_sparse_mask()
        self.fc9.apply_sparse_mask()
        
        
        if self.bias_scaling_vectors[0].grad is not None:
            self.bias_scaling_vectors[0].grad.data *= self.Channel_LLR_mask
        if self.bias_scaling_vectors[1].grad is not None:
            self.bias_scaling_vectors[1].grad.data *= self.Channel_LLR_mask
        if self.bias_scaling_vectors[2].grad is not None:
            self.bias_scaling_vectors[2].grad.data *= self.Channel_LLR_mask
        if self.bias_scaling_vectors[3].grad is not None:
            self.bias_scaling_vectors[3].grad.data *= self.Channel_LLR_mask
        if self.bias_scaling_vectors[4].grad is not None:
            self.bias_scaling_vectors[4].grad.data *= self.Channel_LLR_mask
    
############################################## Model #############################################   