import matplotlib.pyplot as plt

problem_size = [130, 254, 562, 1011, 2362, 3799, 8399, 23045, 37960, 43577, 85401]
special_gauss_normal = [0.004, 0.099, 0.042, 2.857, 9.421, 489.575, 6931.38, 157492, 326775, 1.20751e+06, 776.969]
special_gauss_sse = [0, 0, 0, 0, 7.640, 34.110, 355.266, 14134.1, 34594.3, 93058.7, 205.344]
special_gauss_avx = [0, 0, 0, 0, 11.995, 24.592, 339.004, 11542.1, 30799.4, 76188.4, 195.584]

plt.figure(figsize=(10, 6))
plt.plot(problem_size, special_gauss_normal, marker='o', label='SpecialGauss_normal')
plt.plot(problem_size, special_gauss_sse, marker='o', label='SpecialGauss_SSE')
plt.plot(problem_size, special_gauss_avx, marker='o', label='SpecialGauss_AVX')
plt.xlabel('Problem Size')
plt.ylabel('Time in ms')
plt.title('Comparison of Different Algorithms')
plt.legend()
plt.grid(True)
plt.show()

import matplotlib.pyplot as plt

problem_size = [130, 254, 562, 1011, 2362, 3799, 8399, 23045, 37960, 43577, 85401]
acceleration_SSE = [None, None, None, None, 1.23, 14.35, 19.53, 11.14, 9.46, 12.97, 3.79]
acceleration_AVX = [None, None, None, None, 0.785, 19.89, 20.46, 13.66, 10.60, 15.84, 3.98]

plt.figure(figsize=(10,6))

plt.plot(problem_size, acceleration_SSE, marker='o', linestyle='-', color='r', label='Acceleration SSE')
plt.plot(problem_size, acceleration_AVX, marker='o', linestyle='-', color='b', label='Acceleration AVX')

plt.xlabel('Problem Size')
plt.ylabel('Acceleration')
plt.title('Acceleration of SSE and AVX over normal')
plt.legend()
plt.grid(True)
plt.show()


import matplotlib.pyplot as plt

problem_sizes = [130, 254, 562, 1011, 2362, 3799, 8399, 23045, 37960, 85401]
normal_times = [0.002, 0.099, 0.037, 2.863, 9.409, 500.696, 7084.36, 145198, 327657, 803.02]
neon_times = [0.006, 0.096, 0.041, 2.337, 7.167, 346.6, 4120.65, 87256, 183142, 586.804]

# Plot the data times
plt.figure(figsize=(10, 6))
plt.plot(problem_sizes, normal_times, marker='o', label='SpecialGauss_normal')
plt.plot(problem_sizes, neon_times, marker='o', label='SpecialGauss_Neon')
plt.xlabel('Problem Size')
plt.ylabel('Time (ms)')
plt.legend()
plt.grid(True)
plt.title('Comparison of SpecialGauss_normal and SpecialGauss_Neon')
plt.show()

# Compute the speedup ratios
speedup_ratios = [n / g for n, g in zip(normal_times, neon_times)]

# Plot the speedup ratios
plt.figure(figsize=(10, 6))
plt.plot(problem_sizes, speedup_ratios, marker='o', label='Speedup (Neon/normal)')
plt.xlabel('Problem Size')
plt.ylabel('Speedup Ratio')
plt.legend()
plt.grid(True)
plt.title('Speedup of SpecialGauss_Neon over SpecialGauss_normal')
plt.show()

import matplotlib.pyplot as plt

# 定义数据
thread_counts = [3, 5, 7, 9, 11]
SG = [9453.86] * 5
SG_Pthread = [4220.65, 4062.96, 3974.97, 3634.31, 3468.14]
SG_Neon_standard = [6097.18] * 5
SG_Neon_Pthread = [3516.991, 3240.56, 3010.74, 2861.33, 2617.18]
SG_Neon_unaligned = [6193.36] * 5

# 绘制图形
plt.figure(figsize=(10, 6))
plt.plot(thread_counts, SG, marker='o', label='SG')
plt.plot(thread_counts, SG_Pthread, marker='o', label='SG_Pthread')
plt.plot(thread_counts, SG_Neon_standard, marker='o', label='SG_Neon_standard')
plt.plot(thread_counts, SG_Neon_Pthread, marker='o', label='SG_Neon_Pthread')
plt.plot(thread_counts, SG_Neon_unaligned, marker='o', label='SG_Neon_unaligned')

# 设置图形标签
plt.xlabel('Thread Count')
plt.ylabel('Time (ms)')
plt.title('Performance Comparison')
plt.legend()

# 显示图形
plt.show()

import matplotlib.pyplot as plt

