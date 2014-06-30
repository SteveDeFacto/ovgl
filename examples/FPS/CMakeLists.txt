cmake_minimum_required(VERSION 2.8.7)

project(FPS)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ".")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../bin")

include_directories( "./../../include" )

link_directories( "./../../lib" )

add_executable(FPS WIN32 FPS.cpp)

target_link_libraries( FPS Ovgl)
