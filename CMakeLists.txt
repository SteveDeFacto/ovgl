cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

set(SFML_DIR "${CMAKE_SOURCE_DIR}/dependences/SFML" CACHE PATH "Path to the SFML library.")
set(OPENGL_DIR "${CMAKE_SOURCE_DIR}/dependences/OpenGL" CACHE PATH "Path to the OpenGL library.")
set(OPENAL_DIR "${CMAKE_SOURCE_DIR}/dependences/OpenAL" CACHE PATH "Path to the OpenAL library.")
set(GLEW_DIR "${CMAKE_SOURCE_DIR}/dependences/GLEW" CACHE PATH "Path to the Glew library.")
set(CG_DIR "${CMAKE_SOURCE_DIR}/dependences/CG" CACHE PATH "Path to the CG library.")
set(FREEIMAGE_DIR "${CMAKE_SOURCE_DIR}/dependences/FreeImage" CACHE PATH "Path to the FreeImage library.")
set(BULLET_DIR "${CMAKE_SOURCE_DIR}/dependences/Bullet" CACHE PATH "Path to the Bullet library.")
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/dependences/Assimp" CACHE PATH "Path to the Assimp library.")
set(FFMPEG_DIR "${CMAKE_SOURCE_DIR}/dependences/FFMPEG" CACHE PATH "Path to the FFMPEG library.")

add_definitions(-DUNICODE -D_UNICODE)
set(OVGL_VERSION 0.8.0)

option(OVGL_BUILD_DOC "Auto build documentation evertime Ovgl is compiled?" OFF)
option(OVGL_BUILD_EDITOR "Build Ovgl examples" ON)
option(OVGL_BUILD_EXAMPLES "Build Ovgl examples" ON)

add_subdirectory(ovgl)

if(OVGL_BUILD_EDITOR)
	add_subdirectory(editor)
endif(OVGL_BUILD_EDITOR)

if(OVGL_BUILD_EXAMPLES)
	add_subdirectory(examples/HelloWorld)
	add_subdirectory(examples/FPS)
endif(OVGL_BUILD_EXAMPLES)
