#!/bin/sh

echo "begin stop tcmcenter"
cd ../tcm/bin/ && sh stop_tcmcenter.sh && cd -
echo "end stop tcmcenter"

echo "begin stop tcenterd"
cd ../tcenterd/bin/ && sh stop_tcenterd.sh && cd -
echo "end stop tcenterd"

echo "begin stop tconnd"
cd ../tconnd/bin && sh stop_tconnd.sh && cd -
echo "end stop tconnd"
