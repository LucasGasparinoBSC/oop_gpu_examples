// C headers
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

// GPU headers
#include <cuda.h>
#include <cuda_runtime.h>
#include <nvtx3/nvToolsExt.h>

// Define the number of structs to be created
#define NUM_OBJ 10

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
    if (i < NUM_OBJ)
    {
        struc[i].id = i+1;
        struc[i].value = 2.5f;
    }
}

int main(int argc, const char** argv)
{
    // Host code:
    // Create an array of Basic structs and initialize the attributes
    Basic* h_struc = (Basic*)malloc(NUM_OBJ*sizeof(Basic));
    for (int i = 0; i < NUM_OBJ; i++)
    {
        h_struc[i].id = i;
        h_struc[i].value = 2.0f;
        printf("Host: Basic[%d].id = %d, Basic[%d].value = %f\n", i, h_struc[i].id, i, h_struc[i].value);
    }

    // Device code:
    // Create a pointer to a Basic struct and allocate memory on the device
    Basic* d_struc;
    cudaMalloc((void**)&d_struc, NUM_OBJ*sizeof(Basic));

    // Call the kernel function
    alter_attribute<<<NUM_OBJ, 1>>>(d_struc);

    // Copy the result from device to host
    cudaMemcpy(h_struc, d_struc, NUM_OBJ*sizeof(Basic), cudaMemcpyDeviceToHost);

    // Print the info
    printf("\n");
    for (int i = 0; i < NUM_OBJ; i++)
    {
        printf("Host: Basic[%d].id = %d, Basic[%d].value = %f\n", i, h_struc[i].id, i, h_struc[i].value);
    }

    // Free the host memory
    free(h_struc);

    // Free device memory
    cudaFree(d_struc);

    return 0;
}