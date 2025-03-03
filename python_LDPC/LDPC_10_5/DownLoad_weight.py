# 保存每一層的weight和bias到不同的TXT文件
def download_weight(model,weight_name,bias_name):
    for name, param in model.named_parameters():
        if weight_name in name:
            weight_filename = f"{name}.txt"
            with open(weight_filename, 'w') as f:
                print(param.data)
                for weight in param.data.numpy().flatten():  # 展平並逐一寫入
                    f.write(f"{weight}\n")
            print(f"權重已保存到 {weight_filename}")
        elif bias_name in name:
            bias_filename = f"VnBias_{name}.txt"
            with open(bias_filename, 'w') as f:
                for bias in param.data.numpy().flatten():  # 展平並逐一寫入
                    if bias!=0:
                        f.write(f"{bias}\n")
            print(f"偏置已保存到 {bias_filename}")