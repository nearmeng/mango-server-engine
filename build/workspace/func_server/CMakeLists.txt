pre_project(RUNTIME SERVER)

add_project()

post_project()

if (MSVC)
	target_link_libraries (${TARGET_NAME} 
        debug libcurld.lib
        optimized libcurl.lib
	)
else ()
	target_link_libraries (${TARGET_NAME} 
        curl z crypto ssl
	)
endif ()
