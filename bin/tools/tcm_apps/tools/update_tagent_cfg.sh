#!/bin/sh
if [ $# -ne 5 ]
then
   echo "Please input tagent_cfg_dir old_master_addr old_slave_addr new_master_addr new_slave_addr, eg: /data/tcaplus/tcaplus_service/cfg/tagent 1.1.1.100:9900 1.1.2.100:9900 2.1.1.1:9900 3.1.1.1:9900"
   exit -1
fi


function check_ipaddr()
{
		ip_addr=$1
				
		a=$(echo ${ip_addr} | awk -F '.' '{print $1}')
		b=$(echo ${ip_addr} | awk -F '.' '{print $2}')
		c=$(echo ${ip_addr} | awk -F '.' '{print $3}')
		d=$(echo ${ip_addr} | awk -F '.' '{print $4}')
		
		for num in $a $b $c $d
		do
				echo ${num} | grep "[^0-9]"
				if [ $? -ne 1 ];then
				    echo "${num}: not number"
		        return 1
				fi
				
		    if [ ${num} -gt 255 ] || [ ${num} -lt 0 ];then
		        echo "${num}: not in range [0,255]"
		        return 1
		    fi
		done
		
		return 0
}


#地址格式: "ip:port"
function check_addr_str()
{    
		addr_str=$1
		
		quote=$(echo ${addr_str} | grep ':')
		if [ "${quote}" == "" ];then
		    echo "${addr_str} is error"
		    return 1
		fi
		
		#检查ip
		ip=$(echo ${addr_str} | awk -F ':' '{print $1}')
		port=$(echo ${addr_str} | awk -F ':' '{print $2}')
		
		check_ipaddr ${ip};
		if [ $? -ne 0 ];then
		    echo "ip(${ip}) is error"
		    return 1
		fi
		
		#检查端口
		echo ${port} | grep "[^0-9]"
		if [ $? -ne 1 ];then
		    echo "${port}: not number"
        return 1
		fi
				
		if [ ${port} -gt 65535 ] || [ ${port} -le 0 ];then
		    echo "port(${port}) is error"
		    return 1
		fi
		
		return 0
}


##说明：配置文件tagent.xml中<master>和<slave>中的值不能有空格. eg: <master>10.12.234.221:8899</master>, <slave>10.123.16.138:8899</slave>

tagent_cfg_dir=$1
old_master_addr=$2
old_slave_addr=$3
new_master_addr=$4
new_slave_addr=$5

cfg_file=${tagent_cfg_dir}/tagent.xml

#检查目录下是否有tagent.xml
if [ ! -e ${cfg_file} ];then
		echo "${cfg_file} not exist"
		exit -1
fi

#检查地址: 格式，ip和port
check_addr_str ${old_master_addr};
if [ $? -ne 0 ];then
    echo "input param old_master_addr(${old_master_addr}) is error"
 		exit -1
fi

check_addr_str ${old_slave_addr};
if [ $? -ne 0 ];then
    echo "input param old_slave_addr(${old_slave_addr}) is error"
    exit -1
fi

check_addr_str ${new_master_addr};
if [ $? -ne 0 ];then
    echo "input param new_master_addr(${new_master_addr}) is error"
    exit -1
fi

check_addr_str ${new_slave_addr};
if [ $? -ne 0 ];then
    echo "input param new_slave_addr(${new_slave_addr}) is error"
    exit -1
fi

echo "old_master_addr: ${old_master_addr}, new_master_addr: ${new_master_addr}"
echo "old_slave_addr: ${old_slave_addr}, new_slave_addr: ${new_slave_addr}"

#判断配置中是否存在需要更改的地址
cfg_master_addr=$(grep "<master>" ${cfg_file} | awk -F "<master>" '{print $2}' | awk -F "</master>" '{print $1}')
result=$(echo ${cfg_master_addr} | grep "${old_master_addr}")
if [ "${result}" == "" ];then
		echo "not find master addr: ${old_master_addr}, cfg master is: ${cfg_master_addr}"
		exit -1
fi

cfg_slave_addr=$(grep "<slave>" ${cfg_file} | awk -F "<slave>" '{print $2}' | awk -F "</slave>" '{print $1}')
result=$(echo ${cfg_slave_addr} | grep "${old_slave_addr}")
if [ "${result}" == "" ];then
		echo "not find slave addr: ${old_slave_addr}, cfg slave is: ${cfg_slave_addr}"
		exit -1
fi

#备份
if [ ${old_master_addr} != ${new_master_addr} ] || [ ${old_slave_addr} != ${new_slave_addr} ];then
		currdate=$(date "+%Y-%m-%d_%H:%M:%S")
		cp -p ${cfg_file} ${cfg_file}_${currdate}
fi

#新旧地址不同时才更改
flag=0
if [ ${old_master_addr} != ${new_master_addr} ];then
		sed -i "s/<master>${cfg_master_addr}<\/master>/<master>${new_master_addr}<\/master>/g"  ${cfg_file}
		if [ $? -ne 0 ];then
		    echo "update tagent master addr fail"
		 		exit -1
		fi
		
		#检查是否修改成功
		master_addr=$(grep "<master>" ${cfg_file} | awk -F "<master>" '{print $2}' | awk -F "</master>" '{print $1}')
		if [ ${new_master_addr} == ${master_addr} ];then
		    echo "update master addr succ"
		else
		    echo "update master addr failed, master:(${master_addr})"
		    exit -1
		fi
		
		flag=1
fi

if [ ${old_slave_addr} != ${new_slave_addr} ];then
		sed -i "s/<slave>${cfg_slave_addr}<\/slave>/<slave>${new_slave_addr}<\/slave>/g"  ${cfg_file}
		if [ $? -ne 0 ];then
		    echo "update tagent slave addr fail"
		 		exit -1
		fi
		
		slave_addr=$(grep "<slave>" ${cfg_file} | awk -F "<slave>" '{print $2}' | awk -F "</slave>" '{print $1}')
		if [ ${new_slave_addr} == ${slave_addr} ];then
		    echo "update slave addr succ"
		else
		    echo "update slave addr failed, slave:(${slave_addr})"
		    exit -1
		fi
		
		flag=1
fi

if [ ${flag} == 1 ];then
    echo "update tagent cfg succ"
else
    echo "cfg is same, not change"
fi
exit 0
