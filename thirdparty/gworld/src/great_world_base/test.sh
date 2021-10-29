ROOT_DIR=$(pwd)

# cellmgr resume
cd $ROOT_DIR
sh examples/resume_cellmgr.sh

# cell api
cd $ROOT_DIR
sh examples/cellmgr_run_test.sh

# gamespace 接口层
cd $ROOT_DIR
sh examples/gamespace_run_test.sh

# aoi算法
cd $ROOT_DIR
cd build/test/src/aoi
./aoi_test

# realghost
cd $ROOT_DIR
sh examples/realghost_run_test.sh

