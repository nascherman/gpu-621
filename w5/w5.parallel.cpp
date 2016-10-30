 // Workshop 5 - Word Count Algorithm
 // w5.parallel.cpp
#include <iostream>
#include <fstream>
#include <cstring>
#include <chrono>
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
    void operator()() {
       for (int i = 0; i < len; i++) {
           if (!delimiter(str[i])) {
               int s = 0;
               while (i + s < len && !delimiter(str[i + s])) s++;
               size[i] = s;
               int n = 0;
               for (int j = i + s + 1; j + s < len; j++) {
                   bool bad = false;
                   for (int k = 0;
                    k < s && k + i < len && k + j < len; k++) {
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
};

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
    Body body(str, len, size, numb);
    ts = steady_clock::now();
    tbb::parallel_scan(tbb:blocked_range<int(0, len), body);
    te = steady_clock::now();
    // remove duplicate words
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
    int maxpos  = 0;
    for (int i = 0; i < len; ++i) {
        if (size[i]) {
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
    } else {
        std::cout << "no word repetitions found" << std::endl;
    }
    reportTime(argv[1], te - ts);
    // deallocate
    delete[] size;
    delete[] numb;
}
