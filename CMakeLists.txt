cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}\\modules")

find_package( OpenGL )
find_package( OpenAL )
find_package( CG )
find_package( FreeImage )
find_package( PhysX )
find_package( FBXSDK )
find_package( Ogg )

add_definitions(-DUNICODE -D_UNICODE)
set(OVGL_VERSION 0.2.5)

option(OVGL_BUILD_DOC "Auto build documentation evertime Ovgl is compiled?" OFF)
option(OVGL_BUILD_EDITOR "Build Ovgl examples" ON)
option(OVGL_BUILD_EXAMPLES "Build Ovgl examples" ON)

add_subdirectory(ovgl)

if(OVGL_BUILD_EDITOR)
	add_subdirectory(editor)
endif(OVGL_BUILD_EDITOR)

if(OVGL_BUILD_EXAMPLES)
	add_subdirectory(examples\\HelloWorld)
	add_subdirectory(examples\\FPS)
endif(OVGL_BUILD_EXAMPLES)