cmake_minimum_required(VERSION 2.8.7)

project(FPS)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../bin")

include_directories( "./../../include" )

link_directories( "./../../lib" )

add_executable(FPS WIN32 FPS.cpp)

target_link_libraries( FPS Ovgl)
