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
	)
else ()
	target_link_libraries (${TARGET_NAME} 
		debug libgtestd.a
		optimized libgtest.a
	)
endif ()

add_dependencies (${TARGET_NAME} game_data)
