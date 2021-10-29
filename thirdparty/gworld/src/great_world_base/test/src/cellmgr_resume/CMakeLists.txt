set(TARGET cellmgr_resume)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-g)

function (set_project_ide_folder project folder)
	if(TARGET ${project})
		set_target_properties(${project} PROPERTIES FOLDER ${folder})
	endif()
endfunction(set_project_ide_folder)

add_executable(${TARGET} cellmgr_resume.cpp)
# set_target_properties(${TARGET} PROPERTIES FOLDER "test")
set_project_ide_folder(cellmgr_resume test)
target_link_libraries(${TARGET} PUBLIC gw_mock gw_cellmgr shm_3rd)
