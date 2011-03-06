cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

add_definitions(-DUNICODE -D_UNICODE)
set(OVGL_VERSION 0.2.5)

option(OVGL_BUILD_DOC "Build Ovgl documentation" OFF)
option(OVGL_BUILD_SHARED "Build Ovgl libraries" ON)
option(OVGL_BUILD_EXAMPLES "Build Ovgl examples" ON)

if(OVGL_BUILD_SHARED)
  add_subdirectory(ovgl)
endif(OVGL_BUILD_SHARED)

if(OVGL_BUILD_EXAMPLES)
  add_subdirectory(examples/HelloWorld)
  add_subdirectory(examples/FPS)
endif(OVGL_BUILD_EXAMPLES)

if(OVGL_INSTALL_DOC)
  add_subdirectory(docs)
endif(OVGL_INSTALL_DOC)