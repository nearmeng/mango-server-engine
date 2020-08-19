#!/bin/sh
echo "start tcenterd"
./tcenterd --id 0.0.2.1 --conf-file ../cfg/tcenterd.xml --tlogconf ../cfg/tcenterd_log.xml --bus-key 1000 -D start

