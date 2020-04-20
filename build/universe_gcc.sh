#!/bin/sh

if [ -d "universe" ]; then
	rm -rf universe
fi

mkdir universe

cd universe
if [ -f CMakeCache.txt ];then
	rm CMakeCache.txt
fi
cmake -DCMAKE_BUILD_TYPE=Release ..
#-DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
make -j16
