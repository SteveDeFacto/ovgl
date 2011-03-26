/**
* @file Editor.cpp
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
Ovgl::Camera*				Camera;
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
					g_Active = true;
				}
				else
				{
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
	RenderTarget->CreateText("..\\media\\textures\\Grass.dds", &Ovgl::Vector4Set( 0.0f, 0.0f, 512.0f, 512.0f ));
	Music = Inst->CreateAudioBuffer( "..\\media\\audio\\glacier.ogg" );
	FootStep = Inst->CreateAudioBuffer( "..\\media\\audio\\foot_step.ogg" );
	Scene = Inst->CreateScene( "..\\media\\meshes\\HL2.bin", &Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ), NULL );
	Camera = Scene->CreateCamera(&Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ));
	RenderTarget->view = Camera;
	Music->CreateAudioInstance( NULL );
	Inst->SkyboxEffect->set_texture( "txEnvironment", "..\\media\\textures\\Skybox.dds");
	Inst->DefaultEffect->set_texture( "txEnvironment", "..\\media\\textures\\Skybox.dds");
	Inst->DefaultEffect->set_texture( "txDiffuse", "..\\media\\textures\\Grass.dds");
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
			static POINT LastPoint;
			if( g_Active )
			{
				if ((GetKeyState(VK_RBUTTON) & 0x80) != 0)
				{
					POINT Point;
					GetCursorPos( &Point );
					long mx = Point.x - LastPoint.x;
					long my = Point.y - LastPoint.y;
					Camera->setPose( &((Ovgl::MatrixRotationY(-mx / 1000.0f ) * Ovgl::MatrixRotationX( -my / 1000.0f)) * Camera->getPose() ) );
					if((GetKeyState(0x57) & 0x80) != 0)
						Camera->setPose( &(Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.1f ) * Camera->getPose() ) );
					if((GetKeyState(0x53) & 0x80) != 0)
						Camera->setPose( &(Ovgl::MatrixTranslation( 0.0f, 0.0f, -0.1f ) * Camera->getPose() ) );
					if((GetKeyState(0x44) & 0x80) != 0)
						Camera->setPose( &(Ovgl::MatrixTranslation( 0.1f, 0.0f, 0.0f ) * Camera->getPose() ) );
					if((GetKeyState(0x41) & 0x80) != 0)
						Camera->setPose( &(Ovgl::MatrixTranslation( -0.1f, 0.0f, 0.0f ) * Camera->getPose() ) );
					SetCursorPos( LastPoint.x, LastPoint.y );
				}
				else
				{
					GetCursorPos( &LastPoint );
				}
			}

			previousTime = currentTime;
        }
    }
	Inst->Release();
	UnregisterClass( L"DefaultWindowClass", NULL );
	return 0;
}