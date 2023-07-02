import matplotlib.pyplot as plt
import pandas as pd

# OpenMP Data
omp_data = pd.DataFrame({
    'Threads': [4, 6, 8, 10],
    'OMP_Gauss': [390.67, 308.98, 280.19, 337.94],
    'normal_Gauss': [608.25, 582.92, 580.25, 615.42]
})
omp_data['Speedup'] = omp_data['normal_Gauss'] / omp_data['OMP_Gauss']

plt.figure(figsize=(8, 6))
plt.plot(omp_data['Threads'], omp_data['Speedup'], marker='o')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.title('OpenMP Speedup vs Number of Threads')
plt.grid(True)
plt.show()

# MPI Data
mpi_data = pd.DataFrame({
    'Processes': [2, 3, 4, 5, 6, 7, 8],
    'MPI_block': [956.548, 706.83, 517.79, 422.056, 372.745, 334.622, 280.176],
    'MPI_cycle': [729.078, 520.15, 401.37, 336.64, 284.568, 269.931, 240.453],
    'MPI_pipeline': [743.177, 529.625, 422.632, 338.883, 324.536, 279.942, 260.305],
    'Serial': [1373, 1373.1, 1372.14, 1373.63, 1370.97, 1372.88, 1371.61]
})

mpi_data['Block_Speedup'] = mpi_data['Serial'] / mpi_data['MPI_block']
mpi_data['Cycle_Speedup'] = mpi_data['Serial'] / mpi_data['MPI_cycle']
mpi_data['Pipeline_Speedup'] = mpi_data['Serial'] / mpi_data['MPI_pipeline']

plt.figure(figsize=(8, 6))
plt.plot(mpi_data['Processes'], mpi_data['Block_Speedup'], marker='o', label='Block')
plt.plot(mpi_data['Processes'], mpi_data['Cycle_Speedup'], marker='o', label='Cycle')
plt.plot(mpi_data['Processes'], mpi_data['Pipeline_Speedup'], marker='o', label='Pipeline')
plt.xlabel('Number of Processes')
plt.ylabel('Speedup')
plt.title('MPI Speedup vs Number of Processes')
plt.grid(True)
plt.legend()
plt.show()

# CUDA Data
cuda_data = pd.DataFrame({
    'Matrix_size': [128, 256, 512, 1024, 2048, 4096, 8192],
    'GPU_Time': [0.192837, 0.0553348, 0.212894, 0.0020067, 0.0262904, 0.103494, 0.232984],
    'CPU_Time': [0.0007381, 0.0052385, 0.1394433, 0.429304, 3.34839, 27.2236, 283.141]
})
cuda_data['Speedup'] = cuda_data['CPU_Time'] / cuda_data['GPU_Time']

plt.figure(figsize=(8, 6))
plt.plot(cuda_data['Matrix_size'], cuda_data['Speedup'], marker='o')
plt.xlabel('Matrix Size')
plt.ylabel('Speedup')
plt.title('CUDA Speedup vs Matrix Size')
plt.grid(True)
plt.show()
