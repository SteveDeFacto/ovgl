project(editor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../../bin/editor")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../../bin/editor")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories( "${CMAKE_SOURCE_DIR}/ovgl" )

link_directories( "${CMAKE_SOURCE_DIR}/lib" )

add_executable(editor WIN32 editor.cpp)

target_link_libraries( editor "winmm.lib" debug "ovgld.lib" optimized "ovgl.lib")

add_custom_command(
TARGET
editor 
POST_BUILD
COMMAND
copy "..\\..\\lib\\ovgl.dll" "..\\..\\bin\\editor\\ovgl.dll";
COMMAND
copy "..\\..\\lib\\ovgld.dll" "..\\..\\bin\\editor\\ovgld.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\cudart32_30_9.dll" "..\\..\\bin\\editor\\cudart32_30_9.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\NxCharacter.dll" "..\\..\\bin\\editor\\NxCharacter.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXLoader.dll" "..\\..\\bin\\editor\\PhysXLoader.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXCore.dll" "..\\..\\bin\\editor\\PhysXCore.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXCooking.dll" "..\\..\\bin\\editor\\PhysXCooking.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXDevice.dll" "..\\..\\bin\\editor\\PhysXDevice.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk_20113_1.dll" "..\\..\\bin\\editor\\fbxsdk_20113_1.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk_20113_1d.dll" "..\\..\\bin\\editor\\fbxsdk_20113_1d.dll";
VERBATIM
)