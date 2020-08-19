#!/bin/sh
if [ $UID != 30000 ];then
    echo "Error:Please use the account [user00]"
    exit -1 
fi

./tcmconsole --conf-file=../cfg/tcmconsole.xml --tlogconf=../cfg/tcmconsole_log.xml  start
