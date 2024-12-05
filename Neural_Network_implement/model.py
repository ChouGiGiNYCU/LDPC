import torch
import torch.nn as nn
import torch.optim as optim

# 定義兩層神經網路模型
class SimpleNN(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(SimpleNN, self).__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)  # 第一層
        self.fc2 = nn.Linear(hidden_size, output_size)  # 第二層

    def forward(self, x):
        x = self.fc1(x)
        x = self.fc2(x)  # 輸出層
        return x

# 設定模型參數
input_size = 8  # 假設輸入層有10個神經元
hidden_size = 3  # 隱藏層有5個神經元
output_size = 1  # 輸出層有1個神經元

# 建立模型
model = SimpleNN(input_size, hidden_size, output_size)

# 隨機生成一些訓練數據
x_train = torch.randn(100, input_size)  # 100筆資料，每筆資料有10個特徵
y_train = torch.randn(100, output_size)  # 對應的100個目標值

# 定義損失函數和優化器
criterion = nn.MSELoss()
optimizer = optim.SGD(model.parameters(), lr=0.01)

# 訓練模型
epochs = 1
for epoch in range(epochs):
    # 向前傳播
    model.train()
    output = model(x_train)
    loss = criterion(output, y_train)

    # 反向傳播
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

    if epoch  == 0:
        print(f'Epoch [{epoch+1}/{epochs}], Loss: {loss.item():.4f}')

# 保存模型參數
torch.save(model.state_dict(), 'simple_nn_model.pth')
print("模型參數已保存為 'simple_nn_model.pth'")

# 保存每一層的weight和bias到不同的TXT文件
for name, param in model.named_parameters():
    if 'weight' in name:
        weight_filename = f"{name}.txt"
        with open(weight_filename, 'w') as f:
            print(param.data)
            for weight in param.data.numpy().flatten():  # 展平並逐一寫入
                f.write(f"{weight}\n")
        print(f"權重已保存到 {weight_filename}")
    elif 'bias' in name:
        bias_filename = f"{name}.txt"
        with open(bias_filename, 'w') as f:
            for bias in param.data.numpy().flatten():  # 展平並逐一寫入
                f.write(f"{bias}\n")
        print(f"偏置已保存到 {bias_filename}")

model.eval()
x_test = torch.randn(10, input_size)  # 100筆資料，每筆資料有10個特徵
for i in x_test:
    output = model(i)
    print(f"input : {i} | output : {output}")