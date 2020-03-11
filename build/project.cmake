function (pre_project TARGET_TYPE)
	get_filename_component (TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	get_filename_component (TARGET_DIR ${BASE_DIR}/src/${TARGET_NAME} ABSOLUTE)
	get_filename_component (TARGET_INC_DIR ${BASE_DIR}/inc/${TARGET_NAME} ABSOLUTE)

	#set global value
	set (TARGET_NAME ${TARGET_NAME} PARENT_SCOPE)
	set (TARGET_DIR ${TARGET_DIR} PARENT_SCOPE)
	set (TARGET_INC_DIR ${TARGET_INC_DIR} PARENT_SCOPE)
	set (TARGET_TYPE ${TARGET_TYPE} PARENT_SCOPE)

	#get base src files
	file (GLOB_RECURSE SRC_FILES
		"${TARGET_DIR}/*.cpp"
		"${TARGET_DIR}/*.c"
		"${TARGET_DIR}/*.cc"
		"${TARGET_DIR}/*.h"
		"${TARGET_INC_DIR}/*.h"
		"${TARGET_DIR}/*.pkg"
		"${TARGET_DIR}/*.epkg"
	)

	set (SRC_FILES ${SRC_FILES} PARENT_SCOPE)

	message (---------------------------------------)
	message (pre_project\ ${TARGET_NAME})
endfunction()

function (add_project)
	#add definitions
	if (MSVC)
		add_definitions ("/D WIN32" "/D WIN64" "/D _WINDOWS")
	else ()
		if (${CMAKE_BUILD_TYPE})
			if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
				add_definitions ("/D_DEBUG")
			endif ()
		endif ()
		add_definitions(-D_LINUX)
	endif ()

	include_directories (${TARGET_DIR})

	#compile options
	if (MSVC)
		add_compile_options ("/Yu stdafx.h")
		add_compile_options ("/Zi")
	endif ()

	if (TARGET_TYPE STREQUAL "STATIC")
		add_library (${TARGET_NAME} STATIC ${SRC_FILES})
	elseif (TARGET_TYPE STREQUAL "DYNAMIC")
		add_library (${TARGET_NAME} SHARED ${SRC_FILES})
	elseif (TARGET_TYPE STREQUAL "RUNTIME")
		add_executable(${TARGET_NAME} ${SRC_FILES})
	endif ()

	message(add_project\ ${TARGET_NAME})
endfunction()

function (post_project)
	toluapp (${TARGET_DIR})

	add_precompiled_header (${TARGET_NAME} stdafx.h FORCEINCLUDE SOURCE_CXX stdafx.cpp)

	# target property
	if (TARGET_TYPE STREQUAL "STATIC")
		set_target_properties (${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${BASE_DIR}/lib)
		set_target_properties (${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/lib)
		set_target_properties (${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/lib)
	elseif (TARGET_TYPE STREQUAL "DYNAMIC")
		set_target_properties (${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BASE_DIR}/bin)	
		set_target_properties (${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/bin)	
		set_target_properties (${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/bin)	
	elseif (TARGET_TYPE STREQUAL "RUNTIME")
		set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/bin/${TARGET_NAME})
		set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/bin/${TARGET_NAME})
		set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/bin/${TARGET_NAME})
	endif ()

	set_target_properties (${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${TARGET_NAME}_d)
	set_target_properties (${TARGET_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${TARGET_NAME})

	#default link libaray
	if (UNIX)
		target_link_libraries (${TARGET_NAME}
			dl
			lua
			toluapp
			protobuf
			tconnapi
		)
		
		if (TARGET_TYPE STREQUAL "RUNTIME")
			target_link_libraries (${TARGET_NAME}
				common
				rt
				tsf4g
				scew
				expat
				pthread
				readline
				tdr
				ncurses
				game_data
				router_client
				server_base
			)
		endif ()
	elseif (MSVC)
		target_link_libraries (${TARGET_NAME}
			debug lua.lib
			optimized lua.lib
			debug toluappd.lib
			optimized toluapp.lib
			debug libprotobufd.lib
			optimized libprotobuf.lib
			debug libtconnapi_d.lib
			optimized libtconnapi.lib
		)

		if (TARGET_TYPE STREQUAL "RUNTIME")
			target_link_libraries (${TARGET_NAME} 
				debug common_d.lib
				optimized common.lib
				debug libcomm_d.lib	
				optimized libcomm.lib
				debug game_data_d.lib
				optimized game_data.lib
				debug router_client_d.lib
				optimized router_client.lib
				debug server_base_d.lib
				optimized server_base.lib
			)
		endif ()
	endif ()

	#set visual studio filter
	if (MSVC)
		set_target_properties (${TARGET_NAME} PROPERTIES LINK_FLAG_RELEASE /DEBUG)	

		foreach (SRC_FILE ${SRC_FILES})
			set (PARENT_DIR "")

			#.h _inl.h file parent dir
			if (${SRC_FILE} MATCHES ".*[.]h$")
				
				if (${SRC_FILE} MATCHES ".*_inl[.]h$")
					string (REGEX REPLACE "_inl[.]h$" ".c" CPP_FILE ${SRC_FILE})
				else ()
					string (REGEX REPLACE "[.]h$" ".c" CPP_FILE ${SRC_FILE})
				endif ()

				get_filename_component (CPP_FILE ${CPP_FILE} NAME)
				foreach (F ${SRC_FILES})
					if (${F} MATCHES "/${CPP_FILE}")
						get_filename_component (PARENT_DIR "${F}" PATH)
						get_filename_component (PARENT_DIR "${PARENT_DIR}" NAME)
						break ()
					endif ()
				endforeach ()
			endif ()

			#data file parent dir
			if(${SRC_FILE} MATCHES ".*_data[.]h$")
				set (PARENT_DIR "data")
			endif()

			#tolua file parent dir
			if ((PARENT_DIR STREQUAL "") AND (${SRC_FILE} MATCHES ".*tolua_.*[.]cpp$"))
				set(PARENT_DIR "generated")
			endif ()

			#lua file parent dir
			if (${SRC_FILE} MATCHES ".*[.]lua")
				get_filename_component (PARENT_DIR "${SRC_FILE}" PATH)
				get_filename_component (PARENT_DIR "${PARENT_DIR}" NAME)
				set(PARENT_DIR "lua\\${PARENT_DIR}")	
			endif()

			#default parent dir
			if (PARENT_DIR STREQUAL "")
				get_filename_component (PARENT_DIR "${SRC_FILE}" PATH)
				get_filename_component (PARENT_DIR "${PARENT_DIR}" NAME)
			endif ()

			# target parent dir
			if (${PARENT_DIR} STREQUAL ${TARGET_NAME})
				set (PARENT_DIR "main")
			endif ()

			#lua file parent dir
			if (${SRC_FILE} MATCHES ".*[.]lua$")
				get_filename_component (PARENT_DIR "${SRC_FILE}" PATH)
				get_filename_component (PARENT_DIR "${PARENT_DIR}" NAME)
				set (PARENT_DIR "lua\\${PARENT_DIR}")
			endif ()

			source_group ("${PARENT_DIR}" FILES "${SRC_FILE}")
			
			#log
			get_filename_component(SRC_FILE_NAME ${SRC_FILE} NAME)
			message(\tadded\ ${SRC_FILE_NAME}\ into\ group\ ${PARENT_DIR})

		endforeach ()
	endif ()

	message (post_project\ ${TARGET_NAME})
endfunction ()
