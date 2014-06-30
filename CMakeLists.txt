cmake_minimum_required (VERSION 2.8.7)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

set (OVGL_VERSION 0.8.0)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

SET (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )

SET (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name")

SET (BIN_DESTINATION "${CMAKE_INSTALL_PREFIX}/share/ovgl/bin" CACHE STRING "Directory to install examples and editor")

SET (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/ovgl/" CACHE PATH "The subdirectory to the header prefix")

SET (PKGCONFIG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig/" CACHE STRING "Base directory for pkgconfig files")

option (OVGL_BUILD_DOC "Auto build documentation evertime Ovgl is compiled?" OFF)
option (OVGL_BUILD_EDITOR "Build Ovgl examples" ON)
option (OVGL_BUILD_EXAMPLES "Build Ovgl examples" ON)

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
	set(ZLIB_DIR "./../dependencies/ZLib" CACHE PATH "Path to the ZLIB library.")
	string( REGEX REPLACE "/" "\\\\" MS_SDL_DIR ${SDL_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_OPENAL_DIR ${OPENAL_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_GLEW_DIR ${GLEW_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_CG_DIR ${CG_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_BULLET_DIR ${BULLET_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_ASSIMP_DIR ${ASSIMP_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_FREETYPE_DIR ${FREETYPE_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_FREEIMAGE_DIR ${FREEIMAGE_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_FFMPEG_DIR ${FFMPEG_DIR} )
	string( REGEX REPLACE "/" "\\\\" MS_ZLIB_DIR ${ZLIB_DIR} )
ELSE(WIN32)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/ovgl.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/ovgl.pc @ONLY)
	INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/ovgl.pc DESTINATION ${PKGCONFIG_INSTALL_PREFIX})
ENDIF(WIN32)

add_subdirectory(src)

if(OVGL_BUILD_EDITOR)
	add_subdirectory(editor)
endif(OVGL_BUILD_EDITOR)

if(OVGL_BUILD_EXAMPLES)
	add_subdirectory(examples/HelloWorld)
	add_subdirectory(examples/FPS)
endif(OVGL_BUILD_EXAMPLES)

if(UNIX AND OVGL_BUILD_EDITOR OR OVGL_BUILD_EXAMPLES)
	install(DIRECTORY media DESTINATION ${CMAKE_INSTALL_PREFIX}/share/ovgl)
endif(UNIX AND OVGL_BUILD_EDITOR OR OVGL_BUILD_EXAMPLES)

set(CPACK_PACKAGE_NAME "Ovgl")
set(CPACK_PACKAGE_VENDOR "Ovgl.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OVGL offers a light weight cross platform framework for 3D video game development.")
set(CPACK_PACKAGE_VERSION ${OVGL_VERSION} )
set(CPACK_SET_DESTDIR "ON")
include(CPack)
