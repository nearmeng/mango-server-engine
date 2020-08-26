pre_project(RUNTIME SERVER)

add_project()

post_project()

if (MSVC)
	target_link_libraries (${TARGET_NAME} 
		hiredis.lib
	)
else ()
	target_link_libraries (${TARGET_NAME} 
		libhiredis.a
	)
endif ()
