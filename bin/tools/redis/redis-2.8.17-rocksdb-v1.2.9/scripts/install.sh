#!/bin/sh
#start a redis server
#by tencent dba @ 20130724

function usage () {
	echo "usage:"
	echo "$0 3689" 
}

PORT=$1

if [ ! -n "$PORT"  ];then
	echo "PORT not set, exit"
	usage;
	exit;
fi

shift

#check if this server is docker?!
#docker server mount the disk to /data1, but /data is int the / without mount to any dev

dfinfo=`df -h`
idata1=`echo "$dfinfo"|grep -w  "/data1"|wc -l`
idata=`echo "$dfinfo"|grep -w "/data"|wc -l`
idataredis=` ls -ld /data/redis/ 2>/dev/null|wc -l`
idata1redis=`ls -ld /data1/redis/ 2>/dev/null|wc -l`


#check if there is /data1 exist mounted, if exists,use it.
EX_DATA1=`df -h|grep -E "/data1$" |wc -l`

#default dir
DISKROOT="/data"

if [ $EX_DATA1 -gt 0 ]
then
    DISKROOT="/data1"
else
    DISKROOT="/data"
fi
	

rootdir="${DISKROOT}/redis/$PORT/"
datadir="${DISKROOT}/redis/$PORT/data"
confpath="${DISKROOT}/redis/$PORT/redis.conf"
instconfpath="${DISKROOT}/redis/$PORT/instance.conf"

binlogdir="${DISKROOT}/redis/$PORT/rbinlog/"

mylocalip=`/sbin/ifconfig |  grep -A1 "eth" | grep "inet addr:" | awk -F: '{ print $2 }' | grep -E "^10\.|^192\.|^172\." | awk '{ print $1 }'|head -n 1`
#for support 100.64/10 network special
mylocal100=`/sbin/ifconfig |  grep -A1 "eth" | grep "inet addr:" | awk -F: '{ print $2 }' | grep -E "^100\." | awk '{ print $1 }'|head -n 1`

if [  -d "$rootdir" ];then
	echo "dir $rootdir exists"
	exit;
fi

mkdir -p $datadir
mkdir -p $binlogdir

if [ "$mylocalip" != "" ]
then
sed -e "s/\$PORT/$PORT/g" redis.conf  | sed -e "s/bind\ 127.0.0.1/bind\ 127.0.0.1\ $mylocalip/g" | sed -e "s|\$DISKROOT|$DISKROOT|g" > $confpath
else
sed -e "s/\$PORT/$PORT/g" redis.conf  | sed -e "s/bind\ 127.0.0.1/bind\ 127.0.0.1\ $mylocal100/g" | sed -e "s|\$DISKROOT|$DISKROOT|g" > $confpath
fi

touch $instconfpath

