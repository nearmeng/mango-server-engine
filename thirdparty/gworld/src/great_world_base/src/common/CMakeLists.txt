cmake_minimum_required(VERSION 3.10)
project(common)

set(TARGET gw_common)

file(GLOB_RECURSE SRC_LIST *.h *.cpp)
add_library(${TARGET} OBJECT ${SRC_LIST})
set_target_properties(${TARGET} PROPERTIES FOLDER "src")
