/****************************************************************************
 * HEAT2D Example - Parallelized C Version
 * FILE: mpi_heat2D.c
 * OTHER FILES: draw_heat.c  
 * DESCRIPTIONS:  This example is based on a simplified two-dimensional heat 
 *   equation domain decomposition.  The initial temperature is computed to be 
 *   high in the middle of the domain and zero at the boundaries.  The 
 *   boundaries are held at zero throughout the simulation.  During the 
 *   time-stepping, an array containing two domains is used; these domains 
 *   alternate between old data and new data.
 *
 *   In this parallelized version, the grid is decomposed by the master
 *   process and then distributed by rows to the worker processes.  At each 
 *   time step, worker processes must exchange border data with neighbors, 
 *   because a grid point's current temperature depends upon it's previous
 *   time step value plus the values of the neighboring grid points.  Upon
 *   completion of all time steps, the worker processes return their results
 *   to the master process.
 *
 *   Two data files are produced: an initial data set and a final data set.
 *   An X graphic of these two states displays after all calculations have
 *   completed.
 *
 * AUTHOR: Blaise Barney - adapted from D. Turner's serial version. Converted
 *   to MPI George L. Gusciora (1/25/95)
 * LAST REVISED: 12/14/95  Blaise Barney
 ****************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include "draw_heat.c"
extern void draw_heat(int, int);       /* X routine to create graph */

#define NXPROB      20                 /* x dimension of problem grid */
#define NYPROB      20                 /* y dimension of problem grid */
#define STEPS       50                 /* number of time steps */
#define MAXWORKER   8                  /* maximum number of worker tasks */
#define MINWORKER   3                  /* minimum number of worker tasks */
#define BEGIN       1                  /* message type */
#define NGHBOR1     2                  /* message type */
#define NGHBOR2     3                  /* message type */
#define NONE        0                  /* indicates no neighbor */
#define DONE        4                  /* message type */
#define MASTER      0                  /* taskid of first process */

struct Parms { 
  float cx;
  float cy;
} parms = {0.1, 0.1};

