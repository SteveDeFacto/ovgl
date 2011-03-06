/**
* @file OvglIncludes.h
* Copyright 2011 Steven Batchelor
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* @brief None.
*/

#pragma once

// Windows Headers
#include <windows.h>
#include <wchar.h>
#include <mmsystem.h>
#include <comdef.h>
#include <sys/stat.h>
#include <winnt.h>
#include <Dbghelp.h>

// C++ Standard Library Headers
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

// XAudio2 Headers
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>

// OGG Headers
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

// DirectX Headers
#include <d3d10.h>
#include <d3dx10.h>
#include <DXGI.h>

// PhysX Headers
#include <NxPhysics.h>
#include <NxCooking.h>
#include <NxStreamDefault.h>
#include <NxUserAllocatorDefault.h>
#include <NxControllerManager.h>
#include <NxCapsuleController.h>

// FBX Headers
#include <fbxsdk.h>

// Recast Headers
#include <Recast.h>
#include <RecastAlloc.h>
#include <RecastAssert.h>

// Detour Headers
#include <DetourAlloc.h>
#include <DetourAssert.h>
#include <DetourCommon.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <DetourNode.h>
