 // Workshop 2 - Calculate PI by integrating 1/(1+x^2)
 // w2.serial.cpp
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <array>
using namespace std::chrono;

std::array<double,  2> getPiSerial(int n) {
    int nthreads;
    std::array<double, 2> results;
    double runtime = 0;
    steady_clock::time_point ts, te;
    // calculate pi by integrating the area under 1/(1 + x^2) in n steps 
    ts = steady_clock::now();
    double x, pi, sum = 0.0;
    double stepSize = 1.0 / (double) n;
    for (int i = 0; i < n; i++) {
        x = ((double)i + 0.5) * stepSize;
        sum += 1.0 / (1.0 + x * x);
    }
    pi = 4.0 * sum * stepSize;
    te = steady_clock::now();
    auto duration = duration_cast<milliseconds>(te - ts);
    runtime = duration.count();
    results = { pi, runtime };
    return results;
}