#!/bin/sh
if [ $# -ne 1 ]
then
   echo "Please input tagent_cfg_dir, eg: /data/tcaplus/tcaplus_service/cfg/tagent"
   exit -1
fi

tagent_cfg_dir=$1
cfg_file=${tagent_cfg_dir}/tagent.xml

master_addr=$(grep "<master>" ${cfg_file} | awk -F "<master>" '{ print $2 }' | awk -F "</master>" '{ print $1 }')
slave_addr=$(grep "<slave>" ${cfg_file} | awk -F "<slave>" '{ print $2 }' | awk -F "</slave>" '{ print $1 }')
echo "master:(${master_addr}), slave:(${slave_addr})"
exit 0
