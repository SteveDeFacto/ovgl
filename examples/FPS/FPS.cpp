/**
* @file FPS.cpp
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

#include <windows.h>
#include <mmsystem.h>
#include <winnt.h>
#include <Ovgl.h>

HWND						hWnd;
Ovgl::Instance*				Inst;
Ovgl::RenderTarget*			RenderTarget;
Ovgl::AudioBuffer*			Music;
Ovgl::AudioBuffer*			FootStep;
Ovgl::Scene*				Scene;
Ovgl::Actor*				Actor;
Ovgl::Emitter*				Emitter;
bool						g_Active;
bool						g_Sizing;

LRESULT CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch( message )
    {
        case WM_KEYDOWN:
		{
            if( wParam == VK_ESCAPE )
                PostQuitMessage( 0 );
			if( wParam == 0x57 && (lParam >> 30 & 1) == 0 )
			{
				Actor->trajectory = Actor->trajectory + Ovgl::Vector3Set( 0.0f, 0.0f, 1.0f );
				FootStep->CreateAudioInstance(NULL);
			}
			if( wParam == 0x53 && (lParam >> 30 & 1) == 0 )
				Actor->trajectory = Actor->trajectory + Ovgl::Vector3Set( 0.0f, 0.0f, -1.0f );
			if( wParam == 0x44 && (lParam >> 30 & 1) == 0 )
				Actor->trajectory = Actor->trajectory + Ovgl::Vector3Set( 1.0f, 0.0f, 0.0f );
			if( wParam == 0x41 && (lParam >> 30 & 1) == 0 )
				Actor->trajectory = Actor->trajectory + Ovgl::Vector3Set( -1.0f, 0.0f, 0.0f );
			if( wParam == VK_SPACE && (lParam >> 30 & 1) == 0 )
				Actor->Jump( 1.0f );
			if( wParam == VK_CONTROL && (lParam >> 30 & 1) == 0 )
				Actor->crouch = true;
            break;
		}

		case WM_KEYUP:
		{
			if( wParam == 0x57 )
				Actor->trajectory = Actor->trajectory - Ovgl::Vector3Set( 0.0f, 0.0f, 1.0f );
			if( wParam == 0x53 )
				Actor->trajectory = Actor->trajectory - Ovgl::Vector3Set( 0.0f, 0.0f, -1.0f );
			if( wParam == 0x44 )
				Actor->trajectory = Actor->trajectory - Ovgl::Vector3Set( 1.0f, 0.0f, 0.0f );
			if( wParam == 0x41 )
				Actor->trajectory = Actor->trajectory - Ovgl::Vector3Set( -1.0f, 0.0f, 0.0f );
			if( wParam == VK_CONTROL )
				Actor->crouch = false;
            break;
		}

		case WM_ENTERSIZEMOVE:
		{
			g_Sizing = true;
			break;
		}

		case WM_EXITSIZEMOVE:
		{
			g_Sizing = false;
			break;
		}

        case WM_ACTIVATEAPP:
		{
			if( !g_Sizing )
			{
				if( wParam )
				{
					RECT WindowRect;
					GetWindowRect( hWnd, &WindowRect );
					ClipCursor( &WindowRect );
					ShowCursor( false );
					g_Active = true;
				}
				else
				{
					ClipCursor( NULL );
					ShowCursor( TRUE );
					g_Active = false;
				}
			}
			break;
		}

        case WM_ACTIVATE:
		{
			if( RenderTarget->GetFullscreen() )
			{
				if( LOWORD(wParam) == WA_INACTIVE )
				{
					ShowWindow( hWnd, SW_MINIMIZE );
					RenderTarget->SetFullscreen( false);
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

    return DefWindowProc( hWnd, message, wParam, lParam );
}

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    WNDCLASSEX wcex;
	ZeroMemory( &wcex, sizeof( wcex ) );
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.lpfnWndProc = WinProc;
	wcex.hInstance = GetModuleHandle( NULL );
	wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.lpszClassName = L"DefaultWindowClass";
    RegisterClassEx( &wcex );
	hWnd = NULL;
	hWnd = CreateWindow( L"DefaultWindowClass", L"test", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, wcex.hInstance, NULL );
	ShowWindow( hWnd, SW_SHOW );
	Inst = Ovgl::Create( 0 );
	RenderTarget = Inst->CreateRenderTarget(hWnd, NULL, 0);
	RenderTarget->CreateText("..\\..\\media\\textures\\Grass.dds", &Ovgl::Vector4Set( 0.0f, 0.0f, 512.0f, 512.0f ));
	Music = Inst->CreateAudioBuffer( "..\\..\\media\\audio\\glacier.ogg" );
	FootStep = Inst->CreateAudioBuffer( "..\\..\\media\\audio\\foot_step.ogg" );
	Scene = Inst->CreateScene( "..\\..\\media\\meshes\\HL2.bin", &Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ), NULL );
	Actor = Scene->CreateActor( NULL, 0.25f, 0.75f, &Ovgl::MatrixTranslation( -75.0f, 5.0f, 0.0f ) );
	RenderTarget->view = Actor->camera;
	Music->CreateAudioInstance( NULL );
	Inst->SkyboxEffect->set_texture( "txEnvironment", "..\\..\\media\\textures\\Skybox.dds");
	Inst->DefaultEffect->set_texture( "txEnvironment", "..\\..\\media\\textures\\Skybox.dds");
	Inst->DefaultEffect->set_texture( "txDiffuse", "..\\..\\media\\textures\\Grass.dds");
	float data[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
	Inst->DefaultEffect->set_variable( "Ambient", 4, data); 
	DWORD previousTime = timeGetTime();
	// Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            DWORD currentTime = timeGetTime();
			DWORD elapsedTime = currentTime - previousTime;
			Scene->Update(elapsedTime);
			RenderTarget->Render();
			if( g_Active )
			{
				RECT WindowRect;
				GetWindowRect( hWnd, &WindowRect );
				POINT Point;
				GetCursorPos( &Point );
				long mx = Point.x - ((WindowRect.left + WindowRect.right) / 2);
				long my = Point.y - ((WindowRect.top +  WindowRect.bottom) / 2);
				Actor->direction.z = Actor->direction.z + (mx / 1000.0f);
				Actor->direction.y = Actor->direction.y + (my / 1000.0f);
				if(Actor->direction.y < Ovgl::DegToRad(-90.0f))
				{
					Actor->direction.y = Ovgl::DegToRad(-90.0f);
				}
				else if(Actor->direction.y > Ovgl::DegToRad(90.0f))
				{
					Actor->direction.y = Ovgl::DegToRad(90.0f);
				}
				SetCursorPos( (WindowRect.left + WindowRect.right) / 2, (WindowRect.top +  WindowRect.bottom) / 2 );
			}
			previousTime = currentTime;
        }
    }
	Inst->Release();
	UnregisterClass( L"DefaultWindowClass", NULL );
	return 0;
}