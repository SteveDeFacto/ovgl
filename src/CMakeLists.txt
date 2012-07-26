cmake_minimum_required(VERSION 2.8.7)

project(Ovgl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "./../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "./../lib")
set(CMAKE_DEBUG_POSTFIX "d")

IF(WIN32)

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
ELSEIF(UNIX)
    include_directories("/usr/local/include/bullet")
ENDIF(WIN32)


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
optimized "sfml-graphics"
debug "sfml-graphics-d"
optimized "sfml-window"
debug "sfml-window-d"
optimized "sfml-system"
debug "sfml-system-d"
optimized "sfml-audio"
debug "sfml-audio-d"
optimized "sfml-network"
debug "sfml-network-d"
"GL"
"openal"
"Cg"
"CgGL"
"GLEW"
optimized "BulletDynamics"
debug "BulletDynamics_d"
optimized "BulletCollision"
debug "BulletCollision_d"
optimized "LinearMath"
debug "LinearMath_d"
optimized "assimp")

IF(WIN32)
target_link_libraries( Ovgl
optimized "sfml-main"
debug "sfml-main-d")
ENDIF(WIN32)

add_definitions( -DNOMINMAX -DGLEW_STATIC -DSFML_STATIC  )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/ovgl/Doxyfile" )
endif(OVGL_BUILD_DOC)	
