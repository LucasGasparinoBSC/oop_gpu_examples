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

/**
 * @brief Simple struct containing an int and a float attribute
 * @author Lucas Gasparino
 */
struct Basic
{
    int id;
    float value;
};

// Kernel to alter the attribute
__global__ void alter_attribute(Basic* struc)
{
    int i = blockIdx.x;
    if (i < 1)
    {
        struc[i].id = 3;
        struc[i].value = 2.5f;
    }
}

int main(int argc, const char** argv)
{
    // Host code:
    // Create a single instance of Basic and initialize the atttributes
    Basic h_struc;
    h_struc.id = 2;
    h_struc.value = 2.0f;

    // Print the info
    printf("Host: Basic.id = %d\n", h_struc.id);
    printf("Host: Basic.value = %f\n", h_struc.value);

    // Device code:
    // Create a single instance of Basic and initialize the atttributes
    Basic* d_struc;
    cudaMalloc(&d_struc, sizeof(Basic));

    // Call the kernel function
    alter_attribute<<<1, 1>>>(d_struc);

    // Copy the result from device to host
    cudaMemcpy(&h_struc, d_struc, sizeof(Basic), cudaMemcpyDeviceToHost);

    // Print the info
    printf("Device: Basic.id = %d\n", h_struc.id);
    printf("Device: Basic.value = %f\n", h_struc.value);

    // Free device memory
    cudaFree(d_struc);

    return 0;
}