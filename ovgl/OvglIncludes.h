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
#include <math.h>
#include <algorithm>
#include <iostream>
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
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <..\\OpenGL\\GLDebugDrawer.h>

// FBX Headers
#include <fbxsdk.h>

// OGG Headers
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>