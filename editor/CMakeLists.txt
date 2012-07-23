cmake_minimum_required(VERSION 2.8.7)

project(editor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../bin/editor")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../bin/editor")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories( "${CMAKE_SOURCE_DIR}/ovgl" )

link_directories( "${CMAKE_SOURCE_DIR}/lib" )

add_executable( editor WIN32 editor.cpp)

target_link_libraries( editor debug "ovgld.lib" optimized "ovgl.lib")

add_custom_command(
TARGET
editor 
POST_BUILD
COMMAND
copy "..\\lib\\ovgl.dll" "..\\bin\\editor\\ovgl.dll";
COMMAND
copy "..\\lib\\ovgld.dll" "..\\bin\\editor\\ovgld.dll";
VERBATIM
)