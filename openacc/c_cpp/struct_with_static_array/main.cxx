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
    float value[SIZE];
};

int main(int argc, const char** argv)
{
    // Host code:
    // Create a single instance of Basic and initialize the atttributes
    Basic h_struc;
    h_struc.id = 1;
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
    Basic d_struc;
    #pragma acc enter data create(d_struc[0:1])

    // Modify the device struct attributes:
    // Modify id
    #pragma acc kernels present(d_struc)
    {
        d_struc.id = 2;
    }

    // Modify value
    #pragma acc parallel loop present(d_struc)
    for (int i = 0; i < SIZE; i++)
    {
        d_struc.value[i] = 2.0f + (float) i;
    }

    // Update the host and print the info
    #pragma acc update host(d_struc[0:1])
    printf("Device: Basic.id = %d\n", d_struc.id);
    for (int i = 0; i < SIZE; i++)
    {
        printf("Device: Basic.value[%d] = %f\n", i, d_struc.value[i]);
    }
    printf("\n");

    // Free the device memory and exit
    #pragma acc exit data delete(d_struc[0:1])
    return 0;
}