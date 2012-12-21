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
* @brief This file contains all classes used for windows and keyboard/mouse inputs.
*/

#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglGraphics.h"
#include "OvglWindow.h"
#include <SDL2/SDL.h>

namespace Ovgl
{
unsigned char SFKeyToASCII(SDL_Keycode keycode)
{
    switch (keycode)
    {
    case SDLK_ESCAPE:
        return 27;
        break;
    case SDLK_LCTRL:
        return 162;
        break;
    case SDLK_LSHIFT:
        return 160;
        break;
    case SDLK_LALT:
        return 164;
        break;
    case SDLK_RGUI:
        return 91;
        break;
    case SDLK_RCTRL:
        return 163;
        break;
    case SDLK_RSHIFT:
        return 161;
        break;
    case SDLK_RALT:
        return 165;
        break;
    case SDLK_MENU:
        return 93;
        break;
    case SDLK_LEFTPAREN:
        return 91;
        break;
    case SDLK_RIGHTPAREN:
        return 90;
        break;
    case SDLK_SEMICOLON:
        return 59;
        break;
    case SDLK_COMMA:
        return 44;
        break;
    case SDLK_PERIOD:
        return 46;
        break;
    case SDLK_QUOTE:
        return 34;
        break;
    case SDLK_SLASH:
        return 47;
        break;
    case SDLK_BACKSLASH:
        return 92;
        break;
    case SDLK_EQUALS:
        return 61;
        break;
    case SDLK_MINUS:
        return 45;
        break;
    case SDLK_SPACE:
        return 32;
        break;
    case SDLK_RETURN:
        return 13;
        break;
    case SDLK_BACKSPACE:
        return 8;
        break;
    case SDLK_TAB:
        return 9;
        break;
    case SDLK_PAGEUP:
        return 33;
        break;
    case SDLK_PAGEDOWN:
        return 34;
        break;
    case SDLK_END:
        return 35;
        break;
    case SDLK_HOME:
        return 36;
        break;
    case SDLK_INSERT:
        return 45;
        break;
    case SDLK_DELETE:
        return 48;
        break;
    case SDLK_PAUSE:
        return 19;
        break;
    case SDLK_KP_PLUS:
        return 43;
        break;
    case SDLK_KP_MINUS:
        return 45;
        break;
    case SDLK_KP_MULTIPLY:
        return 42;
        break;
    case SDLK_KP_DIVIDE:
        return 47;
        break;
    case SDLK_LEFT:
        return 37;
        break;
    case SDLK_RIGHT:
        return 39;
        break;
    case SDLK_UP:
        return 38;
        break;
    case SDLK_DOWN:
        return 40;
        break;
    case SDLK_KP_0:
        return 96;
        break;
    case SDLK_KP_1:
        return 97;
        break;
    case SDLK_KP_2:
        return 98;
        break;
    case SDLK_KP_3:
        return 99;
        break;
    case SDLK_KP_4:
        return 100;
        break;
    case SDLK_KP_5:
        return 101;
        break;
    case SDLK_KP_6:
        return 102;
        break;
    case SDLK_KP_7:
        return 103;
        break;
    case SDLK_KP_8:
        return 104;
        break;
    case SDLK_KP_9:
        return 105;
        break;
    case SDLK_F1:
        return 112;
        break;
    case SDLK_F2:
        return 113;
        break;
    case SDLK_F3:
        return 114;
        break;
    case SDLK_F4:
        return 115;
        break;
    case SDLK_F5:
        return 116;
        break;
    case SDLK_F6:
        return 117;
        break;
    case SDLK_F7:
        return 118;
        break;
    case SDLK_F8:
        return 119;
        break;
    case SDLK_F9:
        return 120;
        break;
    case SDLK_F10:
        return 121;
        break;
    case SDLK_F11:
        return 122;
        break;
    case SDLK_F12:
        return 123;
        break;
    case SDLK_F13:
        return 124;
        break;
    case SDLK_F14:
        return 125;
        break;
    case SDLK_F15:
        return 126;
        break;
    case SDLK_0:
        return 48;
        break;
    case SDLK_1:
        return 49;
        break;
    case SDLK_2:
        return 50;
        break;
    case SDLK_3:
        return 51;
        break;
    case SDLK_4:
        return 52;
        break;
    case SDLK_5:
        return 53;
        break;
    case SDLK_6:
        return 54;
        break;
    case SDLK_7:
        return 55;
        break;
    case SDLK_8:
        return 56;
        break;
    case SDLK_9:
        return 57;
        break;
    case SDLK_a:
        return 65;
        break;
    case SDLK_b:
        return 66;
        break;
    case SDLK_c:
        return 67;
        break;
    case SDLK_d:
        return 68;
        break;
    case SDLK_e:
        return 69;
        break;
    case SDLK_f:
        return 70;
        break;
    case SDLK_g:
        return 71;
        break;
    case SDLK_h:
        return 72;
        break;
    case SDLK_i:
        return 73;
        break;
    case SDLK_j:
        return 74;
        break;
    case SDLK_k:
        return 75;
        break;
    case SDLK_l:
        return 76;
        break;
    case SDLK_m:
        return 77;
        break;
    case SDLK_n:
        return 78;
        break;
    case SDLK_o:
        return 79;
        break;
    case SDLK_p:
        return 80;
        break;
    case SDLK_q:
        return 81;
        break;
    case SDLK_r:
        return 82;
        break;
    case SDLK_s:
        return 83;
        break;
    case SDLK_t:
        return 84;
        break;
    case SDLK_u:
        return 85;
        break;
    case SDLK_v:
        return 86;
        break;
    case SDLK_w:
        return 87;
        break;
    case SDLK_x:
        return 88;
        break;
    case SDLK_y:
        return 89;
        break;
    case SDLK_z:
        return 90;
        break;

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
    lockmouse = false;
    title = name.c_str();
    On_KeyDown = NULL;
    On_KeyUp = NULL;
    On_MouseMove = NULL;
    On_MouseDown = NULL;
    On_MouseUp = NULL;
    On_MouseOver = NULL;
    On_MouseOut = NULL;
    hWnd = SDL_CreateWindow( name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    SDL_GL_MakeCurrent(hWnd, context->hWnd);
    glDisable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_CULL_FACE);
    SDL_GL_MakeCurrent(0, 0);
    context->Windows.push_back(this);
};

void Window::LockMouse( bool state )
{
    if( state )
    {
        lockmouse = true;
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    else
    {
        lockmouse = false;
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

void Window::DoEvents()
{
    SDL_Event sdl_event;
    Event event;
    while(SDL_PollEvent(&sdl_event))
    {
        switch (sdl_event.type)
        {
        case SDL_KEYDOWN:
            event.type = OVGL_KEYDOWN;
            event.key = SFKeyToASCII(sdl_event.key.keysym.sym);
            if(On_KeyDown)
            {
                On_KeyDown( event.key );
            }
            for( uint32_t r = 0; r < RenderTargets.size(); r++ )
            {
                RenderTargets[r]->DoEvent(event);
            }
            break;

        case SDL_KEYUP:
            event.type = OVGL_KEYUP;
            event.key = SFKeyToASCII(sdl_event.key.keysym.sym);
            if(On_KeyUp)
            {
                On_KeyUp( event.key );
            }
            for( uint32_t r = 0; r < RenderTargets.size(); r++ )
            {
                RenderTargets[r]->DoEvent(event);
            }
            break;
        case SDL_MOUSEMOTION:
            event.type = OVGL_MOUSEMOTION;
            event.mouse_x = sdl_event.motion.x;
            event.mouse_y = sdl_event.motion.y;
            if(!lockmouse)
            {
                if(On_MouseMove)
                {
                    On_MouseMove( event.mouse_x, event.mouse_y );
                }
                for( uint32_t r = 0; r < RenderTargets.size(); r++ )
                {
                    RenderTargets[r]->DoEvent(event);
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            event.type = OVGL_MOUSEBUTTONDOWN;
            event.mouse_x = sdl_event.motion.x;
            event.mouse_y = sdl_event.motion.y;
            event.button = sdl_event.button.button;
            if(!lockmouse)
            {
                if(On_MouseDown)
                {
                    On_MouseDown( event.mouse_x, event.mouse_y, event.button );
                }
                for( uint32_t r = 0; r < RenderTargets.size(); r++ )
                {
                    RenderTargets[r]->DoEvent(event);
                }
            }
            else
            {
                if(On_MouseDown)
                {
                    On_MouseDown( 0, 0, event.button );
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            event.type = OVGL_MOUSEBUTTONUP;
            event.mouse_x = sdl_event.motion.x;
            event.mouse_y = sdl_event.motion.y;
            event.button = sdl_event.button.button;
            if(!lockmouse)
            {
                if(On_MouseUp)
                {
                    On_MouseUp( event.mouse_x, event.mouse_y, event.button );
                }
                for( uint32_t r = 0; r < RenderTargets.size(); r++ )
                {
                    RenderTargets[r]->DoEvent(event);
                }
            }
            else
            {
                if(On_MouseUp)
                {
                    On_MouseUp( 0, 0, event.button );
                }
            }
            break;
        case SDL_WINDOWEVENT:
            event.type = OVGL_WINDOWEVENT;
            switch (sdl_event.window.event)
            {
            case SDL_WINDOWEVENT_ENTER:
                event.window_event = OVGL_WINDOWEVENT_ENTER;
                if(On_MouseOver)
                {
                    On_MouseOver();
                }
                break;
            case SDL_WINDOWEVENT_LEAVE:
                event.window_event = OVGL_WINDOWEVENT_LEAVE;
                if(On_MouseOut)
                {
                    On_MouseOut();
                }
                break;
            case SDL_WINDOWEVENT_RESIZED:
                event.window_event = OVGL_WINDOWEVENT_RESIZED;
                for(uint32_t i = 0; i < RenderTargets.size(); i++)
                {
                    RenderTargets[i]->Update();
                }
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                event.window_event = OVGL_WINDOWEVENT_FOCUS_GAINED;
                active = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                event.window_event = OVGL_WINDOWEVENT_FOCUS_LOST;
                active = false;
                break;
            case SDL_WINDOWEVENT_CLOSE:
                event.window_event = OVGL_WINDOWEVENT_CLOSE;
                context->g_Quit = true;
                break;
            }
            break;
        default:
            break;
        }
    }
    if(lockmouse && active)
    {

        int32_t cx, cy;
        SDL_GetWindowSize( hWnd, &cx, &cy);
        cx /= 2;
        cy /= 2;

        int32_t mx, my;
        SDL_GetRelativeMouseState( &mx, &my );
        if(On_MouseMove)
        {
            On_MouseMove( mx, my );
        }
        SDL_WarpMouseInWindow(hWnd,cx, cy);
    }
    SDL_GL_SwapWindow(hWnd);
}

void Window::SetFullscreen( bool state )
{
    if(state)
    {
        SDL_SetWindowFullscreen(hWnd, SDL_TRUE);
        fullscreen = true;
    }
    else
    {
        SDL_SetWindowFullscreen(hWnd, SDL_FALSE);
        fullscreen = false;
    }
}

void Window::SetVSync( bool state )
{
    vsync = state;
    SDL_GL_SetSwapInterval(state);
}

Window::~Window()
{
    SDL_DestroyWindow(hWnd);
}
}
