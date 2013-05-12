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

#ifdef _WIN32
#  define DLLEXPORT __declspec( dllexport )
#else
#  define DLLEXPORT
#endif

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
            Context* context;
            SDL_Window* sdl_window;
            std::vector< RenderTarget* > render_targets;
            std::string title;
			bool sizing;
			bool active;
			bool fullscreen;
			bool vsync;
			bool lock_mouse;
			void do_events();
			bool get_maximized();
			bool get_active();
			void set_vsync( bool state );
			void set_fullscreen( bool state );
			void set_lock_mouse( bool state );
			void (*on_key_down)(char);
			void (*on_key_up)(char);
			void (*on_mouse_move)(long, long);
			void (*on_mouse_down)(long, long, int);
			void (*on_mouse_up)(long, long, int);
            void (*on_mouse_over)();
            void (*on_mouse_out)();
		};
	}
}
