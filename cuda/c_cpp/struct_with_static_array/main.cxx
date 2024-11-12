// C headers
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

// GPU headers
#include <cuda.h>
#include <cuda_runtime.h>
#include <nvToolsExt.h>

// Define the array size
#define SIZE 3

/**
 * @brief Simple struct containing an int and a float static array
 * @author Lucas Gasparino
 */
struct Basic
{
    int id;
    float value[SIZE];
};

// Kernel to alter the attribute
__global__ void alter_attribute(Basic* struc)
{
    int i = blockIdx.x;
    int j = threadIdx.x;
    if (i < 1)
    {
        struc[i].id = 3;
        if (j < SIZE)
        {
            struc[i].value[j] = 3.0f + (float) j;
        }
    }
}

int main(int argc, const char** argv)
{

    // Host:
    // Create a single instance of Basic and initialize the atttributes
    Basic h_struc;
    h_struc.id = 2;
    for (int i = 0; i < SIZE; i++)
    {
        h_struc.value[i] = 2.0f + (float) i;
    }

    // Print the info
    printf("Host: Basic.id = %d\n", h_struc.id);
    for (int i = 0; i < SIZE; i++)
    {
        printf("Host: Basic.value[%d] = %f\n", i, h_struc.value[i]);
    }

    // Device:
    // Create a single instance of Basic and initialize the atttributes
    Basic* d_struc;
    cudaMalloc(&d_struc, sizeof(Basic));

    // Call the kernel
    alter_attribute<<<1, SIZE>>>(d_struc);

    // Copy the result from device to host
    cudaMemcpy(&h_struc, d_struc, sizeof(Basic), cudaMemcpyDeviceToHost);

    // Print the info
    printf("Device: Basic.id = %d\n", h_struc.id);
    for (int i = 0; i < SIZE; i++)
    {
        printf("Device: Basic.value[%d] = %f\n", i, h_struc.value[i]);
    }

    // Free device memory
    cudaFree(d_struc);

    return 0;
}