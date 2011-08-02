SET(CG_FOUND FALSE)
SET(CG_INCLUDE_DIR "NOTFOUND")
SET(CG_LIBRARIES "NOTFOUND")

IF(WIN32)

FIND_PATH(CG_SDK_DIR include\\Cg\\cg.h PATHS
"$ENV{PROGRAMFILES}\\NVIDIA Corporation\\Cg"
"$ENV{PROGRAMW6432}\\NVIDIA Corporation\\Cg"
)

IF(CG_SDK_DIR)
    SET(CG_FOUND TRUE)
    SET(CG_BINARY_DIR "${CG_SDK_DIR}\\Bin")
	SET(CG_LIBRARY_DIR "${CG_SDK_DIR}\\lib")
    SET(CG_INCLUDE_DIR "${CG_SDK_DIR}\\include")
	FIND_PATH(CG_LIBRARY NAMES cg.lib PATHS "${CG_SDK_DIR}\\lib")
	FIND_PATH(CG_GL_LIBRARY NAMES cgGL.lib PATHS "${CG_SDK_DIR}\\lib")
    SET(CG_LIBRARIES ${CG_LIBRARY} ${CG_GL_LIBRARY})
ELSE(CG_SDK_DIR)
    SET(CG_FOUND FALSE)
ENDIF(CG_SDK_DIR)

ENDIF(WIN32)

IF(CG_FOUND)
    MESSAGE(STATUS "Found nVidia CG")
ELSE(CG_FOUND)
    MESSAGE(FATAL_ERROR "Could NOT find nVidia CG.")
ENDIF(CG_FOUND)