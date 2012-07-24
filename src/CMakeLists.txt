cmake_minimum_required(VERSION 2.8.7)

project(Ovgl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "./../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "./../lib")
set(CMAKE_DEBUG_POSTFIX "d")

set(SFML_DIR "./../dependencies/SFML" CACHE PATH "Path to the SFML library.")
set(OPENGL_DIR "./../dependencies/OpenGL" CACHE PATH "Path to the OpenGL library.")
set(OPENAL_DIR "./../dependencies/OpenAL" CACHE PATH "Path to the OpenAL library.")
set(GLEW_DIR "./../dependencies/GLEW" CACHE PATH "Path to the Glew library.")
set(CG_DIR "./../dependencies/CG" CACHE PATH "Path to the CG library.")
set(BULLET_DIR "./../dependencies/Bullet" CACHE PATH "Path to the Bullet library.")
set(ASSIMP_DIR "./../dependencies/Assimp" CACHE PATH "Path to the Assimp library.")

include_directories(
"${SFML_DIR}/include"
"${OPENAL_DIR}/include"
"${CG_DIR}/include"
"${ASSIMP_DIR}/include"
"${BULLET_DIR}/include"
"${BULLET_DIR}/include/bullet"
"${GLEW_DIR}/include")

link_directories(
"${SFML_DIR}/lib"
"${OPENAL_DIR}/lib"
"${CG_DIR}/lib"
"${ASSIMP_DIR}/lib"
"${BULLET_DIR}/lib"
"${GLEW_DIR}/lib")

add_library( Ovgl SHARED
"OvglAudio.h"
"OvglGraphics.h"
"OvglIncludes.h"
"OvglInstance.h"
"OvglMath.h"
"OvglMedia.h"
"OvglMesh.h"
"OvglScene.h"
"OvglWindow.h"
"OvglSkeleton.h"
"OvglAudio.cpp"
"OvglGraphics.cpp"
"OvglInstance.cpp"
"OvglMath.cpp"
"OvglMedia.cpp"
"OvglMesh.cpp"
"OvglScene.cpp"
"OvglWindow.cpp"
"OvglSkeleton.cpp")

target_link_libraries( Ovgl
optimized "sfml-graphics-s"
debug "sfml-graphics-s-d"
optimized "sfml-window-s"
debug "sfml-window-s-d"
optimized "sfml-system-s"
debug "sfml-system-s-d"
optimized "sfml-audio-s"
debug "sfml-audio-s-d"
optimized "sfml-network-s"
debug "sfml-network-s-d"
optimized "sfml-main"
debug "sfml-main-d"
"OpenGL32.lib"
"OpenAL32.lib"
"cg.lib"
"cgGL.lib"
optimized "BulletDynamics"
debug "BulletDynamics_d"
optimized "BulletCollision"
debug "BulletCollision_d"
optimized "LinearMath"
debug "LinearMath_d"
optimized "assimp"
debug "assimp_d"
"zlib")

add_definitions( -DNOMINMAX -DGLEW_STATIC -DSFML_STATIC  )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/ovgl/Doxyfile" )
endif(OVGL_BUILD_DOC)	