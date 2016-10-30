#pragma once
// Workshop 3 - Prefix Scan
// Balanced Tree Case
// prefix_scan.h
#include <omp.h>
#define SHORT_CALL
#define BALANCED

template <typename T, typename C>
int incl_scan(
  const T* in, // source data
  T* out,      // output data
  int size,    // size of source, output data sets
  C combine,   // combine expression
  T initial    // initial value
)
{
  // initialize
  for (int i = 0; i < size; i++)
    out[i] = in[i];
  // upsweep (reduction)
  for (int stride = 1; stride < size; stride <<= 1) {
    for (int i = 0; i < size; i += 2 * stride)
      out[2 * stride + i - 1] = combine(out[2 * stride + i - 1],
        out[stride + i - 1]);
  }
  // clear last element
  T last = out[size - 1];
  out[size - 1] = T(0);
  // downsweep
  for (int stride = size / 2; stride > 0; stride >>= 1) {
    for (int i = 0; i < size; i += 2 * stride) {
      T temp = out[stride + i - 1];
      out[stride + i - 1] = out[2 * stride + i - 1];
      out[2 * stride + i - 1] = combine(temp, out[2 * stride + i - 1]);
    }
  }
  // shift left for inclusive scan and add last
  for (int i = 0; i < size - 1; i++)
    out[i] = out[i + 1];
  out[size - 1] = last;

  return 1;
}

template <typename T, typename C>
int incl_scan_parallel(
  const T* in, // source data
  T* out,      // output data
  int size,    // size of source, output data sets
  C combine,   // combine expression
  T initial    // initial value
)
{
  int numThreads = 0;
  // initialize
  #pragma omp parallel 
  {
    numThreads = omp_get_num_threads();
    #pragma omp for
    for (int i = 0; i < size; i++)
      out[i] = in[i];
    // add inner parallell for
    // upsweep (reduction)
    for (int stride = 1; stride < size; stride <<= 1) {
      #pragma omp for
      for (int i = 0; i < size; i += 2 * stride)
        out[2 * stride + i - 1] = combine(out[2 * stride + i - 1],
          out[stride + i - 1]);
    }
    // clear last element
    T last = out[size - 1];
    out[size - 1] = T(0);
    // downsweep
    for (int stride = size / 2; stride > 0; stride >>= 1) {
      #pragma omp for
      for (int i = 0; i < size; i += 2 * stride) {
        T temp = out[stride + i - 1];
        out[stride + i - 1] = out[2 * stride + i - 1];
        out[2 * stride + i - 1] = combine(temp, out[2 * stride + i - 1]);
      }
    }
    // shift left for inclusive scan and add last
    #pragma omp for
    for (int i = 0; i < size - 1; i++)
      out[i] = out[i + 1];

    out[size - 1] = last;
  }
  return numThreads;
}

template <typename T, typename C>
int excl_scan(
  const T* in, // source data
  T* out,      // output data
  int size,    // size of source, output data sets
  C combine,   // combine expression
  T initial    // initial value
)
{
  // initialize
  for (int i = 0; i < size; i++)
    out[i] = in[i];

  // upsweep (reduction)
  for (int stride = 1; stride < size; stride <<= 1) {
    for (int i = 0; i < size; i += 2 * stride)
      out[2 * stride + i - 1] = combine(out[2 * stride + i - 1],
        out[stride + i - 1]);
  }

  // clear last element
  out[size - 1] = T(0);

  // downsweep
  for (int stride = size / 2; stride > 0; stride >>= 1) {
    for (int i = 0; i < size; i += 2 * stride) {
      T temp = out[stride + i - 1];
      out[stride + i - 1] = out[2 * stride + i - 1];
      out[2 * stride + i - 1] = combine(temp, out[2 * stride + i - 1]);
    }
  }
  return 1;
}

template <typename T, typename C>
int excl_scan_parallel(
  const T* in, // source data
  T* out,      // output data
  int size,    // size of source, output data sets
  C combine,   // combine expression
  T initial    // initial value
)
{
  int numThreads = 0;
  // initialize
  #pragma omp parallel
  {
    numThreads = omp_get_num_threads();
    #pragma omp for
    for (int i = 0; i < size; i++)
      out[i] = in[i];
    // upsweep (reduction)
    for (int stride = 1; stride < size; stride <<= 1) {
      #pragma omp for
      for (int i = 0; i < size; i += 2 * stride)
        out[2 * stride + i - 1] = combine(out[2 * stride + i - 1],
          out[stride + i - 1]);
    }
    // clear last element
    out[size - 1] = T(0);
    // downsweep
    for (int stride = size / 2; stride > 0; stride >>= 1) {
      #pragma omp for
      for (int i = 0; i < size; i += 2 * stride) {
        T temp = out[stride + i - 1];
        out[stride + i - 1] = out[2 * stride + i - 1];
        out[2 * stride + i - 1] = combine(temp, out[2 * stride + i - 1]);
      }
    }  
  }
  return numThreads;  
}

template <typename T, typename C, typename S>
int scan(
  const T* in,   // source data
  T* out,        // output data
  int size,      // size of source, output data sets
  C combine,     // combine expression
  S scan_fn,     // scan function (exclusive or inclusive)
  T initial      // initial value
)
{
  return scan_fn(in, out, size, combine, T(0));
}