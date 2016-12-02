#!/bin/bash

#Assignment 8 tests
make serial
make parallel 

export mpirun=/opt/intel/compilers_and_libraries_2017.0.098/linux/mpi/intel64/bin/mpirun

./w8-serial 4 20 20 50
./w8-serial 4 20 20 100
./w8-serial 4 20 20 150
./w8-serial 4 20 20 200

${mpirun} -np 4 ./w8-parallel 20 20 50
${mpirun} -np 4 ./w8-parallel 20 20 100
${mpirun} -np 4 ./w8-parallel 20 20 150
${mpirun} -np 4 ./w8-parallel 20 20 200

${mpirun} -np 6 ./w8-parallel 20 20 50
${mpirun} -np 6 ./w8-parallel 20 20 100
${mpirun} -np 6 ./w8-parallel 20 20 150
${mpirun} -np 6 ./w8-parallel 20 20 200

${mpirun} -np 8 ./w8-parallel 20 20 50
${mpirun} -np 8 ./w8-parallel 20 20 100
${mpirun} -np 8 ./w8-parallel 20 20 150
${mpirun} -np 8 ./w8-parallel 20 20 200