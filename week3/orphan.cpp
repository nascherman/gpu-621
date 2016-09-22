/******************************************************************************
* FILE: omp_orphan.cpp
* DESCRIPTION:
*   OpenMP Example - Parallel region with an orphaned directive - C++ Version
*   This example demonstrates a dot product  being performed by an orphaned
*   loop reduction construct.  Scoping of the reduction variable is critical.
* AUTHOR: Blaise Barney  5/99
* LAST REVISED: 06/30/05
* https://computing.llnl.gov/tutorials/openMP/samples/C/omp_orphan.c
* MODIFIED: Chris Szalwinski 1/15
******************************************************************************/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <omp.h>

//#define PRINTF

float sum = 0.0f;

void dotprod(const float* a, const float* b, int n) {
    int tid;

    tid = omp_get_thread_num();
    #pragma omp for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum = sum + (a[i]*b[i]);
        #ifndef PRINTF
        std::cout << "  tid= " << tid << " i= " << i << std::endl;
        #else
        std::printf("  tid= %d i= %d\n", tid, i);
        #endif
    }
}

int main (int argc, char *argv[]) {
    std::cout << "\nCommand Line : ";
    std::cout << omp_get_max_threads();
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i] << ' ';
    }
    std::cout << std::endl;
    if (argc != 2) {
        std::cerr << "\n*** Incorrect number of arguments ***\n";
        std::cerr << "\nomp_orphan vector_size\n";
        return 1;
    }
    int n = std::atoi(argv[1]);
    float* a = new float[n];
    float* b = new float[n];

    for (int i = 0; i < n; i++)
        a[i] = b[i] = 1.0 * i;

    #pragma omp parallel
    dotprod(a, b, n);

    #ifndef PRINTF
    std::cout << "Sum = " << sum << std::endl;
    #else
    std::printf("Sum = %f\n", sum);
    #endif
    delete [] a;
    delete [] b;
}
