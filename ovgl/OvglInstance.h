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
struct ID3D10InputLayout;
struct ID3D10Effect;
struct ID3D10EffectTechnique;
struct ID3D10EffectScalarVariable;
struct ID3D10EffectVectorVariable;
struct ID3D10EffectShaderResourceVariable;
struct ID3D10EffectMatrixVariable;
struct ID3D10Device;
struct ID3D10RenderTargetView;
struct ID3D10RasterizerState;
struct ID3D10RasterizerState;
struct ID3D10ShaderResourceView;
struct ID3DX10Sprite;
struct ID3D10Buffer;
struct ID3D10Buffer;
struct IDXGIFactory;
struct IXAudio2;
typedef BYTE X3DAUDIO_HANDLE[20];
struct IXAudio2MasteringVoice;
struct XAUDIO2_DEVICE_DETAILS;
struct X3DAUDIO_DSP_SETTINGS;
class NxPhysicsSDK;
class NxCookingInterface;
class NxUserAllocatorDefault;
class NxControllerManager;
class NxConvexMesh;

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

		class __declspec(dllexport) Effect
		{
		public:
			Instance*							Inst;
			ID3D10InputLayout*					Layout;
			ID3D10Effect*						SFX;
			ID3D10EffectTechnique*				Technique;
			ID3D10EffectMatrixVariable*			Bones;
			ID3D10EffectMatrixVariable*			View;
			ID3D10EffectMatrixVariable*			Projection;
			ID3D10EffectScalarVariable*			Light_Count;
			ID3D10EffectVectorVariable*			Lights;
			ID3D10EffectVectorVariable*			Light_Colors;
			ID3D10EffectMatrixVariable*			Cube_Views;
			ID3D10EffectShaderResourceVariable*	Shadow_Maps;

			void set_variable(const std::string& variable, UINT count, float data[] );
			void set_texture(const std::string& variable, const std::string& file);
			void Release();
		};

		// Instance Class
		class __declspec(dllexport) Instance
		{
		public:
			ID3D10Device*							D3DDevice;
			ID3D10RenderTargetView*					RenderTargetView;
			ID3D10RasterizerState*					SolidRasterState;
			ID3D10RasterizerState*					WireFrameRasterState;
			ID3D10ShaderResourceView*				ShaderResourceView;
			ID3DX10Sprite*							MainSprite;
			ID3D10Buffer*							CubeVertexBuffer;
			ID3D10Buffer*							CubeIndexBuffer;
			IDXGIFactory*							Factory;
			IXAudio2*								XAudio2;
			X3DAUDIO_HANDLE							X3DAudio;
			IXAudio2MasteringVoice*					MasteringVoice;
			XAUDIO2_DEVICE_DETAILS*					DeviceDetails;
			X3DAUDIO_DSP_SETTINGS*					DSPSettings;
			NxPhysicsSDK*							PhysX;
			NxCookingInterface*						Cooking;
			NxUserAllocatorDefault*					Allocator;
			NxControllerManager*					Manager;
			NxConvexMesh*							Shapes[1];
			void*									FBXManager;
			Interface*								ActiveText;
			bool									Active;
			bool									CursorBlink;
			Effect*									DefaultEffect;
			std::vector< std::string >				ErrorLog;
			std::string								TextCursor;
			std::vector<RenderTarget*>				RenderTargets;
			std::vector<Scene*>						Scenes;
			std::vector<Effect*>					Effects;
			std::vector<Mesh*>						Meshes;
			RenderTarget*							CreateRenderTarget( HWND window, RECT* rect, DWORD flags );
			Scene*									CreateScene( const std::string& file, Matrix44* offset, DWORD flags );
			AudioBuffer*							CreateAudioBuffer( const std::string& file );
			Mesh*									CreateMesh( const std::string& file );
			Effect*									CreateEffect( const std::string& file );
			void									Release();
		};
	
		__declspec(dllexport) Instance* Create( DWORD flags );
	}
}