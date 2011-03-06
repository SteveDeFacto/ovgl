project(FPS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../../bin/FPS")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../../bin/FPS")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories( "${CMAKE_SOURCE_DIR}/ovgl" )

link_directories( "${CMAKE_SOURCE_DIR}/lib" )

add_executable(FPS WIN32 FPS.cpp)

target_link_libraries( FPS "winmm.lib" debug "ovgld.lib" optimized "ovgl.lib")

add_custom_command(
TARGET
FPS 
POST_BUILD
COMMAND
copy "..\\..\\lib\\ovgl.dll" "..\\..\\bin\\FPS\\ovgl.dll";
COMMAND
copy "..\\..\\lib\\ovgld.dll" "..\\..\\bin\\FPS\\ovgld.dll";
COMMAND
copy "..\\..\\lib\\NxCharacter.dll" "..\\..\\bin\\FPS\\NxCharacter.dll";
COMMAND
copy "..\\..\\lib\\fbxsdk_20113_1.dll" "..\\..\\bin\\FPS\\fbxsdk_20113_1.dll";
COMMAND
copy "..\\..\\lib\\fbxsdk_20113_1d.dll" "..\\..\\bin\\FPS\\fbxsdk_20113_1d.dll";
VERBATIM
)