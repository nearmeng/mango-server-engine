mkdir -p build
mkdir -p deploy

# export CC=/usr/local/bin/gcc93
# export CXX=/usr/local/bin/g++93

root=$(pwd)
install_dir=/data/evanyu/g6/mmo_space/Server/deps/greatworldbase
echo $install_dir

GW_INSTALL_PREFIX=$install_dir

cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARD_LIB=ON \
    -DCMAKE_INSTALL_PREFIX=$GW_INSTALL_PREFIX \
    -DSTATIC_LINK_STD=ON \
    ../

make -j8
make install

rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_SHARD_LIB=ON \
    -DCMAKE_INSTALL_PREFIX=$GW_INSTALL_PREFIX \
    -DSTATIC_LINK_STD=ON \
    ../

make -j8
make install
