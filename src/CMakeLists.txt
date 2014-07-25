cmake_minimum_required(VERSION 2.8.7)

project(Ovgl)

set(LIBRARY_OUTPUT_PATH "./../bin")

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
	"${FREETYPE_DIR}/include/freetype2"
	"${FREEIMAGE_DIR}/include"
	"${FFMPEG_DIR}/include"
	"./../include")

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
	include_directories( "/usr/include/freetype2" "./../include" "/usr/include/bullet")
ENDIF(WIN32)

add_library( Ovgl SHARED
"../include/OvglAudio.h"
"../include/OvglGraphics.h"
"../include/OvglContext.h"
"../include/OvglMath.h"
"../include/OvglResource.h"
"../include/OvglMesh.h"
"../include/OvglScene.h"
"../include/OvglWindow.h"
"../include/OvglSkeleton.h"
"OvglAudio.cpp"
"OvglGraphics.cpp"
"OvglContext.cpp"
"OvglMath.cpp"
"OvglResource.cpp"
"OvglMesh.cpp"
"OvglScene.cpp"
"OvglWindow.cpp"
"OvglSkeleton.cpp")

add_library( Ovgl_Static STATIC
"../include/OvglAudio.h"
"../include/OvglGraphics.h"
"../include/OvglContext.h"
"../include/OvglMath.h"
"../include/OvglResource.h"
"../include/OvglMesh.h"
"../include/OvglScene.h"
"../include/OvglWindow.h"
"../include/OvglSkeleton.h"
"OvglAudio.cpp"
"OvglGraphics.cpp"
"OvglContext.cpp"
"OvglMath.cpp"
"OvglResource.cpp"
"OvglMesh.cpp"
"OvglScene.cpp"
"OvglWindow.cpp"
"OvglSkeleton.cpp")

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")

set_target_properties(Ovgl Ovgl_Static PROPERTIES INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")

IF(WIN32)
	target_link_libraries( Ovgl Ovgl_Static
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
	target_link_libraries( Ovgl Ovgl_Static
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

IF(UNIX)
INSTALL(FILES ./../include/Ovgl.h DESTINATION ${INCLUDE_INSTALL_DIR})
INSTALL(FILES ./../bin/libOvgl_Static.a DESTINATION ${LIB_DESTINATION})
INSTALL(FILES ./../bin/libOvgl.so DESTINATION ${LIB_DESTINATION})
ENDIF(UNIX)

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl Ovgl_Static POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/ovgl/Doxyfile" )
endif(OVGL_BUILD_DOC)	
