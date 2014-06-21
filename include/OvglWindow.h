/**
 * @file OvglWindow.h
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

#include "OvglCommon.h"

namespace Ovgl
{
extern "C"
{
	class RenderTarget;

	class DLLEXPORT Window
	{
		public:
			Window( Ovgl::Context* context, const std::string& name );
			Window( Ovgl::Context* pcontext, const std::string& name, unsigned int width, unsigned int height );
			~Window();
			Context*                                              context;
			SDL_Window*                                           sdlWindow;
			SDL_GLContext                                         windowContext;
			std::vector< RenderTarget* >                          renderTargets;
			std::string                                           title;
			bool                                                  sizing;
			bool                                                  active;
			bool                                                  fullscreen;
			bool                                                  vsync;
			bool                                                  lockMouse;
			void doEvents();
			bool getMaximized();
			bool getActive();
			void setVsync( bool state );
			void setFullscreen( bool state );
			void setLockMouse( bool state );
			void (*onKeyDown)(char);
			void (*onKeyUp)(char);
			void (*onMouseMove)(long, long);
			void (*onMouseDown)(long, long, int);
			void (*onMouseUp)(long, long, int);
			void (*onMouseOver)();
			void (*onMouseOut)();
	};
}
}
