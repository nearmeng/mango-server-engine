pre_project(RUNTIME SERVER)

add_project()

if (MSVC)
	target_link_libraries (${TARGET_NAME} 
		debug liblmdbd.lib
		optimized liblmdb.lib
	)
else ()
	target_link_libraries (${TARGET_NAME} 
		lmdb
	)
endif ()

post_project()
