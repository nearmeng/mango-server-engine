#!/bin/sh


#if [ $UID != 30000 ];then
#    echo "Error:Please use the account [user00]"
#    exit -1
#fi

if [ $# -ne 1 ];then
    echo "Usage: tagent_init.sh value1 "
    exit -1
else
    sed -i s#127.0.0.1#$1#g ../tagent/cfg/tagent.xml
fi

echo "Msg:You can start tagent!"
