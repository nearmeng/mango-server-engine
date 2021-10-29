set(TARGET common_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-g)


add_executable(${TARGET} timer_main.cpp)

target_link_libraries(${TARGET} PUBLIC gw_mock gw_cellmgr gtest_3rd gtest_main_3rd)
set_target_properties(${TARGET} PROPERTIES FOLDER "test")
if(UNIX)
  target_link_libraries(${TARGET} PUBLIC pthread)
endif()
