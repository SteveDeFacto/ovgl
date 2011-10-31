/**
* @file OvglGraphics.h
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

// Forward declare external classes.
typedef struct _CGcontext *CGcontext;

namespace Ovgl
{
	extern "C"
	{
		class Camera;
		class Interface;

		class __declspec(dllexport) RenderTarget
		{
		public:
			Instance* Inst;
			Camera* view;
			GLuint MultiSampleFrameBuffer;
			GLuint EffectFrameBuffer;
			GLuint DepthBuffer;
			GLuint ColorBuffer;
			GLuint HDRTex;
			GLuint PrimaryTex;
			GLuint SecondaryTex;
			GLuint PrimaryBloomTex;
			GLuint SecondaryBloomTex;
			HWND hWnd;
			HDC hDC;
			HGLRC hRC;
			Ovgl::Vector4 Rect;
			std::vector< Ovgl::Interface* > Interfaces;
			Ovgl::Interface* CreateSprite( Ovgl::Texture* Texture, Ovgl::Vector4* rect );
			Ovgl::Interface* CreateText( const std::string& text, Ovgl::Vector4* rect );
			float eye_luminance;
			bool debugMode;
			int bloom;
			bool autoLuminance;
			bool multiSample;
			bool motionBlur;
			void AutoLuminance();
			void Bloom();
			void MotionBlur( float x, float y );
			void DrawMarker( Matrix44& matrix );
			void SetVSync( bool state );
			bool GetFullscreen();
			void SetFullscreen( bool state );
			void Release();
			void Render();
			void RenderMesh( Ovgl::Mesh* mesh, Matrix44& matrix, std::vector< Matrix44 >& pose, std::vector< Material* >& materials, bool PostRender );
		};

		class Interface
		{
		public:
			Ovgl::RenderTarget*					RenderTarget;
			Ovgl::Texture*						Texture;
			int									CSize;
			std::string							Text;
			std::string							Font;
			float								Size;
			bool								Bold;
			bool								Italic;
			Ovgl::Vector4						Rect;
			bool								Enabled;
			bool								Over;
			void UpdateText();
			void Release();
		};
	}
}