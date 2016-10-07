#pragma once
// Workshop 3 - Prefix Scan
// Balanced Tree Case
// prefix_scan.h
#include <omp.h>
#define SHORT_CALL
#define BALANCED

#include <iostream>

template <typename T, typename C>
void incl_scan(
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
}

template <typename T, typename C>
void incl_scan_parallel(
  const T* in, // source data
  T* out,      // output data
  int size,    // size of source, output data sets
  C combine,   // combine expression
  T initial    // initial value
)
{
  // initialize
  // parallelize
  #pragma omp parallel 
  {
    #pragma omp parallel for
    for (int i = 0; i < size; i++)
      out[i] = in[i];
    // add inner parallell for
    // upsweep (reduction)
    for (int stride = 1; stride < size; stride <<= 1) {
      #pragma omp parallel for
      for (int i = 0; i < size; i += 2 * stride)
        out[2 * stride + i - 1] = combine(out[2 * stride + i - 1],
          out[stride + i - 1]);
    }
    // clear last element
    T last = out[size - 1];
    out[size - 1] = T(0);
    // downsweep
    // add inner parallell for
    for (int stride = size / 2; stride > 0; stride >>= 1) {
      #pragma omp for
      for (int i = 0; i < size; i += 2 * stride) {
        T temp = out[stride + i - 1];
        out[stride + i - 1] = out[2 * stride + i - 1];
        out[2 * stride + i - 1] = combine(temp, out[2 * stride + i - 1]);
      }
    }
    // shift left for inclusive scan and add last
    #pragma omp parallel for
    for (int i = 0; i < size - 1; i++)
      out[i] = out[i + 1];

    out[size - 1] = last;
  }
}

template <typename T, typename C>
void excl_scan(
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
}

template <typename T, typename C>
void excl_scan_parallel(
  const T* in, // source data
  T* out,      // output data
  int size,    // size of source, output data sets
  C combine,   // combine expression
  T initial    // initial value
)
{
  // initialize
  #pragma omp parallel
  {
    #pragma omp parallel for
    for (int i = 0; i < size; i++)
      out[i] = in[i];
    // upsweep (reduction)
    for (int stride = 1; stride < size; stride <<= 1) {
      #pragma omp parallel for
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
  scan_fn(in, out, size, combine, T(0));
  return 1; // returns number of threads
}