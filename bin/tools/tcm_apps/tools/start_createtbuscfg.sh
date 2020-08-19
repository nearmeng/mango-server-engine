#!/bin/bash

if [ $# -ne 1 ];then
   echo "useage:sh start_createtbuscfg.sh conf_file_path, eg: ../bin/tcm_bak/tcmcenter.xml_2017-06-26:19:46:00"
   exit -1
fi

conf_file_path=$1

./createtbuscfg --conf-file=${conf_file_path} --tlogconf=./createtbuscfg_log.xml --conf-format=3  start
