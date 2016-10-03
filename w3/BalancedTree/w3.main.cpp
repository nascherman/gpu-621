// Workshop 3 - Prefix Scan
// Prefix Scan Solution
// w3.main.cpp

#include <iostream>
#include <cstdlib>
#include <chrono>
#include "prefix_scan.h"
using namespace std::chrono;
#define SHORT_CALL  // comment out for Tiled, SPMD, and Work Sharing 

// report system time
//
void reportTime(const char* msg, steady_clock::duration span) {
	auto ms = duration_cast<milliseconds>(span);
	std::cout << msg << " - took - " <<
		ms.count() << " milliseconds" << std::endl;
}

int main(int argc, char** argv) {
	if (argc > 2) {
		std::cerr << argv[0] << ": invalid number of arguments\n";
		std::cerr << "Usage: " << argv[0] << "\n";
		std::cerr << "Usage: " << argv[0] << "  power_of_2\n";
		return 1;
	}

	// initial values for testing
	const int N = 9;
	const int in_[N] = { 3, 1, 7, 0, 1, 4, 5, 9, 2 };

	// command line arguments - none for testing, 1 for large arrays
	int n, nt;
	if (argc == 1) {
		n = N;
	}
	else {
		n = 1 << std::atoi(argv[1]);
		if (n < N) n = N;
	}
	int* in = new int[n];
	int* out = new int[n];

	// initialize
	for (int i = 0; i < N; i++)
		in[i] = in_[i];
	for (int i = N; i < n; i++)
		in[i] = 1;
	auto add = [](int a, int b) { return a + b; };

	steady_clock::time_point ts, te;
	ts = steady_clock::now();
	nt = scan(in, out, n,
#ifndef SHORT_CALL
		reduce<int, decltype(add)>,
#endif
		add, incl_scan<int, decltype(add)>, (int)0);
	te = steady_clock::now();

	std::cout << nt << " threads" << std::endl;
	for (int i = 0; i < N; i++)
		std::cout << out[i] << ' ';
	std::cout << out[n - 1] << std::endl;

	reportTime("Inclusive Prefix Scan", te - ts);

	ts = steady_clock::now();
	nt = scan(in, out, n,
#ifndef SHORT_CALL
		reduce<int, decltype(add)>,
#endif
		add, excl_scan<int, decltype(add)>, (int)0);
	te = steady_clock::now();

	std::cout << nt << " threads" << std::endl;
	for (int i = 0; i < N; i++)
		std::cout << out[i] << ' ';
	std::cout << out[n - 1] << std::endl;

	reportTime("Exclusive Prefix Scan", te - ts);

	delete[] in;
	delete[] out;

	// terminate
	char c;
	std::cout << "Press Enter key to exit ... ";
	std::cin.get(c);
}