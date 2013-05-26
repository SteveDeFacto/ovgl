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

#include <Ovgl.h>

Ovgl::Context*				context;
Ovgl::RenderTarget*			render_target;
Ovgl::Window*				window;
Ovgl::ResourceManager*		resources;
Ovgl::Texture*				texture1;
Ovgl::Interface*			interface1;
Ovgl::Font*					font1;

int main()
{
    // Create Main Context
    context = new Ovgl::Context( 0 );

    // Create Window
    window = new Ovgl::Window( context, "Hello World!", 320, 240);

    // Create Render Target
    render_target = new Ovgl::RenderTarget( context, window, Ovgl::URect( 0.0f, 0.0f, 1.0f, 1.0f ), 0 );

	// Create Resource Manager
    resources = new Ovgl::ResourceManager(context, "");

	// Create an interface
	interface1 = new Ovgl::Interface( render_target, Ovgl::URect( 0.0f, 0.0f, 1.0f, 1.0f ) );

	// Load a font
    font1 = new Ovgl::Font(resources, "../media/fonts/ArchitectsDaughter.ttf", 48);

	// Set interface font
	interface1->font = font1;

	// Set the interface text
	interface1->set_text("Hello World!");

	// Start main loop 
    context->start();

    // Release all
    delete context;

	// No errors happend so return zero
    return 0;
}
