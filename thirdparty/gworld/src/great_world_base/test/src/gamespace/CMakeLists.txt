set(TARGET gamespace_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-g)

# add_definitions(-DRESUME_TEST)


add_executable(${TARGET} gamespace_main.cpp)

target_link_libraries(${TARGET} PUBLIC gw_mock gw_gamespace gtest_3rd shm_3rd)
set_target_properties(${TARGET} PROPERTIES FOLDER "test")

if(UNIX)
  target_link_libraries(${TARGET} PUBLIC pthread)
endif()
