cmake_minimum_required(VERSION 2.8.7)

project(FPS)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../bin")
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
ELSEIF(UNIX)
        include_directories( "/usr/include/freetype2" "./../../include" "/usr/include/bullet")
ENDIF(WIN32)

link_directories( "./../../lib" )

add_executable(FPS WIN32 FPS.cpp)

target_link_libraries( FPS "Ovgl")
