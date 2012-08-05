/**
* @file OvglInstance.h
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

// GLEW Headers
#include <GL/glew.h>

// SFML Headers
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

// OpenAl Headers
#include <AL/al.h>
#include <AL/alc.h>

// Cg Headers
#include <Cg/cg.h>
#include <Cg/cgGL.h>

// Bullet Headers
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

// Assimp Headers
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Ovgl
{
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
		class Instance;
		class Interface;
		class RenderTarget;
		class Effect;
		class MediaLibrary;
		class Window;

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
            Rect( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );
            uint32_t								left;
            uint32_t								top;
            uint32_t								right;
            uint32_t								bottom;
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

		// Instance Class
        class DLLEXPORT Instance
		{
		public:
			Instance( uint32_t flags );
			~Instance();
			bool									g_Quit;
			sf::Context*							hWnd;
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
			Window*									CreateOWindow( const std::string& name );
			void									Start();
			RenderTarget*							CreateRenderTarget( Ovgl::Window* window, Ovgl::Vector4* rect, uint32_t flags );
			MediaLibrary*							CreateMediaLibrary( const std::string& file );
		};
	
        DLLEXPORT Instance* Create( uint32_t flags );
	}
}
