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
copy "${PHYSX_BINARY_DIR}\\cudart32_30_9.dll" "..\\..\\bin\\HelloWorld\\cudart32_30_9.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\NxCharacter.dll" "..\\..\\bin\\HelloWorld\\NxCharacter.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXLoader.dll" "..\\..\\bin\\HelloWorld\\PhysXLoader.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXCore.dll" "..\\..\\bin\\HelloWorld\\PhysXCore.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXCooking.dll" "..\\..\\bin\\HelloWorld\\PhysXCooking.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXDevice.dll" "..\\..\\bin\\HelloWorld\\PhysXDevice.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk_20113_1.dll" "..\\..\\bin\\HelloWorld\\fbxsdk_20113_1.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk_20113_1d.dll" "..\\..\\bin\\HelloWorld\\fbxsdk_20113_1d.dll";
VERBATIM
)