#!/bin/sh


#if [ $UID != 30000 ];then
#    echo "Error:Please use the account [user00]"
#    exit -1
#fi

if [ ! -f "./tbusmgr.xml" ];then
    echo "Error:Package is not complete!"
    exit -1
fi
./tbusmgr -C tbusmgr.xml -X
./tbusmgr --conf-file=./tbusmgr.xml --write

local_ip=`/sbin/ifconfig -a|grep inet|grep -v 127.0.0.1| grep -v 192.168|grep -v inet6|awk '{print $2}'|tr -d "addr:"`
sed -i s#0.0.0.0#$local_ip#g ../tcenterd/cfg/tcenterd.xml

echo "Msg:You can start tconnd and tcenterd!"

#generate tcm cfg
cd ../../deploy/tcm/
sh build_tcm_conf.sh
cp conf_output/* ../../tcm_apps/tcm/cfg/
cp tcm_so.so ../../tcm_apps/tcm/lib/
cd -
