// Workshop 7 - Fully Recursive Quick Sort
 // w7.fr.cpp
 // after McCool etal. (2012) Structured Parallel Programming

 #include <iostream>
 #include <iomanip>
 #include <cstdlib>
 #include <cstddef>
 #include <algorithm>
 #include <chrono>
 using namespace std::chrono;

 ptrdiff_t QUICKSORT_CUTOFF = 500;
 // report system time
 //
 void reportTime(const char* msg, steady_clock::duration span) {
     auto ms = duration_cast<milliseconds>(span);
     std::cout << msg << " - took - " <<
         ms.count() << " milliseconds" << std::endl;
 }
 // return the address of the median value across 3 addresses
 template <typename T>
 T* median_of_three(T* x, T* y, T* z) {
     return *x < *y ? *y < *z ? y :
            *x < * z ? z : x :
            *z < *y ? y : *z <  *x ? z : x;
 }
 // return the address that holds the partition value
 // select that value to be the median of 3 medians
 template <typename T>
 T* select_partition_value(T* first, T* last) {
    int offset = (last - first) / 8;
    return median_of_three(
        median_of_three(first, first + offset, first + offset * 2),
        median_of_three(first + offset * 3, first + offset * 4,
         last - (3 * offset + 1)),
        median_of_three(last - (2 * offset + 1), last - (offset + 1), 
         last - 1)
    );
 }
 // return the address of the partition value in the partitioned
 // sequence; return nullptr if all values are equal
 // divide the sequence [first ... last) by selecting the
 // partition value and then rearranging the values into a
 // partitioned sequence [first ... middle, middle + 1 ... last)
 template <typename T>
 T* divide(T* first, T* last) {
     // select partition value
     T* p = select_partition_value(first, last);
     // move partition value to the front of the sequence
     std::swap(*first, *p);
     // copy the partition value
     T pv = *first;
     // rearrange the values into two partitions and retrieve
     // the address of end of the first partition
     T* middle = std::partition(first + 1, last,
      [=](const T& x) { return x < pv;}) - 1;
     if (middle != first) {
         // move the partition value to the end of the first
         // partition
         std::swap(*first, *middle);
     }
     else {
         // check if all values are equal
         if (last == std::find_if(first + 1, last,
          [=](const T& x) { return pv < x; }) )
             return nullptr;
     }
     return middle;
 }
 // quicksort divides the sequence [first ... last) into
 // two partitions and sorts each partition independently
 template <typename T>
 void quick_sort(T* first, T* last) {
     while (last - first > QUICKSORT_CUTOFF) {
        T* middle = divide(first, last);
        if(!middle) return;
        if(middle - first < last - (middle + 1)) {
            quick_sort(first, middle);
            first = middle + 1;
        }
        else {
            quick_sort(middle + 1, last);
            last = middle;
        }
    }
    std::sort(first, last);   
 }
 int main(int argc, char** argv) {
     if (argc != 2) {
         std::cerr << "*** Incorrect number of arguments ***\n";
         std::cerr << "Usage: " << argv[0]
          << " no_of_rows_or_columns\n";
         return 1;
     }

     // size of array
     int n = std::atoi(argv[1]);

     // allocate memory for the array
     steady_clock::time_point ts, te;
     int* a;
     try {
         a = new int[n];
     } catch (const std::exception& e) {
         std::cerr << e.what() << std::endl;
         return 2;
     }
     // initialize to random values
     for (int i = 0; i < n; i++)
         a[i] = std::rand();

     // sort the elements
     ts = steady_clock::now();
     quick_sort(a, a + n);
     te = steady_clock::now();
     reportTime("w7 semi recursive serial", te - ts);

     std::cout << n << " elements sorted\n";
     if (n < 100) {
         for (int i = 0; i < n; i++) {
             if (i && i % 8 == 0) std::cout << std::endl;
             std::cout << std::setw(10) << a[i] << ' ';
         }
     }
     else {
         std::cout << std::setw(10) << a[0] << ' ';
         std::cout << std::setw(10) << a[1] << ' ';
         std::cout << std::setw(10) << "..." << ' ';
         std::cout << std::setw(10) << a[n-2] << ' ';
         std::cout << std::setw(10) << a[n-1] << ' ';
     }
     std::cout << std::endl;

     // deallocate memory
     delete [] a;
 }
 