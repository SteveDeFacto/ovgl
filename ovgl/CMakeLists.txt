project(Ovgl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ".\\..\\lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ".\\..\\lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ".\\..\\lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ".\\..\\lib")
set(CMAKE_DEBUG_POSTFIX "d")

include_directories("${OPENAL_DIR}\\include"
"${CG_DIR}\\include"
"${FREEIMAGE_DIR}\\Dist"
"${ASSIMP_DIR}\\include"
"${BULLET_DIR}\\include\\bullet"
"${GLEW_DIR}\\include"
"${FFMPEG_DIR}\\include")

link_directories("${OPENAL_DIR}\\libs\\Win32"
"${CG_DIR}\\lib"
"${FREEIMAGE_DIR}\\Dist"
"${ASSIMP_DIR}\\lib"
"${BULLET_DIR}\\lib\\$(ConfigurationName)"
"${GLEW_DIR}\\lib"
"${FFMPEG_DIR}\\lib")

add_library( Ovgl SHARED "OvglAudio.h"
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

target_link_libraries( Ovgl "wininet.lib"
"winmm.lib"
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
debug "AssimpD.lib"
"avcodec.lib"
"avformat.lib"
"avutil.lib")

add_definitions( -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DFREEIMAGE_LIB -DGLEW_STATIC )

if(OVGL_BUILD_DOC)
	find_package( Doxygen )
	add_custom_command( TARGET Ovgl POST_BUILD COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}\\ovgl\\Doxyfile" )
endif(OVGL_BUILD_DOC)	