cmake_minimum_required(VERSION 2.8.7)

project(Ovgl)

set(LIBRARY_OUTPUT_PATH "./../lib")
set(CMAKE_DEBUG_POSTFIX "_d")

IF(WIN32)
    include_directories(
    "${SDL_DIR}/include"
    "${OPENAL_DIR}/include"
    "${CG_DIR}/include"
    "${ASSIMP_DIR}/include"
    "${BULLET_DIR}/include"
    "${BULLET_DIR}/include/bullet"
    "${GLEW_DIR}/include"
	"${FREETYPE_DIR}/include"
	"${FREEIMAGE_DIR}/include"
	"${FFMPEG_DIR}/include"
	"./../dependencies/FreeType/include/freetype2"
	"./../include"
	"./../dependencies/Bullet/include/bullet")

    link_directories(
    "${SDL_DIR}/lib"
    "${OPENAL_DIR}/lib"
    "${CG_DIR}/lib"
    "${ASSIMP_DIR}/lib"
    "${BULLET_DIR}/lib"
    "${GLEW_DIR}/lib"
	"${FREETYPE_DIR}/lib"
	"${FREEIMAGE_DIR}/lib"
	"${FFMPEG_DIR}/lib")
ELSEIF(UNIX)
    include_directories( "/usr/include/freetype2" "./../include" "/usr/local/include/bullet")
ENDIF(WIN32)

add_library( Ovgl SHARED
"../include/OvglAudio.h"
"../include/OvglGraphics.h"
"../include/OvglInstance.h"
"../include/OvglMath.h"
"../include/OvglMedia.h"
"../include/OvglMesh.h"
"../include/OvglScene.h"
"../include/OvglWindow.h"
"../include/OvglSkeleton.h"
"OvglAudio.cpp"
"OvglGraphics.cpp"
"OvglInstance.cpp"
"OvglMath.cpp"
"OvglMedia.cpp"
"OvglMesh.cpp"
"OvglScene.cpp"
"OvglWindow.cpp"
"OvglSkeleton.cpp")

IF(WIN32)
target_link_libraries( Ovgl
optimized "SDL2"
debug "SDL2_d"
"OpenGL32"
"OpenAL32"
"Cg"
"CgGL"
"glew32"
optimized "BulletDynamics"
debug "BulletDynamics_d"
optimized "BulletCollision"
debug "BulletCollision_d"
optimized "LinearMath"
debug "LinearMath_d"
"assimp"
"freetype"
"freeimage"
"avcodec"
"avformat"
"avutil")
ELSEIF(UNIX)
target_link_libraries( Ovgl
"SDL2"
"GL"
"openal"
"Cg"
"CgGL"
"GLEW"
"BulletDynamics"
"BulletCollision"
"LinearMath"
"assimp"
"freetype"
"freeimage"
"avcodec"
"avformat"
"avutil")
ENDIF(WIN32)

add_definitions( -DNOMINMAX )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/ovgl/Doxyfile" )
endif(OVGL_BUILD_DOC)	
