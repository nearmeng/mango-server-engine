project(shm)
set(LIB_NAME ${CMAKE_PROJECT_NAME}) # 工程名, 默认文件夹名

cmake_minimum_required(VERSION 3.0)
# set(DEPENDENCY_MODULE log)

################################################
# 下面的一般不用改
################################################

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE ALL_SOURCE_FILES 
    *.h
    *.cpp
    *.cc
    *.c
)
set(SOURCE_FILES ${ALL_SOURCE_FILES})
set(TEST_SOURCE_FILES ${ALL_SOURCE_FILES})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(TEST_SOURCE_REGEX ".*_test.(cpp|cc|h)$") 
list(FILTER SOURCE_FILES EXCLUDE REGEX ${TEST_SOURCE_REGEX})
list(FILTER TEST_SOURCE_FILES INCLUDE REGEX ${TEST_SOURCE_REGEX})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_SOURCE_DIR}/source/)

add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
set_property(TARGET ${LIB_NAME} PROPERTY FOLDER ${PROJ_NAME})

set(BUILD_TEST ON)

# if to use with address sanitizer uncomment below
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1 -g ")
# set(BUILD_TEST OFF)


if(UNIX AND NOT APPLE AND NOT ANDROID)
        set(LINUX TRUE)
endif()

if(DEPENDENCY_MODULE)
    target_link_libraries(${LIB_NAME} ${DEPENDENCY_MODULE})
endif(DEPENDENCY_MODULE)

if(BUILD_TEST)
    if (LINUX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --static")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --static")
    endif(LINUX)

	find_package(GTest REQUIRED)
    find_package(Threads)
    set(LIBTEST_NAME ${LIB_NAME}_test)
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(${LIBTEST_NAME} ${TEST_SOURCE_FILES})
    target_link_libraries(${LIBTEST_NAME} GTest::GTest GTest::Main ${LIB_NAME} ${CMAKE_THREAD_LIBS_INIT})
    set_property(TARGET ${LIBTEST_NAME} PROPERTY FOLDER "test")
    gtest_add_tests(${LIBTEST_NAME} "" AUTO)
endif(BUILD_TEST)
