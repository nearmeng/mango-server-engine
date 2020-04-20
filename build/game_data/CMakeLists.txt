pre_project (STATIC LIB)

if (MSVC)
	file (GLOB_RECURSE DEF_FILES 
		"${BASE_DIR}/inc/define/*.h"
		"${BASE_DIR}/inc/protocol/*.h"
		"${BASE_DIR}/bin/server_config/*.lua"
		"${BASE_DIR}/bin/script/*.lua"
		"${BASE_DIR}/src/proto/db/*.proto"
		"${BASE_DIR}/src/proto/db/*.h"
		"${BASE_DIR}/src/proto/db/*.cc"
		"${BASE_DIR}/src/proto/protocol/*.proto"
		"${BASE_DIR}/src/proto/protocol/*.h"
		"${BASE_DIR}/src/proto/protocol/*.cc"
	)
	set (SRC_FILES ${SRC_FILES} ${DEF_FILES})

	list (APPEND SRC_FILES
		"${BASE_DIR}/build/CMakeLists.txt"
		"${BASE_DIR}/build/precompile.cmake"
		"${BASE_DIR}/build/project.cmake"
		"${BASE_DIR}/build/proto.cmake"
		"${BASE_DIR}/build/toluapp.cmake"
	)
endif ()

add_project ()

post_project ()
