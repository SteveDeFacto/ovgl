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

#pragma warning(disable : 4251) // Disable std warnings

#pragma warning(disable : 4244) // Disable conversion loss of data warnings

#define _CRT_SECURE_NO_WARNINGS // Disable security warnings

#define __STDC_CONSTANT_MACROS // Enable c99 macros

// Windows Headers
#include <windows.h>
#include <wchar.h>
#include <mmsystem.h>
#include <sys/stat.h>
#include <winnt.h>

// C++ Standard Library Headers
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <set>

// OpenAl Headers
#include <al.h>
#include <alc.h>

// OpenGL Headers
#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\gl.h>

// FreeImage Headers
#include <FreeImage.h>

// Cg Headers
#include <Cg/cg.h>
#include <Cg/cgGL.h>

// Bullet Headers
#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

// Assimp Headers
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// FFMPEG Headers
extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}