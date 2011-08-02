SET(OPENAL_FOUND FALSE)
SET(OPENAL_INCLUDE_DIR "NOTFOUND")
SET(OPENAL_LIBRARIES "NOTFOUND")

IF(WIN32)

FIND_PATH(OPENAL_SDK_DIR include\\al.h PATHS
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\3.05;InstallDir]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\3.05;InstallDir]
)

IF(OPENAL_SDK_DIR)
    SET(OPENAL_FOUND TRUE)
	SET(OPENAL_LIBRARY_DIR "${OPENAL_SDK_DIR}\\libs\\Win32")
    SET(OPENAL_INCLUDE_DIR "${OPENAL_SDK_DIR}\\include")
	
	FIND_PATH(OPENAL_LIBRARY NAMES OpenAL32.lib PATHS "${OPENAL_SDK_DIR}\\libs\\Win32")
    SET(OPENAL_LIBRARIES ${OPENAL_LIBRARY})
ELSE(OPENAL_SDK_DIR)
    SET(OPENAL_FOUND FALSE)
ENDIF(OPENAL_SDK_DIR)

ENDIF(WIN32)

IF(OPENAL_FOUND)
    MESSAGE(STATUS "Found OpenAL")
ELSE(OPENAL_FOUND)
    MESSAGE(FATAL_ERROR "Could NOT find OpenAL.")
ENDIF(OPENAL_FOUND)