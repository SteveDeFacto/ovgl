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
* @brief None.
*/

#include "OvglIncludes.h"
#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglGraphics.h"
#include "OvglWindow.h"

LRESULT CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	Ovgl::Window* Window = (Ovgl::Window*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
	if(Window != NULL)
	{
		switch( message )
		{
			case WM_KEYDOWN:
			{
				if(Window->On_KeyDown)
				{
					Window->On_KeyDown( (char)wParam);
				}
		        break;
			}

			case WM_KEYUP:
			{
				if(Window->On_KeyUp)
				{
					Window->On_KeyUp( (char)wParam);
				}
		        break;
			}

			case WM_MOUSEMOVE:
			{
				if(Window->On_MouseMove)
				{
					if(Window->lockmouse)
					{
						RECT WindowRect;
						GetWindowRect( Window->hWnd, &WindowRect );
						long cx = WindowRect.left + ((WindowRect.right - WindowRect.left) / 2);
						long cy = WindowRect.top + ((WindowRect.bottom - WindowRect.top) / 2);
						POINT Point;
						GetCursorPos( &Point );
						Window->On_MouseMove( Point.x - cx, Point.y - cy );
						SetCursorPos(  cx, cy );
						PeekMessage( NULL, Window->hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE );
					}
					else
					{
						Window->On_MouseMove( LOWORD(lParam), HIWORD(lParam) );
					}
				}
		        break;
			}

			case WM_LBUTTONUP:
			{
				if(Window->On_MouseUp)
				{
					Window->On_MouseUp( LOWORD(lParam), HIWORD(lParam), 0 );
				}
		        break;
			}

			case WM_RBUTTONUP:
			{
				if(Window->On_MouseUp)
				{
					Window->On_MouseUp( LOWORD(lParam), HIWORD(lParam), 1 );
				}
		        break;
			}

			case WM_MBUTTONUP:
			{
				if(Window->On_MouseUp)
				{
					Window->On_MouseUp( LOWORD(lParam), HIWORD(lParam), 2 );
				}
		        break;
			}

			case WM_LBUTTONDOWN:
			{
				if(Window->On_MouseDown)
				{
					Window->On_MouseUp( LOWORD(lParam), HIWORD(lParam), 0 );
				}
		        break;
			}

			case WM_RBUTTONDOWN:
			{
				if(Window->On_MouseDown)
				{
					Window->On_MouseDown( LOWORD(lParam), HIWORD(lParam), 1 );
				}
		        break;
			}

			case WM_MBUTTONDOWN:
			{
				if(Window->On_MouseDown)
				{
					Window->On_MouseDown( LOWORD(lParam), HIWORD(lParam), 2 );
				}
		        break;
			}

			case WM_ENTERSIZEMOVE:
			{
				Window->sizing = true;
				break;
			}

			case WM_EXITSIZEMOVE:
			{
				for(unsigned int i = 0; i < Window->RenderTargets.size(); i++)
				{
					Window->RenderTargets[i]->Update();
				}
				Window->sizing = false;
				break;
			}

		    case WM_ACTIVATEAPP:
			{
				if( !Window->sizing )
				{
					if( wParam )
					{
						Window->active = true;
					}
					else
					{
						Window->active = false;
					}
				}
				break;
			}

			case WM_ACTIVATE:
			{
				if( Window->fullscreen )
				{
					if( LOWORD(wParam) == WA_INACTIVE )
					{
						ShowWindow( hWnd, SW_RESTORE );
						ShowWindow( hWnd, SW_MINIMIZE );
						ChangeDisplaySettings( NULL, 0 );
						SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
					}
					else if( LOWORD(wParam) == WA_ACTIVE )
					{
						WINDOWPLACEMENT wp;
						GetWindowPlacement(hWnd, &wp);
						DEVMODE dmScreenSettings				= {0};
						dmScreenSettings.dmSize					= sizeof(dmScreenSettings);
						dmScreenSettings.dmPelsWidth			= wp.rcNormalPosition.right - wp.rcNormalPosition.left;
						dmScreenSettings.dmPelsHeight			= wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
						dmScreenSettings.dmFields				= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
						dmScreenSettings.dmBitsPerPel			= 32;
						SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
						ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN );
						ShowWindow( hWnd, SW_SHOWMAXIMIZED );
						SetFocus(hWnd);
					}
				}
		        break;
			}

	        case WM_DESTROY:
	        {
	            PostQuitMessage( 0 );
	            break;
	        }
		}
	}

    return DefWindowProc( hWnd, message, wParam, lParam );
}

