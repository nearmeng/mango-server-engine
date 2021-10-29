mkdir -p ./build/test/src/cellmgr_resume/space
cp test/src/test_space.xml ./build/test/src/cellmgr_resume/space/test_space.xml
cd build/test/src/cellmgr_resume
ipcs -m |grep `whoami`|grep 3221225472|awk '{print $2}'|xargs -I {} ipcrm -m {}
./cellmgr_resume
./cellmgr_resume 1