#
# Created by jul3x on 16.07.19.
#

cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# Common build
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake_modules" ${CMAKE_MODULE_PATH})

# Change this if you're working on Windows to your SFML path.
set(SFML_ROOT "C:/SFML-2.5.1")

find_package(SFML 2 REQUIRED COMPONENTS graphics window system audio)

include_directories(../engine/include)
include_directories(../common/include)
include_directories(../game/include)
include_directories(${SFML_INCLUDE_DIR})

set(EXECUTABLE_NAME "Rag3Test")
set(J3X_TEST "J3xTest")
set(TEST_MAIN "gtest.cpp")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build_test")

file(GLOB_RECURSE TEST_SRC "src/*Test.cpp")
list(REMOVE_ITEM TEST_SRC "src/engine/j3x/Test.cpp")
file(GLOB_RECURSE ENGINE_SRC "../engine/src/*.cpp")
list(FILTER TEST_SRC EXCLUDE REGEX "src/engine/j3x/Test.cpp")

add_executable(${EXECUTABLE_NAME} ${TEST_MAIN} ${TEST_SRC} ${ENGINE_SRC})
add_executable(${J3X_TEST} "src/engine/j3x/Test.cpp" ${ENGINE_SRC})

target_link_libraries(${EXECUTABLE_NAME} stdc++fs)
target_link_libraries(${EXECUTABLE_NAME} gtest
        ${SFML_GRAPHICS_LIBRARY}
        ${SFML_WINDOW_LIBRARY}
        ${SFML_SYSTEM_LIBRARY}
        ${SFML_AUDIO_LIBRARY})

target_link_libraries(${J3X_TEST} stdc++fs)
target_link_libraries(${J3X_TEST}
        ${SFML_GRAPHICS_LIBRARY}
        ${SFML_WINDOW_LIBRARY}
        ${SFML_SYSTEM_LIBRARY}
        ${SFML_AUDIO_LIBRARY})

add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
add_test(NAME ${J3X_TEST} COMMAND "src/engine/j3x/semantic_checker_test.sh")


