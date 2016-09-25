/*
 #include <stdio.h>
 
 const int N = 16;
 const int blocksize = 16;
 
 __global__
 void hello(char *a, int *b)
 {
	a[threadIdx.x] += b[threadIdx.x];
 }
 
 int main()
 {
	char a[N] = "Hello \0\0\0\0\0\0";
	int b[N] = {15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
	char *ad;
	int *bd;
	const int csize = N*sizeof(char);
	const int isize = N*sizeof(int);
 
	printf("%s", a);
 
	cudaMalloc( (void**)&ad, csize );
	cudaMalloc( (void**)&bd, isize );
	cudaMemcpy( ad, a, csize, cudaMemcpyHostToDevice );
	cudaMemcpy( bd, b, isize, cudaMemcpyHostToDevice );
	
	dim3 dimBlock( blocksize, 1 );
	dim3 dimGrid( 1, 1 );
	hello<<<dimGrid, dimBlock>>>(ad, bd);
	cudaMemcpy( a, ad, csize, cudaMemcpyDeviceToHost );
	cudaFree( ad );
	cudaFree( bd );
	
	printf("%s\n", a);
	return EXIT_SUCCESS;
 }

 */

//for CUDA API
#include "cuda.h"
//for NVRTC
#ifdef CUDA_USE_NVRTC
#   include "nvrtc.h"
#endif
//STD
#include <vector>
#include <string>
#include <fstream>

//misc stuff

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

enum LogType { LogTypeInfo = 0, LogTypeInfoRaw = 0, LogTypeWarning, LogTypeError, LogTypeNone };

const LogType LOG_LEVEL = LogTypeInfo;

inline
void printLog(LogType priority, const char *format, ...) {
    
    if(priority < LOG_LEVEL)
        return;
    
    if (priority != LogTypeInfoRaw) {
        char s[512];
        
        time_t t = time(NULL);
        struct tm * p = localtime(&t);
        strftime(s, 512, "[%H:%M:%S] ", p);
        
        printf("%s", s);
        switch (priority) {
            case LogTypeInfo:
                printf("Info: ");
                break;
            case LogTypeWarning:
                printf("Warning: ");
                break;
            case LogTypeError:
                printf("Error: ");
                break;
            default:
                break;
        }
            
    }
    
    va_list args;
    va_start(args, format);
    
    vprintf(format, args);
    
    va_end(args);
}

namespace CUDAErrorCheck {
template<typename T> inline
void __checkError(T error, const char* file, int line) {
    if (error != CUDA_SUCCESS) {
        printLog(LogTypeError, "error %i in file %s, line %i", error, file, line);
        exit(EXIT_FAILURE);
    }
}
} //CUDAErrorCheck

#define CHECK_ERROR(X) CUDAErrorCheck::__checkError(X, __FILE__, __LINE__)

std::string getProgramSource(const std::string& path) {
    std::ifstream programSource(path.c_str());
    if (!programSource.good()) printLog(LogTypeError, "program source not found\n");
    return std::string((std::istreambuf_iterator <char>(programSource)), std::istreambuf_iterator <char>());
}

std::vector<char> getProgramBinary(const std::string& path) {
    std::ifstream input(path.c_str(), std::ios::binary);
    if (!input.good()) {
        printLog(LogTypeError, "program binary not found\n");
    }
   std::vector<char> result((std::istreambuf_iterator<char>(input)),
                             std::istreambuf_iterator<char>());
    return std::move(result);
}

inline void pushContext(CUcontext context) {
    CUresult err = cuCtxPushCurrent(context);
    CHECK_ERROR(err);
}

inline void popContext(CUcontext context) {
    CUresult err = cuCtxPopCurrent(&context);
    CHECK_ERROR(err);
}

struct DeviceBuffer {
    CUdeviceptr ptr;
    DeviceBuffer(int64_t numBytes) {
        CUresult res = cuMemAlloc(&ptr, numBytes);
        CHECK_ERROR(res);
    }
    DeviceBuffer(int64_t numBytes, void* srcPtr) {
        CUresult res = cuMemAlloc(&ptr, numBytes);
        CHECK_ERROR(res);
        res = cuMemcpyHtoD(ptr, srcPtr, numBytes);
        CHECK_ERROR(res);
    }
    ~DeviceBuffer() {
        CUresult res = cuMemFree(ptr);
        CHECK_ERROR(res);
    }
    operator CUdeviceptr() {
        return ptr;
    }
};

