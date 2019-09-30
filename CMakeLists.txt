#
# Created by jprolejko on 26.02.19.
#

cmake_minimum_required(VERSION 3.6)
project(Rag3)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)

include_directories(.)
include_directories(include)
include_directories(${SFML_INCLUDE_DIR})

# Define sources and executable
set(EXECUTABLE_NAME "Rag3")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build")

file(GLOB_RECURSE MAIN_SRC "src/*.cpp")

add_executable(${EXECUTABLE_NAME} ${MAIN_SRC})

target_link_libraries(${EXECUTABLE_NAME} stdc++fs)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})



