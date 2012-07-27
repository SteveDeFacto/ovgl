cmake_minimum_required(VERSION 2.8.7)

project(editor)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../bin")
set(CMAKE_DEBUG_POSTFIX "_d")

include_directories( "./../include" "/usr/local/include/bullet")

link_directories( "./../lib" )

add_executable( editor WIN32 editor.cpp)

target_link_libraries( editor debug "Ovgl_d" optimized "Ovgl")