void initCUDAAPI(std::vector<CUdevice>& devices, std::vector<CUcontext>& contexts, std::vector<CUmodule>& programs) {
    
    CUresult err = cuInit(0);
    CHECK_ERROR(err);
    
    int devicesCount = 0;
    err = cuDeviceGetCount(&devicesCount);
    CHECK_ERROR(err);
    
    for (unsigned i = 0; i < devicesCount; ++i) {
        CUdevice device;
        err = cuDeviceGet(&device, i);
        CHECK_ERROR(err);
        
        devices.push_back(device);
        
        char* buffer[1024];
        err = cuDeviceGetName(	(char*)buffer,
                              (int)sizeof(buffer),
                              device
                              );
        CHECK_ERROR(err);
        
        printLog(LogTypeInfo, "Found device named %s\n", (char*)buffer);
        
        CUcontext pctx;
        err = cuCtxCreate(&pctx, CU_CTX_SCHED_AUTO, device);
        CHECK_ERROR(err);
        
        contexts.push_back(pctx);
    }

}

void loadCUDAKernel(std::vector<CUdevice>& devices, std::vector<CUcontext>& contexts, std::vector<CUmodule>& programs) {
#ifdef CUDA_USE_NVRTC
    const char* pathToKernelSource = "../../cuda_hello_world/kernels.cu";
    printLog(LogTypeInfo, "Trying to load kernel from source located at %s\n", pathToKernelSource);
    std::string source = getProgramSource(pathToKernelSource);
    
    nvrtcResult nvRes;
    nvrtcProgram program;
    nvRes = nvrtcCreateProgram(&program, source.c_str(), "compiled_kernel", 0, NULL, NULL);
    
    const char* options[] = {"--gpu-architecture=compute_20","--maxrregcount=64","--use_fast_math"};//, "--std=c++11"};
    nvRes = nvrtcCompileProgram(program, COUNT_OF(options), options);
    
    if (nvRes != NVRTC_SUCCESS) {
        size_t programLogSize;
        nvRes = nvrtcGetProgramLogSize(program, &programLogSize);
        CHECK_ERROR(nvRes);
        std::unique_ptr<char[]> log (new char[programLogSize + 1]);
        
        nvRes = nvrtcGetProgramLog(program, log.get());
        CHECK_ERROR(nvRes);
        printLog(LogTypeError, "%s", log.get());
        return;
    }
    
    size_t ptxSize;
    nvRes = nvrtcGetPTXSize(program, &ptxSize);
    CHECK_ERROR(nvRes);
    
    std::unique_ptr<char[]> ptx(new char[ptxSize + 1]);
    nvRes = nvrtcGetPTX(program, ptx.get());
    
    const char* TARGET_CUDA_SAVE_PTX_PATH = "../../blago.ptx";
    
    {
        std::fstream ptxStream(TARGET_CUDA_SAVE_PTX_PATH, std::ios_base::trunc | std::ios_base::out);
        if (!ptxStream.good()) {
            printLog(LogTypeWarning, "Could not save PTX IR to %s\n", TARGET_CUDA_SAVE_PTX_PATH);
        } else {
            printLog(LogTypeInfo, "PTX IR saved to %s\n", TARGET_CUDA_SAVE_PTX_PATH);
        }
        ptxStream << ptx.get();
    }
    
    const size_t JIT_NUM_OPTIONS = 9;
    const size_t JIT_BUFFER_SIZE_IN_BYTES = 1024;
    char logBuffer[JIT_BUFFER_SIZE_IN_BYTES];
    char errorBuffer[JIT_BUFFER_SIZE_IN_BYTES];
    
    CUjit_option jitOptions[JIT_NUM_OPTIONS];
    int optionsCounter = 0;
    jitOptions[optionsCounter++] = CU_JIT_MAX_REGISTERS;
    jitOptions[optionsCounter++] = CU_JIT_OPTIMIZATION_LEVEL;
    jitOptions[optionsCounter++] = CU_JIT_TARGET_FROM_CUCONTEXT;
    jitOptions[optionsCounter++] = CU_JIT_FALLBACK_STRATEGY;
    jitOptions[optionsCounter++] = CU_JIT_INFO_LOG_BUFFER;
    jitOptions[optionsCounter++] = CU_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
    jitOptions[optionsCounter++] = CU_JIT_ERROR_LOG_BUFFER;
    jitOptions[optionsCounter++] = CU_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
    jitOptions[optionsCounter++] = CU_JIT_GENERATE_LINE_INFO;
    void* jitValues[JIT_NUM_OPTIONS];
    const int maxRegCount = 63;
    int valuesCounter = 0;
    jitValues[valuesCounter++] = (void*)maxRegCount;
    const int optimizationLevel = 4;
    jitValues[valuesCounter++] = (void*)optimizationLevel;
    const int dummy = 0;
    jitValues[valuesCounter++] = (void*)dummy;
    const CUjit_fallback_enum fallbackStrategy = CU_PREFER_PTX;
    jitValues[valuesCounter++] = (void*)fallbackStrategy;
    jitValues[valuesCounter++] = (void*)logBuffer;
    const int logBufferSize = JIT_BUFFER_SIZE_IN_BYTES;
    jitValues[valuesCounter++] = (void*)logBufferSize;
    jitValues[valuesCounter++] = (void*)errorBuffer;
    const int errorBufferSize = JIT_BUFFER_SIZE_IN_BYTES;
    jitValues[valuesCounter++] = (void*)errorBufferSize;
    const int generateLineInfo = 1;
    jitValues[valuesCounter++] = (void*)generateLineInfo;
    for (int i = 0; i < devices.size(); ++i) {
        CUmodule program;
        CUresult err = cuModuleLoadDataEx(&program, ptx.get(), JIT_NUM_OPTIONS, jitOptions, jitValues);
        CHECK_ERROR(err);
        programs.push_back(program);
        printLog(LogTypeInfo, "program for device %i compiled\n", i);
    }
    nvRes = nvrtcDestroyProgram(&program);
    CHECK_ERROR(nvRes);
#else
    const char* pathToBinaryKernel = "/Developer/git/hpc2015/GPAPI/GPAPI/HPC2015/cuda_hello_world/cuda_hello_world/kernels.cubin";
    std::vector<char> image;
    printLog(LogTypeInfo, "Tryign to load binari kernel from %s\n", pathToBinaryKernel);
    image = getProgramBinary(pathToBinaryKernel);
    
    for (int i = 0; i < devices.size(); ++i) {
        CUmodule program;
        
        CUresult err = cuModuleLoadData(&program, (void*)&image[0]);
        CHECK_ERROR(err);
        programs.push_back(program);
        printLog(LogTypeInfo, "program for device %i compiled\n", i);
    }
    
#endif //CUDA_USE_NVRTC
}

