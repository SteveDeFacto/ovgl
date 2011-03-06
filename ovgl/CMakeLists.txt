project(Ovgl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "./../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "./../lib")
set(CMAKE_DEBUG_POSTFIX "d")

find_package( DX10 )
find_package( PhysX )
find_package( Ogg )
find_package( XAudio2 )
find_package( FBXSDK )
find_package( Recast )

include_directories( ${DX3D10_INCLUDE_PATH}
"$ENV{PROGRAMW6432}/Autodesk/FBX/FbxSdk/2011.3.1/include"
"$ENV{PROGRAMFILES}/Recastnavigation/Detour/Include"
"$ENV{PROGRAMFILES}/Recastnavigation/Recast/Include"
"$ENV{PROGRAMFILES}/OGG/include"
${PHYSX_INCLUDE_PATH}
${PHYSXLOADER_INCLUDE_PATH}
${PHYSXCHARACTER_INCLUDE_PATH}
${PHYSXCOOKING_INCLUDE_PATH}
${PHYSXFOUNDATION_INCLUDE_PATH})

link_directories( ${D3D10_LIBRARY}
"$ENV{PROGRAMW6432}/Autodesk/FBX/FbxSdk/2011.3.1/lib/vs2010"
"$ENV{PROGRAMFILES}/OGG/libvorbis-1.3.1/bin"
"$ENV{PROGRAMFILES}/OGG/libogg-1.2.0/bin"
${PHYSXLOADER_LIBRARY})

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