#!/bin/bash
make serial semi-serial cilk semi-cilk
chmod 755 w8-*
./w7-serial 1000000
./w7-semi-serial 1000000
./w7-cilk 1000000
./w7-semi-cilk 1000000

./w7-serial 3000000
./w7-semi-serial 3000000
./w7-cilk 3000000
./w7-semi-cilk 3000000

./w7-serial 5000000
./w7-semi-serial 5000000
./w7-cilk 5000000
./w7-semi-cilk 5000000

./w7-serial 7000000
./w7-semi-serial 7000000
./w7-cilk 7000000
./w7-semi-cilk 7000000

./w7-serial 9000000
./w7-semi-serial 9000000
./w7-cilk 9000000
./w7-semi-cilk 9000000

./w7-serial 11000000
./w7-semi-serial 11000000
./w7-cilk 11000000
./w7-semi-cilk 11000000

./w7-serial 13000000
./w7-semi-serial 13000000
./w7-cilk 13000000
./w7-semi-cilk 13000000

./w7-serial 15000000
./w7-semi-serial 15000000
./w7-cilk 15000000
./w7-semi-cilk 15000000