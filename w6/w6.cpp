// Workshop 6 - Threading and Vectorization
// w6.matmul.cpp
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>

using namespace std::chrono;

void reportTime(const char* msg, steady_clock::duration span) {
	auto ms = duration_cast<milliseconds>(span);
	std::cout << msg << " - took - " <<
		ms.count() << " milliseconds" << std::endl;
}
// matmul_0 returns the product a * b using basic serial logic
double matmul_0(const double* a, const double* b, double* c, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			double sum = 0.0;
			for (int k = 0; k < n; k++)
				sum += a[i * n + k] * b[k * n + j];
			c[i * n + j] = sum;
		}
	}
	double diag = 0.0;
	for (int i = 0; i < n; i++)
		diag += c[i * n + i];
	return diag;
}
// matmul_1 returns the product a * b using serial logic loops reversed 
double matmul_1(const double* a, const double* b, double* c, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) 
			c[i * n + j] = 0;	
	}

	for (int i = 0; i < n; i++) {
		for (int k = 0; k < n; k++) {
			for (int j = 0; j < n; j++)
				c[i * n + j] += a[i * n + k] * b[k * n + j];
		}
	}
	double diag = 0.0;
	for (int i = 0; i < n; i++) {
		diag += c[i * n + i];
	}
	return diag;
}
// matmul_2 returns the product a * b using cilk for
double matmul_2(const double* a, const double* b, double* c, int n) {	
	cilk_for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) 
			c[i * n + j] = 0;	
	}

	cilk_for (int i = 0; i < n; i++) {
		for (int k = 0; k < n; k++) {
			for (int j = 0; j < n; j++)
				c[i * n + j] += a[i * n + k] * b[k * n + j];
		}
	}
	double diag = 0.0;
	cilk_for(int i = 0; i < n; i++) {
		diag += c[i * n + i];
	}
	return diag;
}
// matmul_3 returns the product a * b using cilk for
// and reducer hyperobject
double matmul_3(const double* a, const double* b, double* c, int n) {
	cilk_for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) 
			c[i * n + j] = 0;	
	}
	cilk_for (int i = 0; i < n; i++) {
		for (int k = 0; k < n; k++) {
			for (int j = 0; j < n; j++)
				c[i * n + j] += a[i * n + k] * b[k * n + j];
		}
	}
	cilk::reducer_opadd<double > diag(0.0);
	cilk_for(int i = 0; i < n; i++) {
		diag += c[i * n + i];
	}
	return diag.get_value();
}
// matmul_4 returns the product a * b using cilk for, reducer,
// and vectorization
double matmul_4(const double* a, const double* b, double* c, int n) {
	c[0:n*n] = 0;
	cilk_for (int i = 0; i < n; i++) {
		#pragma simd
		for (int k = 0; k < n; k++) {
			for (int j = 0; j < n; j++) {
				c[i * n + j] += a[i * n + k] * b[k * n + j];
			}
		}
	}
	cilk::reducer_opadd<double > diag(0.0);
	cilk_for(int i = 0; i < n; i++) {
		diag += c[i * n + i];
	}
	return diag.get_value();
}
int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "*** Incorrect number of arguments ***\n";
		std::cerr << "Usage: " << argv[0]
			<< " no_of_rows_or_columns\n";
		return 1;
	}
	std::cout << std::fixed << std::setprecision(2);

	// size of each matrix (len by len)
	int len = std::atoi(argv[1]);

	// allocate memory for matrices
	steady_clock::time_point ts, te;
	double diag;
	double* a, *b, *c;
	try {
		a = new double[len*len];
		b = new double[len*len];
		c = new double[len*len];
	}
	catch (std::exception& e) {
		std::cerr << "*** Failed to Allocate Memory for "
			<< len << " by " << len << "matrices" << std::endl;
		return 2;
	}

	// initialize a and b
	for (int i = 0; i < len*len; i++) {
		a[i] = 0.0;
		b[i] = 0.0;
	}
	for (int i = 0; i < len; i++) {
		a[i*len + i] = 1.0;
		b[i*len + i] = 1.0;
	}

	ts = steady_clock::now();
	diag = matmul_0(a, b, c, len);
	te = steady_clock::now();
	reportTime("w6 serial        ", te - ts);
	std::cout << len << " = " << diag << std::endl;

	ts = steady_clock::now();
	diag = matmul_1(a, b, c, len);
	te = steady_clock::now();
	reportTime("\nw6 serial reverse", te - ts);
	std::cout << len << " = " << diag << std::endl;

	ts = steady_clock::now();
	diag = matmul_2(a, b, c, len);
	te = steady_clock::now();
	reportTime("\nw6 cilk_for      ", te - ts);
	std::cout << len << " = " << diag << std::endl;

	ts = steady_clock::now();
	diag = matmul_3(a, b, c, len);
	te = steady_clock::now();
	reportTime("\nw6 cilk_for reduc", te - ts);
	std::cout << len << " = " << diag << std::endl;

	ts = steady_clock::now();
	diag = matmul_4(a, b, c, len);
	te = steady_clock::now();
	reportTime("\nw6 cilk_for simd ", te - ts);
	std::cout << len << " = " << diag << std::endl;

	// deallocate
	delete[] a;
	delete[] b;
	delete[] c;
	std::cout << std::endl;
	char ci;
	std::cin >> ci;
}