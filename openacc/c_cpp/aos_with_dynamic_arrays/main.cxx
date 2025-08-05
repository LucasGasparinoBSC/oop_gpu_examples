// C headers
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

// GPU headers
#include <cuda.h>
#include <openacc.h>
#include <nvtx3/nvToolsExt.h>

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
    // Create an array of Structs on the host, setting initial values to zero
    Basic* d_struc = (Basic*) malloc(NUM_OBJECTS * sizeof(Basic));
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        d_struc[i].id = 0;
        d_struc[i].value = (float*) malloc(SIZE * sizeof(float));
        memset(d_struc[i].value, 0, SIZE * sizeof(float));
    }

    // Copy the object to the device
    #pragma acc enter data copyin(d_struc[0:NUM_OBJECTS])
    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        // For every object, copy the data pointer to the device
        // NOTE: the scalar member is copied automatically
        #pragma acc enter data copyin(d_struc[i].value[0:SIZE])
    }

    // Course notes: it is possible to extract a set of local
    // arrays that would be private to the gang, possibly
    // residing in shared memory if the sizes are small enough.
    // This is left as an exercise.

    // Kernel 1: Initialize the struct array on the device
    #pragma acc parallel loop gang present(d_struc[0:NUM_OBJECTS])
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        // id = i+1
        d_struc[i].id = i+1;
        #pragma acc loop vector private(d_struc[i].value)
        for (int j = 0; j < SIZE; j++)
        {
            // val = 2*j + i
            d_struc[i].value[j] = 2.0f + (float) j + (float) i;
        }
    }

    // Course notes: at this point, calling Kernel 2 should not
    // cause extra data transfers, since the AoS is already on device,
    // and has never been touched by host. The present clause
    // is a safety mechanism: if data is not on device, the
    // program will fail at runtime.

    // Kernel 2: modify the existing device objects
    #pragma acc parallel loop gang present(d_struc[0:NUM_OBJECTS])
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        d_struc[i].id -= 1; // Decrement id by 1
        #pragma acc loop vector private(d_struc[i].value)
        for (int j = 0; j < SIZE; j++)
        {
            d_struc[i].value[j] *= 2.0f;
        }
    }

    // Course notes: here we update the host by copying
    // out the AoS data from the device. Notice that
    // the data pointer is copied back first, otherwise
    // a reference to it would be lost. Another option
    // would be to use the update clase, left as an exercise.

    // Copy the object back to host
    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        // First, for every object, copy back the data pointer
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