/**
 * @file main.cxx
 * @author Lucas Gasparino
 * @brief Example of a complex data hierarchy with self-instantiations
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

// Parent Point class
class Point
{
    protected:
        int pID;      // Point ID
        int dataSize; // Data field size
        float* xyz;   // Coordinates in 3D
        float* data;  // Data field array
    public:

    // Empty constructor (host only)
    Point() {
        pID = -1;
        dataSize = -1;
        xyz = nullptr;
        data = nullptr;
    }

    // Parametrized constructor (host only)
    Point(int id, int size) {
        this->pID = id;
        this->dataSize = size;
        xyz = (float *)calloc(3, sizeof(float));
        data = (float *)calloc(size, sizeof(float));
        // Deep copy a Point object to device
        PUSH_RANGE("Point::deep_copy",0)
        #pragma acc enter data copyin(this)
        #pragma acc enter data copyin(xyz[0:3])
        #pragma acc enter data copyin(data[0:dataSize])
        POP_RANGE
    }

    // Destructor
    ~Point() {
    }

    // Set point parameters (host only, used if empty constructor is called)
    void setPoint(int id, int size) {
        this->pID = id;
        this->dataSize = size;
        xyz = (float *)calloc(3, sizeof(float));
        data = (float *)calloc(size, sizeof(float));
        // Deep-copy a Point object to device
        PUSH_RANGE("Point::setPoint", 0);
        #pragma acc enter data copyin(this)
        #pragma acc enter data copyin(xyz[0:3])
        #pragma acc enter data copyin(data[0:dataSize])
        POP_RANGE
    }

    // Set point coordinates (host/device callable)
    void setPointCoords(float x, float y, float z) {
        xyz[0] = x;
        xyz[1] = y;
        xyz[2] = z;
    }

    // Set point data entry (host/device callable)
    void setPointDataEntry(int idx, float value) {
        data[idx] = value;
    }

    // Printer
    void print() const {
        // Only update the attributes of the host object
        PUSH_RANGE("Point::print_copyout", 0);
        #pragma acc update host(xyz[0:3])
        #pragma acc update host(data[0:dataSize])
        POP_RANGE
        std::cout << "Point ID: " << pID << ", Coordinates: (" 
                  << xyz[0] << ", " << xyz[1] << ", " << xyz[2] 
                  << "), Data Size: " << dataSize << std::endl;
        for (int i = 0; i < dataSize; ++i) {
            std::cout << "Data[" << i << "] = " << data[i] << std::endl;
        }
    }
};

// Child GaussPoint class, derived from Point
class GaussPoint : public Point
{
    private:
        float gpWeight;
    public:
        // Empty constructor
        GaussPoint() : Point(), gpWeight(0.0f) {}

        // Parametrized constructor
        GaussPoint(int id, int size, float weight) : Point(id, size) {
            gpWeight = weight;
        }

        // Destructor
        ~GaussPoint() {}

        // Setters
        void setGaussPoint(int id, int size, float weight) {
            this->setPoint(id, size);
            gpWeight = weight;
        }

        void setGaussPointWeight(float weight) {
            gpWeight = weight;
        }

        // Printer
        void print() const
        {
            PUSH_RANGE("GaussPoint::print_copyout", 0);
            #pragma acc update host(xyz[0 : 3])
            #pragma acc update host(data[0 : dataSize])
            POP_RANGE
            printf("gpID: %d, Weight: %f\n", pID, gpWeight);
            printf("Data:\n");
            for (int i = 0; i < dataSize; ++i) {
                printf("Data[%d] = %f\n", i, data[i]);
            }
        }
};

// Line class that uses an array of Points and an array of GaussPoints
class Line
{
    private:
        int lineID;
        int numPoints;
        int numGaussPoints;
        Point *points;           // Host-side empty construction of an array of objects
        GaussPoint *gaussPoints; // Host-side empty construction of an array of objects
    public:
        // Empty constructor (host only)
        Line() {
            lineID = -1;
            numPoints = -1;
            numGaussPoints = -1;
            points = nullptr;
            gaussPoints = nullptr;
        }

        // Param. constructor (host only)
        Line(int id, int np, int ngp) {
            lineID = id;
            numPoints = np;
            numGaussPoints = ngp;

            // Copy the Line object to device using self-referencing
            PUSH_RANGE("Line::constructor_copy_this", 0);
            #pragma acc enter data copyin(this[0:1])
            POP_RANGE

            points = new Point[numPoints];
            // Create an empty array of Point objects
            PUSH_RANGE("Line::constructor_create_points", 0);
            #pragma acc enter data create(points[0:numPoints])
            POP_RANGE

            gaussPoints = new GaussPoint[numGaussPoints];
            // Create an empty array of GaussPoint objects
            PUSH_RANGE("Line::constructor_create_gaussPoints", 0);
            #pragma acc enter data create(gaussPoints[0 : numGaussPoints])
            POP_RANGE

            // Fill up the Point objects
            PUSH_RANGE("Line::constructor_fill_points", 0);
            for (int i = 0; i < numPoints; ++i) {
                int pid = lineID * numPoints + i;
                points[i].setPoint(pid, 5); // Each point has 5 data entries
            }
            POP_RANGE

            // Fill up the GaussPoint objects
            PUSH_RANGE("Line::constructor_fill_gaussPoints", 0);
            for (int i = 0; i < numGaussPoints; ++i) {
                gaussPoints[i].setGaussPoint(i, 5, 0.577f); // Each Gauss point has 5 data entries and a weight
            }
            POP_RANGE
        }

        // Destructor (host only)
        ~Line() {
        }

        // Setters

        // Set the line in case of empty construction (host only)
        void setLine(int id, int np, int ngp) {
            lineID = id;
            numPoints = np;
            numGaussPoints = ngp;

            PUSH_RANGE("Line::setLine_copy_this", 0);
            #pragma acc enter data copyin(this[0:1])
            POP_RANGE

            points = new Point[numPoints];
            PUSH_RANGE("Line::setLine_create_points", 0);
            #pragma acc enter data create(points[0 : numPoints])
            POP_RANGE

            gaussPoints = new GaussPoint[numGaussPoints];
            PUSH_RANGE("Line::setLine_create_gaussPoints", 0);
            #pragma acc enter data create(gaussPoints[0 : numGaussPoints])
            POP_RANGE

            // Fill up the GaussPoint objects
            PUSH_RANGE("Line::setLine_fill_gaussPoints", 0);
            for (int i = 0; i < numGaussPoints; ++i) {
                gaussPoints[i].setGaussPoint(i, 5, 0.577f); // Each Gauss point has 5 data entries and a weight
            }
            POP_RANGE

            // Fill up the Point objects
            PUSH_RANGE("Line::setLine_fill_points", 0);
            for (int i = 0; i < numPoints; ++i) {
                int pid = lineID * numPoints + i;
                points[i].setPoint(pid, 5); // Each point has 5 data entries
            }
            POP_RANGE
        }

        // Modify data entry of a Point object
        void modifyPointDataEntry(int pointIndex, int dataIdx, float value) {
            points[pointIndex].setPointDataEntry(dataIdx, value);
        }

        // Modify data entry of a GaussPoint object
        void modifyGaussPointDataEntry(int gaussIndex, int dataIdx, float value) {
            gaussPoints[gaussIndex].setPointDataEntry(dataIdx, value);
        }

        // Print all Point objects in a Line
        void printPoints() const {
            for (int i = 0; i < numPoints; ++i) {
                points[i].print();
            }
        }

        // Print all GaussPoint objects in a Line
        void printGaussPoints() const {
            for (int i = 0; i < numGaussPoints; ++i) {
                gaussPoints[i].print();
            }
        }
};

int main() {

    // Create an array of Lines
    PUSH_RANGE("main::create_lines", 0);
    Line* lines = new Line[3];
    #pragma acc enter data create(lines[0:3])
    for (int i = 0; i < 3; ++i) {
        lines[i].setLine(i, 3, 2); // Each line has 3 points and 2 Gauss points
    }
    POP_RANGE

    PUSH_RANGE("main::parallel_loop", 0);
    #pragma acc parallel loop gang
    for (int iline = 0; iline < 3; ++iline) {
        #pragma acc loop vector
        for (int i = 0; i < 3; ++i) {
            lines[iline].modifyPointDataEntry(i, 0, static_cast<float>(i * 1.5)); // Set some data for each point
        }
        #pragma acc loop vector
        for (int i = 0; i < 2; ++i) {
            lines[iline].modifyGaussPointDataEntry(i, 1, static_cast<float>((i+1) * 2.5)); // Set some data for each Gauss point
        }
    }
    POP_RANGE

    // Print each line
    PUSH_RANGE("main::print_lines", 0);
    std::cout << "Lines: " << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Line ID: " << i << std::endl;
        lines[i].printPoints();
        lines[i].printGaussPoints();
    }
    POP_RANGE

    return 0;

}