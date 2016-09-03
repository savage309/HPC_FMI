#define kernel extern "C" __global__
#define device __device__
#define shared __shared__
#define syncThreads __syncthreads

#if __cplusplus > 199711L
#define HAS_CPP_11
#endif

#define BLOCK_SIZE 32

device int getGlobalID() {
    return blockIdx.x*blockDim.x + threadIdx.x;
}

kernel void position196 (int *a) {
    const int i = getGlobalID();
    a[i] = 196;
}

kernel void positionBlockIdx(int *a) {
    const int i = getGlobalID();
    a[i] = blockIdx.x;
}

kernel void positionThreadIdx(int *a) {
    const int i = getGlobalID();
    a[i] = threadIdx.x;
}

kernel void positionGlobalIdx(int *a) {
    const int i = getGlobalID();
    a[i] = i;
}

//************************************************

kernel void raceCondition(int* a) {
    *a += 50;
}

//************************************************

__noinline__
__device__
int testNoInlineFunc(int* ptr) {
    int result = 0;
    for (int i = 0; i < ptr[0]; ++i)
        result += ptr[i];

    return result;

}

#ifdef HAS_CPP_11
#include "stdio.h"
struct A {
private:
    enum class Options {None, One, All};

    int a;
public:
    device A() {
        auto& b = a;
        if (&b != nullptr)
            b = [&]{ return 4; }();
        
        int arr[3] {0, 0, 0};
        for (auto& e: arr) {
            printf("%i\n", e);
        }
        
        static_assert(196 > 42, "This better compiles");
    }
    device ~A(){}
    device A(const A&&){}
    device virtual void foo() final { }
};
#endif //HAS_CPP_11

//************************************************

kernel void sum0(int* a, int* countPtr, int* result) {
    const int i = getGlobalID();
    
    const int count = *countPtr;
    
    if (i > count) {
        return;
    }
    atomicAdd(result, a[i]);
}

//************************************************

kernel void sum1(int* a, int* countPtr, int* result) {
    shared int partialSum;
    
    const int i = getGlobalID();
    const int count = *countPtr;
    
    if (i > count)
        return;
    
    if (threadIdx.x == 0)
        partialSum = 0;
    
    syncThreads();
    atomicAdd(&partialSum, a[i]);

    if (threadIdx.x == 0)
        atomicAdd(result, partialSum);
}

//************************************************

kernel void adjDiff0(int* result, int* input) {
    const int i = getGlobalID();
    
    if (i > 0) {
    
        int curr = input[i];
        int prev = input[i - 1];
        
        result[i] = curr - prev;
    }
}

//************************************************

kernel void adjDiff1(int* result, int* input) {
    int tx = threadIdx.x;
    
    shared int sharedData[BLOCK_SIZE]; //compile-time vs run-time
    
    const int i = getGlobalID();
    sharedData[tx] = input[i];
    //
    syncThreads();
    
    if (tx > 1)
        result[i] = sharedData[tx] - sharedData[tx - 1];
    else if (i > 1) {
        result[i] = sharedData[tx] - input[i - 1];
    }
}

//************************************************

kernel void badKernel0(int* foo) {
    shared int sharedInt;
    int* privatePtr = NULL;
    if (getGlobalID()%2) {
        privatePtr = &sharedInt;
    } else {
        privatePtr = foo;
    }
}

//************************************************

kernel void badKernel1(int* foo) { //hard crash
    shared int sharedInt;
    int* privatePtr = NULL;
    if (getGlobalID()%2) {
        syncThreads();
    } else {
        privatePtr = foo;
    }
}

//************************************************

kernel void matMul0(float* a, float* b, float* ab, int* widthPtr) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int column = blockIdx.x * blockDim.x + threadIdx.x;
    const int width = *widthPtr;
    float res = 0;
    
    for (int k = 0; k < width; ++k)
        res += a[row * width + k] * b[k * width + column];
    
    ab[row * width + column] = res;
}

//************************************************
/*in sync with main.cpp::main::mul1::TILE_WIDTH*/
#define TILE_WIDTH 8

kernel void matMul1(float* a, float* b, float* ab, int* widthPtr) {
    int tx = threadIdx.x, ty = threadIdx.y;
    int bx = blockIdx.x, by = blockIdx.y;

    shared float sA[TILE_WIDTH][TILE_WIDTH];
    shared float sB[TILE_WIDTH][TILE_WIDTH];
    
    int row = by * blockDim.y + ty;
    int col = bx * blockDim.x + tx;
    
    float res = 0;
    
    const int width = *widthPtr;
  
    for (int p = 0; p < width/TILE_WIDTH; ++p) {
        sA[ty][tx] = a[row*width + (p*TILE_WIDTH + tx)];
        sB[ty][tx] = b[(p*TILE_WIDTH + ty)*width + col];
        
        syncThreads();
        
        for (int k = 0; k < TILE_WIDTH; ++k)
            res += sA[ty][k] * sB[k][tx];
        
        syncThreads();
    }
    
    ab[row*width + col] = res;
}

//************************************************

kernel void badMemoryAccess(int* input, int* output) {
    const int i = getGlobalID();
    
    int a = input[i];
    
    int STRIDE = 2;
    
    int b = input[i * STRIDE];

    output[i] = a + b;
}

//************************************************
//reduce example
kernel void blockSum(int* input, int* results, size_t* nPtr) {
    
    size_t n = *nPtr;
    
    shared int sharedData[BLOCK_SIZE];
    const int i = getGlobalID();
    const int tx = threadIdx.x;
    //
    if (threadIdx.x == 0) {
        for (int i = 0; i < BLOCK_SIZE; ++i)
            sharedData[i] = 0;
    }
    
    syncThreads();
    
    int x = 0;
    
    if (i >= n)
        return;
    
    x = input[i];
    
    sharedData[tx] = x;
    syncThreads();
    
    
    for (int offset = blockDim.x / 2;
         offset > 0;
         offset >>= 1)
    {
        if (tx < offset) {
            sharedData[tx] += sharedData[tx + offset];
        }
        syncThreads();
    }
    if (threadIdx.x == 0) {
        results[blockIdx.x] = sharedData[0];
    }
}

//0 1 2 3 4
//0 1 3 6 10
//************************************************
//results local to each block
kernel void inclusiveScan(int* data) {
    shared int sdata[BLOCK_SIZE];
    const int i = getGlobalID();
    
    int sum = data[i];
    
    sdata[threadIdx.x] = sum;
    
    syncThreads();
   
    for (int o = 1; o < blockDim.x; o <<= 1) {
        if (threadIdx.x >= o)
            sum += sdata[threadIdx.x - o];
        
        syncThreads();
    
        sdata[threadIdx.x] = sum;
        
        syncThreads();
    }
    
    data[i] = sdata[threadIdx.x];
}