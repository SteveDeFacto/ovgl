/**
* @file HelloWorld.cpp
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

Ovgl::Instance*				Inst;
Ovgl::RenderTarget*			RenderTarget;
Ovgl::MediaLibrary*			MediaLibrary;
Ovgl::AudioBuffer*			Music;
Ovgl::AudioBuffer*			FootStep;
Ovgl::Scene*				Scene;
Ovgl::Actor*				Actor;
Ovgl::Camera*				Camera;
Ovgl::Light*				Light;
Ovgl::Emitter*				Emitter;
Ovgl::Texture*				Texture1;
Ovgl::Texture*				Texture2;
Ovgl::Mesh*					PedestalMesh;
Ovgl::Mesh*					BlockMesh;
Ovgl::Object*				Pedestal;
bool						g_Active;
bool						g_Sizing;
float mx, my;

void CreateIShape( Ovgl::Matrix44* offset )
{
	Ovgl::Prop* Block1;
	Ovgl::Prop* Block2;
	Ovgl::Prop* Block3;
	Ovgl::Prop* Block4;
	Block1 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, -0.3f, 0.0f ) * *offset));
	Block1->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block2 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, -0.1f, 0.0f ) * *offset));
	Block2->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block3 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, 0.1f, 0.0f ) * *offset));
	Block3->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block4 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, 0.3f, 0.0f ) * *offset));
	Block4->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Scene->CreateJoint( Block1->bones[0], Block2->bones[0], NULL );
	Scene->CreateJoint( Block2->bones[0], Block3->bones[0], NULL );
	Scene->CreateJoint( Block3->bones[0], Block4->bones[0], NULL );
}

void CreateLShape( Ovgl::Matrix44* offset )
{
	Ovgl::Prop* Block1;
	Ovgl::Prop* Block2;
	Ovgl::Prop* Block3;
	Ovgl::Prop* Block4;
	Block1 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, 0.2f, 0.0f ) * *offset));
	Block1->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block2 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ) * *offset));
	Block2->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block3 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, -0.2f, 0.0f ) * *offset));
	Block3->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block4 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.2f, -0.2f, 0.0f ) * *offset));
	Block4->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Scene->CreateJoint( Block1->bones[0], Block2->bones[0], NULL );
	Scene->CreateJoint( Block2->bones[0], Block3->bones[0], NULL );
	Scene->CreateJoint( Block3->bones[0], Block4->bones[0], NULL );
}

void CreateSShape( Ovgl::Matrix44* offset )
{
	Ovgl::Prop* Block1;
	Ovgl::Prop* Block2;
	Ovgl::Prop* Block3;
	Ovgl::Prop* Block4;
	Block1 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.2f, 0.1f, 0.0f ) * *offset));
	Block1->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block2 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, 0.1f, 0.0f ) * *offset));
	Block2->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block3 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, -0.1f, 0.0f ) * *offset));
	Block3->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block4 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( -0.2f, -0.1f, 0.0f ) * *offset));
	Block4->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Scene->CreateJoint( Block1->bones[0], Block2->bones[0], NULL );
	Scene->CreateJoint( Block2->bones[0], Block3->bones[0], NULL );
	Scene->CreateJoint( Block3->bones[0], Block4->bones[0], NULL );
}

void CreateOShape( Ovgl::Matrix44* offset )
{
	Ovgl::Prop* Block1;
	Ovgl::Prop* Block2;
	Ovgl::Prop* Block3;
	Ovgl::Prop* Block4;
	Block1 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.1f, 0.1f, 0.0f ) * *offset));
	Block1->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block2 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( -0.1f, 0.1f, 0.0f ) * *offset));
	Block2->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block3 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( -0.1f, -0.1f, 0.0f ) * *offset));
	Block3->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block4 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.1f, -0.1f, 0.0f ) * *offset));
	Block4->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Scene->CreateJoint( Block1->bones[0], Block2->bones[0], NULL );
	Scene->CreateJoint( Block2->bones[0], Block3->bones[0], NULL );
	Scene->CreateJoint( Block3->bones[0], Block4->bones[0], NULL );
}

void CreateTShape( Ovgl::Matrix44* offset )
{
	Ovgl::Prop* Block1;
	Ovgl::Prop* Block2;
	Ovgl::Prop* Block3;
	Ovgl::Prop* Block4;
	Block1 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ) * *offset));
	Block1->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block2 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( -0.2f, 0.0f, 0.0f ) * *offset));
	Block2->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block3 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.2f, 0.0f, 0.0f ) * *offset));
	Block3->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Block4 = Scene->CreateProp(BlockMesh, &(Ovgl::MatrixTranslation( 0.0f, 0.2f, 0.0f ) * *offset));
	Block4->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
	Scene->CreateJoint( Block1->bones[0], Block4->bones[0], NULL );
	Scene->CreateJoint( Block1->bones[0], Block2->bones[0], NULL );
	Scene->CreateJoint( Block1->bones[0], Block3->bones[0], NULL );
}

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
	HWND hWnd = CreateWindow( L"DefaultWindowClass", L"test", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, wcex.hInstance, NULL );
	ShowWindow( hWnd, SW_SHOW );
	Inst = Ovgl::Create( 0 );
	RenderTarget = Inst->CreateRenderTarget(hWnd, NULL, 0);
	MediaLibrary = Inst->CreateMediaLibrary("");
	Music = MediaLibrary->ImportOGG( "..\\media\\audio\\glacier.ogg" );
	FootStep = MediaLibrary->ImportOGG( "..\\media\\audio\\foot_step.ogg" );
	Texture1 = MediaLibrary->ImportCubeMap( "..\\media\\textures\\skybox\\posx.png", "..\\media\\textures\\skybox\\negx.png", "..\\media\\textures\\skybox\\posy.png",
											"..\\media\\textures\\skybox\\negy.png", "..\\media\\textures\\skybox\\posz.png", "..\\media\\textures\\skybox\\negz.png");
	Texture2 = MediaLibrary->ImportTexture( "..\\media\\textures\\Grass.png" );
	PedestalMesh = MediaLibrary->ImportFBX( "..\\media\\meshes\\Barcelona Pavilion.fbx", "Obj_0000955" );
	BlockMesh = MediaLibrary->ImportFBX( "..\\media\\meshes\\block.fbx", "Block" );
	Scene = MediaLibrary->CreateScene();
	Light = Scene->CreateLight(&Ovgl::MatrixTranslation( 0.0f, 1.75f, -4.0f ), &Ovgl::Vector4Set( 1.0f, 1.0f, 1.0f, 1.0f ));
	Camera = Scene->CreateCamera(&Ovgl::MatrixTranslation( 0.0f, 1.75f, -4.0f ));
	Pedestal = Scene->CreateObject(PedestalMesh, &Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ));
	//MediaLibrary->Load("..\\media\\meshes\\scene.bin");
	//Scene = MediaLibrary->Scenes[0];
	//Camera = MediaLibrary->Scenes[0]->cameras[0];
	RenderTarget->view = Camera;
	Music->CreateAudioInstance( NULL );
	Inst->DefaultMedia->Effects[1]->set_texture( "txSkybox", Texture1);

	Inst->DefaultMedia->Effects[0]->set_texture( "txDiffuse", Texture2);
	float data[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
	Inst->DefaultMedia->Effects[0]->set_variable( "Ambient", 4, data);
	DWORD previousTime = timeGetTime();
	DWORD previousTime2 = timeGetTime();

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
			DWORD elapsedTime2 = currentTime - previousTime2;
			Scene->Update(elapsedTime);
			RenderTarget->Render();
			static POINT LastPoint;
			if( g_Active )
			{
				if ((GetKeyState(VK_RBUTTON) & 0x80) != 0)
				{
					POINT Point;
					GetCursorPos( &Point );
					mx = mx + ((LastPoint.x - Point.x) / 100.0f);
					my = my + ((LastPoint.y - Point.y) / 100.0f);
					if(my > 0.45f)
					{
						my = 0.45f;
					}
					if(my < -0.1f)
					{
						my = -0.1f;
					}
					Camera->setPose( &( Ovgl::MatrixTranslation(0.0f, 1.75f, -4.0f) * (Ovgl::MatrixRotationX( my ) * Ovgl::MatrixRotationY( mx )) ) );
					SetCursorPos( LastPoint.x, LastPoint.y );
				}
				else
				{
					GetCursorPos( &LastPoint );
				}
			}
			if(elapsedTime2 > 5000)
			{
				CreateOShape( &Ovgl::MatrixTranslation( 0.0f, 5.0f, 0.0f ) );
				//Scene->CreateProp(BlockMesh, &Ovgl::MatrixTranslation( 0.0f, 5.0f, 0.0f ))->bones[0]->SetFlags( Ovgl::FROZEN_ROT );
				MediaLibrary->Save("..\\media\\meshes\\scene.bin");
				previousTime2 = currentTime;
			}
			previousTime = currentTime;
        }
    }
	Inst->Release();
	UnregisterClass( L"DefaultWindowClass", NULL );
	return 0;
}