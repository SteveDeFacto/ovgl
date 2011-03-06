IF( WIN32 )

        FIND_PATH(PHYSX_INCLUDE_PATH NxPhysics.h
                PATHS
                        "$ENV{PROGRAMFILES}/NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.3/SDKs/Physics/include"
                DOC "The directory where NxPhysics.h resides")

        FIND_PATH(PHYSXLOADER_INCLUDE_PATH PhysXLoader.h
                PATHS
                        "$ENV{PROGRAMFILES}/NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.3/SDKs/PhysXLoader/include"
                DOC "The directory where PhysXLoader.h resides")

        FIND_PATH(PHYSXCHARACTER_INCLUDE_PATH NxControllerManager.h
                PATHS
                        "$ENV{PROGRAMFILES}/NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.3/SDKs/NxCharacter/include"
                DOC "The directory where NxControllerManager.h resides")

        FIND_PATH(PHYSXCOOKING_INCLUDE_PATH NxCooking.h
                PATHS
                        "$ENV{PROGRAMFILES}/NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.3/SDKs/Cooking/include"
                DOC "The directory where NxCooking.h resides")
				
        FIND_PATH(PHYSXFOUNDATION_INCLUDE_PATH NxStreamDefault.h
                PATHS
                        "$ENV{PROGRAMFILES}/NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.3/SDKs/Foundation/include"
                DOC "The directory where NxStreamDefault.h resides")

        FIND_PATH(PHYSXLOADER_LIBRARY PhysXLoader.lib
                PATHS
                        "$ENV{PROGRAMFILES}/NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.3/SDKs/lib/Win32"
                DOC "The directory where d3d10.lib resides")

        FIND_PATH(PHYSXCOOKING_LIBRARY PhysXCooking.lib
                PATHS
                        "$ENV{PROGRAMFILES}/NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.3/SDKs/lib/Win32"
                DOC "The directory where PhysXCooking.lib resides")

        FIND_PATH(PHYSXCHARACTER_LIBRARY NxCharacter.lib
                PATHS
                        "$ENV{PROGRAMFILES}/NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.3/SDKs/lib/Win32"
                DOC "The directory where NxCharacter.lib resides")

        SET( DX10_LIBRARIES ${PHYSXLOADER_LIBRARY} ${PHYSXCOOKING_LIBRARY} ${PHYSXCHARACTER_LIBRARY} )
ENDIF( WIN32 )