set(TARGET shm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-g)


add_executable(${TARGET} shm_main.cpp)
set_target_properties(${TARGET} PROPERTIES FOLDER "test")

# link 右边的先被链接
target_link_libraries(${TARGET} PUBLIC gw_cellmgr)
