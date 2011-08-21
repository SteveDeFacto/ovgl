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
Ovgl::MediaLibrary*			MediaLibrary;
Ovgl::Scene*				Scene;
Ovgl::AudioBuffer*			Music;
Ovgl::AudioBuffer*			FootStep;
Ovgl::Mesh*					PavilionMesh;
Ovgl::Mesh*					ChairMesh;
Ovgl::Object*				Pavilion;
Ovgl::Prop*					Chair;
Ovgl::Actor*				Actor;
Ovgl::Light*				Light;
Ovgl::Camera*				Camera;
Ovgl::Emitter*				Emitter;
Ovgl::Texture*				Texture1;
Ovgl::Texture*				Texture2;
Ovgl::Texture*				Texture3;
Ovgl::Material*				Material1;
Ovgl::Material*				Material2;
Ovgl::Material*				Material3;
Ovgl::Material*				Material4;
Ovgl::Material*				Material5;
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
			if( wParam == VK_SPACE && (lParam >> 30 & 1) == 0 )
				Actor->Jump(7.5f);
			if( wParam == VK_CONTROL && (lParam >> 30 & 1) == 0 )
				Actor->crouch = true;
            break;
		}

		case WM_KEYUP:
		{
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
	RECT DesktopRect;
	GetWindowRect(GetDesktopWindow(), &DesktopRect);
	hWnd = CreateWindow( L"DefaultWindowClass", L"test",  WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, DesktopRect.right, DesktopRect.bottom, NULL, NULL, wcex.hInstance, NULL );
	ShowWindow( hWnd, SW_SHOW );
	Inst = Ovgl::Create( 0 );
	RenderTarget = Inst->CreateRenderTarget(hWnd, NULL, 0);

	RenderTarget->bloom = 4;
	RenderTarget->motionBlur = true;
	RenderTarget->multiSample = true;
	RenderTarget->SetFullscreen( true );
	RenderTarget->SetVSync( true );

	MediaLibrary = Inst->CreateMediaLibrary("");
	Music = MediaLibrary->ImportOGG( "..\\media\\audio\\Fireproof Babies - Swim below as Leviathans.ogg" );
	FootStep = MediaLibrary->ImportOGG( "..\\media\\audio\\foot_step.ogg" );
	Texture1 = MediaLibrary->ImportCubeMap( "..\\media\\textures\\NightSky\\front.png", "..\\media\\textures\\NightSky\\back.png", "..\\media\\textures\\NightSky\\top.png",
											"..\\media\\textures\\NightSky\\bottom.png", "..\\media\\textures\\NightSky\\left.png", "..\\media\\textures\\NightSky\\right.png");
	Texture2 = MediaLibrary->ImportTexture( "..\\media\\textures\\white marble.png" );
	Texture3 = MediaLibrary->ImportTexture( "..\\media\\textures\\test.jpg" );
	PavilionMesh = MediaLibrary->ImportFBX( "..\\media\\meshes\\test.fbx" );
	//ChairMesh = MediaLibrary->ImportFBX( "..\\media\\meshes\\teapot.fbx" );


	Scene = MediaLibrary->CreateScene();
	Scene->skybox = Texture1;
	Pavilion = Scene->CreateObject(PavilionMesh, &Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ));
	//Chair = Scene->CreateProp(ChairMesh, &(Ovgl::MatrixTranslation( 1.0f, 1.0f, 0.0f ) * Ovgl::MatrixRotationX(1.57f)));
	Light = Scene->CreateLight(&Ovgl::MatrixTranslation( 0.5f, 2.2f, 0.0f ), &Ovgl::Vector4Set( 10.0f, 10.0f, 10.0f, 10.0f ));
	Scene->CreateLight(&Ovgl::MatrixTranslation( 6.5f, 2.2f, 0.0f ), &Ovgl::Vector4Set( 10.0f, 10.0f, 10.0f, 10.0f ));
	Scene->CreateLight(&Ovgl::MatrixTranslation( 1.0f, 2.2f, -7.0f ), &Ovgl::Vector4Set( 10.0f, 10.0f, 10.0f, 10.0f ));
	Scene->CreateLight(&Ovgl::MatrixTranslation( 12.3f, 1.2f, 1.5f ), &Ovgl::Vector4Set( 10.0f, 10.0f, 10.0f, 10.0f ));
	Scene->CreateLight(&Ovgl::MatrixTranslation( 14.3f, 1.2f, 5.0f ), &Ovgl::Vector4Set( 10.0f, 10.0f, 10.0f, 10.0f ));
	Actor = Scene->CreateActor( NULL, 0.2f, 0.5f, &Ovgl::MatrixTranslation( 0.0f, 2.5f, 0.0f ) );
//	Camera = Scene->CreateCamera(&Ovgl::MatrixTranslation( 0.0f, 3.0f, 0.0f ));
	RenderTarget->view = Actor->camera;
	Emitter = Scene->CreateEmitter( &Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ) );
	Music->CreateAudioInstance( NULL, true );

	RenderTarget->CreateText( "Hello World!", &Ovgl::Vector4Set( 50.0f, 100.0f, 250.0f, 200.0f ) );
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
				Actor->lookDirection.z = Actor->lookDirection.z - (mx / 1000.0f);
				Actor->lookDirection.y = Actor->lookDirection.y + (my / 1000.0f);
				if(Actor->lookDirection.y < Ovgl::DegToRad(-90.0f))
				{
					Actor->lookDirection.y = Ovgl::DegToRad(-90.0f);
				}
				else if(Actor->lookDirection.y > Ovgl::DegToRad(90.0f))
				{
					Actor->lookDirection.y = Ovgl::DegToRad(90.0f);
				}
				SetCursorPos( (WindowRect.left + WindowRect.right) / 2, (WindowRect.top +  WindowRect.bottom) / 2 );
				Actor->walkDirection = Ovgl::Vector3Set(0.0f, 0.0f, 0.0f);
				if( GetKeyState(0x57) & WM_KEYDOWN )
					Actor->walkDirection = Actor->walkDirection + Ovgl::Vector3Set(0.0f, 0.0f, 1.0f);
				if( GetKeyState(0x53) & WM_KEYDOWN )
					Actor->walkDirection = Actor->walkDirection + Ovgl::Vector3Set(0.0f, 0.0f, -1.0f);
				if( GetKeyState(0x44) & WM_KEYDOWN )
					Actor->walkDirection = Actor->walkDirection + Ovgl::Vector3Set(-1.0f, 0.0f, 0.0f);
				if( GetKeyState(0x41) & WM_KEYDOWN )
					Actor->walkDirection = Actor->walkDirection + Ovgl::Vector3Set(1.0f, 0.0f, 0.0f);
				if( Actor->walkDirection != Ovgl::Vector3Set(0.0f, 0.0f, 0.0f) )
					Actor->walkDirection = Ovgl::Vector3Normalize(&Actor->walkDirection) * 0.05f;
			}
			previousTime = currentTime;
        }
    }
	Inst->Release();
	UnregisterClass( L"DefaultWindowClass", NULL );
	return 0;
}