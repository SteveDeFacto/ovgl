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
		class Vector4;

		class __declspec(dllexport) RenderTarget
		{
		public:
			/**
			* This is the constuctor for a window based render target.
			*/
			RenderTarget( Ovgl::Instance* Instance, Ovgl::Window* window, const Ovgl::Vector4& viewport, uint32_t flags );
			/**
			* This is the constuctor for a texture based render target.
			*/
			RenderTarget( Ovgl::Instance* Instance, Ovgl::Texture* texture, const Ovgl::Vector4& viewport, uint32_t flags );
			/**
			* This is a pointer to the instance which owns this render target.
			*/
			Instance* Inst;
			/**
			* This is a pointer to the camera which the render target will render from.
			*/
			Camera* View;
			/**
			* This frame buffer is used to provide multi sampling.
			*/
			GLuint MultiSampleFrameBuffer;
			/**
			* This frame buffer is used to apply effects.
			*/
			GLuint EffectFrameBuffer;
			/**
			* Depth buffer used for rendering.
			*/
			GLuint DepthBuffer;
			/**
			* Color buffer used for rendering.
			*/
			GLuint ColorBuffer;
			/**
			* This is the primary texture.
			*/
			GLuint PrimaryTex;
			/**
			* This is the secondary texture.
			*/
			GLuint SecondaryTex;
			/**
			* This is the primary bloom texture.
			*/
			GLuint PrimaryBloomTex;
			/**
			* This is the secondary bloom texture.
			*/
			GLuint SecondaryBloomTex;
			/**
			* Pointer to window if this is a window based render target.
			*/
			Ovgl::Window* hWin;
			/**
			* Pointer to texture if this is a texture based render target.
			*/
			Ovgl::Texture* hTex;
			/**
			* The area of the texture or window to render to.
			*/
			Ovgl::Vector4 Rect;
			/**
			* List of user interfaces.
			*/
			std::vector< Ovgl::Interface* > Interfaces;
			/**
			* This function creates a sprite user interface.
			*/
			Ovgl::Interface* CreateSprite( Ovgl::Texture* Texture, Ovgl::Vector4* rect );
			/**
			* This function creates a text user interface.
			*/
			Ovgl::Interface* CreateText( const std::string& text, Ovgl::Vector4* rect );
			/**
			* Last position of camera. Used for motion blur.
			*/
			Ovgl::Vector2 LastCamVec;
			/**
			* The area of the texture or window to render to.
			*/
			float eye_luminance;
			/**
			* Indicates if render target is to display debug information.
			*/
			bool debugMode;
			/**
			* Indicates if bloom is enabled.
			*/
			int bloom;
			/**
			* Indicates if automatic luminance is enabled.
			*/
			bool autoLuminance;
			/**
			* Indicates if multisampling is enabled.
			*/
			bool multiSample;
			/**
			* Indicates if motion blur is enabled.
			*/
			bool motionBlur;
			/**
			* Handle to draw context.
			*/
			HDC hDC;
			/**
			* Render auto luminance effect.
			*/
			void AutoLuminance();
			/**
			* Render auto luminance effect.
			*/
			void Bloom();
			/**
			* Render auto luminance effect.
			*/
			void MotionBlur( float x, float y );
			/**
			* Render debug marker.
			*/
			void DrawMarker( const Matrix44& matrix );
			/**
			* Release render target.
			*/
			void Release();
			/**
			* Render entire scene.
			*/
			void Render();
			/**
			* Update dimensions of render target.
			*/
			void Update();
			/**
			* Render a single mesh.
			*/
			void RenderMesh( const Ovgl::Mesh& mesh, const Matrix44& matrix, std::vector< Matrix44 >& pose, std::vector< Material* >& materials, bool PostRender );
		};

		class Interface
		{
		public:
			/**
			* Pointer to render target.
			*/
			Ovgl::RenderTarget*					RenderTarget;
			/**
			* Pointer to texture to display for this interface.
			*/
			Ovgl::Texture*						Texture;
			int									CSize;
			/**
			* Text to display for this interface.
			*/
			std::string							Text;
			/**
			* Font to use for this interface.
			*/
			std::string							Font;
			/**
			* Font size to use for this interface.
			*/
			float								Size;
			/**
			* Specifies if font is bold.
			*/
			bool								Bold;
			/**
			* Specifies if font is italic.
			*/
			bool								Italic;
			/**
			* Area to render interface.
			*/
			Ovgl::Vector4						Rect;
			/**
			* Specifies if render target is enabled.
			*/
			bool								Enabled;
			/**
			* Specifies if mouse pointer is over interface.
			*/
			bool								Over;
			/**
			* Updates texture to display the specified text.
			*/
			void UpdateText();
			/**
			* Releases this interface.
			*/
			void Release();
		};
	}
}
