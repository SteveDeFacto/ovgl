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

#include "OvglIncludes.h"
#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglGraphics.h"
#include "OvglWindow.h"

namespace Ovgl
{
	unsigned char SFKeyToASCII(sf::Keyboard::Key keycode)
	{
		switch (keycode)
		{
		case sf::Keyboard::Escape:
			return 27;
			break;
		case sf::Keyboard::LControl:
			return 162;
			break;
		case sf::Keyboard::LShift:
			return 160;
			break;
		case sf::Keyboard::LAlt:
			return 164;
			break;
		case sf::Keyboard::LSystem:
			return 91;
			break;
		case sf::Keyboard::RControl:
			return 163;
			break;
		case sf::Keyboard::RShift:
			return 161;
			break;
		case sf::Keyboard::RAlt:
			return 165;
			break;
		case sf::Keyboard::RSystem:
			return 92;
			break;
		case sf::Keyboard::Menu:
			return 93;
			break;
		case sf::Keyboard::LBracket:
			return 91;
			break;
		case sf::Keyboard::RBracket:
			return 90;
			break;
		case sf::Keyboard::SemiColon:
			return 59;
			break;
		case sf::Keyboard::Comma:
			return 44;
			break;
		case sf::Keyboard::Period:
			return 46;
			break;
		case sf::Keyboard::Quote:
			return 34;
			break;
		case sf::Keyboard::Slash:
			return 47;
			break;
		case sf::Keyboard::BackSlash:
			return 92;
			break;
		case sf::Keyboard::Tilde:
			return 152;
			break;
		case sf::Keyboard::Equal:
			return 61;
			break;
		case sf::Keyboard::Dash:
			return 45;
			break;
		case sf::Keyboard::Space:
			return 32;
			break;
		case sf::Keyboard::Return:
			return 13;
			break;
		case sf::Keyboard::Back:
			return 8;
			break;
		case sf::Keyboard::Tab:
			return 9;
			break;
		case sf::Keyboard::PageUp:
			return 33;
			break;
		case sf::Keyboard::PageDown:
			return 34;
			break;
		case sf::Keyboard::End:
			return 35;
			break;
		case sf::Keyboard::Home:
			return 36;
			break;
		case sf::Keyboard::Insert:
			return 45;
			break;
		case sf::Keyboard::Delete:
			return 48;
			break;
		case sf::Keyboard::Pause:
			return 19;
			break;
		case sf::Keyboard::Add:
			return 43;
			break;
		case sf::Keyboard::Subtract:
			return 45;
			break;
		case sf::Keyboard::Multiply:
			return 42;
			break;
		case sf::Keyboard::Divide:
			return 47;
			break;
		case sf::Keyboard::Left:
			return 37;
			break;
		case sf::Keyboard::Right:
			return 39;
			break;
		case sf::Keyboard::Up:
			return 38;
			break;
		case sf::Keyboard::Down:
			return 40;
			break;
		case sf::Keyboard::Numpad0:
			return 96;
			break;
		case sf::Keyboard::Numpad1:
			return 97;
			break;
		case sf::Keyboard::Numpad2:
			return 98;
			break;
		case sf::Keyboard::Numpad3:
			return 99;
			break;
		case sf::Keyboard::Numpad4:
			return 100;
			break;
		case sf::Keyboard::Numpad5:
			return 101;
			break;
		case sf::Keyboard::Numpad6:
			return 102;
			break;
		case sf::Keyboard::Numpad7:
			return 103;
			break;
		case sf::Keyboard::Numpad8:
			return 104;
			break;
		case sf::Keyboard::Numpad9:
			return 105;
			break;
		case sf::Keyboard::F1:
			return 112;
			break;
		case sf::Keyboard::F2:
			return 113;
			break;
		case sf::Keyboard::F3:
			return 114;
			break;
		case sf::Keyboard::F4:
			return 115;
			break;
		case sf::Keyboard::F5:
			return 116;
			break;
		case sf::Keyboard::F6:
			return 117;
			break;
		case sf::Keyboard::F7:
			return 118;
			break;
		case sf::Keyboard::F8:
			return 119;
			break;
		case sf::Keyboard::F9:
			return 120;
			break;
		case sf::Keyboard::F10:
			return 121;
			break;
		case sf::Keyboard::F11:
			return 122;
			break;
		case sf::Keyboard::F12:
			return 123;
			break;
		case sf::Keyboard::F13:
			return 124;
			break;
		case sf::Keyboard::F14:
			return 125;
			break;
		case sf::Keyboard::F15:
			return 126;
			break;
		case sf::Keyboard::Num0:
			return 48;
			break;
		case sf::Keyboard::Num1:
			return 49;
			break;
		case sf::Keyboard::Num2:
			return 50;
			break;
		case sf::Keyboard::Num3:
			return 51;
			break;
		case sf::Keyboard::Num4:
			return 52;
			break;
		case sf::Keyboard::Num5:
			return 53;
			break;
		case sf::Keyboard::Num6:
			return 54;
			break;
		case sf::Keyboard::Num7:
			return 55;
			break;
		case sf::Keyboard::Num8:
			return 56;
			break;
		case sf::Keyboard::Num9:
			return 57;
			break;
		case sf::Keyboard::A:
			return 65;
			break;
		case sf::Keyboard::B:
			return 66;
			break;
		case sf::Keyboard::C:
			return 67;
			break;
		case sf::Keyboard::D:
			return 68;
			break;
		case sf::Keyboard::E:
			return 69;
			break;
		case sf::Keyboard::F:
			return 70;
			break;
		case sf::Keyboard::G:
			return 71;
			break;
		case sf::Keyboard::H:
			return 72;
			break;
		case sf::Keyboard::I:
			return 73;
			break;
		case sf::Keyboard::J:
			return 74;
			break;
		case sf::Keyboard::K:
			return 75;
			break;
		case sf::Keyboard::L:
			return 76;
			break;
		case sf::Keyboard::M:
			return 77;
			break;
		case sf::Keyboard::N:
			return 78;
			break;
		case sf::Keyboard::O:
			return 79;
			break;
		case sf::Keyboard::P:
			return 80;
			break;
		case sf::Keyboard::Q:
			return 81;
			break;
		case sf::Keyboard::R:
			return 82;
			break;
		case sf::Keyboard::S:
			return 83;
			break;
		case sf::Keyboard::T:
			return 84;
			break;
		case sf::Keyboard::U:
			return 85;
			break;
		case sf::Keyboard::V:
			return 86;
			break;
		case sf::Keyboard::W:
			return 87;
			break;
		case sf::Keyboard::X:
			return 88;
			break;
		case sf::Keyboard::Y:
			return 89;
			break;
		case sf::Keyboard::Z:
			return 90;
			break;

		default:
			return 0;
		}
	}

