#!/bin/sh
if [ $# -ne 1 ]
then
   echo "Please input tagent_workdir, eg: /data/tcaplus/tcaplus_service/bin"
   exit -1
fi

echo "begin to reload tagent..."

workdir=$1

#id�ɸ���ʵ�������д
${workdir}/tagent --id=0.0.3.1 --pid-file=${workdir}/tagent_pid.pid reload 
if [ $? -ne 0 ];then
    echo "reload tagent fail"
 		exit -1
fi

echo "reload tagent succ"
exit 0


