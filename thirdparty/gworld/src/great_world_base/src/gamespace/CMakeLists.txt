cmake_minimum_required(VERSION 3.10)
project(g6gamespace)

set(TARGET gw_gamespace)

file(GLOB_RECURSE SRC_LIST *.h *.cpp)
add_library(${TARGET} STATIC $<TARGET_OBJECTS:gw_common> ${SRC_LIST})


install(TARGETS ${TARGET} ARCHIVE DESTINATION ${DEPLOY_STATIC_LIB_DIR}/)
set_target_properties(${TARGET} PROPERTIES FOLDER "src")

if(BUILD_SHARD_LIB)
  if(STATIC_LINK_STD)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
  endif()
  set(SHARED_TARGET ${TARGET}-shared)    #设置静态库的原始名称
  message("begin build ${SHARED_TARGET}")
  add_library(${SHARED_TARGET} SHARED $<TARGET_OBJECTS:gw_common> ${SRC_LIST} ${COMMON_SRC_LIST})
  set_target_properties(${SHARED_TARGET} PROPERTIES OUTPUT_NAME ${TARGET})

  install(TARGETS ${SHARED_TARGET} LIBRARY DESTINATION ${DEPLOY_SHARED_LIB_DIR}/)
endif()