// Workshop 8 - Domain Decomposition
// based on code from LLNL tutorial mpi_heat2d.c
// Master-Worker Programming Model
// Chris Szalwinski
// Version 1.0 - 20/11/2016
// w8.cpp
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
using namespace std::chrono;

void initialize(int, int, float*);
void update(int, int, int, const float, const float, const float*, float*);
void output(int, int, const float*, const char*);

// report system time
//
void reportTime(const char* msg, steady_clock::duration span) {
    auto ms = duration_cast<nanoseconds>(span);
    std::cout << msg << " - took - " <<
        ms.count() << " nanoseconds" << std::endl;
}

// solution constants
const int NONE = 0;
const int MINPARTITIONS = 3;
const int MAXPARTITIONS = 8;

// weights
const float wx = 0.1f;
const float wy = 0.1f;

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "*** Incorrect number of arguments ***\n";
        std::cerr << "Usage: " << argv[0]
            << " no_of_partitions no-of_rows no_of_columns\n";
       return 1;
    }
    const int NITERATIONS = std::atoi(argv[4]);

    // grid size
    int nPartitions = std::atoi(argv[1]);
    if (nPartitions < MINPARTITIONS || nPartitions > MAXPARTITIONS) {
        std::cerr << "*** Number of partitions out of bounds ***\n";
        std::cerr << "Bounds: " << MINPARTITIONS - 1 << " < no_of_partitions <" 
            << MAXPARTITIONS + 1 << std::endl;
        return 2;
    }
    int nRowsTotal = std::atoi(argv[2]);
    int nColumns = std::atoi(argv[3]);
    int nRowsPerWorker = (nRowsTotal + nPartitions - 1) / nPartitions;
    int nRowsLeftOver = (nPartitions) ? nRowsTotal - (nPartitions - 1)
     * nRowsPerWorker : nRowsTotal;

    // allocate memory for decomposition
    steady_clock::time_point ts, te;
    float* x;
    try {
        // 2 * for double buffering
        x = new float[2 * nRowsTotal * nColumns];
    }
    catch (std::bad_alloc) {
        std::cerr << "*** Failed to Allocate Memory for 2 * "
            << nRowsTotal << " by " << nColumns << " grid" << std::endl;
        return 3;
    }

    // initialize first buffer x[0: nRowsTotal*nColumns)
    initialize(nRowsTotal, nColumns, x);
    // initialize second buffer x[nRowsTotal*nColumns : nRowsTotal*nColumns)
    for (int i = 0; i < nRowsTotal * nColumns; i++)
     x[nRowsTotal * nColumns + i] = 0.0f;

    // write original data to file
    output(nRowsTotal, nColumns, x, "original.dat");

    // decompose the data and iterate on each partition
    ts = steady_clock::now();
    int iz = 0;
    for (int i = 0; i < NITERATIONS; i++) {
        int rowOffset = 0;
        for (int iWorker = 0; iWorker < nPartitions; iWorker++) {
            int nRows, above, below;
            // identify the neighboring partitions
            if (iWorker == 0)
                above = NONE;
            else
                above = iWorker - 1;
            if (iWorker == nPartitions - 1) {
                nRows = nRowsLeftOver;
                below = NONE;
            }
            else {
                nRows = nRowsPerWorker;
                below = iWorker + 1;
            }
            // set row indices for updating - do not update first or last row
            int startRow = rowOffset;
            int endRow = rowOffset + nRows - 1;
            if (rowOffset == 0) startRow = 1;
            if (rowOffset + nRows == nRowsTotal) endRow--;
            update(startRow, endRow, nColumns, wx, wy,
                x + iz * nRowsTotal * nColumns,
                x + (1 - iz) * nRowsTotal * nColumns);
    //        std::cout << "Sent to worker " << iWorker << " " << nRows
    //            << " rows [row rowOffset " << rowOffset << "] above "
    //            << above << " below " << below << std::endl;
            rowOffset += nRows;
        }
        // switch buffers
        iz = 1 - iz;
    }
    te = steady_clock::now();
    reportTime("\nw8 Domain Decomposition ", te - ts);
    std::cout << "Number of Partitions " << nPartitions << std::endl;
    std::cout << "Number of Rows       " << nRowsTotal << std::endl;
    std::cout << "Number of Columns    " << nColumns << std::endl;

    // write results to file
    output(nRowsTotal, nColumns, x, "results.dat");
    // deallocate
    delete[] x;
 }

 // initialize for high value at above end middle row
 //
 void initialize(int nRowsTotal, int nColumns, float* x) {
    for (int i = 0; i < nRowsTotal; i++)
        for (int j = 0; j < nColumns; j++)
            x[i * nColumns + j] = (float)
             (i * (nRowsTotal - i - 1) * j * (nColumns - j - 1));
 }

 // update data using weighted neighboring values
 //
 void update(int startRow, int endRow, int nColumns,
    const float wx, const float wy, const float* x_old, float* x_new) {
        for (int i = startRow; i <= endRow; i++)
            for (int j = 1; j < nColumns - 1; j++)
                x_new[i * nColumns + j] = x_old[i * nColumns + j]
                + wx * (x_old[(i + 1) * nColumns + j] +
                  x_old[(i - 1) * nColumns + j] - 2.0f * x_old[i * nColumns + j])
                + wy * (x_old[i * nColumns + j + 1] +
                  x_old[i * nColumns + j - 1] - 2.0f * x_old[i * nColumns + j]);
 }

 // output data
 //
 void output(int nRowsTotal, int nColumns, const float* x,
    const char* filename) {
       std::ofstream file(filename);
       file << std::fixed << std::setprecision(1);
        for (int j = nColumns - 1; j >= 0; j--) {
            for (int i = 0; i < nRowsTotal; i++)
                file << std::setw(8) << x[i * nColumns + j];
            file << std::endl;
        }
 }