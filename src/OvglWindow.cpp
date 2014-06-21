/**
 * @file OvglWindow.cpp
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
 * @brief This part handles windows and keyboard/mouse inputs.
 */

#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglGraphics.h"
#include "OvglWindow.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

namespace Ovgl
{
unsigned char SDLKeyToASCII(SDL_Keycode keycode)
{
	switch (keycode)
	{
		case SDLK_ESCAPE:
			return 27;
		case SDLK_LCTRL:
			return 162;
		case SDLK_LSHIFT:
			return 160;
		case SDLK_LALT:
			return 164;
		case SDLK_RGUI:
			return 91;
		case SDLK_RCTRL:
			return 163;
		case SDLK_RSHIFT:
			return 161;
		case SDLK_RALT:
			return 165;
		case SDLK_MENU:
			return 93;
		case SDLK_LEFTPAREN:
			return 91;
		case SDLK_RIGHTPAREN:
			return 90;
		case SDLK_SEMICOLON:
			return 59;
		case SDLK_COMMA:
			return 44;
		case SDLK_PERIOD:
			return 46;
		case SDLK_QUOTE:
			return 34;
		case SDLK_SLASH:
			return 47;
		case SDLK_BACKSLASH:
			return 92;
		case SDLK_EQUALS:
			return 61;
		case SDLK_MINUS:
			return 45;
		case SDLK_SPACE:
			return 32;
		case SDLK_RETURN:
			return 13;
		case SDLK_BACKSPACE:
			return 8;
		case SDLK_TAB:
			return 9;
		case SDLK_PAGEUP:
			return 33;
		case SDLK_PAGEDOWN:
			return 34;
		case SDLK_END:
			return 35;
		case SDLK_HOME:
			return 36;
		case SDLK_INSERT:
			return 45;
		case SDLK_DELETE:
			return 48;
		case SDLK_PAUSE:
			return 19;
		case SDLK_KP_PLUS:
			return 43;
		case SDLK_KP_MINUS:
			return 45;
		case SDLK_KP_MULTIPLY:
			return 42;
		case SDLK_KP_DIVIDE:
			return 47;
		case SDLK_LEFT:
			return 37;
		case SDLK_RIGHT:
			return 39;
		case SDLK_UP:
			return 38;
		case SDLK_DOWN:
			return 40;
		case SDLK_KP_0:
			return 96;
		case SDLK_KP_1:
			return 97;
		case SDLK_KP_2:
			return 98;
		case SDLK_KP_3:
			return 99;
		case SDLK_KP_4:
			return 100;
		case SDLK_KP_5:
			return 101;
		case SDLK_KP_6:
			return 102;
		case SDLK_KP_7:
			return 103;
		case SDLK_KP_8:
			return 104;
		case SDLK_KP_9:
			return 105;
		case SDLK_F1:
			return 112;
		case SDLK_F2:
			return 113;
		case SDLK_F3:
			return 114;
		case SDLK_F4:
			return 115;
		case SDLK_F5:
			return 116;
		case SDLK_F6:
			return 117;
		case SDLK_F7:
			return 118;
		case SDLK_F8:
			return 119;
		case SDLK_F9:
			return 120;
		case SDLK_F10:
			return 121;
		case SDLK_F11:
			return 122;
		case SDLK_F12:
			return 123;
		case SDLK_F13:
			return 124;
		case SDLK_F14:
			return 125;
		case SDLK_F15:
			return 126;
		case SDLK_0:
			return 48;
		case SDLK_1:
			return 49;
		case SDLK_2:
			return 50;
		case SDLK_3:
			return 51;
		case SDLK_4:
			return 52;
		case SDLK_5:
			return 53;
		case SDLK_6:
			return 54;
		case SDLK_7:
			return 55;
		case SDLK_8:
			return 56;
		case SDLK_9:
			return 57;
		case SDLK_a:
			return 65;
		case SDLK_b:
			return 66;
		case SDLK_c:
			return 67;
		case SDLK_d:
			return 68;
		case SDLK_e:
			return 69;
		case SDLK_f:
			return 70;
		case SDLK_g:
			return 71;
		case SDLK_h:
			return 72;
		case SDLK_i:
			return 73;
		case SDLK_j:
			return 74;
		case SDLK_k:
			return 75;
		case SDLK_l:
			return 76;
		case SDLK_m:
			return 77;
		case SDLK_n:
			return 78;
		case SDLK_o:
			return 79;
		case SDLK_p:
			return 80;
		case SDLK_q:
			return 81;
		case SDLK_r:
			return 82;
		case SDLK_s:
			return 83;
		case SDLK_t:
			return 84;
		case SDLK_u:
			return 85;
		case SDLK_v:
			return 86;
		case SDLK_w:
			return 87;
		case SDLK_x:
			return 88;
		case SDLK_y:
			return 89;
		case SDLK_z:
			return 90;

		default:
			return 0;
	}
}

Window::Window( Context* pcontext, const std::string& name )
{
	context = pcontext;
	fullscreen = false;
	sizing = false;
	active = true;
	lockMouse = false;
	title = name.c_str();
	onKeyDown = NULL;
	onKeyUp = NULL;
	onMouseMove = NULL;
	onMouseDown = NULL;
	onMouseUp = NULL;
	onMouseOver = NULL;
	onMouseOut = NULL;
	SDL_GL_MakeCurrent(context->contextWindow, context->glContext);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	sdlWindow = SDL_CreateWindow( name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	windowContext = SDL_GL_CreateContext(sdlWindow); 
	SDL_GL_MakeCurrent(sdlWindow, windowContext);
	glDisable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_CULL_FACE);
	SDL_GL_MakeCurrent(0, 0);
	context->windows.push_back(this);
};

Window::Window( Context* pcontext, const std::string& name, unsigned int width, unsigned int height )
{
	context = pcontext;
	fullscreen = false;
	sizing = false;
	active = true;
	lockMouse = false;
	title = name.c_str();
	onKeyDown = NULL;
	onKeyUp = NULL;
	onMouseMove = NULL;
	onMouseDown = NULL;
	onMouseUp = NULL;
	onMouseOver = NULL;
	onMouseOut = NULL;
	SDL_GL_MakeCurrent(context->contextWindow, context->glContext);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	sdlWindow = SDL_CreateWindow( name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,  width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	windowContext = SDL_GL_CreateContext(sdlWindow); 
	SDL_GL_MakeCurrent(sdlWindow, windowContext);
	glDisable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_CULL_FACE);
	SDL_GL_MakeCurrent(0, 0);
	context->windows.push_back(this);
};

Window::~Window()
{
	for( uint32_t r = 0; r < renderTargets.size(); r++ )
	{
		delete renderTargets[r];
	}
	SDL_DestroyWindow(sdlWindow);
}

void Window::setLockMouse( bool state )
{
	if( state )
	{
		lockMouse = true;
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else
	{
		lockMouse = false;
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}

void Window::doEvents()
{
	SDL_Event sdlEvent;
	Event event;
	while(SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
			case SDL_KEYDOWN:
				event.type = OVGL_KEYDOWN;
				event.key = SDLKeyToASCII(sdlEvent.key.keysym.sym);
				if(onKeyDown)
				{
					onKeyDown( event.key );
				}
				for( uint32_t r = 0; r < renderTargets.size(); r++ )
				{
					renderTargets[r]->doEvent(event);
				}
				break;

			case SDL_KEYUP:
				event.type = OVGL_KEYUP;
				event.key = SDLKeyToASCII(sdlEvent.key.keysym.sym);
				if(onKeyUp)
				{
					onKeyUp( event.key );
				}
				for( uint32_t r = 0; r < renderTargets.size(); r++ )
				{
					renderTargets[r]->doEvent(event);
				}
				break;
			case SDL_MOUSEMOTION:
				event.type = OVGL_MOUSEMOTION;
				event.mouseX = sdlEvent.motion.x;
				event.mouseY = sdlEvent.motion.y;
				if(!lockMouse)
				{
					if(onMouseMove)
					{
						onMouseMove( event.mouseX, event.mouseY );
					}
					for( uint32_t r = 0; r < renderTargets.size(); r++ )
					{
						renderTargets[r]->doEvent(event);
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				event.type = OVGL_MOUSEBUTTONDOWN;
				event.mouseX = sdlEvent.motion.x;
				event.mouseY = sdlEvent.motion.y;
				event.button = sdlEvent.button.button;
				if(!lockMouse)
				{
					if(onMouseDown)
					{
						onMouseDown( event.mouseX, event.mouseY, event.button );
					}
					for( uint32_t r = 0; r < renderTargets.size(); r++ )
					{
						renderTargets[r]->doEvent(event);
					}
				}
				else
				{
					if(onMouseDown)
					{
						onMouseDown( 0, 0, event.button );
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				event.type = OVGL_MOUSEBUTTONUP;
				event.mouseX = sdlEvent.motion.x;
				event.mouseY = sdlEvent.motion.y;
				event.button = sdlEvent.button.button;
				if(!lockMouse)
				{
					if(onMouseUp)
					{
						onMouseUp( event.mouseX, event.mouseY, event.button );
					}
					for( uint32_t r = 0; r < renderTargets.size(); r++ )
					{
						renderTargets[r]->doEvent(event);
					}
				}
				else
				{
					if(onMouseUp)
					{
						onMouseUp( 0, 0, event.button );
					}
				}
				break;
			case SDL_WINDOWEVENT:
				event.type = OVGL_WINDOWEVENT;
				switch (sdlEvent.window.event)
				{
					case SDL_WINDOWEVENT_ENTER:
						event.windowEvent = OVGL_WINDOWEVENT_ENTER;
						if(onMouseOver)
						{
							onMouseOver();
						}
						break;
					case SDL_WINDOWEVENT_LEAVE:
						event.windowEvent = OVGL_WINDOWEVENT_LEAVE;
						if(onMouseOut)
						{
							onMouseOut();
						}
						break;
					case SDL_WINDOWEVENT_RESIZED:
						event.windowEvent = OVGL_WINDOWEVENT_RESIZED;
						for(uint32_t i = 0; i < renderTargets.size(); i++)
						{
							renderTargets[i]->update();
						}
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						event.windowEvent = OVGL_WINDOWEVENT_FOCUS_GAINED;
						active = true;
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						event.windowEvent = OVGL_WINDOWEVENT_FOCUS_LOST;
						active = false;
						break;
					case SDL_WINDOWEVENT_CLOSE:
						event.windowEvent = OVGL_WINDOWEVENT_CLOSE;
						context->gQuit = true;
						break;
				}
				break;
			default:
				break;
		}
	}
	if(lockMouse && active)
	{

		int32_t cx, cy;
		SDL_GetWindowSize( sdlWindow, &cx, &cy);
		cx /= 2;
		cy /= 2;

		int32_t mx, my;
		SDL_GetRelativeMouseState( &mx, &my );
		if(onMouseMove)
		{
			onMouseMove( mx, my );
		}
		SDL_WarpMouseInWindow(sdlWindow, cx, cy);
	}
	SDL_GL_MakeCurrent( sdlWindow, windowContext );
	SDL_GL_SwapWindow( sdlWindow );
}

void Window::setFullscreen( bool state )
{
	if(state)
	{
		SDL_SetWindowFullscreen(sdlWindow, SDL_TRUE);
		fullscreen = true;
	}
	else
	{
		SDL_SetWindowFullscreen(sdlWindow, SDL_FALSE);
		fullscreen = false;
	}
}

void Window::setVsync( bool state )
{
	vsync = state;
	SDL_GL_SetSwapInterval(state);
}
}
