// workshop 4 - Convolution Application
// kernel.cpp
// Chris Szalwinski

#include "kernel.h"

unsigned char bracket(int x) {
	if (x > 255) x = 255;
	else if (x < 0) x = 0;
	return (unsigned char)x;
}

int bracket(int x, int max) {
	if (x > max) x = max;
	else if (x < 0) x = 0;
	return x;
}

Pixel Kernel::combine(const unsigned char* pixels, const int px, const int py, const int tWidth, const int tHeight) {

	int accuR = 0;
	int accuG = 0;
	int accuB = 0;
	int accuA = 255;

	for (int kernelY = 0; kernelY < height; kernelY++) {

		for (int kernelX = 0; kernelX < width; kernelX++) {

			int targetPixelX = bracket(px - width / 2 + kernelX, tWidth - 1);
			int targetPixelY = bracket(py - height / 2 + kernelY, tHeight - 1);

			int pixelIndex = (targetPixelY * tWidth + targetPixelX) * 4;
			accuR += pixels[pixelIndex++] * kernel[kernelY * width + kernelX];
			accuG += pixels[pixelIndex++] * kernel[kernelY * width + kernelX];
			accuB += pixels[pixelIndex++] * kernel[kernelY * width + kernelX];
		}
	}

	return Pixel { bracket(accuR), bracket(accuG), bracket(accuB), bracket(accuA) };
}