	Window::Window( Instance* instance, const std::string& name )
	{
		inst = instance;
		fullscreen = false;
		sizing = false;
		active = true;
		lockmouse = false;
		On_KeyDown = NULL;
		On_KeyUp = NULL;
		On_MouseMove = NULL;
		On_MouseDown = NULL;
		On_MouseUp = NULL;
		sf::ContextSettings settings;
		settings.depthBits = 24;
		settings.stencilBits = 8;
		settings.antialiasingLevel = 4;
		settings.majorVersion = 3;
		settings.minorVersion = 1;
		hWnd = new sf::Window(sf::VideoMode(640, 480, 32), name.c_str(), sf::Style::Default, settings);
		instance->Windows.push_back(this);
	};

	void Window::LockMouse( bool state )
	{
		if( state )
		{
			lockmouse = true;
			Ovgl::Rect WindowRect;
			WindowRect.left = hWnd->getPosition().x;
			WindowRect.top = hWnd->getPosition().y;
			WindowRect.right = hWnd->getPosition().x + hWnd->getSize().x;
			WindowRect.bottom = hWnd->getPosition().y + hWnd->getSize().y;
			WindowRect.left = WindowRect.left + GetSystemMetrics(SM_CXSIZEFRAME);
			WindowRect.right = WindowRect.right - GetSystemMetrics(SM_CXSIZEFRAME);
			WindowRect.top = WindowRect.top + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);
			WindowRect.bottom = WindowRect.bottom - GetSystemMetrics(SM_CYSIZEFRAME);
			hWnd->setMouseCursorVisible( false );
		}
		else
		{
			lockmouse = false;
			hWnd->setMouseCursorVisible( true);
		}
	}

	void Window::DoEvents()
	{
		sf::Event Event;
		hWnd->pollEvent(Event);
		switch (Event.type)
		{
			case sf::Event::KeyPressed:
				if(On_KeyDown)
				{
					On_KeyDown( SFKeyToASCII(Event.key.code) );
				}
				break;

			case sf::Event::KeyReleased:
				if(On_KeyUp)
				{
					On_KeyUp( SFKeyToASCII(Event.key.code) );
				}
				break;
			case sf::Event::MouseMoved:
				if(On_MouseMove)
				{
					if(!lockmouse)
					{
						On_MouseMove( Event.mouseMove.x, Event.mouseMove.y );
					}
				}
				break;
			case sf::Event::MouseButtonPressed:
				if(On_MouseDown)
				{
					On_MouseDown( Event.mouseButton.x, Event.mouseButton.y, Event.mouseButton.button );
				}
				break;
			case sf::Event::MouseButtonReleased:
				if(On_MouseUp)
				{
					On_MouseUp( Event.mouseButton.x, Event.mouseButton.y, Event.mouseButton.button );
				}
				break;
			case sf::Event::Resized:
				for(uint32_t i = 0; i < RenderTargets.size(); i++)
				{
					RenderTargets[i]->Update();
				}
				while(hWnd->pollEvent(Event)){}
				break;

			case sf::Event::GainedFocus:
				active = true;
				break;

			case sf::Event::LostFocus:
				active = false;
				break;

			default:
				break;
		}

		if(lockmouse)
		{
			Ovgl::Rect WindowRect;
			WindowRect.left = hWnd->getPosition().x;
			WindowRect.top = hWnd->getPosition().y;
			WindowRect.right = hWnd->getPosition().x + hWnd->getSize().x;
			WindowRect.bottom = hWnd->getPosition().y + hWnd->getSize().y;
			long cx = WindowRect.left + ((WindowRect.right - WindowRect.left) / 2);
			long cy = WindowRect.top + ((WindowRect.bottom - WindowRect.top) / 2);
			sf::Vector2i Point = sf::Mouse::getPosition();
			On_MouseMove( Point.x - cx, Point.y - cy );
			sf::Mouse::setPosition( sf::Vector2i(cx, cy) );
		}

		hWnd->display();
		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glClear( GL_COLOR_BUFFER_BIT );
	}

	void Window::SetFullscreen( bool state )
	{
		if(state)
		{

			fullscreen = true;
		}
		else
		{

			fullscreen = false;
		}
	}

	void Window::SetVSync( bool state )
	{
		vsync = state;
		hWnd->setVerticalSyncEnabled(state);
	}

	Window::~Window()
	{
		hWnd->close();
		delete hWnd;
	}
}
