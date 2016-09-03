//example for OpenCL kernel that accepts 2 array of floats(in and out)
//and computes out[i] = in[i] * in[i]

#include <stdio.h>
#ifdef __APPLE__
#   include "OpenCL/cl.h"
#else
#   include "cl.h"
#endif

//macro we use to check if any of the OpenCL API returned error
//if so, print the file name and line number of the API call and exit from the program
#define CHECK_ERROR(X) __checkError(__FILE__, __LINE__, X)
inline void __checkError(const char* file, int line, cl_int error) {
    if (error != CL_SUCCESS) {
        printf("error %i in file %s, line %i\n", error, file, line);
        exit(EXIT_FAILURE);
    }
}

//the size of the buffers that we will alloc/dealloc on the devices
const size_t DATA_SIZE = 1024;

// (1) the source of the GPU kernel itself - it does simple x*x on arrays
const char* KERNEL_SOURCE = "\n" \
"__kernel void square(                                                  \n" \
"   __global const float* restrict input,                               \n" \
"   __global float* restrict output,                                    \n" \
"   const unsigned int count)                                           \n" \
"{                                                                      \n" \
"   int i = get_global_id(0);                                           \n" \
"   if(i < count)                                                       \n" \
"       output[i] = input[i] * input[i];                                \n" \
"}                                                                      \n" \
"\n";


int main(int argc, char** argv) {
    cl_int err = CL_SUCCESS; // error code returned from api calls
    
    //(2) allocate memory on the host and fill it with random data
    //we will later transfer this memory to the devices and use
    //it as "input" parameter of the "square" kernel
    float data[DATA_SIZE];
    for(int i = 0; i < DATA_SIZE; i++)
        data[i] = rand() / (float)RAND_MAX;
    
    // Connect to a compute device
    cl_device_id device_id;// compute device id
    
    cl_uint numPlatforms = 0;
    //when called with second arg NULL, clGetPlatformIDs returns in its
    //third arg the number of the platforms the system has
    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    CHECK_ERROR(err);
    printf("Found %i platforms\n", (int)numPlatforms);
    if (numPlatforms == 0)
        exit(EXIT_FAILURE);
    
    //now, alloc one unique id per platform
    cl_platform_id* platforms = new cl_platform_id[numPlatforms];
    //and get the ids of all platforms the system has
    err = clGetPlatformIDs(numPlatforms, platforms, NULL);
    //print the name of each and every platfrom we had found
    for (int i = 0; i < numPlatforms; ++i) {
        char chBuffer[1024];
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
        CHECK_ERROR(err);
        printf("found platform '%i' = \"%s\"\n", i, chBuffer);
    }
    
    err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    CHECK_ERROR(err);
    
    //now, prepare the first device from the first platform
    //Create a compute context to bind with the device
    cl_context context; // compute context
    
    context = clCreateContext(0 /*platform id 0*/, 1 /*one device from it*/
                              , &device_id, NULL, NULL, &err);
    CHECK_ERROR(err);
    
    //Create a command queue. We will use those to fill it with commands
    //that we will send to the device to be executed
    cl_command_queue commands;// compute command queue
    
    commands = clCreateCommandQueue(context, device_id, 0, &err);
    CHECK_ERROR(err);
    
    // (3)Compile the kernel itself from the source buffer
    cl_program program; // compute program, may have multiple kernels in it
    
    //create a program id
    program = clCreateProgramWithSource(context, 1, (const char **) & KERNEL_SOURCE, NULL, &err);
    CHECK_ERROR(err);
    
    //compile the program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(EXIT_FAILURE);
    }
    
    //get the kernel we want from the compiled program
    cl_kernel kernel;// compute kernel
    
    kernel = clCreateKernel(program, "square", &err);
    CHECK_ERROR(err);
    
    //Create the input and output arrays in device memory for our calculation
    cl_mem input;  // handle to device memory used for the input array
    cl_mem output; // handle to device memory used for the output array
    
    //(4) allocate memory on the device
    //memory marked as input/output can increase the performance of the kernel
    input = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * DATA_SIZE, NULL, &err);
    CHECK_ERROR(err);
    output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * DATA_SIZE, NULL, &err);
    CHECK_ERROR(err);
    
    //Copy data to the device memory
    err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * DATA_SIZE, data, 0, NULL, NULL);
    CHECK_ERROR(err);
    
    //Prepare to call the kernel
    //Set the arguments to our compute kernel
    err = 0;
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
    err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &DATA_SIZE);
    CHECK_ERROR(err);
    
    size_t global;  // number of thread blocks
    size_t local;   // thread block size
    
    //Get the maximum work group size for executing the kernel on the device
    err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    CHECK_ERROR(err);
    
    //(5) Execute the kernel over the entire range of our 1d input data set
    //using the maximum number of work group items for this device
    global = DATA_SIZE;
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    CHECK_ERROR(err);
    
    //Wait for the command commands to get serviced before reading back results
    clFinish(commands);
    
    float results[DATA_SIZE];// results returned from device
    
    //(6) Read back the results from the device to verify the output
    err = clEnqueueReadBuffer( commands, output, CL_TRUE, 0, sizeof(float) * DATA_SIZE, results, 0, NULL, NULL );
    CHECK_ERROR(err);
    
    unsigned int correct;               // number of correct results returned
    //Validate results
    correct = 0;
    for(int i = 0; i < DATA_SIZE; i++) {
        if(results[i] == data[i] * data[i])
            correct++;
    }
    
    //Print a brief summary detailing the results
    printf("Computed '%d/%d' correct values!\n", correct, (int)DATA_SIZE);
    
    //Shutdown and cleanup
    clReleaseMemObject(input);
    clReleaseMemObject(output);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    delete[] platforms;
    return 0;
}