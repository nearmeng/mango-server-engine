if(WIN32)
  add_library(gtest_3rd STATIC IMPORTED GLOBAL)
  set_target_properties(gtest_3rd PROPERTIES
      IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest/win_lib/gtestd.lib
      INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest/include)

  add_library(gtest_main_3rd STATIC IMPORTED GLOBAL)
  set_target_properties(gtest_main_3rd PROPERTIES
      IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest/win_lib/gtest_maind.lib
      INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest/include)
else()
  add_library(gtest_3rd STATIC IMPORTED GLOBAL)
  set_target_properties(gtest_3rd PROPERTIES
      IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest/lib/libgtest.a
      INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest/include)

  add_library(gtest_main_3rd STATIC IMPORTED GLOBAL)
  set_target_properties(gtest_main_3rd PROPERTIES
      IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest/lib/libgtest_main.a
      INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest/include)
endif()

add_library(shm_3rd STATIC IMPORTED GLOBAL)
set_target_properties(shm_3rd PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/shm/lib/libshm.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/deps/shm/src)

set(PROJECT_TEST_PUBLIC_MOCK ${CMAKE_CURRENT_SOURCE_DIR}/src/mock)
file(GLOB_RECURSE MOCK_TIMER_LIST ${PROJECT_TEST_PUBLIC_MOCK}/timer/*.cpp)
file(GLOB_RECURSE MOCK_SRC_LIST ${PROJECT_TEST_PUBLIC_MOCK}/*.cpp)

add_library(gw_mock STATIC ${MOCK_SRC_LIST})
set_target_properties(gw_mock PROPERTIES FOLDER "test")
target_include_directories(gw_mock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/mock)
target_include_directories(gw_mock PUBLIC ${PROJECT_INCLUDE_PRIVATE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(src)
