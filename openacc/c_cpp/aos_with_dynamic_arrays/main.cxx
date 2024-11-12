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
#define SIZE 128

// Define the number of objects
#define NUM_OBJECTS 10

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
    // Create an array of Structs and initialize the values
    Basic* h_struc = (Basic*) malloc(NUM_OBJECTS * sizeof(Basic));
    printf("Host:\n");
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        h_struc[i].id = i;
        printf("Host: Basic[%d].id = %d\n", i, h_struc[i].id);
        h_struc[i].value = (float*) malloc(SIZE * sizeof(float));
        printf("Host: Basic[%d].value = [", i);
        for (int j = 0; j < SIZE; j++)
        {
            h_struc[i].value[j] = 1.0f + (float) j + (float) i;
            printf("%f, ", h_struc[i].value[j]);
        }
        printf("]\n");
    }
    printf("\n");

    // Device code:
    // Create an array of Structs on the device
    Basic* d_struc = (Basic*) malloc(NUM_OBJECTS * sizeof(Basic));
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        d_struc[i].id = 0;
        d_struc[i].value = (float*) malloc(SIZE * sizeof(float));
        memset(d_struc[i].value, 0, SIZE * sizeof(float));
    }
    #pragma acc enter data copyin(d_struc[0:NUM_OBJECTS])
    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        #pragma acc enter data copyin(d_struc[i].value[0:SIZE])
    }

    #pragma acc parallel loop gang present(d_struc[0:NUM_OBJECTS])
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        d_struc[i].id = i+1;
        #pragma acc loop vector private(d_struc[i].value)
        for (int j = 0; j < SIZE; j++)
        {
            d_struc[i].value[j] = 2.0f + (float) j + (float) i;
        }
    }

    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        #pragma acc exit data copyout(d_struc[i].value[0:SIZE])
    }
    #pragma acc exit data copyout(d_struc[0:NUM_OBJECTS])

    printf("Device:\n");
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        printf("Device: Basic[%d].id = %d\n", i, d_struc[i].id);
        printf("Device: Basic[%d].value = [", i);
        for (int j = 0; j < SIZE; j++)
        {
            printf("%f, ", d_struc[i].value[j]);
        }
        printf("]\n");
    }
    printf("\n");

    // Free the memory on the host
    return 0;
}