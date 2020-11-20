#/bin/bash
#if [ $UID != 30000 ];then
#    echo "Error:Please use the account [user00]"
#    exit -1 
#fi
./tconnd --id=0.0.3.1 --use-bus --bus-key=1000 --conf-file=../cfg/tconnd.xml --tlogconf=../cfg/tconnd_log.xml -D start


