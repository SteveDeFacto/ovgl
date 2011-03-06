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
struct IDXGISwapChain;
struct ID3D10DepthStencilView;
struct D3D10_VIEWPORT;

namespace Ovgl
{
	extern "C"
	{
		class Camera;
		class Interface;

		class __declspec(dllexport) RenderTarget
		{
		public:
			Instance*					Inst;
			Camera*						view;
			HWND						hWnd;
			IDXGISwapChain*				SwapChain;
			ID3D10RenderTargetView*		RenderTargetView;
			ID3D10DepthStencilView*		DepthStencilView;
			D3D10_VIEWPORT*				ViewPort;
			bool						GetFullscreen();
			void						SetFullscreen( bool state );
			void						Release();
			void						Update();
		};
	}
}