pre_project (RUNTIME)

add_project ()

post_project ()

add_dependencies (${TARGET_NAME} game_data common)

if (MSVC)
	target_link_libraries (${TARGET_NAME} 
		tgcpapi.lib
	)
else ()
	target_link_libraries (${TARGET_NAME} 
		tgcpapi
	)
endif ()
