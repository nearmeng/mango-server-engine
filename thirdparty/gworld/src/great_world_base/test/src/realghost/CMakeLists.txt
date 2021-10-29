set(TARGET realghost_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-g)


add_executable(spacecat space_cat.cpp)
set_target_properties(spacecat PROPERTIES FOLDER "test")
target_link_libraries(spacecat PUBLIC gw_mock gw_cellmgr gtest_3rd gtest_main_3rd)
if(UNIX)
  target_link_libraries(spacecat PUBLIC pthread)
endif()

add_executable(${TARGET} realghost_test.cpp)
set_target_properties(${TARGET} PROPERTIES FOLDER "test")

target_link_libraries(${TARGET} PUBLIC gw_mock gw_gamespace gtest_3rd gtest_main_3rd)
if(UNIX)
  target_link_libraries(${TARGET} PUBLIC pthread)
endif()
