// Workshop 4 - Convolution Application
// convolute.cpp
// Chris Szalwinski

#include "convolute.h"
#include "kernel.h"
#include <omp.h>

corona::Image* convolute(corona::Image* image) {
	const int width = image->getWidth();
	const int height = image->getHeight();
	int px, py;
	unsigned char* pixels = (unsigned char*)(image->getPixels());
	corona::Image* result = corona::CloneImage(image);
	unsigned char* resultPixels = (unsigned char*)(result->getPixels());
	Kernel kernel;

	for (py = 0; py < height; py++)
		for (px = 0; px < width; px++)
			(Pixel&)resultPixels[(py * width + px) * 4] =
			kernel.combine(pixels, px, py, width, height);

	return result;
}

corona::Image* convolute(corona::Image* image, const int threadsRequested, int& threadsSpawned) {
	const int width = image->getWidth();
	const int height = image->getHeight();
	int px, py;
	unsigned char* pixels = (unsigned char*)(image->getPixels());
	corona::Image* result = corona::CloneImage(image);
	unsigned char* resultPixels = (unsigned char*)(result->getPixels());
	Kernel kernel;

	// find shared and private variables declared in each loop
	// add omp set_num_threads() 
	#pragma omp parallel num_threads(threadsRequested) shared(pixels, resultPixels) private(py)
	{
		int ti = omp_get_thread_num(); 
		if(ti == 0) {
			threadsSpawned = omp_get_num_threads();
		}

		for (py = 0; py < height; py++)
			#pragma omp for
			for (px = 0; px < width; px++)
				(Pixel&)resultPixels[(py * width + px) * 4] = 
					kernel.combine(pixels, px, py, width, height);
	}
	
	return result;
}

