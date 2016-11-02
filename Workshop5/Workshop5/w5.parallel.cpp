// Workshop 5 - Word Count Algorithm
// w5.parallel.cpp
#include <iostream>
#include <fstream>
#include <cstring>
#include <tbb/tbb.h>
#include <chrono>
#include <stdlib.h>

#define GRAIN_SIZE 1

using namespace std::chrono;
// report system time
void reportTime(const char* msg, steady_clock::duration span) {
	auto ms = duration_cast<milliseconds>(span);
	std::cout << msg << " - took - " <<
		ms.count() << " milliseconds" << std::endl;
}
// delimiter returns true if s is a delimiter
bool delimiter(char s) {
	return s == ' ' || s == '.' || s == ',' || s == '\n';
}

class Body {
	const  std::string& str;
	int len;
	int * size;
	int * numb;
public:
	Body(const std::string& s, int l, int* sz, int* n) : str(s), size(sz), len(l), numb(n) {}
	Body(Body& b, tbb::split) : str(b.str), size(b.size), len(b.len), numb(b.numb) {}
	template<typename Tag>
	void operator()(const tbb::blocked_range<int>& r, Tag) {
		for (int i = r.begin(); i < r.end(); i++) {
			if (!delimiter(str[i])) {
				unsigned int s = 0;
				while (i + s < r.end() && !delimiter(str[i + s])) {
		///			std::cout << str[i + s] << " String at index " << std::endl;
					s++;
				}
				size[i] = s;
	//			std::cout << "Size at " << i << " : " << size[i] << std::endl;
				unsigned int n = 0;
				for (int j = i + s + 1; j + s < r.end(); j++) {
					bool bad = false;
					for (int k = r.begin();
						k < s && k + i < r.end() && k + j < r.end(); k++) {
						if (str[i + k] != str[j + k]) {
							bad = true;
							break;
						}
					}
					if (!bad && delimiter(str[j + s])) n++;
				}
				numb[i] = n;
			}
			else {
				size[i] = 0;
				numb[i] = 0;
			}
			i += size[i];
		}
	}
	void reverse_join(Body& a) { 
		for (int i = 0; i < len; i++) {
			numb[i] += a.numb[i];
			size[i] += a.size[i];
		}
	}
	void assign(Body& b) { 
		for (int i = 0; i < len; i++) {
			numb[i] = b.numb[i];
			size[i] = b.size[i];
		}
	}
};

// template<typename T, typename C>
void scan(
	const std::string& str,
	int len,
	int* size,
	int* numb
)
{
	Body body(str, len, size, numb);
	size_t grainsize = GRAIN_SIZE;
	tbb::parallel_scan(tbb::blocked_range<int>(0, len, grainsize), body, tbb::simple_partitioner());
}

int main(int argc, char** argv) {
	
	if (argc != 2) {
		std::cerr << "*** Incorrect number of arguments ***\n";
		std::cerr << "Usage: " << argv[0] << " filename\n";
		return 1;
	}
	// load text from file to string
	std::ifstream file(argv[1]);
	std::string str;
	file.seekg(0, std::ios::end);
	// request a change in capacity
	str.reserve((unsigned)file.tellg());
	file.seekg(0, std::ios::beg);
	str.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	// allocate memory for analysis
	steady_clock::time_point ts, te;
	int len = str.size();
	int* size = new int[len];
	int* numb = new int[len];
	//Body body(str, len, size, numb);
	ts = steady_clock::now();
	scan(str, len, size, numb);
	// tbb::parallel_scan(tbb::blocked_range<int>(0, len, grainsize), body, tbb::simple_partitioner());
	te = steady_clock::now();
	std::cout << "Completed parallel scan " << std::endl;
	// remove duplicate words
	std::cout << "remove duplicates " << std::endl;
	for (int i = 0; i < len; i++) {
		if (size[i]) {
			for (int j = i + 1; j < len; j++) {
				if (size[j] == size[i]) {
					bool duplicate = true;
					for (int k = 0; k < size[i]; k++) {
						if (str[i + k] != str[j + k]) {
							duplicate = false;
							break;
						}
					}
					if (duplicate) {
						numb[j] = 0;
					}
				}
			}
		}
	}
	// determine longest word
	int maxsize = 0;
	int maxpos = 0;
	std::cout << "determining longest word" << std::endl;
	for (int i = 0; i < len; ++i) {
		if (size[i] & size[i] > 0) {
			if (numb[i] != 0) {
				std::cout << numb[i] << " * ";
				for (int j = 0; j < size[i]; ++j)
					std::cout << str[i + j];
				std::cout << std::endl;
				if (size[i] > maxsize) {
					maxsize = size[i];
					maxpos = i;
				}
			}
			i += size[i];
		}
	}
	// report the longest word
	if (maxsize) {
		std::cout << "\nLongest Word\n";
		std::cout << numb[maxpos] << " * ";
		for (int k = 0; k < maxsize; k++)
			std::cout << str[maxpos + k];
		std::cout << std::endl;
	}
	else {
		std::cout << "no word repetitions found" << std::endl;
	}
	reportTime(argv[1], te - ts);
	// deallocate
	delete[] size;
	delete[] numb;

	char c;
	std::cin >> c;
}