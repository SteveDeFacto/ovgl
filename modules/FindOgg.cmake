SET(OGG_FOUND FALSE)
SET(OGG_INCLUDE_DIR "NOTFOUND")
SET(OGG_LIBRARIES "NOTFOUND")

IF(WIN32)

FIND_PATH(OGG_DIR "include/ogg/ogg.h" PATHS
"${CMAKE_SOURCE_DIR}/ogg"
)

FIND_PATH(VORBIS_DIR "include/vorbis/vorbisfile.h" PATHS
"${CMAKE_SOURCE_DIR}/vorbis"
)

IF(OGG_DIR AND VORBIS_DIR)
    SET(OGG_FOUND TRUE)
	SET(OGG_LIBRARY_DIR "${OGG_DIR}\\bin")
    SET(OGG_INCLUDE_DIR "${OGG_DIR}\\include")
	SET(VORBIS_LIBRARY_DIR "${VORBIS_DIR}\\bin")
    SET(VORBIS_INCLUDE_DIR "${VORBIS_DIR}\\include")
	FIND_LIBRARY(OGG_LIBRARY libogg_static.lib PATHS "${OGG_LIBRARY_DIR}")
	FIND_LIBRARY(VORBIS_LIBRARY NAMES libvorbis_static.lib PATHS "${VORBIS_LIBRARY_DIR}")
	FIND_LIBRARY(VORBISFILE_LIBRARY NAMES libvorbisfile_static.lib PATHS "${VORBIS_LIBRARY_DIR}")
    SET(OGG_LIBRARIES ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARY})
    SET(OGG_INCLUDE_DIRS ${OGG_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR})
ELSE(OGG_DIR AND VORBIS_DIR)
    SET(OGG_FOUND FALSE)
ENDIF(OGG_DIR AND VORBIS_DIR)

ENDIF(WIN32)

IF(OGG_FOUND)
    MESSAGE(STATUS "Found Ogg Vorbis")
ELSE(OGG_FOUND)
    MESSAGE(FATAL_ERROR "Could NOT find Ogg Vorbis. You will need to set the paths manually.")
ENDIF(OGG_FOUND)