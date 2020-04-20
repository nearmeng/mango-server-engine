pre_project (RUNTIME LIB)

add_project ()

post_project ()

if (MSVC)
	target_link_libraries (${TARGET_NAME} 
		tgcpapi.lib
		debug game_data_d.lib
		optimized game_data.lib
	)
else ()
	target_link_libraries (${TARGET_NAME} 
		tgcpapi
		game_data
	)
endif ()

add_dependencies (${TARGET_NAME} game_data)