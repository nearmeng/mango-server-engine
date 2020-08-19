#!/bin/sh
echo "========== begin to start tconnd =========="
cd ../../tconnd/bin
./start_tconnd.sh
cd -

sleep 1

echo "========== begin to start tcenterd =========="
cd ../../tcenterd/bin
./start_tcenterd.sh
cd -
sleep 1

echo "========== begin to start tcmcenter ========="
./start_tcmcenter.sh

