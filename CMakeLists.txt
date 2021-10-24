#
# Created by jul3x on 26.02.19.
#

cmake_minimum_required(VERSION 3.8)
project(Rag3)

set(CMAKE_CXX_STANDARD 17)

# Change this if you're working on Windows to your SFML and TGUI path.
set(SFML_ROOT "C:/SFML-2.5.1")
set(TGUI_ROOT "C:/TGUI-0.8")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build")
find_package(SFML 2 REQUIRED COMPONENTS graphics window system audio network)
find_package(TGUI 0.8 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/engine/include)
include_directories(${CMAKE_SOURCE_DIR}/common/include)
include_directories(${CMAKE_SOURCE_DIR}/editor/include)
include_directories(${CMAKE_SOURCE_DIR}/game/include)
include_directories(${CMAKE_SOURCE_DIR}/multi/include)
include_directories(${SFML_INCLUDE_DIR} ${TGUI_INCLUDE_DIR})

# R3Engine

set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build")
file(GLOB_RECURSE ENGINE_SRC "engine/src/*.cpp")
set(R3ENGINE_LIBRARY "R3Engine")

add_library(${R3ENGINE_LIBRARY} STATIC ${ENGINE_SRC})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${R3ENGINE_LIBRARY}
            stdc++fs
            ${SFML_GRAPHICS_LIBRARY_DEBUG}
            ${SFML_WINDOW_LIBRARY_DEBUG}
            ${SFML_SYSTEM_LIBRARY_DEBUG}
            ${SFML_AUDIO_LIBRARY_DEBUG}
            ${SFML_NETWORK_LIBRARY_DEBUG}
            ${TGUI_LIBRARY_DEBUG})
else()
    target_link_libraries(${R3ENGINE_LIBRARY}
            stdc++fs
            ${SFML_GRAPHICS_LIBRARY_RELEASE}
            ${SFML_WINDOW_LIBRARY_RELEASE}
            ${SFML_SYSTEM_LIBRARY_RELEASE}
            ${SFML_AUDIO_LIBRARY_RELEASE}
            ${SFML_NETWORK_LIBRARY_RELEASE}
            ${TGUI_LIBRARY_RELEASE})
endif()

# Common
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build")
file(GLOB_RECURSE COMMON_SRC "common/src/*.cpp")
set(COMMON_LIBRARY "Common")

add_library(${COMMON_LIBRARY} STATIC ${COMMON_SRC})

# R3Editor
set(EDITOR_NAME "Rag3Editor")
file(GLOB_RECURSE EDITOR_SRC "editor/src/*.cpp")
add_executable(${EDITOR_NAME} ${EDITOR_SRC})

target_link_libraries(${EDITOR_NAME}
        ${COMMON_LIBRARY}
        ${R3ENGINE_LIBRARY})

# RAG3
set(EXECUTABLE_NAME "Rag3")
file(GLOB_RECURSE GAME_SRC "game/src/*.cpp")
add_executable(${EXECUTABLE_NAME} ${GAME_SRC})

target_link_libraries(${EXECUTABLE_NAME}
        ${COMMON_LIBRARY}
        ${R3ENGINE_LIBRARY})

# RAG3 Server & Client
set(SERVER_NAME "Rag3Server")
set(CLIENT_NAME "Rag3Client")
file(GLOB_RECURSE MULTI_SRC "multi/src/*.cpp")
list(FILTER MULTI_SRC EXCLUDE REGEX "(server|client|main_client|main_server)")

file(GLOB_RECURSE SERVER_SRC "multi/src/server/*.cpp")
file(GLOB_RECURSE CLIENT_SRC "multi/src/client/*.cpp")

add_executable(${SERVER_NAME} ${MULTI_SRC} ${SERVER_SRC} multi/src/main_server.cpp)
add_executable(${CLIENT_NAME} ${MULTI_SRC} ${CLIENT_SRC} multi/src/main_client.cpp)

target_link_libraries(${SERVER_NAME}
        ${COMMON_LIBRARY}
        ${R3ENGINE_LIBRARY})

target_link_libraries(${CLIENT_NAME}
        ${COMMON_LIBRARY}
        ${R3ENGINE_LIBRARY})


