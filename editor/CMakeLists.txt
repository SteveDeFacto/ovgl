cmake_minimum_required(VERSION 2.8.7)

project(editor)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../bin")

include_directories( "./../include" )

link_directories( "./../lib" )

add_executable( editor WIN32 editor.cpp)

target_link_libraries( editor Ovgl)

IF(WIN32)
	add_custom_command(
		TARGET
		editor
		POST_BUILD
		COMMAND
		copy ${MS_SDL_DIR}\\bin\\SDL.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\SDL.dll;
		COMMAND
		copy ${MS_OPENAL_DIR}\\bin\\OpenAL32.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\OpenAL32.dll;
		COMMAND
		copy ${MS_CG_DIR}\\bin\\cg.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\cg.dll;
		COMMAND
		copy ${MS_CG_DIR}\\bin\\cgGL.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\cgGL.dll;
		COMMAND
		copy ${MS_ASSIMP_DIR}\\bin\\Assimp32.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\Assimp32.dll;
		COMMAND
		copy ${MS_GLEW_DIR}\\bin\\glew32.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\glew32.dll;
		COMMAND
		copy ${MS_FREETYPE_DIR}\\bin\\freetype6.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\freetype6.dll;
		COMMAND
		copy ${MS_FREEIMAGE_DIR}\\bin\\FreeImage.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\FreeImage.dll;
		COMMAND
		copy ${MS_FFMPEG_DIR}\\bin\\avcodec-54.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\avcodec-54.dll;
		COMMAND
		copy ${MS_FFMPEG_DIR}\\bin\\avdevice-54.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\avdevice-54.dll;
		COMMAND
		copy ${MS_FFMPEG_DIR}\\bin\\avfilter-3.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\avfilter-3.dll;
		COMMAND
		copy ${MS_FFMPEG_DIR}\\bin\\avformat-54.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\avformat-54.dll;
		COMMAND
		copy ${MS_FFMPEG_DIR}\\bin\\avutil-51.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\avutil-51.dll;
		COMMAND
		copy  ${MS_ZLIB_DIR}\\bin\\zlib1.dll .\\..\\bin\\${CMAKE_CFG_INTDIR}\\zlib1.dll;
		VERBATIM
	)
ENDIF(WIN32)
