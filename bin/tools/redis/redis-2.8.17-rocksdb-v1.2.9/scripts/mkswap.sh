data="/data/swap"
mkdir -p $data

function check_swapsize () { 
        memsize=`free -k | grep Mem: | awk '{print $2}'`
        swapsize=`free -k | grep Swap: | awk '{print $2}'`
        if (($swapsize >= $memsize )) ;then
                echo OK memsize $memsize swapsize $swapsize
                exit;
        fi
}


check_swapsize 
num=`cat /proc/meminfo  | grep MemTotal | awk '{printf "%d", $2/1024/1024/8/3 + 1}'`
for id in `seq 1 $num`
do
        file="$data/swapfile$id"
        echo $file
        [ ! -f $file ] && head -c 8192m  /dev/zero  > $file 
        ON=`cat /proc/swaps  | awk '{print $1}' | grep -E "^$file$"`
        if [ -z "$ON" ];then
                echo swapon $file
                mkswap $file
                swapon $file
        fi
        sleep 1
        check_swapsize 

done
