pre_project (STATIC)

if (MSVC)
	file (GLOB_RECURSE DEF_FILES 
		"${BASE_DIR}/inc/define/*.h"
	)
	set (SRC_FILES ${SRC_FILES} ${DEF_FILES})
endif ()

add_project ()

post_project ()
