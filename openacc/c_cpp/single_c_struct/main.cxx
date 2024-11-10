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
    Basic d_struc;
    #pragma acc enter data create(d_struc[0:1])

    #pragma acc kernels present(d_struc[0:1])
    {
        d_struc.id = 3;
        d_struc.value = 3.0f;
    }

    // Update the host and print the info
    #pragma acc update host(d_struc[0:1])
    printf("Device: Basic.id = %d\n", d_struc.id);
    printf("Device: Basic.value = %f\n", d_struc.value);

    return 0;
}