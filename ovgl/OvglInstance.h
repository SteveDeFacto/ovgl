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

// Forward declare external classes
struct _CGprogram;
struct _CGcontext;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btConvexHullShape;

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
		class Frame;
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

		class __declspec(dllexport) Texture
		{
		public:
			MediaLibrary*							MLibrary;
			UINT									Image;

			void Release();
		};

		class __declspec(dllexport) Shader
		{
		public:
			MediaLibrary*							MLibrary;
			_CGprogram*								VertexProgram;
			_CGprogram*								FragmentProgram;
			_CGprogram*								GeometryProgram;
			void Release();
		};

		class __declspec(dllexport) Material
		{
		public:
			MediaLibrary*							MLibrary;
			Shader*									ShaderProgram;
			bool									PostRender;
			bool									NoZBuffer;
			bool									NoZWrite;
			std::vector< std::pair<std::string, std::vector<float>> > Variables;
			std::vector< std::pair<std::string, Texture*> > Textures;
			void set_variable(const std::string& variable, const std::vector<float>& data);
			void set_texture(const std::string& variable, Texture* texture);
			void Release();
		};

		// Instance Class
		class __declspec(dllexport) Instance
		{
		public:
			HWND									hWnd;
			HDC										hDC;
			HGLRC									hRC;
			_CGcontext*								CgContext;
			btDefaultCollisionConfiguration*		PhysicsConfiguration;
			btCollisionDispatcher*					PhysicsDispatcher;
			btBroadphaseInterface*					PhysicsBroadphase;
			btSequentialImpulseConstraintSolver*	PhysicsSolver;
			btConvexHullShape*						Shapes[1];
			void*									FBXManager;
			MediaLibrary*							DefaultMedia;
			std::vector<RenderTarget*>				RenderTargets;
			std::vector<MediaLibrary*>				MediaLibraries;

			RenderTarget*							CreateRenderTarget( HWND window, RECT* rect, DWORD flags );
			MediaLibrary*							CreateMediaLibrary( const std::string& file );
			void									Release();
		};
	
		__declspec(dllexport) Instance* Create( DWORD flags );
	}
}