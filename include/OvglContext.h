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

typedef void *SDL_GLContext;
class SDL_Window;
extern "C"
{
void SDL_GetWindowPosition( SDL_Window * window, int *x, int *y );
void SDL_GetWindowSize( SDL_Window * window, int *x, int *y );
int SDL_GL_MakeCurrent( SDL_Window * window, SDL_GLContext context );
}
typedef struct _CGeffect *CGeffect;
typedef struct _CGparameter *CGparameter;
typedef struct _CGcontext *CGcontext;
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
typedef struct FT_LibraryRec_  *FT_Library;
typedef float btScalar;

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btManifoldPoint;
class btCollisionObject;
class btBroadphaseProxy;
class btGeneric6DofConstraint;
class btKinematicCharacterController;
class btPairCachingGhostObject;

struct	ContactResultCallback
{
    short int	m_collisionFilterGroup;
    short int	m_collisionFilterMask;
    ContactResultCallback();
    virtual ~ContactResultCallback();
    virtual bool needsCollision(btBroadphaseProxy* proxy0) const;
    virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1);
};

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
		class MediaLibrary;
		class Window;

        class DLLEXPORT Event
        {
        public:
            OVGL_EventType type;
            OVGL_WindowEvent window_event;
            unsigned char key;
            uint32_t mouse_x;
            uint32_t mouse_y;
            uint32_t button;
        };

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

        class DLLEXPORT Font
        {
        public:
            Font( Context* context,  const std::string& file, uint32_t size );
            uint32_t								charset[256];
            uint32_t								charoffsets[256];
            uint32_t                                size;
        };

        class DLLEXPORT Texture
		{
		public:
			MediaLibrary*							MLibrary;
			uint32_t								Image;
			std::string								File;
			bool									HasAlpha;
			void Release();
		};

        class DLLEXPORT Shader
		{
		public:
			MediaLibrary*							MLibrary;
            CGeffect                                effect;
			void Release();
		};

        class DLLEXPORT Material
		{
		public:
			MediaLibrary*							MLibrary;
			Shader*									ShaderProgram;
			bool									PostRender;
			bool									NoZBuffer;
			bool									NoZWrite;
			std::vector< std::pair< CGparameter, std::vector< float > > > Variables;
			std::vector< std::pair< CGparameter, Ovgl::Texture* > > Textures;
            void setEffectVariable(const std::string& variable, const std::vector< float >& data);
            void setEffectTexture(const std::string& variable, Texture* texture);
			void Release();
		};

        // Context Class
        class DLLEXPORT Context
		{
		public:
            Context( uint32_t flags );
            ~Context();
			bool									g_Quit;
            SDL_GLContext							hWnd;
			SDL_Window*								ContextWindow;
			_CGcontext*								CgContext;
			btDefaultCollisionConfiguration*		PhysicsConfiguration;
			btCollisionDispatcher*					PhysicsDispatcher;
			btBroadphaseInterface*					PhysicsBroadphase;
			btSequentialImpulseConstraintSolver*	PhysicsSolver;
			ALCdevice*								aldevice;
			ALCcontext*								alcontext;
			MediaLibrary*							DefaultMedia;
			std::vector< MediaLibrary* >			MediaLibraries;
			std::vector< Window* >					Windows;
            FT_Library                              ftlibrary;
			Window*									CreateOWindow( const std::string& name );
            void									Start();
			RenderTarget*							CreateRenderTarget( Ovgl::Window* window, Ovgl::Vector4* rect, uint32_t flags );
			MediaLibrary*							CreateMediaLibrary( const std::string& file );
		};
	}
}
