#!/bin/sh
if [ $UID != 30000 ];then
    echo "Error:Please use the account [user00]"
    exit -1 
fi
./tcmcenter  --id=0.0.1.1 reload

