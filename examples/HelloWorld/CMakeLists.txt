cmake_minimum_required(VERSION 2.8.7)

project(HelloWorld)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../bin")
set(CMAKE_CFG_INTDIR "/")
set(CMAKE_DEBUG_POSTFIX "_d")

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
ELSEIF(UNIX)
	include_directories( "/usr/include/freetype2" "./../include" "/usr/local/include/bullet")
ENDIF(WIN32)

link_directories( "./../../lib" )

add_executable(HelloWorld WIN32 HelloWorld.cpp)

target_link_libraries( HelloWorld debug "Ovgl_d" optimized "Ovgl")