# Test cases
cases = [8, 9, 10]

# Computation time for different algorithms
time_seq_alg1 = [257.48, 485.28, 2084.17]
time_seq_alg2 = [419.47, 1006.49, 4220.47]
time_mpi_alg2 = [204.86, 418.64, 1788.68]

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(cases, time_seq_alg1, marker='o', label="Sequential Algorithm 1")
plt.plot(cases, time_seq_alg2, marker='o', label="Sequential Algorithm 2")
plt.plot(cases, time_mpi_alg2, marker='o', label="MPI Based Algorithm 2")
plt.xlabel('Test Cases')
plt.ylabel('Computation Time (ms)')
plt.title('Computation Time of Different Algorithms')
plt.legend()
plt.grid(True)
plt.show()

import matplotlib.pyplot as plt

# Test cases
x = [8, 9, 10]

# Execution times in ms
sequential_time = [419.47, 1006.49, 4220.47]
mpi_time = [204.86, 418.64, 1788.68]

# Speedup ratios
speedup_ratio = [sequential_time[i]/mpi_time[i] for i in range(len(sequential_time))]

# Create new figure
plt.figure(figsize=(10, 6))

# Plot execution times
plt.plot(x, sequential_time, marker='o', linestyle='-', color='r', label='Sequential Algorithm 2')
plt.plot(x, mpi_time, marker='o', linestyle='-', color='b', label='MPI Based Algorithm 2')

# Plot speedup ratios
plt.plot(x, speedup_ratio, marker='o', linestyle='-', color='g', label='Speedup Ratio')

# Set labels and title
plt.xlabel('Test Cases', fontsize=14)
plt.ylabel('Execution Time (ms) / Speedup Ratio', fontsize=14)
plt.title('Execution Time and Speedup Ratio for Different Algorithms', fontsize=16)

# Show legend
plt.legend()

# Display the plot
plt.show()

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

# 创建一个数据字典
data = {
    'Scale': [5, 6, 7, 8, 9],
    'SG': [44.559, 659.612, 9453.86, 184078, 572441],
    'SG_OMP': [27.551, 379.514, 4227.41, 99476.4, 265478],
    'SG_Neon_std': [34.192, 485.469, 6097.18, 101525, 208105],
    'SG_Neon_OMP': [20.355, 309.498, 3262.5, 74918.3, 214208],
    'SG_Neon_unaligned': [37.989, 549.966, 6193.36, 104205, 195830]
}

# 转换为Pandas DataFrame
df = pd.DataFrame(data)

# 设置样式
sns.set(style="whitegrid")

# 创建一个折线图
plt.figure(figsize=(10, 6))
for column in df.drop('Scale', axis=1):
    plt.plot(df['Scale'], df[column], marker='o', label=column)

plt.xlabel('Scale', size=12)
plt.ylabel('Time (ms)', size=12)
plt.title('Performance data for various implementations', size=15)
plt.legend()
plt.show()


import matplotlib.pyplot as plt
import numpy as np

# 数据
test_cases = [7, 8, 9]
serial_time = [41.41, 484.92, 1741.74]
cuda_time = [14.19, 140.56, 398.83]

x = np.arange(len(test_cases))  # 标签位置
width = 0.35  # 柱子的宽度

fig, ax = plt.subplots()

rects1 = ax.bar(x - width/2, serial_time, width, label='Serial')
rects2 = ax.bar(x + width/2, cuda_time, width, label='CUDA')

# 添加一些文本标签，比如x轴和y轴的标题，图例等等。
ax.set_xlabel('Test Cases')
ax.set_ylabel('Time (ms)')
ax.set_title('Comparison between Serial and CUDA Algorithm')
ax.set_xticks(x)
ax.set_xticklabels(test_cases)
ax.legend()

# 自动提供足够的标签
def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')


autolabel(rects1)
autolabel(rects2)

fig.tight_layout()

plt.show()


import matplotlib.pyplot as plt
import numpy as np

# 数据
test_cases = [7, 8, 9]
speedup = [2.92, 3.45, 4.37]

x = np.arange(len(test_cases))  # 标签位置
width = 0.35  # 柱状图的宽度

fig, ax = plt.subplots()

rects1 = ax.bar(x, speedup, width, color='g')

# 添加一些文本标签，比如x轴和y轴的标题，图例等等。
ax.set_xlabel('Test Cases')
ax.set_ylabel('Speedup')
ax.set_title('Speedup of CUDA Algorithm over Serial Algorithm')
ax.set_xticks(x)
ax.set_xticklabels(test_cases)

# 自动提供足够的标签
def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')


autolabel(rects1)

fig.tight_layout()

plt.show()


