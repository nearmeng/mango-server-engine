# mkdir -p ./build/test/src/gamespace/space
# cp test/src/test_space.xml ./build/test/src/cellmgr/space/test_space.xml
cd build/test/src/gamespace_resume
ipcs -m |grep `whoami`|grep 3221225472|awk '{print $2}'|xargs -I {} ipcrm -m {}
./gamespace_resume
# ./gamespace_resume 1