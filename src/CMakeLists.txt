cmake_minimum_required(VERSION 2.8.7)

project(Ovgl)

set(LIBRARY_OUTPUT_PATH "./../lib")
set(CMAKE_DEBUG_POSTFIX "_d")

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
    include_directories( "./../include" "/usr/local/include/bullet")
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
optimized "sfml-main"
debug "sfml-main-d"
"sfgui"
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
ELSEIF(UNIX)
target_link_libraries( Ovgl
"sfml-graphics"
"sfml-window"
"sfml-system"
"sfml-audio"
"sfml-network"
"sfgui"
"GL"
"openal"
"Cg"
"CgGL"
"GLEW"
"BulletDynamics"
"BulletCollision"
"LinearMath"
"assimp"
"CEGUIBase"
"CEGUIOpenGLRenderer")
ENDIF(WIN32)

add_definitions( -DNOMINMAX )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/ovgl/Doxyfile" )
endif(OVGL_BUILD_DOC)	
