SET(ASSIMP_FOUND FALSE)
SET(ASSIMP_INCLUDE_DIR "NOTFOUND")
SET(ASSIMP_LIBRARIES "NOTFOUND")

IF( WIN32 )
	FIND_PATH(ASSIMP_DIR FILES "include/assimp.h" PATHS "${CMAKE_SOURCE_DIR}/Assimp")

	IF(ASSIMP_DIR)
		SET(ASSIMP_FOUND TRUE)	
		IF( MSVC10 )
			SET(ASSIMP_LIBRARY_DIR "${ASSIMP_DIR}\\lib\\assimp_release-dll_win32")
		ENDIF( MSVC10 )
	
		SET(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY_DIR}  )
		SET(ASSIMP_INCLUDE_DIR "${ASSIMP_DIR}\\include")
	ENDIF( ASSIMP_DIR )
	
ENDIF( WIN32 )

IF(ASSIMP_FOUND)
    MESSAGE(STATUS "Found Assimp")
ELSE(ASSIMP_FOUND)
    MESSAGE(FATAL_ERROR "Could NOT find Assimp.")
ENDIF(ASSIMP_FOUND)