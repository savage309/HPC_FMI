/Developer/NVIDIA/CUDA-8.0/bin/nvcc kernels.cu --resource-usage -gencode arch=compute_30,code=sm_30 -cubin -o "kernels.cubin" -x cu
