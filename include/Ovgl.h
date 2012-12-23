/**
* @file Ovgl.h
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

#ifndef _Ovgl
#define _Ovgl

#define __STDC_CONSTANT_MACROS

#pragma warning(disable:4251)

// C++ Standard Library Headers
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <set>

// This file includes all the other files which you will need to build a client application
#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglGraphics.h"
#include "OvglAudio.h"
#include "OvglMesh.h"
#include "OvglMedia.h"
#include "OvglScene.h"
#include "OvglSkeleton.h"
#include "OvglWindow.h"

// Need to redirect WinMain to the main function to enable code to work the same across all platforms.
#ifdef _WIN32
#include <windows.h>
int main();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
    main();
    return 0;
}
#endif

#endif
