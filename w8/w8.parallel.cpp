
 // Workshop 8 - Domain Decomposition
 // based on code from LLNL tutorial mpi_heat2d.c
 // Master-Worker Programming Model
 // Chris Szalwinski - 20/11/2016
 #include <iostream>
 #include <fstream>
 #include <iomanip>
 #include <cstdlib>
 #include <chrono>
 #include <mpi.h>

 using namespace std::chrono;

 void initialize(int, int, float*);
 void update(int, int, int, const float, const float, const float*, float*);
 void output(int, int, const float*, const char*);

 // report system time
 //
 void reportTime(const char* msg, steady_clock::duration span) {
     auto ms = duration_cast<nanoseconds>(span);
     std::cout << msg << " - took - " <<
         ms.count() << " nanoseconds" << std::endl;
 }

 const int MIN_WORKERS = 3;
 const int MAX_WORKERS = 8;
 const int MASTER = 0;
 const int NONE  = 0;
 // message tags
 const int BEGIN = 1;
 const int ABOVE = 2;
 const int BELOW = 3;
 const int END   = 4;
 // weights
 const float wx = 0.1f;
 const float wy = 0.1f;

 int main(int argc, char** argv) {
     if (argc != 4) {
         std::cerr << "*** Incorrect number of arguments ***\n";
         std::cerr << "Usage: " << argv[0] <<
          " no-of_rows no_of_columns no_of_iterations\n";
         return 1;
     }

     // grid properties
     int nRowsTotal = std::atoi(argv[1]);
     int nColumns = std::atoi(argv[2]);
     int nIterations = std::atoi(argv[3]);

     // allocate memory for data
     steady_clock::time_point ts, te;
     float* data;
     try {
         // 2 * for double buffering
         data = new float[2 * nRowsTotal*nColumns];
     }
     catch (std::bad_alloc) {
         std::cerr << "*** Failed to Allocate Memory for 2 * "
             << nRowsTotal << " by " << nColumns << " grid" << std::endl;
         return 3;
     }
     // initialize first buffer [0: nRowsTotal*nColumns)
     initialize(nRowsTotal, nColumns, data);
     // initialize second buffer [nRowsTotal*nColumns : nRowsTotal*nColumns)
     for (int i = 0; i < nRowsTotal * nColumns; i++)
      data[nRowsTotal * nColumns + i] = 0.0f;

     // MPI startup
    int nTasks;
    int iTask;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &iTask);
    MPI_Comm_size(MPI_COMM_WORLD, &nTasks);
    MPI_Status status;


     // validate data
     int nWorkers = nTasks - 1;
     if (nWorkers < MIN_WORKERS || nWorkers > MAX_WORKERS) {
         std::cerr << "*** Number of workers specified is outside bounds ***\n";
         std::cerr << "Bounds: " << MIN_WORKERS << " <= no_of_WORKERS <="
          << MAX_WORKERS << std::endl;
         // close MPI
         
         return 2;
     }
     // determine row partitioning
     int nRowsPerWorker = (nRowsTotal + nWorkers - 1) / nWorkers;
     int nRowsLeftOver = (nWorkers) ? nRowsTotal - (nWorkers - 1) *
      nRowsPerWorker : nRowsTotal;

     // SPMD Start
     if (iTask == MASTER) {
         std::cout << "Number of Workers " << nWorkers << std::endl;
         std::cout << "Number of Rows    " << nRowsTotal << std::endl;
         std::cout << "Number of Columns " << nColumns << std::endl;
         std::cout << "Number of Rows Per Worker " << nRowsPerWorker << std::endl;
         std::cout << "Number of Rows Last Worker " << nRowsLeftOver << std::endl;
         // write original data to file
         output(nRowsTotal, nColumns, data, "original.dat");

         // decompose - partition original data amongst the workers
         ts = steady_clock::now();
         int rowOffset = 0;
         for (int iWorker = 0; iWorker < nWorkers; iWorker++) {
             int nRowsThisWorker = (nRowsPerWorker) ?
              nRowsPerWorker + 1 : nRowsLeftOver;
             // identify the worker's neighbours
             int above;
             int below;
             if (iWorker == 0) {
                 above = NONE;
             }
             else {
                 above = iWorker;
             }
             if (iWorker == nWorkers - 1) {
                 nRowsThisWorker = (nRowsLeftOver == 0) ?
                  nRowsPerWorker : nRowsLeftOver;
                 below = NONE;
             }
             else {
                 nRowsThisWorker = nRowsPerWorker;
                 below = iWorker + 2;
             }
             // Send startup data partition to the worker
             // - row offset
             // - number of rows for this worker
             // - process id for worker above
             // - process id for worker below
             // - data for this worker

             int destination = iWorker + 1;
             MPI_Send(&nRowsThisWorker, 1, MPI_INT, destination, BEGIN, MPI_COMM_WORLD);
             MPI_Send(&rowOffset, 1, MPI_INT, destination, BEGIN, MPI_COMM_WORLD);
             MPI_Send(&above, 1, MPI_INT, destination, BEGIN, MPI_COMM_WORLD);
             MPI_Send(&below, 1, MPI_INT, destination, BEGIN, MPI_COMM_WORLD);
             MPI_Send(&data[rowOffset * nColumns], nRowsThisWorker * nColumns - 1, MPI_FLOAT, destination, BEGIN, MPI_COMM_WORLD);

   //          std::cout << "Sent to process " << destination << " "
   //           << nRowsThisWorker << " rows data["
   //           << std::setw(3) << rowOffset * nColumns << "..." << std::setw(3)
   //           << rowOffset * nColumns + nRowsThisWorker * nColumns - 1
   //           << "]" << std::endl;
             rowOffset += nRowsThisWorker;
         }

         // receive results of calculations from each worker
         for (int iWorker = 0; iWorker < nWorkers; iWorker++) {
             int source = iWorker + 1;
             int msgTag = END;
             int nRowsThisWorker;
             int rowOffset;
             // receive from iWorker
             // - row offset to start of its data
             // - number of rows processed by iWorker
             // - data from iWorker
            MPI_Recv(&nRowsThisWorker, 1, MPI_INT, source, msgTag, MPI_COMM_WORLD, &status);
            MPI_Recv(&rowOffset, 1, MPI_INT, source, msgTag, MPI_COMM_WORLD, &status);
            MPI_Recv(&data[rowOffset * nColumns], nRowsThisWorker * nColumns - 1, MPI_FLOAT, source,
                   msgTag, MPI_COMM_WORLD, &status);



         }
         te = steady_clock::now();
         reportTime("\n=======================\nw8 Domain Decomposition ", te - ts);

         // write results to file
         std::cout << "Writing results.dat file...\n";
         output(nRowsTotal, nColumns, data, "results.dat");
     }
     else {
         // each worker operates on its share of the data
         int source = MASTER;
         int nRows;
         int above;
         int below;
         int rowOffset;
         // this worker receives information for its allocated partition
         int msgTag = BEGIN;
         MPI_Recv(&nRows, 1, MPI_INT, source, msgTag, MPI_COMM_WORLD, &status);
         MPI_Recv(&rowOffset, 1, MPI_INT, source, msgTag, MPI_COMM_WORLD, &status);
         MPI_Recv(&above, 1, MPI_INT, source, msgTag, MPI_COMM_WORLD, &status);
         MPI_Recv(&below, 1, MPI_INT, source, msgTag, MPI_COMM_WORLD, &status);
         MPI_Recv(&data[rowOffset * nColumns], nRows * nColumns - 1, MPI_FLOAT, source, msgTag, MPI_COMM_WORLD, &status);
      //  std::cout << "Process " << iTask << " received " << nRows << " rows data[" 
      //       << std::setw(3) << rowOffset * nColumns << "..." << std::setw(3)
      //       << rowOffset * nColumns + nRows * nColumns - 1 << "]" << std::endl;
         // set row indices for updating - do not update the first or the last row
         int startRow = rowOffset;
         int endRow = rowOffset + nRows - 1;
         if (rowOffset == 0) startRow = 1;
         if (rowOffset + nRows == nRowsTotal) endRow--;
         // this worker iterates on its share of the data
         int iBuffer = 0;
         for (int iter = 0; iter < nIterations; iter++) {
             // sends its boundary data to neighbouring workers - above and below
             if (above != NONE) {
                 // send to the neighbour above
                 MPI_Send(&data[rowOffset * nColumns], nColumns - 1, MPI_FLOAT, above,
                      BELOW, MPI_COMM_WORLD);
                //  std::cout << std::setw(3) << iter + 1 << "[" << above
                //   << ":" << iTask << ":" << below << "]";
                //  std::cout << " sends    data[" << std::setw(3)
                //   << rowOffset * nColumns << "..." << std::setw(3)
                //   << rowOffset * nColumns + nColumns - 1
                //   << "] to   worker " << above << " above " << std::endl;
                 source = above;
                 msgTag = ABOVE;
                 // receive from the neighbour above
                 MPI_Recv(&data[(rowOffset -1) * nColumns], nColumns - 1, MPI_FLOAT, source,
                      msgTag, MPI_COMM_WORLD, &status);
                // std::cout << std::setw(3) << iter + 1 << "[" << above
                //   << ":" << iTask << ":" << below << "]";
                // std::cout << " receives data[" << std::setw(3)
                //  << (rowOffset - 1) * nColumns << "..." << std::setw(3)
                //  << (rowOffset - 1) * nColumns + nColumns - 1
                //  << "] from worker " << above << " above " << std::endl;
             }
             if (below != NONE) {
                 // send to the neighbour below
                 MPI_Send(&data[(rowOffset + nRows - 1) * nColumns], nColumns - 1, MPI_FLOAT, below,
                      ABOVE, MPI_COMM_WORLD);
                // std::cout << std::setw(3) << iter + 1 << "[" << above
                //  << ":" << iTask << ":" << below << "]";
                // std::cout << " sends    data[" << std::setw(3)
                //  << (rowOffset + nRows - 1) * nColumns << "..." << std::setw(3)
                //  << (rowOffset + nRows - 1) * nColumns + nColumns - 1
                //  << "] to   worker " << below << " below " << std::endl;
                 source = below;
                 msgTag = BELOW;
                 // receive from the neighbour below
                 MPI_Recv(&data[(rowOffset + nRows) * nColumns], nColumns - 1, MPI_FLOAT, source,
                      msgTag, MPI_COMM_WORLD, &status);
                // std::cout << std::setw(3) << iter + 1 << "[" << above
                //  << ":" << iTask << ":" << below << "]";
                // std::cout << " receives data[" << std::setw(3)
                //  << (rowOffset + nRows) * nColumns << "..." << std::setw(3)
                //  << (rowOffset + nRows) * nColumns + nColumns - 1
                //  << "] from worker " << below << " below " << std::endl;
             }
           //  std::cout << "Updating rows " << startRow << " through "
           //   << endRow << std::endl;
             // update its share of the data
             update(startRow, endRow, nColumns, wx, wy,
              data + iBuffer * nRowsTotal * nColumns,
              data + (1 - iBuffer) * nRowsTotal * nColumns);
             // switch buffers
             iBuffer = 1 - iBuffer;
         }
         // return the results of the iterations by this worker to the master
         msgTag = END;
         // send results for this worker
         // - row offset for this worker's data
         // - number of rows processed by this worker
         // - data processed by this worker
         MPI_Send(&nRows, 1, MPI_INT, MASTER, msgTag, MPI_COMM_WORLD);
         MPI_Send(&rowOffset, 1, MPI_INT, MASTER, msgTag, MPI_COMM_WORLD);
         MPI_Send(&data[rowOffset * nColumns], nColumns - 1, MPI_FLOAT, MASTER, msgTag, MPI_COMM_WORLD);


        // std::cout << "Worker " << iTask << " sends data[" << std::setw(3)
        //  << rowOffset * nColumns << "..." << std::setw(3)
        //  << (rowOffset + nRows) * nColumns - 1 << "] to master " << std::endl;
     }
     // SPMD ends

     // close down MPI
     MPI_Finalize();

     // deallocate data
     delete[] data;
 }

 void initialize(int nRowsTotal, int nColumns, float* x) {
     for (int i = 0; i < nRowsTotal; i++)
         for (int j = 0; j < nColumns; j++)
             x[i * nColumns + j] = (float)(i * (nRowsTotal - i - 1) *
              j * (nColumns - j - 1));
 }

 void update(int startRow, int endRow, int nColumns, const float wx,
  const float wy, const float* x_old, float* x_new) {
     for (int i = startRow; i <= endRow; i++)
         for (int j = 1; j < nColumns - 1; j++)
             x_new[i * nColumns + j] = x_old[i * nColumns + j]
             + wx * (x_old[(i + 1) * nColumns + j] +
              x_old[(i - 1) * nColumns + j] - 2.0f * x_old[i * nColumns + j])
             + wy * (x_old[i * nColumns + j + 1] +
              x_old[i * nColumns + j - 1] - 2.0f * x_old[i * nColumns + j]);
 }

 void output(int nRowsTotal, int nColumns, const float* x,
  const char* filename) {
     std::ofstream file(filename);
     file << std::fixed << std::setprecision(1);
     for (int j = nColumns - 1; j >= 0; j--) {
         for (int i = 0; i < nRowsTotal; i++)
             file << std::setw(8) << x[i * nColumns + j];
         file << std::endl;
     }
 }