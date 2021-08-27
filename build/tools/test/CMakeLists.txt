pre_project (RUNTIME test)

add_project ()

# push it before post_project, because game_data.lib is depend on common.lib
# and common.lib is linked in post project, in linux game_data.lib shound put
# ahead
post_project ()

if (MSVC)
	target_link_libraries (${TARGET_NAME} 
		debug gtestd.lib
		optimized gtest.lib
		debug game_data_d.lib
		optimized game_data.lib
	)
else ()
	target_link_libraries (${TARGET_NAME} 
		debug libgtestd.a
		optimized libgtest.a
		game_data
	)
endif ()

add_dependencies (${TARGET_NAME} game_data)
