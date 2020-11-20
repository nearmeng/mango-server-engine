#!/bin/sh

echo "begin start tcenterd"
cd ../tcenterd/bin/ && sh start_tcenterd.sh && cd -
echo "end start tcenterd"

echo "begin start tcmcenter"
cd ../tcm/bin/ && sh start_tcmcenter.sh && cd -
echo "end start tcmcenter"

echo "begin start tconnd"
cd ../tconnd/bin && sh start_tconnd.sh && cd -
echo "end start tconnd"
