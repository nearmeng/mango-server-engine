#!/bin/sh

# Fisrt of all, you should set PATH to xml2c and xml2h properly

PATH=$1:${PATH}
echo ${PATH}
export PATH


#MMOGXYX_GAME_HOME=../../../../mmogxyz/dev/src/gamesvr/framework
#cp ${MMOGXYX_GAME_HOME}/gamesvr_def.xml	./

${TSF4G_INSTALL_HOME}/tools/tdr --xml2h gamesvr_def.xml
${TSF4G_INSTALL_HOME}/tools/tdr --xml2c gamesvr_def.xml -o gamesvr_def.c


#cp  ../../../../../lib_src/tsec/tsecbasedef.xml ./
#cp  ../../../../tconnd/tconndsvr/tconnddef.xml ./

${TSF4G_INSTALL_HOME}/tools/tdr --xml2c -o tconnddef.c tsecbasedef.xml tconnddef.xml
${TSF4G_INSTALL_HOME}/tools/tdr --xml2h tsecbasedef.xml tconnddef.xml




