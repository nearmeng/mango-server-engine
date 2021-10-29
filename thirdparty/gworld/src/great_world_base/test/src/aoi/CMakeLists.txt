set(TARGET aoi_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-g)


option(USE_ASAN "use asan" OFF)

if(USE_ASAN)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
  message("use asan")
endif()


add_executable(${TARGET} aoi_main.cpp)

# link 右边的先被链接
target_link_libraries(${TARGET} PUBLIC gw_mock gw_gamespace gtest_3rd gtest_main_3rd)
set_target_properties(${TARGET} PROPERTIES FOLDER "test")
if(UNIX)
  target_link_libraries(${TARGET} PUBLIC pthread)
endif()

if(USE_ASAN)
  target_link_libraries(${TARGET} PRIVATE asan)
endif()
