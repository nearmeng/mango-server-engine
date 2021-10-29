cmake_minimum_required(VERSION 3.10)
project(g6greatworldbase VERSION 0.0.1)

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Debug")
endif()
message("CMAKE_BUILD_TYPE:" ${CMAKE_BUILD_TYPE})

if (MSVC)
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

		set ("${flag}" "${${flag}} /bigobj /GR /fp:fast /Zc:wchar_t-")
		message ("${flag} ${${flag}}")
	endforeach ()

	set ("${CMAKE_C_FLAGS_RELEASE}" "${${CMAKE_C_FLAGS_RELEASE}} /arch:AVX2
	/Oi /Ot /GL")
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_INCLUDE "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_INCLUDE_PRIVATE "${PROJECT_INCLUDE}/private")
set(PROJECT_INCLUDE_PUBLIC "${PROJECT_INCLUDE}/public")

set(DEPLOY_DIR ${CMAKE_INSTALL_PREFIX})
# set(DEPLOY_STATIC_LIB_DIR ${DEPLOY_DIR}/${CMAKE_BUILD_TYPE}/static_lib)
# set(DEPLOY_SHARED_LIB_DIR ${DEPLOY_DIR}/${CMAKE_BUILD_TYPE}/shared_lib)
set(DEPLOY_STATIC_LIB_DIR ${DEPLOY_DIR}/lib)
set(DEPLOY_SHARED_LIB_DIR ${DEPLOY_DIR}/lib)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_definitions(-O0)
else()
  add_definitions(-O2)
endif()

add_definitions(-fPIC)
if(STATIC_LINK_STD)
  add_definitions(-fvisibility=hidden)
endif()
add_definitions(-DGW_LOCAL)
add_definitions(-std=c++17)
add_definitions(-g)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

include_directories(${PROJECT_INCLUDE_PUBLIC})

add_subdirectory(src)
add_subdirectory(test)

install(DIRECTORY ${PROJECT_INCLUDE_PUBLIC}/ DESTINATION ${DEPLOY_DIR}/include)