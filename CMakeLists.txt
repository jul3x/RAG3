#
# Created by jul3x on 26.02.19.
#

cmake_minimum_required(VERSION 3.6)
project(Rag3)

set(CMAKE_CXX_STANDARD 14)

# Change this if you're working on Windows to your SFML path.
set(SFML_ROOT "C:/SFML-2.5.1")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED COMPONENTS graphics window system audio)

include_directories(${CMAKE_SOURCE_DIR}/engine/include)
include_directories(${CMAKE_SOURCE_DIR}/game/include)
include_directories(${SFML_INCLUDE_DIR})

# R3Engine
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build")
file(GLOB_RECURSE ENGINE_SRC "engine/src/*.cpp")
set(R3ENGINE_LIBRARY "R3Engine")

add_library(${R3ENGINE_LIBRARY} STATIC ${ENGINE_SRC})
target_link_libraries(${R3ENGINE_LIBRARY}
        stdc++fs
        ${SFML_GRAPHICS_LIBRARY_RELEASE}
        ${SFML_WINDOW_LIBRARY_RELEASE}
        ${SFML_SYSTEM_LIBRARY_RELEASE}
        ${SFML_AUDIO_LIBRARY_RELEASE})

# RAG3
set(EXECUTABLE_NAME "Rag3")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build")

file(GLOB_RECURSE GAME_SRC "game/src/*.cpp")
add_executable(${EXECUTABLE_NAME} ${GAME_SRC})

target_link_libraries(${EXECUTABLE_NAME} stdc++fs)
target_link_libraries(${EXECUTABLE_NAME}
        ${R3ENGINE_LIBRARY}
        ${SFML_GRAPHICS_LIBRARY_RELEASE}
        ${SFML_WINDOW_LIBRARY_RELEASE}
        ${SFML_SYSTEM_LIBRARY_RELEASE}
        ${SFML_AUDIO_LIBRARY_RELEASE})

