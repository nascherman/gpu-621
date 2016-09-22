// Workshop 1 - Platforms and Optimizations
// w1.cpp

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include <cstdlib>

#define SAMPLES 100

using namespace std::chrono;

class Version {
public:
  void operator()() const {
    std::cout << "Hello from the ";
    #if defined(__GNUC__)
        std::cout << "gnu compiler: ";
        std::cout << __GNUC__ + "\n";
    #elif defined (__INTEL_COMPILER)
        std::cout << "Intel compiler: ";
        std::cout << __INTEL_COMPILER << std::endl;
    #elif defined(_MSC_VER)
        std::cout << "Visual Studio compiler: ";
        std::cout << _MSC_FULL_VER << std::endl;
    #else
      std::cout << "unknown compiler: ";
    #endif
  }
};

// report system time
void reportTime(const char* msg, double ms, int samples) {
  std::cout << msg << " - took an average of - " <<
    ms/samples << " milliseconds over " << samples << " iterations." << std::endl;
}

double magnitude(const double* x, int n) {
  double sum = 0.0;
  for (int i = 0; i < n; i++)
    sum += x[i] * x[i];
  return sqrt(sum);
}

void print(int n, int samples) {
  double aggregate = 0;
  double averageMagnitude = 0;
  
  for (int i = 0; i < samples; i++) {
    steady_clock::time_point ts, te;
    
    ts = steady_clock::now();
    double* a = new double[n];
    
    for (int i = 0; i < n; i++)
      a[i] = 1.0;
    te = steady_clock::now();
    ts = steady_clock::now();
    double length = magnitude(a, n);
    averageMagnitude += length;
    te = steady_clock::now();
  
    auto duration = duration_cast<milliseconds>(te - ts);
    aggregate += duration.count();

    delete[] a;
  }
  std::cout << " average magnitude = " << averageMagnitude / samples << std::endl;
   reportTime(" - magnitude calculation", aggregate, samples);
}

int main(int argc, char* argv[]) {
  Version version;
  version();
  if (argc != 1) {
    std::cerr << argv[0] << ": invalid number of arguments\n";
    std::cerr << "Usage: " << argv[0] << "  no_of_elements\n";
    return 1;
  }
  
  int iter [] = { 5000000, 10000000, 15000000 };

  for (auto i = 0; i < (sizeof(iter) / 3) - 1; i++) {
    print(iter[i], SAMPLES);
  }

  // terminate
  char c;
  std::cout << "Press Enter key to exit ... ";
  std::cin.get(c);
}
