/**
 * @file main.cxx
 * @author Lucas Gasparino
 * @brief Simple example of a self-allocating class on device
 * @version 0.1
 * @date 2025-08-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

// C headers
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <iostream>

// GPU headers
#include <cuda.h>
#include <openacc.h>
#include <nvtx3/nvToolsExt.h>

// Macro utility for NVTX ranges
const uint32_t colors[] = { 0xff00ff00, 0xff0000ff, 0xffffff00, 0xffff00ff, 0xff00ffff, 0xffff0000, 0xffffffff };
const int num_colors = sizeof(colors)/sizeof(uint32_t);

// Push function for starting NVTX ranges
#define PUSH_RANGE(name,cid) { \
    int color_id = cid; \
    color_id = color_id%num_colors;\
    nvtxEventAttributes_t eventAttrib = {0}; \
    eventAttrib.version = NVTX_VERSION; \
    eventAttrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE; \
    eventAttrib.colorType = NVTX_COLOR_ARGB; \
    eventAttrib.color = colors[color_id]; \
    eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII; \
    eventAttrib.message.ascii = name; \
    nvtxRangePushEx(&eventAttrib); \
}

// Pop function for ending NVTX ranges
#define POP_RANGE nvtxRangePop();

// Basic class: contains a scalar data (array size) and a dynamically allocatable pointer holding floats
class Basic
{
    private:
        int arrSize; // No. elements in data pointer
        float* data; // Data pointer
    public:
        // Empty constructor
        Basic() {
            arrSize = -1;
            data = nullptr;
        }

        // Param. constructor
        Basic(int size) {
            arrSize = size;
            data = (float*)calloc(arrSize, sizeof(float));
            PUSH_RANGE("Basic::constructor", 0);
            // Deep-copy the object to the device
            #pragma acc enter data copyin(this[0:1])
            #pragma acc enter data copyin(data[0:arrSize])
            POP_RANGE
        }

        // Destructor
        ~Basic() {
            PUSH_RANGE("Basic::destructor", 0);
            if (data) {
                #pragma acc exit data delete(data[0:arrSize])
                free(data);
                data = nullptr;
            }
            #pragma acc exit data delete(this[0:1])
            POP_RANGE
        }

        // Getters
        int getSize() const {
            return arrSize;
        }
        float* getData() const {
            return data;
        }

        // Setters
        void setDataEntry(int index, float value) {
            data[index] = value;
        }

        // Printer
        void printObj() const {
            PUSH_RANGE("Basic::printObj", 0);
            #pragma acc update host(data[0:arrSize])
            POP_RANGE
            printf("Object size: %d\n", arrSize);
            printf("Data:\n");
            for (int i = 0; i < arrSize; ++i) {
                printf("A[%d] := %f, ", i, data[i]);
            }
        }
};

int main() {
    int aSize = 10;
    Basic obj(aSize);

    PUSH_RANGE("Main::first_parallel_loop", 0);
    #pragma acc parallel loop present(obj)
    for (int i = 0; i < obj.getSize(); i++) {
        float value = static_cast<float>(i+1);
        obj.setDataEntry(i, value);
    }
    POP_RANGE

    PUSH_RANGE("Main::second_parallel_loop", 1);
    #pragma acc parallel loop present(obj)
    for (int i = 0; i < obj.getSize(); i++) {
        float value = static_cast<float>(i+2);
        obj.setDataEntry(i, value);
    }
    POP_RANGE

    obj.printObj();
    return 0;
}