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
set(FREEIMAGE_DIR "./../dependencies/FreeImage" CACHE PATH "Path to the FreeImage library.")
set(BULLET_DIR "./../dependencies/Bullet" CACHE PATH "Path to the Bullet library.")
set(ASSIMP_DIR "./../dependencies/Assimp" CACHE PATH "Path to the Assimp library.")

include_directories(
"${SFML_DIR}/include"
"${OPENAL_DIR}/include"
"${CG_DIR}/include"
"${FREEIMAGE_DIR}/include"
"${ASSIMP_DIR}/include"
"${BULLET_DIR}/include"
"${BULLET_DIR}/include/bullet"
"${GLEW_DIR}/include")

link_directories(
"${SFML_DIR}/lib"
"${OPENAL_DIR}/lib"
"${CG_DIR}/lib"
"${FREEIMAGE_DIR}/lib"
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
optimized "libsfml-graphics-s.a"
debug "libsfml-graphics-s-d.a"
optimized "libsfml-window-s.a"
debug "libsfml-window-s-d.a"
optimized "libsfml-system-s.a"
debug "libsfml-system-s-d.a"
optimized "libsfml-audio-s.a"
debug "libsfml-audio-s-d.a"
optimized "libsfml-network-s.a"
debug "libsfml-network-s-d.a"
optimized "libsfml-main.a"
debug "libsfml-main-d.a"
"OpenGL32.lib"
"glew32s.lib"
"OpenAL32.lib"
"FreeImage.lib"
"cg.lib"
"cgGL.lib"
"BulletDynamics.a"
"BulletCollision.a"
"LinearMath.a"
"libassimp.a"
"libzlib.a")

add_definitions( -DNOMINMAX -DGLEW_STATIC -DSFML_STATIC  )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/ovgl/Doxyfile" )
endif(OVGL_BUILD_DOC)	

add_custom_command(
TARGET
Ovgl
POST_BUILD
COMMAND
copy "..\\dependencies\\SFML\\bin\\libsndfile-1.dll" "..\\lib\\libsndfile-1.dll";
VERBATIM
)