Ovgl::Window::Window( Ovgl::Instance* instance, const std::string& name )
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
	LastPoint.x = 0;
	LastPoint.y = 0;
	WNDCLASSEX wcex;
	ZeroMemory( &wcex, sizeof( wcex ) );
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.lpfnWndProc = WinProc;
	wcex.hInstance = GetModuleHandle( NULL );
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.cbWndExtra = 4;
	wcex.lpszClassName = L"DefaultWindowClass";
	RegisterClassEx( &wcex );
	RECT DesktopRect;
	GetWindowRect(GetDesktopWindow(), &DesktopRect);
	hWnd = CreateWindowA( "DefaultWindowClass", name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, wcex.hInstance, NULL );
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)this );
	ShowWindow( hWnd, SW_SHOWNORMAL );
	SetForegroundWindow(hWnd);
	inst = instance;
	hDC = GetDC(hWnd);
	int pixelFormat;
	UINT numFormats;
	float fAttributes[] = {0,0};
	int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,24,
		WGL_ALPHA_BITS_ARB,0,
		WGL_DEPTH_BITS_ARB,0,
		WGL_STENCIL_BITS_ARB,0,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		0, 0};
	wglChoosePixelFormatARB( hDC, iAttributes, fAttributes,1, &pixelFormat, &numFormats);
	SetPixelFormat( hDC, pixelFormat, NULL );
	wglMakeCurrent( hDC, instance->hRC );
	SwapBuffers( hDC );
};

void Ovgl::Window::LockMouse( bool state )
{
	if( state )
	{
		lockmouse = true;
		RECT WindowRect;
		GetWindowRect( hWnd, &WindowRect );
		WindowRect.left = WindowRect.left + GetSystemMetrics(SM_CXSIZEFRAME);
		WindowRect.right = WindowRect.right - GetSystemMetrics(SM_CXSIZEFRAME);
		WindowRect.top = WindowRect.top + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);
		WindowRect.bottom = WindowRect.bottom - GetSystemMetrics(SM_CYSIZEFRAME);
		ClipCursor( &WindowRect );
		ShowCursor( FALSE );
	}
	else
	{
		lockmouse = false;
		ClipCursor( NULL );
		ShowCursor( TRUE );
	}
}

void Ovgl::Window::DoEvents()
{
	MSG msg = {0};
	if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	wglMakeCurrent( hDC, inst->hRC );
	SwapBuffers( hDC );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Ovgl::Window::SetFullscreen( bool state )
{
	if(state)
	{
		WINDOWPLACEMENT wp;
		GetWindowPlacement(hWnd, &wp);
		DEVMODE dmScreenSettings				= {0};
		dmScreenSettings.dmSize					= sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth			= wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dmScreenSettings.dmPelsHeight			= wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		dmScreenSettings.dmFields				= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		dmScreenSettings.dmBitsPerPel			= 32;
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
		ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN );
		ShowWindow( hWnd, SW_SHOWMAXIMIZED );
		SetFocus(hWnd);
		fullscreen = true;
	}
	else
	{
		SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		ChangeDisplaySettings( NULL, 0 );
		ShowWindow( hWnd, SW_SHOW );
		fullscreen = false;
	}
}