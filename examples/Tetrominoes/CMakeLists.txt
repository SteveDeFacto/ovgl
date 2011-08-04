project(Tetrominoes)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../../bin/Tetrominoes")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../../bin/Tetrominoes")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories( "${CMAKE_SOURCE_DIR}/ovgl" )

link_directories( "${CMAKE_SOURCE_DIR}/lib" )

add_executable(Tetrominoes WIN32 Tetrominoes.cpp)

target_link_libraries( Tetrominoes "winmm.lib" debug "ovgld.lib" optimized "ovgl.lib")

add_custom_command(
TARGET
Tetrominoes
POST_BUILD
COMMAND
copy "..\\..\\lib\\ovgl.dll" "..\\..\\bin\\Tetrominoes\\ovgl.dll";
COMMAND
copy "..\\..\\lib\\ovgld.dll" "..\\..\\bin\\Tetrominoes\\ovgld.dll";
COMMAND
copy "${FREEIMAGE_LIBRARY_DIR}\\FreeImage.dll" "..\\..\\bin\\Tetrominoes\\FreeImage.dll";
COMMAND
copy "${CG_BINARY_DIR}\\cg.dll" "..\\..\\bin\\Tetrominoes\\cg.dll";
COMMAND
copy "${CG_BINARY_DIR}\\cgGL.dll" "..\\..\\bin\\Tetrominoes\\cgGL.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk-2012.1.dll" "..\\..\\bin\\Tetrominoes\\fbxsdk-2012.1.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk-2012.1d.dll" "..\\..\\bin\\Tetrominoes\\fbxsdk-2012.1d.dll";
VERBATIM
)