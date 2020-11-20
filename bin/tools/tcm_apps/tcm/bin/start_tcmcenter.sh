#!/bin/sh
#if [ $UID != 30000 ];then
#    echo "Error:Please use the account [user00]"
    #exit -1 
#fi

./tcmcenter --id=0.0.1.1 --bus-beat-gap=600 --conf-file=../cfg/tcmcenter.xml --tlogconf=../cfg/tcmcenter_log.xml --bus-key=1000 --conf-format=3 -D start
