cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

set(OVGL_VERSION 0.8.0)

option(OVGL_BUILD_DOC "Auto build documentation evertime Ovgl is compiled?" OFF)
option(OVGL_BUILD_EDITOR "Build Ovgl examples" ON)
option(OVGL_BUILD_EXAMPLES "Build Ovgl examples" ON)

IF(WIN32)
	set(SDL_DIR "./../dependencies/SDL" CACHE PATH "Path to the SDL library.")
	set(OPENAL_DIR "./../dependencies/OpenAL" CACHE PATH "Path to the OpenAL library.")
	set(GLEW_DIR "./../dependencies/GLEW" CACHE PATH "Path to the Glew library.")
	set(CG_DIR "./../dependencies/CG" CACHE PATH "Path to the CG library.")
	set(BULLET_DIR "./../dependencies/Bullet" CACHE PATH "Path to the Bullet library.")
	set(ASSIMP_DIR "./../dependencies/Assimp" CACHE PATH "Path to the Assimp library.")
	set(FREETYPE_DIR "./../dependencies/FreeType" CACHE PATH "Path to the FreeType library.")
	set(FREEIMAGE_DIR "./../dependencies/FreeImage" CACHE PATH "Path to the FreeImage library.")
	set(FFMPEG_DIR "./../dependencies/FFMPEG" CACHE PATH "Path to the FFMPEG library.")
ENDIF(WIN32)

add_subdirectory(src)

if(OVGL_BUILD_EDITOR)
	add_subdirectory(editor)
endif(OVGL_BUILD_EDITOR)

if(OVGL_BUILD_EXAMPLES)
	add_subdirectory(examples/HelloWorld)
	add_subdirectory(examples/FPS)
endif(OVGL_BUILD_EXAMPLES)
