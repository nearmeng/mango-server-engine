#!/bin/sh


#if [ $UID != 30000 ];then
#    echo "Error:Please use the account [user00]"
#    exit -1
#fi

local_ip=`/sbin/ifconfig -a|grep inet|grep -v 127.0.0.1| grep -v 192.168|grep -v inet6|awk '{print $2}'|tr -d "addr:"`
sed -i s#0.0.0.0#$local_ip#g ../tagent/cfg/tagent.xml

echo "Msg:You can start tagent!"
