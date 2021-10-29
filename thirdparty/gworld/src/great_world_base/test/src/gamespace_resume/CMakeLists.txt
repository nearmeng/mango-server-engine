set(TARGET gamespace_resume)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-g)



add_executable(${TARGET} gamespace_resume.cpp)
set_target_properties(${TARGET} PROPERTIES FOLDER "test")

target_link_libraries(${TARGET} PUBLIC gw_mock gw_gamespace shm_3rd)
