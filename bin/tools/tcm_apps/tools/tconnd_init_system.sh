#!/bin/sh
# ����:hardway Data:20110110
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
    #���ý������������ļ�������
    echo "ulimit -n 40960" >> /etc/profile
    . /etc/profile
    service sshe restart
fi

#����TCPЭ��ջδ������Ӷ��г���
echo "40960" > /proc/sys/net/ipv4/tcp_max_syn_backlog

#����TCPЭ��ջ��������������ɶ��г���
echo "10240" > /proc/sys/net/core/somaxconn

#����TCPЭ��ջд��������󳤶�
echo "20480000" >/proc/sys/net/core/wmem_max

#����TCPЭ��ջ����������󳤶�
echo "1024000" > /proc/sys/net/core/rmem_max

echo "script done"

#exit 0
