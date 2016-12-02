all:
	make serial
serial:
	g++ --std=c++11 w8.serial.cpp -o w8-serial
parallel:
	/opt/intel/compilers_and_libraries_2017.0.098/linux/mpi/intel64/bin/mpicxx -std=c++11 w8.parallel.cpp -o w8-parallel
