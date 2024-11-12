// C headers
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

// GPU headers
#include <cuda.h>
#include <openacc.h>
#include <nvToolsExt.h>

// Define the static array size
#define SIZE 3

/**
 * @brief Simple struct containing an int and a static float array
 * @author Lucas Gasparino
 */
struct Basic
{
    int id;
    float* value = nullptr;
};

int main(int argc, const char** argv)
{
    // Host code:
    // Create a single instance of Basic and initialize the atttributes
    Basic h_struc;
    h_struc.id = 1;
    h_struc.value = (float*) malloc(SIZE * sizeof(float));
    for (int i = 0; i < SIZE; i++)
    {
        h_struc.value[i] = 1.0f + (float) i;
    }

    // Print the info
    printf("Host: Basic.id = %d\n", h_struc.id);
    for (int i = 0; i < SIZE; i++)
    {
        printf("Host: Basic.value[%d] = %f\n", i, h_struc.value[i]);
    }
    printf("\n");

    // Device code:
    // Create a single instance of Basic and create memory on the device
    Basic* d_struc = (Basic*) malloc(sizeof(Basic));
    d_struc[0].id = 0;
    d_struc[0].value = (float*) malloc(SIZE * sizeof(float));
    memset(d_struc[0].value, 0, SIZE * sizeof(float));
    #pragma acc enter data copyin(d_struc[0:1], d_struc[0].value[0:SIZE])

    #pragma acc kernels present(d_struc[0:1])
    {
        d_struc[0].id = 2;
    }

    #pragma acc parallel loop present(d_struc[0:1], d_struc[0].value[0:SIZE])
    for (int i = 0; i < SIZE; i++)
    {
        d_struc[0].value[i] = 2.0f + (float) i;
    }

    #pragma acc exit data copyout(d_struc[0].value[0:SIZE], d_struc[0:1])
    //#pragma acc exit data copyout(d_struc[0:1], d_struc[0].value[0:SIZE]) //! Causes a SEGFAULT

    // Print the id
    printf("Device: Basic.id = %d\n", d_struc[0].id);
    // Print the value
    for (int i = 0; i < SIZE; i++)
    {
        printf("Device: Basic.value[%d] = %f\n", i, d_struc[0].value[i]);
    }
    printf("\n");

    // Free the device memory and exit
    return 0;
}