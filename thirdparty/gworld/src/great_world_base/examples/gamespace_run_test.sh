cd build/test/src/gamespace
ipcs -m |grep `whoami`|grep 3221225472|awk '{print $2}'|xargs -I {} ipcrm -m {}
if [ "$1" = "resume" ];
then
./gamespace_test retain_shm
./gamespace_test is_resume
else
./gamespace_test
fi

