// Workshop 4 - Convolution Application
// w4_main.cpp
// Chris Szalwinski

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <chrono>
#include "convolute.h"
using namespace std::chrono;

const int MAX_NUM_THREADS = 16;

// report system time
//
void reportTime(const std::string& msg, steady_clock::duration span) {
	auto ms = duration_cast<milliseconds>(span);
	std::cout << msg << ms.count() << " milliseconds" << std::endl;
}

int main(int argc, char* argv[]) {
	if (argc > 2) {
		std::cerr << argv[0] << ": invalid number of arguments\n";
		std::cerr << "Usage: " << argv[0] << "  name of image file\n";
		return 1;
	}

	// Open the file named on the command line
	corona::Image* image = corona::OpenImage(argv[1], corona::FF_AUTODETECT, corona::PF_R8G8B8A8);
	if (!image) {
		std::cerr << "Failed to open file named: " << argv[1] << "\n";
		return 2;
	}
	// extract the file descriptors	
	int width    = image->getWidth();
	int height   = image->getHeight();
	void* pixels = image->getPixels();

	int np = 0;
	std::stringstream msg;
	steady_clock::time_point ts, te;

	// Create Reference Case
	ts = steady_clock::now();
	corona::Image* resultImage = convolute(image);
	te = steady_clock::now();
	reportTime("Serial processing time = ", te - ts);
	// Draw the reference convoluted image
	corona::SaveImage("Serial.png", corona::FF_AUTODETECT, resultImage);

	// Generate Multi-Threading Cases
	for (int npr = 1; npr <= MAX_NUM_THREADS; npr++) {
		ts = steady_clock::now();
		resultImage = convolute(image, npr, np);
		te = steady_clock::now();
		msg << "Processing time (" << npr << "," << np << " processor(s)) = ";
		reportTime(msg.str(), te - ts);
		msg.flush();
		msg.seekp(0);
	}
	// Draw the last convoluted image
	corona::SaveImage("MultiThreaded.png", corona::FF_AUTODETECT, resultImage);
}
