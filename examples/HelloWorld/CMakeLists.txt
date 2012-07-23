cmake_minimum_required(VERSION 2.8.7)

project(HelloWorld)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../../bin/HelloWorld")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../../bin/HelloWorld")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories( "${CMAKE_SOURCE_DIR}/ovgl" )

link_directories( "${CMAKE_SOURCE_DIR}/lib" )

add_executable(HelloWorld WIN32 HelloWorld.cpp)

target_link_libraries( HelloWorld debug "ovgld.lib" optimized "ovgl.lib")

add_custom_command(
TARGET
HelloWorld
POST_BUILD
COMMAND
copy "..\\..\\lib\\ovgl.dll" "..\\..\\bin\\HelloWorld\\ovgl.dll";
COMMAND
copy "..\\..\\lib\\ovgld.dll" "..\\..\\bin\\HelloWorld\\ovgld.dll";
VERBATIM
)