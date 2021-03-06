function (pre_project TARGET_TYPE APPLICATION_TYPE)
	get_filename_component (TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	get_filename_component (TARGET_DIR ${BASE_DIR}/src/${TARGET_NAME} ABSOLUTE)
	get_filename_component (TARGET_INC_DIR ${BASE_DIR}/inc/${TARGET_NAME} ABSOLUTE)

	#set global value
	set (TARGET_NAME ${TARGET_NAME} PARENT_SCOPE)
	set (TARGET_DIR ${TARGET_DIR} PARENT_SCOPE)
	set (TARGET_INC_DIR ${TARGET_INC_DIR} PARENT_SCOPE)
	set (TARGET_TYPE ${TARGET_TYPE} PARENT_SCOPE)
	set (APPLICATION_TYPE ${APPLICATION_TYPE} PARENT_SCOPE)

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
		if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			add_definitions ("/D_DEBUG")
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
        if(MSVC)
            #windows, see dll as runtime target, and imported lib as archive target
		    set_target_properties (${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${BASE_DIR}/lib)
		    set_target_properties (${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/lib)
		    set_target_properties (${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/lib)
		
            set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/lib)
		    set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/lib)
		    set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/lib})
        else()
		    set_target_properties (${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BASE_DIR}/bin)	
		    set_target_properties (${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/bin)	
		    set_target_properties (${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/bin)	
        endif()
	elseif (TARGET_TYPE STREQUAL "RUNTIME")
		set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/bin/${TARGET_NAME})
		set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/bin/${TARGET_NAME})
		set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/bin/${TARGET_NAME})
	endif ()
	
    if (MSVC)
    	set_target_properties (${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${TARGET_NAME}_d)
	else()
    	set_target_properties (${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${TARGET_NAME})
	endif()

	set_target_properties (${TARGET_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${TARGET_NAME})

    if (MSVC)
        list (FIND ENGINE_TARGET_LIST ${TARGET_NAME} find_engine_index)
        list (FIND TOOLS_TARGET_LIST ${TARGET_NAME} find_tools_index)

        if (${find_engine_index} GREATER -1)
            set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "engine") 
	        message(\tset\ folder\ engine\ for\ ${TARGET_NAME})
        elseif (${find_tools_index} GREATER -1)
            set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "tools") 
	        message(\tset\ folder\ tools\ for\ ${TARGET_NAME})
        else()
            set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "workspace") 
	        message(\tset\ folder\ workspace\ for\ ${TARGET_NAME})
        endif()
    endif()

	#default link libaray
	if (UNIX)
		if (APPLICATION_TYPE STREQUAL "SERVER")
			target_link_libraries (${TARGET_NAME}
				server_base
				game_data
				router_client
                db_proxy_client
			)
		endif ()
		
		if (TARGET_TYPE STREQUAL "RUNTIME")
			target_link_libraries (${TARGET_NAME}
				tconnapi
				common
				event event_core event_extra
				rt
				tsf4g
				scew
				expat
				pthread
				readline
				tdr
				ncurses
                anl
			)
		target_link_libraries (${TARGET_NAME}
			dl
			lua
			toluapp
			protobuf
            curl
		)
		endif ()
	elseif (MSVC)
	    add_definitions(-DCURL_STATICLIB)

		target_link_libraries (${TARGET_NAME}
		    libevent.lib libevent_core.lib libevent_extras.lib
			debug lua.lib
			optimized lua.lib
			debug toluappd.lib
			optimized toluapp.lib
			debug libprotobufd.lib
			optimized libprotobuf.lib
            debug libcurld.lib
            optimized libcurl.lib
            libssl.lib
            libcrypto.lib
            crypt32.lib
		    Wldap32.lib
		)

		if (TARGET_TYPE STREQUAL "RUNTIME")
			target_link_libraries (${TARGET_NAME} 
				debug common_d.lib
				optimized common.lib
				debug libtconnapi_d.lib
				optimized libtconnapi.lib
				debug libpal_d.lib
				optimized libpal.lib
			)
		endif ()
		
		if (APPLICATION_TYPE STREQUAL "SERVER")
			target_link_libraries (${TARGET_NAME}
				debug game_data_d.lib
				optimized game_data.lib
				debug router_client_d.lib
				optimized router_client.lib
				debug server_base_d.lib
				optimized server_base.lib
				debug db_proxy_client_d.lib
				optimized db_proxy_client.lib
			)
		endif ()
	endif ()
		
	if (APPLICATION_TYPE STREQUAL "SERVER")
		add_dependencies (${TARGET_NAME} server_base game_data db_proxy_client router_client common)
	else()
		add_dependencies (${TARGET_NAME} common)
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
			
            #yaml file parent dir
			if (${SRC_FILE} MATCHES ".*[.]yml$")
				get_filename_component (PARENT_DIR "${SRC_FILE}" PATH)
				get_filename_component (PARENT_DIR "${PARENT_DIR}" NAME)
				set (PARENT_DIR "yaml\\${PARENT_DIR}")
			endif ()

			source_group ("${PARENT_DIR}" FILES "${SRC_FILE}")
			
			#log
			get_filename_component(SRC_FILE_NAME ${SRC_FILE} NAME)
			message(\tadded\ ${SRC_FILE_NAME}\ into\ group\ ${PARENT_DIR})

		endforeach ()
	endif ()

	message (post_project\ ${TARGET_NAME})
endfunction ()
