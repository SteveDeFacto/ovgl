/**
* @file OvglContext.h
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
#include <string.h>
#include <set>

// Predefine some third party classes and structures which we will need later.

extern "C"
{
#ifndef SDLCALL
#if defined(__WIN32__) && !defined(__GNUC__)
#define SDLCALL __cdecl
#else
#define SDLCALL
#endif
#endif

#ifndef DECLSPEC
# if defined(__BEOS__) || defined(__HAIKU__)
#  if defined(__GNUC__)
#   define DECLSPEC	__declspec(dllexport)
#  else
#   define DECLSPEC	__declspec(export)
#  endif
# elif defined(__WIN32__)
#  ifdef __BORLANDC__
#   ifdef BUILD_SDL
#    define DECLSPEC
#   else
#    define DECLSPEC	__declspec(dllimport)
#   endif
#  else
#   define DECLSPEC	__declspec(dllexport)
#  endif
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define DECLSPEC	__attribute__ ((visibility("default")))
#  else
#   define DECLSPEC
#  endif
# endif
#endif

typedef void *SDL_GLContext;
struct SDL_Window;
extern DECLSPEC void SDLCALL SDL_GetWindowPosition( SDL_Window * window, int *x, int *y );
extern DECLSPEC void SDLCALL SDL_GetWindowSize( SDL_Window * window, int *w, int *h );
extern DECLSPEC int SDLCALL SDL_GL_MakeCurrent(SDL_Window * window, SDL_GLContext context);
}

typedef struct _CGeffect *CGeffect;
typedef struct _CGparameter *CGparameter;
typedef struct _CGcontext *CGcontext;
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
typedef struct FT_LibraryRec_  *FT_Library;
typedef float btScalar;
//struct	ContactResultCallback;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btManifoldPoint;
//class btCollisionObject;
struct btBroadphaseProxy;
class btGeneric6DofConstraint;
class btKinematicCharacterController;
class btPairCachingGhostObject;

namespace Ovgl
{
    typedef enum
    {
        OVGL_KEYDOWN,
        OVGL_KEYUP,
        OVGL_MOUSEMOTION,
        OVGL_MOUSEBUTTONDOWN,
        OVGL_MOUSEBUTTONUP,
        OVGL_MOUSEWHEEL,
        OVGL_WINDOWEVENT
    } OVGL_EventType;

    typedef enum
    {
        OVGL_WINDOWEVENT_NONE,
        OVGL_WINDOWEVENT_SHOWN,
        OVGL_WINDOWEVENT_HIDDEN,
        OVGL_WINDOWEVENT_EXPOSED,
        OVGL_WINDOWEVENT_MOVED,
        OVGL_WINDOWEVENT_RESIZED,
        OVGL_WINDOWEVENT_SIZE_CHANGED,
        OVGL_WINDOWEVENT_MINIMIZED,
        OVGL_WINDOWEVENT_MAXIMIZED,
        OVGL_WINDOWEVENT_RESTORED,
        OVGL_WINDOWEVENT_ENTER,
        OVGL_WINDOWEVENT_LEAVE,
        OVGL_WINDOWEVENT_FOCUS_GAINED,
        OVGL_WINDOWEVENT_FOCUS_LOST,
        OVGL_WINDOWEVENT_CLOSE
    } OVGL_WindowEvent;

	extern "C"
	{

        // Predefine some OVGL classes.
		class Matrix22;
		class Matrix33;
		class Matrix34;
		class Matrix43;
		class Matrix44;
		class Vector2;
		class Vector3;
		class Vector4;
		class Vertex;
		class Face;
		class Bone;
		class Joint;
		class Animation;
		class Key;
		class CMesh;
		class Mesh;
		class AudioBuffer;
		class AudioInstance;
		class AudioVoice;
		class Actor;
		class Camera;
		class Light;
		class Prop;
		class Scene;
		class Mesh;
		class Emitter;
        class Context;
		class Interface;
        class RenderTarget;
		class Effect;
        class Event;
		class Resource;
		class Window;

        /**
        * This class is used to store and pass event information from the windows to the hierarchical GUI elements.
        * Primarily for user input such a keyboard events and mouse events.
        * @brief This class represents events.
        */
        class DLLEXPORT Event
        {
        public:
            OVGL_EventType type;
            OVGL_WindowEvent window_event;
            unsigned char key;
            int32_t mouse_x;
            int32_t mouse_y;
            int32_t button;
        };

        /**
        * A UDim(unified dimension) represents both a floating point scale and a per pixel offset.
        * This allows for dynamic resizing of GUI elements.
        * @brief This class represents both scale and offset.
        */
        class DLLEXPORT UDim
        {
        public:
            UDim();
            UDim( int offset);
            UDim( float scale );
            UDim( int32_t offset, float scale );
            int32_t                                 offset;
            float                                   scale;
        };

        /**
        * The URect class contains four UDims. This allows for dynamic resizing of a rectangle which
        * can represent the position and size of a two dimensional GUI element.
        * @brief Two dimensional dynamically resizing rectangle.
        */
        class DLLEXPORT URect
        {
        public:
            URect();
            URect( UDim left, UDim top, UDim right, UDim bottom );
            UDim                    				left;
            UDim                    				top;
            UDim                    				right;
            UDim                    				bottom;
        };

        /**
        * The Rect class contains four integers which represent an absolutely positioned two dimensional
        * rectagle. It can be used to store the position and size of a GUI element.
        * @brief Two dimensional absolutely positioned rectangle.
        */
        class DLLEXPORT Rect
		{
		public:
			Rect();
            Rect( int32_t left, int32_t top, int32_t right, int32_t bottom );
            int32_t                                 left;
            int32_t                                 top;
            int32_t                                 right;
            int32_t                                 bottom;
		};

        /**
        * The context is the base class from which all other classes originate.
        * This class is basically responsible for managing and storing all sub classes and information.
        * @brief Base class which manages all sub classes.
        */
        class DLLEXPORT Context
		{
		public:
            Context( uint32_t flags );
            ~Context();
			bool									g_Quit;
            SDL_GLContext							hWnd;
			SDL_Window*								context_window;
			_CGcontext*								CgContext;
			btDefaultCollisionConfiguration*		PhysicsConfiguration;
			btCollisionDispatcher*					PhysicsDispatcher;
			btBroadphaseInterface*					PhysicsBroadphase;
			btSequentialImpulseConstraintSolver*	PhysicsSolver;
			ALCdevice*								aldevice;
			ALCcontext*								alcontext;
			Resource*								default_media;
			std::vector< Resource* >				media_libraries;
			std::vector< Window* >					windows;
            FT_Library                              ftlibrary;
            void									start();
		};
	}
}
