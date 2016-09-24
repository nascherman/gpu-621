#include <cstdlib>
#include <iostream>
#include <array>
#include <fstream>
#include <iomanip>
#include "header/w2.exclusion.h"
#include "header/w2.naive.h"
#include "header/w2.padded.h"
#include "header/w2.serial.h"
#include <string>

using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) {
      std::cerr << argv[0] << ": invalid number of arguments\n"; 
      std::cerr << "Usage: " << argv[0] << "  type (padded/exclusion/naive)\n"; 
      return 1;
  }
  string type = argv[1];

  int slices = 100000000;
  int threads [] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  array<double, 2> resultSerial = getPiSerial(slices);  
  cout << "Results\n=======\n";
  cout << "Slices = " << slices << "\n\n";
  cout << std::setprecision(20) << "Serial result " << 
    resultSerial[0] << "/" << resultSerial[1] << "\n";

  for(int i = 0; i < 8; i++) {
    cout << "Number of threads used: " << threads[i] << "\n";
      if(type.compare(string("padded")) == 0) {
        array<double, 2> resultPadded = getPiPadded(slices, threads[i]);
        cout << std::setprecision(20) << "Padded result timing and value (value/ms): " << resultPadded[0] << "/" << resultPadded[1] << "\n";
      }
      else if(type.compare(string("exclusion")) == 0) {
        array<double, 2> resultExclusion = getPiExclusion(slices, threads[i]);
        cout << std::setprecision(20) << "Exclusion result timing and value (value/ms): " << resultExclusion[0] << "/" << resultExclusion[1] << "\n\n";
      }        
      else if(type.compare(string("naive")) == 0) {
        array<double, 2> resultNaive = getPiNaive(slices, threads[i]);  
        cout << std::setprecision(20) << "Naive result timing and value (value/ms): " << resultNaive[0] << "/" << resultNaive[1] << "\n";
      }
    }
  
  cout << "Done";
  char c;
  std::cin.get(c);
  return 0;
}

