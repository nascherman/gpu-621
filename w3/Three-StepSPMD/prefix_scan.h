#pragma once
#include <omp.h>
// Workshop 3 - Prefix Scan
// Tiled Serial Case
// prefix_scan.h

template <typename T, typename C>
void incl_scan(
	const T* in, // source data
	T* out,      // output data
	int size,    // size of source, output data sets
	C combine,   // combine expression
	T initial    // initial value
)
{
	for (int i = 0; i < size; i++) {
		initial = combine(initial, in[i]);
		out[i] = initial;
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
	if (size > 0) {
		for (int i = 0; i < size - 1; i++) {
			out[i] = initial;
			initial = combine(initial, in[i]);
		}
		out[size - 1] = initial;
	}
}

/*template <typename T, typename C, typename S>
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
}*/

template <typename T, typename C>
T reduce(
	const T* in, // points to the data set
	int n,       // number of elements in the data set
	C combine,   // combine operation
	T initial    // initial value
) {

	for (int i = 0; i < n; i++)
		initial = combine(initial, in[i]);
	return initial;
}

template <typename T, typename R, typename C, typename S>
int scan(
	const T* in,   // source data
	T* out,        // output data
	int size,      // size of source, output data sets
	R reduce,      // reduction expression
	C combine,     // combine expression
	S scan_fn,     // scan function (exclusive or inclusive)
	T initial      // initial value
)
{
	const int tile_size = 2;
	int nthreads = 1;
	if (size > 0) {
		// requested number of tiles
		int ntiles = 0;
		int max_tiles = omp_get_max_threads();
		T* reduced = new T[max_tiles];
		T* scanRes = new T[max_tiles];
#pragma omp parallel 
		{
			ntiles = omp_get_num_threads();
			int itile = omp_get_thread_num();

			if (itile == 0) nthreads = ntiles;

			int last_tile = ntiles - 1;
			int last_tile_size = size - last_tile * tile_size;


			// step 1 - reduce each tile separately
			for (int itile = 0; itile < ntiles; itile++)
				reduced[itile] = reduce(in + itile * tile_size,
					itile == last_tile ? last_tile_size : tile_size, combine, T(0));
#pragma omp barrier
			// step 2 - perform exclusive scan on all tiles using reduction outputs 
			// store results in scanRes[]
#pragma omp single
			excl_scan(reduced, scanRes, ntiles, combine, T(0));


			// step 3 - scan each tile separately using scanRes[]
			for (int itile = 0; itile < ntiles; itile++)
				scan_fn(in + itile * tile_size, out + itile * tile_size,
					itile == last_tile ? last_tile_size : tile_size, combine,
					scanRes[itile]);

		}
		delete[] reduced;
		delete[] scanRes;
	}
	return nthreads;
}
