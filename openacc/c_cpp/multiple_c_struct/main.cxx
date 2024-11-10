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

/**
 * @brief Simple struct containing an int and a float attribute
 * @author Lucas Gasparino
 */
struct Basic
{
    int id;
    float value;
};

int main(int argc, const char** argv) {

    // Common:
    // Set the number of objects to be created
    const int num_objects = 10;

    // Host code:
    // Create 10 instances of Basic and initialize the atttributes (malloc)
    Basic* h_struc = (Basic*)malloc(num_objects * sizeof(Basic));
    for (int i = 0; i < num_objects; i++)
    {
        h_struc[i].id = i;
        h_struc[i].value = (float) i;
    }

    // Print the info
    for (int i = 0; i < num_objects; i++)
    {
        printf("Host: Basic[%d].id = %d\n", i, h_struc[i].id);
        printf("Host: Basic[%d].value = %f\n", i, h_struc[i].value);
    }
    printf("\n");

    // Device code:
    // Create 10 instances of Basic and initialize the atttributes (acc enter data create)
    Basic* d_struc = (Basic*)malloc(num_objects * sizeof(Basic));
    #pragma acc enter data create(d_struc[0:num_objects])

    //#pragma acc kernels present(d_struc[0:num_objects])
    #pragma acc parallel loop present(d_struc[0:num_objects])
    for (int i = 0; i < num_objects; i++)
    {
        d_struc[i].id = i+1;
        d_struc[i].value = (float) i+1;
    }

    // Update the host and print the info
    #pragma acc update host(d_struc[0:num_objects])

    // Print the updated info
    for (int i = 0; i < num_objects; i++)
    {
        printf("Device: Basic[%d].id = %d\n", i, d_struc[i].id);
        printf("Device: Basic[%d].value = %f\n", i, d_struc[i].value);
    }

    return 0;
}