int main(int argc, const char * argv[]) {
    CUresult err = CUDA_SUCCESS;
    
    std::vector<CUdevice> devices;
    std::vector<CUcontext> contexts;
    std::vector<CUmodule> programs;
    
    initCUDAAPI(devices, contexts, programs);
    loadCUDAKernel(devices, contexts, programs);
    
    //*******************************************************
    
    const int SIZE = 1024;

    pushContext(contexts[0]);
    DeviceBuffer devicePtr(SIZE * sizeof(int));

    const unsigned int localSize = 32;
    
    static_assert((SIZE % localSize) == 0, "number of threads should be multiple of localSize");
    unsigned int globalSize = SIZE / localSize;

    CUfunction kernel;
    
    const char* kernels[] = {"position196",
                            "positionBlockIdx",
                            "positionThreadIdx",
                            "positionGlobalIdx"};
    
    //*******************************************************
    //Hello World kernels (+ globalIdx, localIdx, blockIdx, etc)
#if 1
    for (int i = 0; i < COUNT_OF(kernels); ++i) {
        void *paramsPtrs[1] = {&devicePtr};

        const char* kernelName = kernels[i];
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, globalSize, 1UL, 1UL, // grid size
                             localSize, 1UL, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        CHECK_ERROR(err);
        
        std::unique_ptr<int[]> result(new int[SIZE]);

        err = cuMemcpyDtoH(result.get(), devicePtr, sizeof(int) * SIZE);
        CHECK_ERROR(err);
        for (int i = 0; i < SIZE; ++i) {
            printLog(LogTypeInfoRaw, "%i, ", result[i]);
        }
        
        printLog(LogTypeInfoRaw,"\n\n\n\n");

    }
#endif

#if 1
    //*******************************************************
    //naive sum implementaion
    {
        int sumResult = 0;
        DeviceBuffer resultPtr(sizeof(int), (void*)&sumResult);
        DeviceBuffer countPtr(sizeof(int), (void*)&SIZE);
        
        const char* kernelName = "sum0";
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        void *paramsPtrs[3] = {&devicePtr, &countPtr, &resultPtr};

        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, globalSize, 1UL, 1UL, // grid size
                             localSize, 1UL, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        
        err = cuMemcpyDtoH(&sumResult, resultPtr, sizeof(int));
        CHECK_ERROR(err);
        printLog(LogTypeInfoRaw,"sum0 result %i\n\n\n\n", sumResult);

    }
#endif
    
#if 1
    //*******************************************************
    //better sum implementation
    {
        int sumResult = 0;
        DeviceBuffer resultPtr(sizeof(int), (void*)&sumResult);
        DeviceBuffer countPtr(sizeof(int), (void*)&SIZE);
        
        const char* kernelName = "sum1";
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        void *paramsPtrs[3] = {&devicePtr, &countPtr, &resultPtr};
        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, globalSize, 1UL, 1UL, // grid size
                             localSize, 1UL, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        
        err = cuMemcpyDtoH(&sumResult, resultPtr, sizeof(int));
        CHECK_ERROR(err);
        printLog(LogTypeInfoRaw,"sum1 result %i\n\n\n\n", sumResult);
        
    }
#endif
    
#if 1
    //*******************************************************
    //naive adj difference
    {
        std::unique_ptr<int[]> result(new int[SIZE]);
        memset(result.get(), 0, SIZE * sizeof(int));
        
        DeviceBuffer resultPtr(SIZE * sizeof(int), result.get());
        
        const char* kernelName = "adjDiff0";
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        void *paramsPtrs[2] = {&resultPtr, &devicePtr};
        
        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, globalSize, 1UL, 1UL, // grid size
                             localSize, 1UL, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        
        err = cuMemcpyDtoH(result.get(), resultPtr, SIZE * sizeof(int));
        CHECK_ERROR(err);
        
        for (int i = 0; i < SIZE; ++i)
            printLog(LogTypeInfoRaw,"%i, ", result[i]);
        
        printLog(LogTypeInfoRaw,"\n\n\n\n");
        
    }
#endif
    
#if 1
    //*******************************************************
    //better adj difference
    {
        std::unique_ptr<int[]> result(new int[SIZE]);
        memset(result.get(), 0, SIZE * sizeof(int));
        
        DeviceBuffer resultPtr(SIZE * sizeof(int), result.get());
        
        const char* kernelName = "adjDiff1";
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        void *paramsPtrs[2] = {&resultPtr, &devicePtr};
        
        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, globalSize, 1UL, 1UL, // grid size
                             localSize, 1UL, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        
        err = cuMemcpyDtoH(result.get(), resultPtr, SIZE * sizeof(int));
        CHECK_ERROR(err);
        
        for (int i = 0; i < SIZE; ++i)
            printLog(LogTypeInfoRaw,"%i, ", result[i]);
        
        printLog(LogTypeInfoRaw,"\n\n\n\n");
        
    }
#endif
    
#if 1
    if (1)
    {
        const int STRIDE = 8;
        int matWidth = STRIDE * STRIDE;
        
        std::unique_ptr<float[]> h_a(new float[matWidth * matWidth]);
        std::unique_ptr<float[]> h_b(new float[matWidth * matWidth]);
        std::unique_ptr<float[]> h_ab(new float[matWidth * matWidth]);

        for (int i = 0; i < matWidth; ++i) {
            for (int j = 0; j < matWidth; ++j) {
                const int pos = i + matWidth * j;
                
                h_a[pos] = float(i) / matWidth;
                h_b[pos] = float(j) / matWidth;
                h_ab[pos] = 0.f;
            }
        }


        DeviceBuffer a(matWidth * matWidth * sizeof(float), h_a.get());
   
        DeviceBuffer b(matWidth * matWidth * sizeof(float), h_b.get());
        DeviceBuffer ab(matWidth * matWidth * sizeof(float), h_ab.get());
        DeviceBuffer widthPtr(sizeof(int), &matWidth);

        
        const char* kernelName = "matMul0";
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        void *paramsPtrs[4] = {&a, &b, &ab, &widthPtr};
        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, STRIDE, STRIDE, 1UL, // grid size
                             STRIDE, STRIDE, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        CHECK_ERROR(err);

        err = cuMemcpyDtoH(h_ab.get(), ab, sizeof(float) * matWidth * matWidth);
        CHECK_ERROR(err);
        
        for (int i = 0; i < matWidth; ++i) {
            for (int j = 0; j < matWidth; ++j) {
                const int pos = i + matWidth * j;

                printLog(LogTypeInfoRaw,"%f, ", h_ab[pos]);
            }
        }
        printLog(LogTypeInfoRaw,"\n\n\n\n");
    }
#endif
    
#if 1
    if (1)
    {
        int matWidth = 64;
        
        std::unique_ptr<float[]> h_a(new float[matWidth * matWidth]);
        std::unique_ptr<float[]> h_b(new float[matWidth * matWidth]);
        std::unique_ptr<float[]> h_ab(new float[matWidth * matWidth]);
        for (int i = 0; i < matWidth; ++i) {
            for (int j = 0; j < matWidth; ++j) {
                const int pos = i + matWidth * j;

                h_a[pos] = float(i) / matWidth;
                h_b[pos] = float(j) / matWidth;
                h_ab[pos] = 0.f;
            }
        }
        
        DeviceBuffer a(matWidth * matWidth * sizeof(float), h_a.get());
        
        DeviceBuffer b(matWidth * matWidth * sizeof(float), h_b.get());
        DeviceBuffer ab(matWidth * matWidth * sizeof(float), h_ab.get());
        DeviceBuffer widthPtr(sizeof(int), &matWidth);
        
        
        const char* kernelName = "matMul1";
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        void *paramsPtrs[4] = {&a, &b, &ab, &widthPtr};
        
        /*in sync with kernels.cu::matMul1*/
        const int TILE_WIDTH = 8;
        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, TILE_WIDTH, TILE_WIDTH, 1UL, // grid size
                             matWidth/TILE_WIDTH, matWidth/TILE_WIDTH, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        CHECK_ERROR(err);
        
        err = cuMemcpyDtoH(h_ab.get(), ab, sizeof(float) * matWidth * matWidth);
        CHECK_ERROR(err);
        
        for (int i = 0; i < matWidth; ++i) {
            for (int j = 0; j < matWidth; ++j) {
                const int pos = i + matWidth * j;
                
                printLog(LogTypeInfoRaw, "%f, ", h_ab[pos]);
            }
        }
        printLog(LogTypeInfoRaw, "\n\n\n\n");
    }
#endif 
    
#if 1
    //naive adj difference
    {
        std::unique_ptr<int[]> result(new int[SIZE]);
        for (int i = 0; i < SIZE; ++i) {
            result[i] = i;
            
        }
        DeviceBuffer resultPtr(SIZE * sizeof(int), result.get());
        
        const char* kernelName = "inclusiveScan";
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        void *paramsPtrs[1] = {&resultPtr};
        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, globalSize, 1UL, 1UL, // grid size
                             localSize, 1UL, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        
        err = cuMemcpyDtoH(result.get(), resultPtr, SIZE * sizeof(int));
        CHECK_ERROR(err);
        
        for (int i = 0; i < SIZE; ++i)
            printLog(LogTypeInfoRaw,"%i, ", result[i]);
        
        printLog(LogTypeInfoRaw,"\n\n\n\n");
        
    }
#endif
    
#if 1
    {
        std::unique_ptr<int[]> h_blocks(new int[globalSize]);
        memset(h_blocks.get(), 0, sizeof(int) * globalSize);
        DeviceBuffer blocks(globalSize * sizeof(int), h_blocks.get());
        //
        std::unique_ptr<int[]> h_data(new int[SIZE]);
        for (int i = 0; i < SIZE; ++i)
            h_data[i] = i;
        DeviceBuffer data(SIZE * sizeof(int), h_data.get());
        //
        std::unique_ptr<int> h_n(new int);
        *h_n = SIZE;
        DeviceBuffer n(sizeof(int), h_n.get());
        //
        const char* kernelName = "blockSum";
        err = cuModuleGetFunction(&kernel, programs[0], kernelName);
        CHECK_ERROR(err);
        
        void *paramsPtrs[3] = {&data, &blocks, &n};
        
        printLog(LogTypeInfo, "Launching kernel %s\n", kernelName);
        err = cuLaunchKernel(kernel, globalSize, 1UL, 1UL, // grid size
                             localSize, 1UL, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs[0],
                             NULL
                             );
        CHECK_ERROR(err);
        
        void *paramsPtrs2[3] = {&blocks, &blocks, &n};
        err = cuLaunchKernel(kernel, 1, 1UL, 1UL, // grid size
                             localSize, 1UL, 1UL, // block size
                             0, // shared size
                             NULL, // stream
                             &paramsPtrs2[0],
                             NULL
                             );
        CHECK_ERROR(err);
        err = cuMemcpyDtoH(h_blocks.get(), blocks, globalSize * sizeof(int));
        CHECK_ERROR(err);
        printLog(LogTypeInfo, "%i ", h_blocks[0]);
    }
#endif
    popContext(contexts[0]);
    
    return EXIT_SUCCESS;
}
