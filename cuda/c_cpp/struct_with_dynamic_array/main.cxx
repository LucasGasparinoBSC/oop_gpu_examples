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
    float* value;
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
    h_struc.value = (float*) malloc(SIZE * sizeof(float));
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

    // Internal pointer: use a tmp to create the struct pointer on device
    float* tmp;
    cudaMalloc(&tmp, SIZE * sizeof(float));
    cudaMemcpy(&(d_struc->value), &tmp, sizeof(float*), cudaMemcpyHostToDevice);
    cudaMemcpy(tmp, h_struc.value, SIZE * sizeof(float), cudaMemcpyHostToDevice);

    // Call the kernel function
    alter_attribute<<<1, SIZE>>>(d_struc);

    // Copy the result from device to host:
    // Ensure the internal pointer is copied back to host
    cudaMemcpy(&tmp, &(d_struc->value), sizeof(float*), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_struc.value, tmp, SIZE * sizeof(float), cudaMemcpyDeviceToHost);

    // Copy the struct back to host
    cudaMemcpy(&h_struc, d_struc, sizeof(Basic), cudaMemcpyDeviceToHost);

    // Print the info
    printf("Host: Basic.id = %d\n", h_struc.id);
    for (int i = 0; i < SIZE; i++)
    {
        printf("Host: Basic.value[%d] = %f\n", i, h_struc.value[i]);
    }

    return 0;
}