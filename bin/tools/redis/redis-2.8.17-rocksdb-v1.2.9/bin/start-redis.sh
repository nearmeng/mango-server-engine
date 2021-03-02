#!/bin/sh
#start a redis server
#by tencent dba @ 20130724

function usage () {
	echo "usage:"
	echo "$0 3689" 
	echo "$0 3689 + some redis arg like: $0 3689 --slaveof 1.1.1.1 3679" 
}

PORT=$1

if [ ! -n "$PORT"  ];then
	echo "PORT not set, exit"
	usage;
	exit;
fi

shift

#switch to current dir
CDIR=`dirname $0`
cd $CDIR

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


rootdir="${DISKROOT}/redis/$PORT"
datadir="${rootdir}/data"
confpath="${rootdir}/redis.conf"
binlogdir="${DISKROOT}/redis/$PORT/rbinlog/"

if [ ! -d "$rootdir" ];then
	echo "dir $rootdir not exists"
	usage;
	exit;
fi

if [ ! -f "$confpath" ];then
	echo "file $confpath not exists"
	usage;
	exit;
fi

mkdir -p $binlogdir

if [ ! -f "./deps/libjemalloc.so" ];then
  echo "jemalloc.so file doesnot exists under deps dir,failed to start"
  exit;
fi

export LD_PRELOAD=./deps/libjemalloc.so
export LD_LIBRARY_PATH=LD_LIBRARY_PATH:./deps 
$CDIR/redis-server $rootdir/redis.conf 
