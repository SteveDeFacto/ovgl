SET(OGG_FOUND FALSE)
SET(OGG_INCLUDE_DIR "NOTFOUND")
SET(OGG_LIBRARIES "NOTFOUND")

IF(WIN32)

FIND_PATH(OGG_DIR FILES "include/ogg/ogg.h" "include/vorbis/vorbisfile.h" PATHS "${CMAKE_SOURCE_DIR}/ogg")

IF(OGG_DIR)
    SET(OGG_FOUND TRUE)
	SET(OGG_LIBRARY_DIR "${OGG_DIR}\\lib")
    SET(OGG_INCLUDE_DIR "${OGG_DIR}\\include")
	FIND_LIBRARY(OGG_LIBRARY libogg_static.lib PATHS "${OGG_LIBRARY_DIR}\\Release\\")
	FIND_LIBRARY(VORBIS_LIBRARY NAMES libvorbis_static.lib PATHS "${OGG_LIBRARY_DIR}\\Release\\")
	FIND_LIBRARY(VORBISFILE_LIBRARY NAMES libvorbisfile_static.lib PATHS "${OGG_LIBRARY_DIR}\\Release\\")
    SET(OGG_LIBRARIES ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARY})
ELSE(OGG_DIR)
    SET(OGG_FOUND FALSE)
ENDIF(OGG_DIR)

ENDIF(WIN32)

IF(OGG_FOUND)
    MESSAGE(STATUS "Found Ogg Vorbis")
ELSE(OGG_FOUND)
    MESSAGE(FATAL_ERROR "Could NOT find Ogg Vorbis. You will need to set the path manually.")
ENDIF(OGG_FOUND)