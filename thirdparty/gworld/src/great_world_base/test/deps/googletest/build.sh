mkdir -p build
cd build
#export CC=/usr/local/bin/gcc93
#export CXX=/usr/local/bin/g++93
cmake3 ../
make -j8
