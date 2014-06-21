/**
* @file OvglCommon.h
* Copyright 2014 Steven Batchelor
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

#define SDL_MAIN_HANDLED // Disable SDL main

#ifdef _WIN32
#  define DLLEXPORT __declspec( dllexport )
#else
#  define DLLEXPORT
#endif

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


