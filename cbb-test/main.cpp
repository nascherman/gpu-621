// main.cpp
#include "tbb/task_scheduler_init.h"

int main(int argc, char* argv[]) {
  //  tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
  // implicit tbb::task_sheduler_init::automatic
  tbb::task_scheduler_init init;
  return 0;
}