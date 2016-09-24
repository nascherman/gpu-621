#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <omp.h>
#include <array>

using namespace std::chrono;

std::array<double,  2> getPiExclusion(int n, int threads) {
  int nthreads;
  std::array<double, 2> results;
  double runtime =  0;
  steady_clock::time_point ts, te;
  // calculate pi by integrating the area under 1/(1 + x^2) in n steps 
  double x, pi, sum = 0.0;
  double stepSize = 1.0 / (double) n;
  ts = steady_clock::now();
  #pragma omp parallel num_threads(threads)
  {
     int tid = omp_get_thread_num();
     int nt = omp_get_num_threads();
     if (tid == 0) {
      nthreads = nt;
     }
     double ss = 0.0;
     for (int i = tid; i < n; i += nt) {
      x = ((double)i + 0.5) * stepSize;
      ss += 1.0 / (1.0 + x * x);
     }
     #pragma omp critical 
     sum += ss;
  }

  pi = 4.0 * sum * stepSize;
  te = steady_clock::now();
  auto duration = duration_cast<milliseconds>(te - ts);
  runtime = duration.count();
  results = { pi, runtime };
  return results;
}
