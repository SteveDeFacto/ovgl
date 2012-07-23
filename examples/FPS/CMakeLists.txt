cmake_minimum_required(VERSION 2.8.7)

project(FPS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ".\\..\\..\\bin\\FPS")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ".\\..\\..\\bin\\FPS")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories( "${CMAKE_SOURCE_DIR}\\ovgl" )

link_directories( "${CMAKE_SOURCE_DIR}\\lib" )

add_executable(FPS WIN32 FPS.cpp)

target_link_libraries( FPS debug "ovgld.lib" optimized "ovgl.lib")

add_custom_command(
TARGET
FPS
POST_BUILD
COMMAND
copy "..\\..\\lib\\ovgl.dll" "..\\..\\bin\\FPS\\ovgl.dll";
COMMAND
copy "..\\..\\lib\\ovgld.dll" "..\\..\\bin\\FPS\\ovgld.dll";
VERBATIM
)