#include <cstdlib>
#include <iostream>
#include <array>
#include <fstream>
#include <iomanip>
#include "header/w2.exclusion.h"
#include "header/w2.naive.h"
#include "header/w2.padded.h"
#include "header/w2.serial.h"

using namespace std;

int main() {
  int slices = 100000000;
  int threads [] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  
  ofstream outfile;
  outfile.open("w2-results.txt");

  outfile << "Results\n=======\n";
  outfile << "Slices = " << slices << "\n\n";
  for(int i = 0; i < 8; i++) {
    outfile << "Number of threads used: " << threads[i] << "\n";
    array<double, 2> resultSerial = getPiSerial(slices);
    array<double, 2> resultExclusion = getPiExclusion(slices, threads[i]);
    array<double, 2> resultPadded = getPiPadded(slices, threads[i]);
    array<double, 2> resultNaive = getPiNaive(slices, threads[i]);  

    outfile << std::setprecision(20) << "Serial result timing and value (value/ms): " << resultSerial[0] << "/" << resultSerial[1] << "\n";
    outfile << std::setprecision(20) << "Naive result timing and value (value/ms): " << resultNaive[0] << "/" << resultNaive[1] << "\n";
    outfile << std::setprecision(20) << "Padded result timing and value (value/ms): " << resultPadded[0] << "/" << resultPadded[1] << "\n";
    outfile << std::setprecision(20) << "Exclusion result timing and value (value/ms): " << resultExclusion[0] << "/" << resultExclusion[1] << "\n\n";
  }
  outfile << "Done";
  outfile.close();
  return 0;
}

