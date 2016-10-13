#pragma once
// Workshop 4 - Convolution Application
// kernel.h
// Chris Szalwinski

struct Pixel {
	unsigned char accuR;
	unsigned char accuG;
	unsigned char accuB;
	unsigned char accuA;
};

class Kernel {
	int kernel[81] {
	 0, -1, -1, -2, -2, -2, -1, -1, 0,
	-1, -2, -4, -5, -5, -5, -4, -2, -1,
	-1, -4, -5, -3, -0, -3, -5, -4, -1,
	-2, -5, -3, 12, 24, 12, -3, -5, -2,
	-2, -5, -0, 24, 40, 24, -0, -5, -2,
	-2, -5, -3, 12, 24, 12, -3, -5, -2,
	-1, -4, -5, -3, -0, -3, -5, -4, -1,
	-1, -2, -4, -5, -5, -5, -4, -2, -1,
	 0, -1, -1, -2, -2, -2, -1, -1,  0 };
	int width  { 9 };
	int height { 9 };
  public:
	  Pixel combine(const unsigned char*, const int, const int, const int, const int);
};
