cmake_minimum_required (VERSION 2.8)

get_filename_component (BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. ABSOLUTE)
get_filename_component (FOLDER_NAME ${CMAKE_BINARY_DIR} NAME)

project (${FOLDER_NAME})

include (precompile.cmake)
include (toluapp.cmake)
include (proto.cmake)
include (project.cmake)

#set visual studio build type
if (CMAKE_CONFIGURATION_TYPES)
	set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif ()

if (_FINAL)
	add_definitions(-D_FINAL)
endif ()


#set platform releated compile option
if (UNIX)
	message("os is linux")

	if (NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Debug)
	endif ()

	string(COMPARE EQUAL ${CMAKE_BUILD_TYPE} "Release" is_release)
	string(COMPARE EQUAL ${CMAKE_BUILD_TYPE} "Debug" is_debug)

	if (${is_release})
		set(CMAKE_CXX_FLAGS "-O2 -fPIC -g -msse4.2 -std=c++0x")
	elseif (${is_debug})
		set(CMAKE_CXX_FLAGS "-fPIC -g -msse4.2 -std=c++0x")
	endif ()
elseif (MSVC)
	message("os is windows")

	add_definitions (/MP)

	foreach (flag
		CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG_INIT
		CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINIFO
		CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG_INIT
		CMAKE_CXX_FLAGS
	) 
		string (FIND ${flag} "RELEASE" is_release)
		string (FIND ${flag} "DEBUG" is_debug)

		if (${is_release} GREATER -1) 
			# add MD
			string (FIND "${${flag}}" "/MD" has_md)
			if(has_md EQUAL -1)
				set("${flag}" "${${flag}}" /MD)
			endif()
		elseif (${is_debug} GREATER -1)
			# add MDd
			string (FIND "${${flag}}" "/MDd" has_mdt)
			if(has_mdt EQUAL -1)
				set("${flag}" "${${flag}}" /MDd)
			endif()
		endif ()

		string (REPLACE "/MD" "/MT" "${flag}" "${${flag}}")
		string (REPLACE "EHsc" "" "${flag}" "${${flag}}")

		set ("${flag}" "${${flag}} /bigobj /GR- /fp:fast /Zc:wchar_t-")
		message ("${flag} ${${flag}}")
	endforeach ()

	set ("${CMAKE_C_FLAGS_RELEASE}" "${${CMAKE_C_FLAGS_RELEASE}} /arch:AVX2
	/Oi /Ot /GL")
endif ()

include_directories (
	${BASE_DIR}/inc
	${BASE_DIR}/inc/common
	${BASE_DIR}/inc/server_base
	${BASE_DIR}/game_data
	${BASE_DIR}/src/proto
	${BASE_DIR}/thirdparty
	${BASE_DIR}/thirdparty/lua/inc
	${BASE_DIR}/thirdparty/toluapp/inc
	${BASE_DIR}/thirdparty/tbase
	${BASE_DIR}/thirdparty/tbase/include
	${BASE_DIR}/thirdparty/protobuf/include
	${BASE_DIR}/thirdparty/tconnd/include
	${BASE_DIR}/thirdparty/lmdb/include
	${BASE_DIR}/thirdparty/libevent/include
)

link_directories (
	${BASE_DIR}/lib
	${BASE_DIR}/thirdparty
	${BASE_DIR}/thirdparty/lua/lib
	${BASE_DIR}/thirdparty/toluapp/lib
	${BASE_DIR}/thirdparty/tbase/lib
	${BASE_DIR}/thirdparty/protobuf/lib
	${BASE_DIR}/thirdparty/tconnd/lib
	${BASE_DIR}/thirdparty/lmdb/lib
	${BASE_DIR}/thirdparty/libevent/lib
)

pre_toluapp(${BASE_DIR}/src)
preprocess_proto("src/proto")

#project directories
add_subdirectory (common)
add_subdirectory (game_data)
add_subdirectory (server_base)
add_subdirectory (router_client)
add_subdirectory (server)
add_subdirectory (router_server)
add_subdirectory (service_mgr_server)
