pre_project(RUNTIME SERVER)

add_project()

post_project()

# if you put this after post_project, it means the library you linked is 
# after the common librarys, on the other hand, the library you want to link 
# is before the common librarys
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