int main(argc,argv)
int argc;
char *argv[];
{
void inidat(), prtdat(), update();
float  u[2][NXPROB][NYPROB];        /* array for grid */
int taskid,                     /* this task's unique id */
  numworkers,                 /* number of worker processes */
  numtasks,                   /* number of tasks */
  averow,rows,offset,extra,   /* for sending rows of data */
  dest, source,               /* to - from for message send-receive */
  neighbor1,neighbor2,        /* neighbor tasks */
  msgtype,                    /* for message types */
  rc,start,end,               /* misc */
  i,ix,iy,iz,it;              /* loop variables */
MPI_Status status;


/* First, find out my taskid and how many tasks are running */
   rc = MPI_Init(&argc,&argv);
   rc|= MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   rc|= MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   if (rc != 0)
      printf ("error initializing MPI and obtaining task ID information\n");
   else
      printf ("mpi_heat2D MPI task ID = %d\n", taskid);
   numworkers = numtasks-1;

   if (taskid == MASTER)
   {
      /************************* master code *******************************/
      /* Check if numworkers is within range - quit if not */
      if ((numworkers > MAXWORKER) || (numworkers < MINWORKER))
      {
         printf("MP_PROCS needs to be between %d and %d for this exercise\n",
                 MINWORKER+1,MAXWORKER+1);
         MPI_Finalize();
      }

      /* Initialize grid */
      printf("Grid size: X= %d  Y= %d  Time steps= %d\n",NXPROB,NYPROB,STEPS);
      printf("Initializing grid and writing initial.dat file...\n");
      inidat(NXPROB, NYPROB, u);
      prtdat(NXPROB, NYPROB, u, "initial.dat");

      /* Distribute work to workers.  Must first figure out how many rows to */
      /* send and what to do with extra rows.  */
      averow = NXPROB/numworkers;
      extra = NXPROB%numworkers;
      offset = 0;
      for (i=1; i<=numworkers; i++)
      {
         rows = (i <= extra) ? averow+1 : averow; 
         /* Tell each worker who its neighbors are, since they must exchange */
         /* data with each other. */  
         if (i == 1) 
            neighbor1 = NONE;
         else
            neighbor1 = i - 1;
         if (i == numworkers)
            neighbor2 = NONE;
         else
            neighbor2 = i + 1;
         /*  Now send startup information to each worker  */
         dest = i;
         MPI_Send(&offset, 1, MPI_INT, dest, BEGIN, MPI_COMM_WORLD);
         MPI_Send(&rows, 1, MPI_INT, dest, BEGIN, MPI_COMM_WORLD);
         MPI_Send(&neighbor1, 1, MPI_INT, dest, BEGIN, MPI_COMM_WORLD);
         MPI_Send(&neighbor2, 1, MPI_INT, dest, BEGIN, MPI_COMM_WORLD);
         MPI_Send(&u[0][offset][0], rows*NYPROB, MPI_FLOAT, dest, BEGIN, MPI_COMM_WORLD);
         printf("Sent to= %d offset= %d rows= %d neighbor1= %d neighbor2= %d\n",
                 dest,offset,rows,neighbor1,neighbor2);
         offset = offset + rows;
      }
      /* Now wait for results from all worker tasks */
      for (i=1; i<=numworkers; i++)
      {
         source = i;
         msgtype = DONE;
         MPI_Recv(&offset, 1, MPI_INT, source, msgtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&rows, 1, MPI_INT, source, msgtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&u[0][offset][0], rows*NYPROB, MPI_FLOAT, source,
                   msgtype, MPI_COMM_WORLD, &status);
      }

      /* Write final output and call X graph */
      printf("Writing final.dat file and generating graph...\n");
      prtdat(NXPROB, NYPROB, &u[0][0][0], "final.dat");
      draw_heat(NXPROB,NYPROB);
      MPI_Finalize();
   }   /* End of master code */

   if (taskid != MASTER)
   {
      /************************* worker  code **********************************/
      /* Initialize everything - including the borders - to zero */
      for (iz=0; iz<2; iz++)
         for (ix=0; ix<NXPROB; ix++) 
            for (iy=0; iy<NYPROB; iy++) 
               u[iz][ix][iy] = 0.0;

      /* Now receive my offset, rows, neighbors and grid partition from master */
      source = MASTER;
      msgtype = BEGIN;
      MPI_Recv(&offset, 1, MPI_INT, source, msgtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, source, msgtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&neighbor1, 1, MPI_INT, source, msgtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&neighbor2, 1, MPI_INT, source, msgtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&u[0][offset][0], rows*NYPROB, MPI_FLOAT, source, msgtype, MPI_COMM_WORLD, &status);

      /* Determine border elements.  Need to consider first and last columns. */
      /* Obviously, row 0 can't exchange with row 0-1.  Likewise, the last */
      /* row can't exchange with last+1.  */
      if (offset==0) 
         start=1;
      else 
         start=offset;
      if ((offset+rows)==NXPROB) 
         end=start+rows-2;
      else 
         end = start+rows-1;

      /* Begin doing STEPS iterations.  Must communicate border rows with */
      /* neighbors.  If I have the first or last grid row, then I only need */
      /*  to  communicate with one neighbor  */
      iz = 0;
      for (it = 1; it <= STEPS; it++)
      {
         if (neighbor1 != NONE)
         {
            MPI_Send(&u[iz][offset][0], NYPROB, MPI_FLOAT, neighbor1,
                      NGHBOR2, MPI_COMM_WORLD);
            source = neighbor1;
            msgtype = NGHBOR1;
            MPI_Recv(&u[iz][offset-1][0], NYPROB, MPI_FLOAT, source,
                      msgtype, MPI_COMM_WORLD, &status);
         }
         if (neighbor2 != NONE)
         {
            MPI_Send(&u[iz][offset+rows-1][0], NYPROB, MPI_FLOAT, neighbor2,
                      NGHBOR1, MPI_COMM_WORLD);
            source = neighbor2;
            msgtype = NGHBOR2;
            MPI_Recv(&u[iz][offset+rows][0], NYPROB, MPI_FLOAT, source, msgtype,
                      MPI_COMM_WORLD, &status);
         }
         /* Now call update to update the value of grid points */
         update(start,end,NYPROB,&u[iz][0][0],&u[1-iz][0][0]);
         iz = 1 - iz;
      }
      /* Finally, send my portion of final results back to master */
      MPI_Send(&offset, 1, MPI_INT, MASTER, DONE, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, MASTER, DONE, MPI_COMM_WORLD);
      MPI_Send(&u[iz][offset][0], rows*NYPROB, MPI_FLOAT, MASTER, DONE, MPI_COMM_WORLD);
      MPI_Finalize();
   }
}


/**************************************************************************
 *  subroutine update
 ****************************************************************************/
void update(int start, int end, int ny, float *u1, float *u2)
{
   int ix, iy;
   for (ix = start; ix <= end; ix++) 
      for (iy = 1; iy <= ny-2; iy++) 
         *(u2+ix*ny+iy) = *(u1+ix*ny+iy)  + 
                          parms.cx * (*(u1+(ix+1)*ny+iy) +
                          *(u1+(ix-1)*ny+iy) - 
                          2.0 * *(u1+ix*ny+iy)) +
                          parms.cy * (*(u1+ix*ny+iy+1) +
                         *(u1+ix*ny+iy-1) - 
                          2.0 * *(u1+ix*ny+iy));
}

/*****************************************************************************
 *  subroutine inidat
 *****************************************************************************/
void inidat(int nx, int ny, float *u) {
int ix, iy;

for (ix = 0; ix <= nx-1; ix++) 
  for (iy = 0; iy <= ny-1; iy++)
     *(u+ix*ny+iy) = (float)(ix * (nx - ix - 1) * iy * (ny - iy - 1));
}

/**************************************************************************
 * subroutine prtdat
 **************************************************************************/
void prtdat(int nx, int ny, float *u1, char *fnam) {
int ix, iy;
FILE *fp;

fp = fopen(fnam, "w");
for (iy = ny-1; iy >= 0; iy--) {
  for (ix = 0; ix <= nx-1; ix++) {
    fprintf(fp, "%6.1f", *(u1+ix*ny+iy));
    if (ix != nx-1) 
      fprintf(fp, " ");
    else
      fprintf(fp, "\n");
    }
  }
fclose(fp);
}