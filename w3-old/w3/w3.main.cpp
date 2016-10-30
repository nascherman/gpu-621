// Workshop 3 - Prefix Scan
// Prefix Scan Solution
// w3.main.cpp

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <omp.h>
#include <string>
#include "prefix_scan.h"
using namespace std::chrono;  // comment out for Tiled, SPMD, and Work Sharing 
#include <random>
#include <time.h>

#define MIN_RAND 0
#define MAX_RAND 9

std::mt19937 rng(time(NULL));
std::uniform_int_distribution<int> gen(MIN_RAND, MAX_RAND); // uniform, unbiased

// report system time
//
void reportTime(const char* msg, steady_clock::duration span, int iterations) {
  auto ms = duration_cast<milliseconds>(span);
  std::cout << msg << " - took an average of - " <<
    ms.count()/iterations << " millisecond" << std::endl;
}

int main(int argc, char** argv) {
  if (argc > 3) {
    std::cerr << argv[0] << ": invalid number of arguments\n";
    std::cerr << "Usage: " << argv[0] << "\n";
    std::cerr << "Usage: " << argv[0] << "  power_of_2\n";
    return 1;
  }

  steady_clock::duration aggregate = steady_clock::duration::zero();
  steady_clock::time_point ts, te;

  const char* report;
  std::string report_string;

  // initial values for testing
  const int N = 9;
  int iterations = 0;
  const int in_[N] = { 3, 1, 7, 0, 1, 4, 5, 9, 2 };
  // command line arguments - none for testing, 1 for large arrays
  int n, nt;
  if (argc == 1) {
    n = N;
  }
  else if (argc == 3){
    n = std::atoi(argv[1]);
    if (n < N) n = N;
    iterations = std::atoi(argv[2]);
  }
  else {
    n = std::atoi(argv[1]);
    if (n < N) n = N;
    iterations = 1;
  }

  std::cout << "Size of scan is " <<  n << std::endl;

  int* in = new int[n];
  int* out = new int[n];

  // initialize
  #pragma omp for
  for (int i = 0; i < N; i++)
    in[i] = in_[i];
  #pragma omp barrier

  #pragma omp for
  for (int i = N; i < n; i++) {
    int r = (int) gen(rng);
    in[i] = r;
  }
  #pragma omp barrier 
  auto add = [](int a, int b) { return a + b; };

#ifdef SERIAL
  for(int i = 0; i < iterations; i++) {
    ts = steady_clock::now();
    nt = scan(in, out, n,
#ifndef SHORT_CALL
    reduce<int, decltype(add)>,
#endif
      add, incl_scan<int, decltype(add)>, (int)0);
    te = steady_clock::now();
   
    aggregate += te - ts;
  }
  
  report_string = "Inclusive Prefix Scan over " + std::to_string(iterations) + " iterations ";
  report = report_string.c_str();
  reportTime(report, aggregate, iterations);
#endif
  delete[] out;
  out = new int[n];
  aggregate = steady_clock::duration::zero();

#ifndef SERIAL
  for(int i = 0; i < iterations; i++) {
    ts = steady_clock::now();
#ifndef BALANCED
    nt = scan_parallel(in, out, n,
#ifndef SHORT_CALL
    reduce<int, decltype(add)>,
#endif
    add, incl_scan<int, decltype(add)>, (int)0);
    te = steady_clock::now();
    aggregate += te - ts;
#endif
#ifdef BALANCED
    nt = scan(in, out, n, 
      add, incl_scan_parallel<int, decltype(add)>, (int)0);
    te = steady_clock::now();
    aggregate += te - ts;
#endif
  }

  std::cout << nt << " threads" << std::endl;
  //for (int i = 0; i < N; i++)
  //  std::cout << out[i] << ' ';
  //std::cout << out[n - 1] << std::endl;
  report_string = "Inclusive Prefix Scan Parallel over " + std::to_string(iterations) + " iterations ";
  report = report_string.c_str();
  reportTime(report, aggregate, iterations);
#endif

  delete[] out;
  out = new int[n];
#ifdef SERIAL
  aggregate = steady_clock::duration::zero();
  for(int i = 0; i < iterations; i++) {
    ts = steady_clock::now();
    nt = scan(in, out, n,
#ifndef SHORT_CALL
      reduce<int, decltype(add)>,
#endif
      add, excl_scan<int, decltype(add)>, (int)0);
    te = steady_clock::now();
    aggregate += te - ts;
  }
  
  std::cout << nt << " threads" << std::endl;
  // for (int i = 0; i < N; i++)
  //   std::cout << out[i] << ' ';
  // std::cout << out[n - 1] << std::endl;
  report_string = "Exclusive Prefix Scan over " + std::to_string(iterations) + " iterations ";
  report = report_string.c_str();
  reportTime(report, aggregate, iterations);
#endif
  delete[] out;
  out = new int[n];

#ifndef SERIAL
  aggregate = steady_clock::duration::zero();
  for(int i = 0; i < iterations; i++) {
    ts = steady_clock::now();
#ifndef BALANCED
    nt = scan_parallel(in, out, n,
#ifndef SHORT_CALL
    reduce<int, decltype(add)>,
#endif
    add, excl_scan<int, decltype(add)>, (int)0);
    te = steady_clock::now();
    aggregate += te - ts;
#endif
#ifdef BALANCED
    nt = scan(in, out, n, 
      add, excl_scan_parallel<int, decltype(add)>, (int)0);
      te = steady_clock::now();
      aggregate += te - ts;
#endif 
  }
  
  std::cout << nt << " threads" << std::endl;
  // for (int i = 0; i < N; i++)
  //   std::cout << out[i] << ' ';
  // std::cout << out[n - 1] << std::endl;
  report_string = "Exclusive Prefix Scan Parallel over " + std::to_string(iterations) + " iterations ";
  report = report_string.c_str();
  reportTime(report, aggregate, iterations);
#endif

  delete[] in;
  delete[] out;
  // terminate
  return 0;
}