cmake_minimum_required(VERSION 2.8.7)

project(FPS)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../bin")

include_directories( "./../../include" )

link_directories( "./../../lib" )

add_executable(FPS WIN32 FPS.cpp)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")

set_target_properties(FPS PROPERTIES INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")

target_link_libraries( FPS Ovgl)
