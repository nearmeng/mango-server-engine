pre_project(RUNTIME SERVER)

add_project()

post_project()

#if (MSVC)
	#target_link_libraries (${TARGET_NAME} 
    #    debug gw_gamespace_d.lib
    #    optimized gw_gamespace.lib
#)
#else ()
	#target_link_libraries (${TARGET_NAME} 
    #    gw_gamespace
	#)
#endif ()
