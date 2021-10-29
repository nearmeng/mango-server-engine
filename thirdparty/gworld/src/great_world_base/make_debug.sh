if [ -z $env_is_call ]; then
    echo "please use build_*.sh, this script should not be used directly"
    exit 1
fi

build_type=Debug

cmake3 -DCMAKE_BUILD_TYPE=$build_type \
    -DBUILD_SHARD_LIB=ON \
    -DCMAKE_INSTALL_PREFIX=$GW_INSTALL_PREFIX \
    -DSAVE_GIT_VERSION=$SAVE_GIT_VERSION \
    -DCMAKE_C_COMPILER=/usr/local/gcc-8.3.0/bin/gcc \
    -DCMAKE_CXX_COMPILER=/usr/local/gcc-8.3.0/bin/g++ \
    -Bbuild .

cmake3 --build build --target install --parallel 8