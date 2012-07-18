project(Ovgl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "./../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "./../lib")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories(
"${SFML_DIR}/include"
"${OPENAL_DIR}/include"
"${CG_DIR}/include"
"${FREEIMAGE_DIR}/include"
"${ASSIMP_DIR}/include"
"${BULLET_DIR}/include"
"${BULLET_DIR}/include/bullet"
"${GLEW_DIR}/include"
"${FFMPEG_DIR}/include")

link_directories(
"${SFML_DIR}/lib"
"${OPENAL_DIR}/lib"
"${CG_DIR}/lib"
"${FREEIMAGE_DIR}/lib"
"${ASSIMP_DIR}/lib"
"${BULLET_DIR}/lib/$(ConfigurationName)"
"${GLEW_DIR}/lib"
"${FFMPEG_DIR}/lib")

add_library( Ovgl SHARED
"OvglAudio.h"
"OvglGraphics.h"
"OvglIncludes.h"
"OvglInstance.h"
"OvglMath.h"
"OvglMedia.h"
"OvglMesh.h"
"OvglScene.h"
"OvglWindow.h"
"OvglSkeleton.h"
"OvglAudio.cpp"
"OvglGraphics.cpp"
"OvglInstance.cpp"
"OvglMath.cpp"
"OvglMedia.cpp"
"OvglMesh.cpp"
"OvglScene.cpp"
"OvglWindow.cpp"
"OvglSkeleton.cpp")

target_link_libraries( Ovgl
optimized "sfml-audio-s.lib"
debug "sfml-audio-s-d.lib"
optimized "sfml-graphics-s.lib"
debug "sfml-graphics-s-d.lib"
optimized "sfml-network-s.lib"
debug "sfml-network-s-d.lib"
optimized "sfml-system-s.lib"
debug "sfml-system-s-d.lib"
optimized "sfml-window-s.lib"
debug "sfml-window-s-d.lib"
optimized "sfml-main.lib"
debug "sfml-main-d.lib"
"opengl32.lib"
"glew32s.lib"
"OpenAL32.lib"
optimized "FreeImage.lib"
debug "FreeImaged.lib"
"cg.lib"
"cgGL.lib"
"OpenGLSupport.lib"
"BulletDynamics.lib"
"BulletCollision.lib"
"LinearMath.lib"
optimized "Assimp.lib"
debug "AssimpD.lib")

add_definitions( -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DFREEIMAGE_LIB -DGLEW_STATIC -DSFML_STATIC )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/ovgl/Doxyfile" )
endif(OVGL_BUILD_DOC)	