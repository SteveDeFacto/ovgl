cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

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
