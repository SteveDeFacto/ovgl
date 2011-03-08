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
copy "..\\..\\lib\\cudart32_30_9.dll" "..\\..\\bin\\HelloWorld\\cudart32_30_9.dll";
COMMAND
copy "..\\..\\lib\\NxCharacter.dll" "..\\..\\bin\\HelloWorld\\NxCharacter.dll";
COMMAND
copy "..\\..\\lib\\PhysXLoader.dll" "..\\..\\bin\\HelloWorld\\PhysXLoader.dll";
COMMAND
copy "..\\..\\lib\\PhysXCore.dll" "..\\..\\bin\\HelloWorld\\PhysXCore.dll";
COMMAND
copy "..\\..\\lib\\PhysXCooking.dll" "..\\..\\bin\\HelloWorld\\PhysXCooking.dll";
COMMAND
copy "..\\..\\lib\\PhysXDevice.dll" "..\\..\\bin\\HelloWorld\\PhysXDevice.dll";
COMMAND
copy "..\\..\\lib\\fbxsdk_20113_1.dll" "..\\..\\bin\\HelloWorld\\fbxsdk_20113_1.dll";
COMMAND
copy "..\\..\\lib\\fbxsdk_20113_1d.dll" "..\\..\\bin\\HelloWorld\\fbxsdk_20113_1d.dll";
VERBATIM
)