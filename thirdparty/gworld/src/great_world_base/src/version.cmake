#vProjectDir
execute_process(
  COMMAND basename ${CMAKE_CURRENT_SOURCE_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE vProjectDir
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

#vGitBranch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE vGitBranch
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

#vGitCommit
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE vGitCommit
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(vFileName "${PROJECT_INCLUDE_PUBLIC}/great_world_base/common/version.h")

file(WRITE ${vFileName} "#pragma once\r\n" )
if(${SAVE_GIT_VERSION})
file(APPEND ${vFileName} "// ${vGitBranch}:${vGitCommit}\r\n")
endif(${SAVE_GIT_VERSION})
file(APPEND ${vFileName} "#define GREAT_WORLD_BASE_VERSION \"${PROJECT_VERSION}\"")
file(APPEND ${vFileName} "\r\n")