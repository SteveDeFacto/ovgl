IF( WIN32 )
        FIND_PATH(DX3D10_INCLUDE_PATH d3d10.h
                PATHS
                        "$ENV{DXSDK_DIR}/Include"
                DOC "The directory where D3D10.h resides")

        FIND_PATH(DX3DX10_INCLUDE_PATH d3dx10.h
                PATHS
                        "$ENV{DXSDK_DIR}/Include"
                DOC "The directory where D3Dx10.h resides")

        FIND_PATH(DXGI_INCLUDE_PATH DXGI.h
                PATHS
                        "$ENV{DXSDK_DIR}/Include"
                DOC "The directory where DXGI.h resides")

        FIND_LIBRARY(D3D10_LIBRARY d3d10.lib
                PATHS
                        "$ENV{DXSDK_DIR}/Lib/x86"
                DOC "The directory where d3d10.lib resides")

        FIND_LIBRARY(D3DX10_LIBRARY d3dx10.lib
                PATHS
                        "$ENV{DXSDK_DIR}/Lib/x86"
                DOC "The directory where d3dx10.lib resides")

        FIND_LIBRARY(D3DX10D_LIBRARY d3dx10d.lib
                PATHS
                        "$ENV{DXSDK_DIR}/Lib/x86"
                DOC "The directory where d3dx10d.lib resides")

        FIND_LIBRARY(DXGI_LIBRARY DXGI.lib
                PATHS
                        "$ENV{DXSDK_DIR}/Lib/x86"
                DOC "The directory where DXGI.lib resides")

        SET( DX10_LIBRARIES ${D3DX10_LIBRARY} ${D3DX10D_LIBRARY} ${DXGI_LIBRARY} )
ENDIF( WIN32 )