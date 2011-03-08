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
copy "${PHYSX_BINARY_DIR}\\cudart32_30_9.dll" "..\\..\\bin\\FPS\\cudart32_30_9.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\NxCharacter.dll" "..\\..\\bin\\FPS\\NxCharacter.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXLoader.dll" "..\\..\\bin\\FPS\\PhysXLoader.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXCore.dll" "..\\..\\bin\\FPS\\PhysXCore.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXCooking.dll" "..\\..\\bin\\FPS\\PhysXCooking.dll";
COMMAND
copy "${PHYSX_BINARY_DIR}\\PhysXDevice.dll" "..\\..\\bin\\FPS\\PhysXDevice.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk_20113_1.dll" "..\\..\\bin\\FPS\\fbxsdk_20113_1.dll";
COMMAND
copy "${FBX_LIBRARY_DIR}\\fbxsdk_20113_1d.dll" "..\\..\\bin\\FPS\\fbxsdk_20113_1d.dll";
VERBATIM
)