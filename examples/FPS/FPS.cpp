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
			if( wParam == 0x57 && (lParam >> 30 & 1) == 0 )
				Actor->trajectory = Actor->trajectory + Ovgl::Vector3Set(0.0f, 0.0f, 0.1f);
			if( wParam == 0x53 && (lParam >> 30 & 1) == 0 )
				Actor->trajectory = Actor->trajectory + Ovgl::Vector3Set(0.0f, 0.0f, -0.1f);
			if( wParam == 0x44 && (lParam >> 30 & 1) == 0 )
				Actor->trajectory = Actor->trajectory + Ovgl::Vector3Set(0.1f, 0.0f, 0.0f);
			if( wParam == 0x41 && (lParam >> 30 & 1) == 0 )
				Actor->trajectory = Actor->trajectory + Ovgl::Vector3Set(-0.1f, 0.0f, 0.0f);
			if( wParam == VK_SPACE && (lParam >> 30 & 1) == 0 )
				Actor->controller->jump();
			if( wParam == VK_CONTROL && (lParam >> 30 & 1) == 0 )
				Actor->crouch = true;
            break;
		}

		case WM_KEYUP:
		{
			if( wParam == 0x57 )
				Actor->trajectory = Actor->trajectory - Ovgl::Vector3Set(0.0f, 0.0f, 0.1f);
			if( wParam == 0x53 )
				Actor->trajectory = Actor->trajectory - Ovgl::Vector3Set(0.0f, 0.0f, -0.1f);
			if( wParam == 0x44 )
				Actor->trajectory = Actor->trajectory - Ovgl::Vector3Set(0.1f, 0.0f, 0.0f);
			if( wParam == 0x41 )
				Actor->trajectory = Actor->trajectory - Ovgl::Vector3Set(-0.1f, 0.0f, 0.0f);
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
	hWnd = CreateWindow( L"DefaultWindowClass", L"test", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, DesktopRect.right, DesktopRect.bottom, NULL, NULL, wcex.hInstance, NULL );
	ShowWindow( hWnd, SW_SHOW );
	Inst = Ovgl::Create( 0 );
	RenderTarget = Inst->CreateRenderTarget(hWnd, NULL, 0);

	DEVMODE dmScreenSettings = {0};
	dmScreenSettings.dmSize			= sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth	= DesktopRect.right;
	dmScreenSettings.dmPelsHeight	= DesktopRect.bottom;
	dmScreenSettings.dmBitsPerPel	= 32;
	dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN );

	MediaLibrary = Inst->CreateMediaLibrary("");
	Music = MediaLibrary->ImportOGG( "..\\media\\audio\\Fireproof Babies - Swim below as Leviathans.ogg" );
	FootStep = MediaLibrary->ImportOGG( "..\\media\\audio\\foot_step.ogg" );
	Texture1 = MediaLibrary->ImportCubeMap( "..\\media\\textures\\skybox\\posx.png", "..\\media\\textures\\skybox\\negx.png", "..\\media\\textures\\skybox\\posy.png",
											"..\\media\\textures\\skybox\\negy.png", "..\\media\\textures\\skybox\\posz.png", "..\\media\\textures\\skybox\\negz.png");
	Texture2 = MediaLibrary->ImportTexture( "..\\media\\textures\\white marble.png" );
	Texture3 = MediaLibrary->ImportTexture( "..\\media\\textures\\test.jpg" );
	PavilionMesh = MediaLibrary->ImportFBX( "..\\media\\meshes\\PlaneTest.fbx", "Plane" );
	ChairMesh = MediaLibrary->ImportFBX( "..\\media\\meshes\\Woman.fbx", "Woman" );
	Material1 = MediaLibrary->CreateMaterial();
	Material1->set_texture("txDiffuse", Texture2);
	std::vector<float> EMI(1);
	std::vector<float> Diffuse(4);
	EMI[0] = 0.1f;
	Diffuse[0] = 1.0f;
	Diffuse[1] = 1.0f;
	Diffuse[2] = 1.0f;
	Diffuse[3] = 1.0f;
	Material1->set_variable( "Diffuse", Diffuse );
	Material1->set_variable( "EMI", EMI );

	Material2 = MediaLibrary->CreateMaterial();
	Material2->set_texture("txDiffuse", NULL);
	EMI[0] = 1.0f;
	Diffuse[0] = 1.0f;
	Diffuse[1] = 1.0f;
	Diffuse[2] = 1.0f;
	Diffuse[3] = 0.15f;
	Material2->set_variable( "Diffuse", Diffuse );
	Material2->set_variable( "EMI", EMI );
	Material2->NoZWrite = true;
	Material2->PostRender = true;

	Material3 = MediaLibrary->CreateMaterial();
	Material3->set_texture("txDiffuse", Texture3);
	EMI[0] = 0.1f;
	Diffuse[0] = 1.0f;
	Diffuse[1] = 1.0f;
	Diffuse[2] = 1.0f;
	Diffuse[3] = 1.0f;
	Material3->set_variable( "Diffuse", Diffuse );
	Material3->set_variable( "EMI", EMI );

	Material4 = MediaLibrary->CreateMaterial();
	Material4->set_texture("txDiffuse", Texture2);

	Material5 = MediaLibrary->CreateMaterial();
	EMI[0] = 0.1f;
	Diffuse[0] = 1.0f;
	Diffuse[1] = 1.0f;
	Diffuse[2] = 1.0f;
	Diffuse[3] = 1.0f;
	Material5->set_variable( "Diffuse", Diffuse );
	Material5->set_variable( "EMI", EMI );

	Scene = MediaLibrary->CreateScene();
	Scene->skybox = Texture1;
	Pavilion = Scene->CreateObject(PavilionMesh, &Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ));
	Pavilion->subsets[0] = Material1;
	Pavilion->subsets[1] = Material2;
	Pavilion->subsets[2] = Material3;
	Pavilion->subsets[3] = Material4;
	Pavilion->subsets[4] = Material5;
	Chair = Scene->CreateProp(ChairMesh, &Ovgl::MatrixTranslation( 0.0f, 15.0f, -5.0f ));
	Light = Scene->CreateLight(&Ovgl::MatrixTranslation( 0.0f, 10.0f, 0.0f ), &Ovgl::Vector4Set( 1.0f, 1.0f, 1.0f, 1.0f ));
	Actor = Scene->CreateActor( NULL, 0.25f, 0.75f, &Ovgl::MatrixTranslation( 0.0f, 15.0f, 0.0f ) );
//	Camera = Scene->CreateCamera(&Ovgl::MatrixTranslation( 0.0f, 3.0f, 0.0f ));
	RenderTarget->view = Actor->camera;
	Emitter = Scene->CreateEmitter( &Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ) );
	Music->CreateAudioInstance( Emitter, false );

	Ovgl::Emitter* Emitter2;
	Emitter2 = Scene->CreateEmitter( &Ovgl::MatrixTranslation( 0.0f, 20.0f, 0.0f ) );
	Music->CreateAudioInstance( Emitter2, true );
	RenderTarget->CreateText( "Hello World!", &Ovgl::Vector4Set( 50.0f, 0.0f, 250.0f, 100.0f ) );

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