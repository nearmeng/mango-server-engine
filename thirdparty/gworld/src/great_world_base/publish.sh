source ./env.sh $1

rm -rf ${g6_server_dir}/deps/greatworldbase

rm -rf build
source ./make_debug.sh

rm -rf build
source ./make_release.sh