project(FPS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ".\\..\\..\\bin\\FPS")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ".\\..\\..\\bin\\FPS")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories( "${CMAKE_SOURCE_DIR}\\ovgl" )

link_directories( "${CMAKE_SOURCE_DIR}\\lib" )

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
copy "${FREEIMAGE_LIBRARY_DIR}\\FreeImage.dll" "..\\..\\bin\\FPS\\FreeImage.dll";
COMMAND
copy "${CG_BINARY_DIR}\\cg.dll" "..\\..\\bin\\FPS\\cg.dll";
COMMAND
copy "${CG_BINARY_DIR}\\cgGL.dll" "..\\..\\bin\\FPS\\cgGL.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk-2012.1.dll" "..\\..\\bin\\FPS\\fbxsdk-2012.1.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk-2012.1d.dll" "..\\..\\bin\\FPS\\fbxsdk-2012.1d.dll";
VERBATIM
)