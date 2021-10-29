if [ -z $GW_ROOT_PATH ]; then
	# g6_server_dir=/data/evanyu/g6/mmo_space/Server
	g6_server_dir=/home/quintuszhou/game_play_framework/externals
else
	g6_server_dir=$GW_ROOT_PATH
fi

echo $g6_server_dir

env_is_call=1

# GW_INSTALL_PREFIX=$g6_server_dir/deps/greatworldbase
GW_INSTALL_PREFIX=$g6_server_dir

SAVE_GIT_VERSION=$1
