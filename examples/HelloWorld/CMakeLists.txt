project(HelloWorld)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../../bin/HelloWorld")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../../bin/HelloWorld")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories( "${CMAKE_SOURCE_DIR}/ovgl" )

link_directories( "${CMAKE_SOURCE_DIR}/lib" )

add_executable(HelloWorld WIN32 HelloWorld.cpp)

target_link_libraries( HelloWorld "winmm.lib" debug "ovgld.lib" optimized "ovgl.lib")

add_custom_command(
TARGET
HelloWorld
POST_BUILD
COMMAND
copy "..\\..\\lib\\ovgl.dll" "..\\..\\bin\\HelloWorld\\ovgl.dll";
COMMAND
copy "..\\..\\lib\\ovgld.dll" "..\\..\\bin\\HelloWorld\\ovgld.dll";
COMMAND
copy "${FREEIMAGE_LIBRARY_DIR}\\FreeImage.dll" "..\\..\\bin\\HelloWorld\\FreeImage.dll";
COMMAND
copy "${CG_BINARY_DIR}\\cg.dll" "..\\..\\bin\\HelloWorld\\cg.dll";
COMMAND
copy "${CG_BINARY_DIR}\\cgGL.dll" "..\\..\\bin\\HelloWorld\\cgGL.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk-2012.1.dll" "..\\..\\bin\\HelloWorld\\fbxsdk-2012.1.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk-2012.1d.dll" "..\\..\\bin\\HelloWorld\\fbxsdk-2012.1d.dll";
VERBATIM
)