project(Ovgl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ".\\..\\lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ".\\..\\lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ".\\..\\lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ".\\..\\lib")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories(${OPENAL_INCLUDE_DIR}
${CG_INCLUDE_DIR}
${FREEIMAGE_INCLUDE_DIR}
${FBX_INCLUDE_DIR}
${OGG_INCLUDE_DIR}
${VORBIS_INCLUDE_DIR}
${BULLET_INCLUDE_DIR}

link_directories(${OPENAL_LIBRARY_DIR}
${CG_LIBRARY_DIR}
${FREEIMAGE_LIBRARY_DIR}
${FBX_LIBRARY_DIR}
${OGG_LIBRARY_DIR}
${VORBIS_LIBRARY_DIR}
${BULLET_LIBRARY_DIR})

add_library( Ovgl SHARED "OvglAudio.h"
"OvglGraphics.h"
"OvglIncludes.h"
"OvglInstance.h"
"OvglMath.h"
"OvglMedia.h"
"OvglMesh.h"
"OvglScene.h"
"OvglAudio.cpp"
"OvglGraphics.cpp"
"OvglInstance.cpp"
"OvglMath.cpp"
"OvglMedia.cpp"
"OvglMesh.cpp"
"OvglScene.cpp")

target_link_libraries( Ovgl "wininet.lib"
"winmm.lib"
"opengl32.lib"
"glew32.lib"
"OpenAL32.lib"
"FreeImage.lib"
"cg.lib"
"cgGL.lib"
"libogg_static.lib"
"libvorbis_static.lib"
"libvorbisfile_static.lib"
optimized "fbxsdk-2012.1-md.lib"
optimized "BulletDynamics.lib"
optimized "BulletCollision.lib"
optimized "LinearMath.lib"
debug "fbxsdk-2012.1-mdd.lib")
debug "BulletDynamicsd.lib"
debug "BulletCollisiond.lib"
debug "LinearMathd.lib"

add_definitions( -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DKFBX_DLLINFO )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}\\ovgl\\Doxyfile" )
endif(OVGL_BUILD_DOC)	