#!/bin/bash

#####################
#auto pull msg report
#date:2010-03-26
#####################

PID=$2
PROCNAME=$4
HOSTNAME=`hostname`
LOCALIP=`/sbin/ifconfig | grep -A1 "^eth1" | grep "inet addr:" | awk -F: '{ print $2 }' | awk '{ print $1 }' | head -n1`


NOW=`date "+%Y-%m-%d %H:%M:%S"`


if [  $# -ne 4 ]; then

		REPORT="$HOSTNAME $LOCALIP $NOW:$PROCNAME resume args err"
else
		REPORT="$HOSTNAME $LOCALIP $4($2) auto resume"
fi

#/usr/local/agenttools/agent/agentRepStr 1173831 "$REPORT"

