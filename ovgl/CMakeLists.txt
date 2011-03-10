project(Ovgl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ".\\..\\lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ".\\..\\lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ".\\..\\lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ".\\..\\lib")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories(${DX3D10_INCLUDE_PATH}
${FBX_INCLUDE_DIR}
${OGG_INCLUDE_DIR}
${VORBIS_INCLUDE_DIR}
${PHYSX_INCLUDE_DIR}
${PHYSX_LOADER_INCLUDE_DIR}
${PHYSX_FOUNDATION_INCLUDE_DIR}
${PHYSX_COOKING_INCLUDE_DIR}
${PHYSX_CHARACTER_INCLUDE_DIR})

link_directories(${D3D10_LIBRARY}
${FBX_LIBRARY_DIR}
${OGG_LIBRARY_DIR}
${VORBIS_LIBRARY_DIR}
${PHYSX_LIBRARY_DIR})

add_library( Ovgl SHARED "OvglAudio.h"
"OvglGraphics.h"
"OvglIncludes.h"
"OvglInstance.h"
"OvglMath.h"
"OvglMesh.h"
"OvglScene.h"
"OvglAudio.cpp"
"OvglGraphics.cpp"
"OvglInstance.cpp"
"OvglMath.cpp"
"OvglMesh.cpp"
"OvglScene.cpp")

target_link_libraries( Ovgl "wininet.lib"
"winmm.lib"
"d3d10.lib"
optimized "d3dx10.lib"
debug "d3dx10d.lib"
"DXGI.lib"
"PhysXLoader.lib"
"PhysXCooking.lib"
"NxCharacter.lib"
"libogg_static.lib"
"libvorbis_static.lib"
"libvorbisfile_static.lib"
"x3daudio.lib"
optimized "fbxsdk_20113_1.lib"
debug "fbxsdk_20113_1d.lib")

add_definitions( -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DKFBX_DLLINFO )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}\\ovgl\\Doxyfile" )
endif(OVGL_BUILD_DOC)	