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

#ifdef _WIN32
#  define DLLEXPORT __declspec( dllexport )
#else
#  define DLLEXPORT
#endif

#include <Cg/cg.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

// Forward declare external classes
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btConvexHullShape;
struct ALCdevice_struct;
struct ALCcontext_struct;
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

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
			_CGprogram*								VertexProgram;
			_CGprogram*								FragmentProgram;
			_CGprogram*								GeometryProgram;
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
			void setVSVariable(const std::string& variable, const std::vector< float >& data);
			void setFSVariable(const std::string& variable, const std::vector< float >& data);
			void setVSTexture(const std::string& variable, Texture* texture);
			void setFSTexture(const std::string& variable, Texture* texture);
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
			CGprofile								CgVertexProfile;
			CGprofile								CgFragmentProfile;
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