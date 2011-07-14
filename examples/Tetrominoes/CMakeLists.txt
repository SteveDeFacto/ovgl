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
copy "${PHYSX_BINARY_DIR}\\cudart32_30_9.dll" "..\\..\\bin\\Tetrominoes\\cudart32_30_9.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\NxCharacter.dll" "..\\..\\bin\\Tetrominoes\\NxCharacter.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXLoader.dll" "..\\..\\bin\\Tetrominoes\\PhysXLoader.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXCore.dll" "..\\..\\bin\\Tetrominoes\\PhysXCore.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXCooking.dll" "..\\..\\bin\\Tetrominoes\\PhysXCooking.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXDevice.dll" "..\\..\\bin\\Tetrominoes\\PhysXDevice.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk_20113_1.dll" "..\\..\\bin\\Tetrominoes\\fbxsdk_20113_1.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk_20113_1d.dll" "..\\..\\bin\\Tetrominoes\\fbxsdk_20113_1d.dll";
VERBATIM
)