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

// Declare pointers we will use later
Ovgl::Context*				context;
Ovgl::RenderTarget*			render_target;
Ovgl::Window*				window;
Ovgl::ResourceManager*		resources;
Ovgl::Scene*				scene;
Ovgl::Actor*				actor;
Ovgl::Actor*				actor2;
Ovgl::Camera*				camera;
Ovgl::Emitter*				emitter;
Ovgl::Texture*				texture1;
Ovgl::Texture*				texture2;
Ovgl::Texture*				texture3;
Ovgl::Mesh*					mesh;
Ovgl::Mesh*					mesh2;
Ovgl::Object*				object;
Ovgl::Prop*					prop;
Ovgl::Light*				light;
Ovgl::Interface*			interface1;
Ovgl::Interface*			interface2;
Ovgl::Interface*			interface3;

void MouseMove(long x, long y)
{
	camera->setPose( ((Ovgl::MatrixRotationY(x / 1000.0f ) * Ovgl::MatrixRotationX( -y / 1000.0f)) * camera->getPose() ) );
}

void KeyDown(char key)
{
	switch(key)
	{
	case (char)27: // If escape is pressed tell the context to quit
        context->g_quit = true;
		break;

	case 'W': // If the W key is pressed move camera forward
        camera->setPose( (Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.1f ) * camera->getPose() ) );
		break;

	case 'S': // If the S key is pressed move camera back
		camera->setPose( (Ovgl::MatrixTranslation( 0.0f, 0.0f, -0.1f ) * camera->getPose() ) );
		break;

	case 'A': // If the A key is pressed move camera left
        camera->setPose( (Ovgl::MatrixTranslation( 0.1f, 0.0f, 0.0f ) * camera->getPose() ) );
		break;

	case 'D': // If the D key is pressed move camera right
        camera->setPose( (Ovgl::MatrixTranslation( -0.1f, 0.0f, 0.0f ) * camera->getPose() ) );
		break;
	case 'F': // If the D key is pressed move camera right
        actor->walkDirection = Ovgl::Vector3(0.01f,0.0f,0.0f);
		break;
	}
}
int main()
{
    // Create Main Context
    context = new Ovgl::Context( 0 );

    // Create Window
    window = new Ovgl::Window( context, "Editor");

	// Lock mouse to center of window
    window->set_lock_mouse( true );

	// Vertical synchronization to prevent screen tearing
    window->set_vsync( true );

	// Set mouse move event callback function
    window->on_mouse_move = MouseMove;

	// Set key down event callback function
    window->on_key_down = KeyDown;

    // Create Render Target
    render_target = new Ovgl::RenderTarget( context, window, Ovgl::URect(0, 0, 1.0f, 1.0f), 0);

	// Blurs bloom across four angles
    render_target->bloom = 4;

	// Auto adjust brightness
    render_target->autoLuminance = true;

	// Use motion bluring
    render_target->motionBlur = true;

	// Multi sample to smooth edges
    render_target->multiSample = true;

    // Create Media Library
    resources = new Ovgl::ResourceManager(context, "");

    // Create empty scene
    scene = resources->create_scene();

    // Add light to scene.
    light = scene->CreateLight(Ovgl::MatrixTranslation( -1.8f, 4.0f, -3.35f ), Ovgl::Vector4( 5.0f, 5.0f, 5.0f, 1.0f ));

    // Add camera to scene
    camera = scene->CreateCamera(Ovgl::MatrixTranslation( 0.0f, 0.0f, 0.0f ));

    // Set camera as view for render target
    render_target->view = camera;

    // Import cubemap texture
    texture1 = resources->import_cubemap( "../media/textures/skybox/front.png", "../media/textures/skybox/back.png", "../media/textures/skybox/top.png",
                                            "../media/textures/skybox/bottom.png", "../media/textures/skybox/left.png", "../media/textures/skybox/right.png");
    // Import grass texture
    texture2 = resources->import_texture("../media/textures/Grass.png");

	// Import margle texture
    texture3 = resources->import_texture("../media/textures/white marble.png");

    // Import mesh
    mesh = resources->import_model( "../media/meshes/plane.dae", true );

	// Import another mesh
    mesh2 = resources->import_model( "../media/meshes/harvey.dae", true );


	// Import and play audio
    //resources->import_audio("../media/audio/glacier.ogg")->create_audio_instance(NULL, true);

    // Add object to scene
    object = scene->CreateObject(mesh, Ovgl::MatrixTranslation( 0.0f, -5.0f, 0.0f ));

	// Bind texture to effect
    object->materials[0]->setEffectTexture("txDiffuse", texture2);

    // Add actor to scene
    actor = scene->CreateActor(mesh2, 0.1f, 1.0f, Ovgl::MatrixTranslation(-2.0f, 0.0f, 0.0f), Ovgl::MatrixTranslation(0.0f, 0.0f, 0.0f));

	// Play an animation
//    actor->PlayAnimation( &mesh2->skeleton->animations[0], 0, 10, true);

    // Set scene sky box
    scene->sky_box = texture1;

	// Start main loop 
    context->start();

	// Release all
    delete context;

	// No errors happend so return zero
	return 0;
}
