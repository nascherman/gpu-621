#!/bin/bash
# ./w3 size iterations
make serial
./w3 $((2**20)) 100
./w3 $((2**22)) 100
./w3 $((2**24)) 100
./w3 $((2**26)) 100

make spmd
./w3 $((2**20)) 100
./w3 $((2**22)) 100
./w3 $((2**24)) 100
./w3 $((2**26)) 100

# work sharing
make ws
./w3 $((2**20)) 100
./w3 $((2**22)) 100
./w3 $((2**24)) 100
./w3 $((2**26)) 100

# balanced tree
make balanced
./w3 $((2**20)) 100
./w3 $((2**22)) 100
./w3 $((2**24)) 100
./w3 $((2**26)) 100