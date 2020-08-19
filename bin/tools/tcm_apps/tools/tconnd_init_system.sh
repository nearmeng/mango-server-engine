#!/bin/sh
# 作者:hardway Data:20110110
INIT_SCRIPT ()
{
    SCRIPT_DIR=$(dirname $0)
    SCRIPT_FN=$(basename $0)
    if [ "$SCRIPT_DIR" = "." ]; then
    {
        SCRIPT_DIR="$(pwd)"
    }
    else
    {
        cd $SCRIPT_DIR
        SCRIPT_DIR="$(pwd)"
    }
    fi
   # echo $SCRIPT_DIR
   # echo $SCRIPT_FN

}

CHECK_SUPER_USER ()
{

      if [ "$(whoami)" != "root" ] ; then
          SUPERUSER=0
      else
          SUPERUSER=1
      fi
}
INIT_SCRIPT
SUPERUSER=0
CHECK_SUPER_USER

if [ $SUPERUSER != 1 ]; then
   echo "Need root previlege to excecute this script"
   exec su -c "$SCRIPT_DIR/$SCRIPT_FN"
fi

ULIMIT=`grep -i "ulimit -n" /etc/profile|grep -v "#"`

if [ "$ULIMIT" = ""  ]; then
    #设置进程允许打开最大文件描述符
    echo "ulimit -n 40960" >> /etc/profile
    . /etc/profile
    service sshe restart
fi

#设置TCP协议栈未完成连接队列长度
echo "40960" > /proc/sys/net/ipv4/tcp_max_syn_backlog

#设置TCP协议栈三次握手连接完成队列长度
echo "10240" > /proc/sys/net/core/somaxconn

#设置TCP协议栈写缓冲区最大长度
echo "20480000" >/proc/sys/net/core/wmem_max

#设置TCP协议栈读缓冲区最大长度
echo "1024000" > /proc/sys/net/core/rmem_max

echo "script done"

#exit 0
