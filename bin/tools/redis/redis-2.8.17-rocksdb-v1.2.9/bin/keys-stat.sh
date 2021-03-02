#switch to current dir
CDIR=`dirname $0`
cd $CDIR

function usage () {
    echo "usage:"
    echo "$0 PORT" 
}

PORT=$1

if [ ! -n "$PORT"  ];then
    echo "PORT not set, exit"
    usage;
    exit;
fi

export LD_LIBRARY_PATH=LD_LIBRARY_PATH:./deps 

ddir=/data/redis/$PORT/data/rocksdb
if [ -d $ddir ]
then
    ./ldb --db=$ddir scan | grep '^a\|^L\|^H\|^S\|^Z' | \
     awk '{tot++;a[substr($1,1,1)]++;}END{print "keys count:",tot;for(h in a){ print "type:",h,a[h]}}' 
fi

ddir=/data1/redis/$PORT/data/rocksdb
if [ -d $ddir ]
then
     ./ldb --db=$ddir scan | grep '^a\|^L\|^H\|^S\|^Z' |  \
     awk '{tot++;a[substr($1,1,1)]++;}END{print "keys count:",tot;for(h in a){ print "type:",h,a[h]}}' 
fi


echo ""
echo "-->"
echo "Type description:  a -> string, H -> hash, S -> set, Z -> zset, L -> list"
echo ""

