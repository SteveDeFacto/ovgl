cmake_minimum_required(VERSION 2.8.7)

project(FPS)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../bin")
set(CMAKE_DEBUG_POSTFIX "_d")

include_directories( "./../../include" "/usr/local/include/bullet")

link_directories( "./../../lib" )

add_executable(FPS WIN32 FPS.cpp)

target_link_libraries( FPS debug "Ovgl_d" optimized "Ovgl")
