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

#include <Ovgl.h>

Ovgl::Instance*				Inst;
Ovgl::RenderTarget*			RenderTarget;
Ovgl::Window*				Window;
Ovgl::MediaLibrary*			MediaLibrary;
Ovgl::Scene*				Scene;
Ovgl::Actor*				Actor;
Ovgl::Actor*				Actor2;
Ovgl::Camera*				Camera;
Ovgl::Emitter*				Emitter;
Ovgl::Texture*				Texture1;
Ovgl::Texture*				Texture2;
Ovgl::Mesh*					Mesh;
Ovgl::Mesh*					Mesh2;
Ovgl::Object*				Object;
Ovgl::Light*				Light;

void MouseMove(long x, long y)
{
	Camera->setPose( ((Ovgl::MatrixRotationY(x / 1000.0f ) * Ovgl::MatrixRotationX( -y / 1000.0f)) * Camera->getPose() ) );
}

void KeyDown(char key)
{
	if(key == (char)27)
	{
		Inst->g_Quit = true;
	}
	if( key == 'W')
		Camera->setPose( (Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.1f ) * Camera->getPose() ) );
	if( key == 'S')
		Camera->setPose( (Ovgl::MatrixTranslation( 0.0f, 0.0f, -0.1f ) * Camera->getPose() ) );
	if( key == 'A')
	{
		Actor2->PlayAnimation( &Mesh2->skeleton->animations[0], 1, 6, false);
		Camera->setPose( (Ovgl::MatrixTranslation( 0.1f, 0.0f, 0.0f ) * Camera->getPose() ) );
	}
	if( key == 'D')
	{
		Actor2->PlayAnimation( &Mesh2->skeleton->animations[0], 5, 6, false);
		Camera->setPose( (Ovgl::MatrixTranslation( -0.1f, 0.0f, 0.0f ) * Camera->getPose() ) );
	}
}

int main()
{
	// Create Main Instance
	Inst = new Ovgl::Instance( 0 );

	// Create Window
	Window = new Ovgl::Window( Inst, "Test");
	Window->LockMouse(true);
	Window->SetFullscreen( false );
	Window->SetVSync( false );
	Window->On_MouseMove = MouseMove;
	Window->On_KeyDown = KeyDown;

	// Create Render Target
    RenderTarget = new Ovgl::RenderTarget(Inst, Window, Ovgl::Vector4(0.0f, 0.0f, 0.999f, 0.999f), 0);
	RenderTarget->bloom = 4;
	RenderTarget->autoLuminance = true;
	RenderTarget->motionBlur = true;
	RenderTarget->multiSample = true;
	RenderTarget->debugMode = false;

    // Create Media Library
	MediaLibrary = new Ovgl::MediaLibrary(Inst, "");

	// Create empty scene
	Scene = MediaLibrary->CreateScene();

	// Add light to scene.
    Light = Scene->CreateLight(Ovgl::MatrixTranslation( -1.8f, 4.0f, -3.35f ), Ovgl::Vector4( 10.0f, 10.0f, 10.0f, 10.0f ));

	// Add camera to scene
	Camera = Scene->CreateCamera(Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ));

	// Set camera as view for render target
	RenderTarget->View = Camera;

	// Create cubemap texture
	Texture1 = MediaLibrary->ImportCubeMap( "../media/textures/skybox/front.png", "../media/textures/skybox/back.png", "../media/textures/skybox/top.png",
											"../media/textures/skybox/bottom.png", "../media/textures/skybox/left.png", "../media/textures/skybox/right.png");
	// Create 2D texture
	Texture2 = MediaLibrary->ImportTexture("../media/textures/Grass.png");

	// Import mesh
	Mesh = MediaLibrary->ImportModel( "../media/meshes/plane.dae", true );
	Mesh2 = MediaLibrary->ImportModel( "../media/meshes/test.dae", true );

	MediaLibrary->ImportAudio("../media/audio/glacier.ogg")->CreateAudioInstance(NULL, true);

	// Add object to scene
	Object = Scene->CreateObject(Mesh, Ovgl::MatrixTranslation( 0.0f, -5.0f, 0.0f ));
    Object->materials[0]->setEffectTexture("txDiffuse", Texture2);

	// Add actor to scene
	Actor = Scene->CreateActor(Mesh2, 0.1f, 1.0f, Ovgl::MatrixTranslation(0.0f, 0.0f, 0.0f), Ovgl::MatrixTranslation(0.0f, 0.0f, 0.0f));
	Actor->PlayAnimation( &Mesh2->skeleton->animations[0], 0, 10, true);

	Actor2 = Scene->CreateActor(Mesh2, 0.1f, 1.0f, Ovgl::MatrixTranslation(1.0f, 0.0f, 0.0f), Ovgl::MatrixTranslation(0.0f, 0.0f, 0.0f));
	Actor2->PlayAnimation( &Mesh2->skeleton->animations[0], 5, 6, false);

	// Set scene sky box
	Scene->sky_box = Texture1;

	Inst->Start();

	// Release all
	delete Inst;
	return 0;
}
