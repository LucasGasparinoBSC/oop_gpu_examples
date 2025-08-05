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

const uint32_t colors[] = { 0xff00ff00, 0xff0000ff, 0xffffff00, 0xffff00ff, 0xff00ffff, 0xffff0000, 0xffffffff };
const int num_colors = sizeof(colors)/sizeof(uint32_t);

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
#define POP_RANGE nvtxRangePop();

// Point class containing an id and a dynamic float array
class Point
{
    private:
        int pID;
        int dataSize;
        float* pData;
    public:
        int getId() const { return pID; }
        int getDataSize() const { return dataSize; }
        float* getData() const { return pData; }
        void setData(float* data) { pData = data; }
        void setDataEntry(int i, float value) {
            pData[i] = value;
        }
        void setPoint(int id, int n) {
            this->pID = id;
            this->dataSize = n;
            this->pData = (float*)calloc(n, sizeof(float));
            PUSH_RANGE("Point::setPoint_copyin", pID);
            #pragma acc enter data copyin(pData[0:dataSize])
            POP_RANGE
        }
        void setPoint(int id, float* data) {
            this->pID = id;
            this->pData = data;
            PUSH_RANGE("Point::setPoint_copyin", pID);
            #pragma acc enter data copyin(pData[0:dataSize])
            POP_RANGE
            this->dataSize = sizeof(data) / sizeof(float);
        }
        void print() const {
            printf("Point ID: %d, Data Size: %d, Data: ", pID, dataSize);
            for (int i = 0; i < dataSize; ++i) {
                printf("%f ", pData[i]);
            }
            printf("\n");
        }
};

// Line class contains an array of Point objects
class Line
{
    private:
        int lID;
        int nPoints;
        Point* points;
    public:
        int getId() const { return lID; }
        int getNPoints() const { return nPoints; }
        Point getPoint(int pid) const { return points[pid]; }
        Point* getPoints() const { return points; }
        void setPoints(Point* pts) { points = pts; }
        void setLine(int id, int np, int ndata) {
            this->lID = id;
            this->nPoints = np;
            this->points = (Point*)calloc(np, sizeof(Point));
            PUSH_RANGE("Line::setLine_copyin", lID);
            #pragma acc enter data copyin(points[0:nPoints])
            POP_RANGE
            for (int i = 0; i < np; ++i) {
                int pid = lID * np + i;
                points[i].setPoint(pid, ndata);
            }
        }
        void setLine(int id, Point* pts) {
            this->lID = id;
            this->points = pts;
            PUSH_RANGE("Line::setLine_copyin", lID);
            #pragma acc enter data copyin(points[0:nPoints])
            POP_RANGE
            this->nPoints = sizeof(pts) / sizeof(Point);
        }
        void print() const {
            printf("Line ID: %d, Number of Points: %d\n", lID, nPoints);
            for (int i = 0; i < nPoints; ++i) {
                points[i].print();
            }
            printf("\n");
        }
};

int main(int argc, const char** argv)
{
    // Problem parameters
    const int np = 32;     // Points per line
    const int ndata = 4;  // Data entries per point
    const int nlines = 10; // Number of lines

    // Allocate array of Line objects on the host
    PUSH_RANGE("main::allocate_lines", 0);
    Line* lines = (Line*)calloc(nlines, sizeof(Line));
    #pragma acc enter data copyin(lines[0:nlines])
    POP_RANGE

    // Initialize each Line object
    PUSH_RANGE("main::initialize_lines", 0);
    for (int i = 0; i < nlines; ++i) {
        lines[i].setLine(i, np, ndata);
        lines[i].print();
    }
    POP_RANGE

    PUSH_RANGE("main::kernel_1", 0);
    #pragma acc parallel loop present(lines[0:nlines])
    for (int i = 0; i < nlines; ++i) {
        Point* points = lines[i].getPoints();
        #pragma acc loop
        for (int j = 0; j < np; ++j) {
            float* data = points[j].getData();
            for (int k = 0; k < ndata; ++k) {
                data[k] += static_cast<float>(1);
            }
        }
    }
    POP_RANGE

    PUSH_RANGE("main::kernel_2", 0);
    #pragma acc parallel loop present(lines[0:nlines])
    for (int i = 0; i < nlines; ++i) {
        Point* points = lines[i].getPoints();
        #pragma acc loop vector
        for (int j = 0; j < np; ++j) {
            float* data = points[j].getData();
            #pragma acc loop seq
            for (int k = 0; k < ndata; ++k) {
                data[k] += static_cast<float>(i+j+k);
            }
        }
    }
    POP_RANGE

    PUSH_RANGE("main::copy_back", 0);
    for (int i = 0; i < nlines; ++i) {
        Point* points = lines[i].getPoints();
        for (int j = 0; j < np; ++j) {
            float* data = points[j].getData();
            #pragma acc update self(data[0:ndata])
        }
    }
    POP_RANGE

    // Print the final state of each Line object
    for (int i = 0; i < nlines; ++i) {
        lines[i].print();
    }

   return 0;
}