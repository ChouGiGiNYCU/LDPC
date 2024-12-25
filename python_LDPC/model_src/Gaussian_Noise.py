import numpy as np
import matplotlib.pyplot as plt

def generate_gaussian_noise(mean=0, var=1, size=1):
    """
    生成高斯噪聲。
    Parameters:
        mean (float): 均值。
        var (float): 變異數。
        size (int): 生成的數據點數量。
    """
    # 計算標準差
    std_dev = np.sqrt(var)
    
    # 生成高斯噪聲
    noise = np.random.normal(mean, std_dev, size)
    # print(noise)
    return float(noise[0])

# # 生成高斯噪聲
# mean = 0
# var = 1  # 修改變異數
# size = 10000000  # 您可以根據需要調整這個值
# noise = generate_gaussian_noise()
# # 繪製噪聲的直方圖以可視化
# plt.figure(figsize=(8, 6))
# plt.hist(noise, bins=30, density=True, alpha=0.6, color='g', edgecolor='black')

# # 繪製理論上的高斯分布曲線
# xmin, xmax = plt.xlim()
# x = np.linspace(xmin, xmax, 100)
# p = (1 / (np.sqrt(2 * np.pi * var))) * np.exp(-((x - mean) ** 2) / (2 * var))
# plt.plot(x, p, 'k', linewidth=2, label='Theoretical Gaussian')

# plt.title("Histogram of Gaussian Noise with Variance 5")
# plt.xlabel("Value")
# plt.ylabel("Density")
# plt.legend()
# plt.grid(True)
# plt.show()
