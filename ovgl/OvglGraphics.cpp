/**
* @file OvglGraphics.cpp
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
#include "OvglMedia.h"
#include "OvglGraphics.h"
#include "OvglScene.h"
#include "OvglAudio.h"
#include "OvglMesh.h"

void Ovgl::RenderTarget::Release()
{
	for( UINT r = 0; r < Inst->RenderTargets.size(); r++)
	{
		if(Inst->RenderTargets[r] == this)
		{
			Inst->RenderTargets.erase( Inst->RenderTargets.begin() + r );
		}
	}
	delete this;
}

void Ovgl::RenderTarget::Render()
{
	// Set render target's window to current 
	wglMakeCurrent( hDC, Inst->hRC );

	// Get the window's rect
	RECT WindowRect;
	GetWindowRect( hWnd, &WindowRect );

	// Set the viewport to fit the window
	glViewport( 0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top );

	if( view != NULL )
	{
		Ovgl::Scene* scene = view->scene;
		Ovgl::Matrix44 viewProj = (Ovgl::MatrixInverse( &Ovgl::Vector4Set(0,0,0,0), &view->getPose() ) * view->projMat);

		// Create light arrays
		float LightCount = (float)scene->lights.size();
		std::vector<float> mLights;
		std::vector<float> LightColors;
		for( DWORD l = 0; l < scene->lights.size(); l++)
		{
			mLights.push_back( scene->lights[l]->getPose()._41 );
			mLights.push_back( scene->lights[l]->getPose()._42 );
			mLights.push_back( scene->lights[l]->getPose()._43 );
			mLights.push_back( 1.0f );
			LightColors.push_back( scene->lights[l]->color.x );
			LightColors.push_back( scene->lights[l]->color.y );
			LightColors.push_back( scene->lights[l]->color.z );
			LightColors.push_back( 1.0f );
		}

		// Clear depth buffer
		glDepthMask (GL_TRUE);
		glClear( GL_DEPTH_BUFFER_BIT );

		// Disable depth test
		glDisable (GL_DEPTH_TEST);

		// Set skybox shader View variable
		CGparameter CgView = cgGetNamedParameter( Inst->DefaultMedia->Shaders[1]->VertexProgram, "View" );
		cgGLSetMatrixParameterfc( CgView, (float*)&Ovgl::MatrixTranspose( &Ovgl::MatrixInverse( &Ovgl::Vector4Set(0,0,0,0), &view->getPose()) ) );

		// Set skybox shader Projection variable
		CGparameter CgProjection = cgGetNamedParameter( Inst->DefaultMedia->Shaders[1]->VertexProgram, "Projection" );
		cgGLSetMatrixParameterfc( CgProjection, (float*)&Ovgl::MatrixTranspose( &view->projMat) );

		// Set skybox texture
		CGparameter CgFSTexture = cgGetNamedParameter( Inst->DefaultMedia->Shaders[1]->FragmentProgram, "txSkybox" );
		cgGLSetTextureParameter( CgFSTexture, scene->skybox->Image );
		cgGLEnableTextureParameter( CgFSTexture );
		
		// Bind vertex and index buffers
		glBindBufferARB( GL_ARRAY_BUFFER, Inst->DefaultMedia->Meshes[0]->VertexBuffer );
		glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, Inst->DefaultMedia->Meshes[0]->IndexBuffers[0] );

		// Set vertex attributes
		glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (0)) );
		glVertexAttribPointerARB( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (12)) );
		glVertexAttribPointerARB( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (24)) );
		glVertexAttribPointerARB( 3, 4, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (32)) );
		glVertexAttribPointerARB( 4, 4, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (48)) );

		// Enable vertex attributes
		glEnableVertexAttribArrayARB( 0 );
		glEnableVertexAttribArrayARB( 1 );
		glEnableVertexAttribArrayARB( 2 );
		glEnableVertexAttribArrayARB( 3 );
		glEnableVertexAttribArrayARB( 4 );

		// Bind vertex and fragment programs
		cgGLBindProgram( Inst->DefaultMedia->Shaders[1]->VertexProgram );
		cgGLBindProgram( Inst->DefaultMedia->Shaders[1]->FragmentProgram );

		cgGLEnableProfile( CG_PROFILE_GPU_VP );
		cgGLEnableProfile( CG_PROFILE_GPU_FP );

		// Draw skybox
		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );

		// Disable profiles
		cgGLDisableProfile( CG_PROFILE_GPU_VP );
		cgGLDisableProfile( CG_PROFILE_GPU_FP );

		// Disable vertex attributes
		glDisableVertexAttribArrayARB( 0 );
		glDisableVertexAttribArrayARB( 1 );
		glDisableVertexAttribArrayARB( 2 );
		glDisableVertexAttribArrayARB( 3 );
		glDisableVertexAttribArrayARB( 4 );

		cgGLDisableTextureParameter( CgFSTexture );
		for( DWORD PostRender = 0; PostRender < 2; PostRender++ )
		{
			for( DWORD i = 0; i < scene->objects.size(); i++ )
			{
				Ovgl::Matrix44 worldMat = (scene->objects[i]->getPose() * viewProj );
				glLoadMatrixf((float*)&worldMat);
				for( DWORD s = 0; s < scene->objects[i]->mesh->subset_count; s++)
				if( !!PostRender == scene->objects[i]->subsets[s]->PostRender )
				{
					if(scene->objects[i]->subsets[s]->NoZBuffer)
					{
						glDisable (GL_DEPTH_TEST);
					}
					else
					{
						glEnable (GL_DEPTH_TEST);
					}

					if(scene->objects[i]->subsets[s]->NoZWrite)
					{
						glDepthMask (GL_FALSE);
					}
					else
					{
						glDepthMask (GL_TRUE);
					}

					CGparameter CgWorldMatrix = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->VertexProgram, "World" );
					cgGLSetMatrixParameterfc( CgWorldMatrix, (float*)&Ovgl::MatrixTranspose(&worldMat) );
					CGparameter CgViewProjMatrix = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->VertexProgram, "ViewProj" );
					cgGLSetMatrixParameterfc( CgViewProjMatrix, (float*)&Ovgl::MatrixTranspose(&viewProj) );
						
					CGparameter CgViewPos= cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram, "ViewPos" );
					cgGLSetParameter4f( CgViewPos, view->getPose()._41, view->getPose()._42, view->getPose()._43, view->getPose()._44 );
	
					CGparameter CgBoneMatrices = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->VertexProgram, "Bones" );

					CGparameter CgCurr = cgGetArrayParameter(CgBoneMatrices,0);
					cgGLSetMatrixParameterfc( CgCurr, (float*)&Ovgl::MatrixTranspose(&scene->objects[i]->getPose()) );

					CGparameter CgLightCount = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram, "LightCount" );
					cgGLSetParameter1f( CgLightCount, LightCount );

					CGparameter CgLights = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram, "Lights" );
					CGparameter CgLight;
					for( DWORD v = 0; v < mLights.size() / 4; v++)
					{
						CgLight=cgGetArrayParameter( CgLights, v );
						cgGLSetParameter4fv(CgLight, (float*)&mLights[v * 4]);
					}

					CGparameter CgLightColors = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram, "LightColors" );
					CGparameter CgLightColor;
					for( DWORD v = 0; v < LightColors.size() / 4; v++)
					{
						CgLightColor=cgGetArrayParameter( CgLightColors, v );
						cgGLSetParameter4fv(CgLightColor, (float*)&LightColors[v * 4]);
					}

					for( DWORD v = 0; v < scene->objects[i]->subsets[s]->Textures.size(); v++)
					{
						if(scene->objects[i]->subsets[s]->Textures[v].second)
						{
							CGparameter CgVSTexture = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->VertexProgram, scene->objects[i]->subsets[s]->Textures[v].first.c_str() );
							cgGLSetTextureParameter( CgVSTexture, scene->objects[i]->subsets[s]->Textures[v].second->Image );
							cgGLEnableTextureParameter( CgVSTexture );
							CGparameter CgFSTexture = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram, scene->objects[i]->subsets[s]->Textures[v].first.c_str() );
							cgGLSetTextureParameter( CgFSTexture, scene->objects[i]->subsets[s]->Textures[v].second->Image );
							cgGLEnableTextureParameter( CgFSTexture );
						}
						else
						{
							CGparameter CgVSTexture = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->VertexProgram, scene->objects[i]->subsets[s]->Textures[v].first.c_str() );
							cgGLSetTextureParameter( CgVSTexture, NULL );
							cgGLEnableTextureParameter( CgVSTexture );	
							CGparameter CgFSTexture = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram, scene->objects[i]->subsets[s]->Textures[v].first.c_str() );
							cgGLSetTextureParameter( CgFSTexture, NULL );
							cgGLEnableTextureParameter( CgFSTexture );
						}
					}

					for( DWORD v = 0; v < scene->objects[i]->subsets[s]->Variables.size(); v++)
					{
						CGparameter CgVSVariable = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->VertexProgram, scene->objects[i]->subsets[s]->Variables[v].first.c_str() );
						cgSetParameterValuefr( CgVSVariable, scene->objects[i]->subsets[s]->Variables[v].second.size(), (float*)&scene->objects[i]->subsets[s]->Variables[v].second[0] );			
						CGparameter CgFSVariable = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram, scene->objects[i]->subsets[s]->Variables[v].first.c_str() );
						cgSetParameterValuefr( CgFSVariable, scene->objects[i]->subsets[s]->Variables[v].second.size(), (float*)&scene->objects[i]->subsets[s]->Variables[v].second[0] );
					}

					glBindBufferARB( GL_ARRAY_BUFFER, scene->objects[i]->mesh->VertexBuffer );
					glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, scene->objects[i]->mesh->IndexBuffers[s] );

					// Set vertex attributes
					glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (0)) );
					glVertexAttribPointerARB( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (12)) );
					glVertexAttribPointerARB( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (24)) );
					glVertexAttribPointerARB( 3, 4, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (32)) );
					glVertexAttribPointerARB( 4, 4, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (48)) );

					// Enable vertex attributes
					glEnableVertexAttribArrayARB( 0 );
					glEnableVertexAttribArrayARB( 1 );
					glEnableVertexAttribArrayARB( 2 );
					glEnableVertexAttribArrayARB( 3 );
					glEnableVertexAttribArrayARB( 4 );

					// Bind vertex and fragment programs
					cgGLBindProgram( scene->objects[i]->subsets[s]->ShaderProgram->VertexProgram );
					cgGLBindProgram( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram );

					cgGLEnableProfile( CG_PROFILE_GPU_VP );
					cgGLEnableProfile( CG_PROFILE_GPU_FP );

					int BufferSize;
					glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &BufferSize);
					glDrawElements( GL_TRIANGLES, BufferSize / sizeof(DWORD), GL_UNSIGNED_INT, 0 );

					cgGLDisableProfile( CG_PROFILE_GPU_VP );
					cgGLDisableProfile( CG_PROFILE_GPU_FP );

					// Disable vertex attributes
					glDisableVertexAttribArrayARB( 0 );
					glDisableVertexAttribArrayARB( 1 );
					glDisableVertexAttribArrayARB( 2 );
					glDisableVertexAttribArrayARB( 3 );
					glDisableVertexAttribArrayARB( 4 );

					for( DWORD v = 0; v < scene->objects[i]->subsets[s]->Textures.size(); v++)
					{
						CGparameter CgVSTexture = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->VertexProgram, scene->objects[i]->subsets[s]->Textures[v].first.c_str() );
						cgGLDisableTextureParameter( CgVSTexture );
						CGparameter CgFSTexture = cgGetNamedParameter( scene->objects[i]->subsets[s]->ShaderProgram->FragmentProgram, scene->objects[i]->subsets[s]->Textures[v].first.c_str() );
						cgGLDisableTextureParameter( CgFSTexture );
					}
				}
			}

			for( DWORD i = 0; i < scene->props.size(); i++ )
			{
				Ovgl::Matrix44 worldMat = (scene->props[i]->matrices[scene->props[i]->mesh->root_bone] * viewProj );
				for( DWORD s = 0; s < scene->props[i]->mesh->subset_count; s++)
				if( !!PostRender == scene->props[i]->subsets[s]->PostRender )
				{
					if(scene->props[i]->subsets[s]->NoZBuffer)
					{
						glDisable (GL_DEPTH_TEST);
					}
					else
					{
						glEnable (GL_DEPTH_TEST);
					}

					if(scene->props[i]->subsets[s]->NoZWrite)
					{
						glDepthMask (GL_FALSE);
					}
					else
					{
						glDepthMask (GL_TRUE);
					}

					CGparameter CgWorldMatrix = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->VertexProgram, "World" );
					cgGLSetMatrixParameterfc( CgWorldMatrix, (float*)&Ovgl::MatrixTranspose(&worldMat) );
					CGparameter CgViewProjMatrix = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->VertexProgram, "ViewProj" );
					cgGLSetMatrixParameterfc( CgViewProjMatrix, (float*)&Ovgl::MatrixTranspose(&viewProj) );

					CGparameter CgViewPos= cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->FragmentProgram, "ViewPos" );
					cgGLSetParameter4f( CgViewPos, view->getPose()._41, view->getPose()._42, view->getPose()._43, view->getPose()._44 );


					CGparameter CgBoneMatrices = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->VertexProgram, "Bones" );
					CGparameter CgBone;
					for( DWORD v = 0; v < scene->props[i]->matrices.size(); v++)
					{
						CgBone=cgGetArrayParameter(CgBoneMatrices,v);
						cgGLSetMatrixParameterfc(CgBone, (float*)&Ovgl::MatrixTranspose(&scene->props[i]->matrices[v]));
					}

					CGparameter CgLightCount = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->FragmentProgram, "LightCount" );
					cgGLSetParameter1f( CgLightCount, LightCount );

					CGparameter CgLights = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->FragmentProgram, "Lights" );
					CGparameter CgLight;
					for( DWORD v = 0; v < mLights.size() / 4; v++)
					{
						CgLight=cgGetArrayParameter( CgLights, v );
						cgGLSetParameter4fv(CgLight, (float*)&mLights[v * 4]);
					}

					CGparameter CgLightColors = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->FragmentProgram, "LightColors" );
					CGparameter CgLightColor;
					for( DWORD v = 0; v < LightColors.size() / 4; v++)
					{
						CgLightColor=cgGetArrayParameter( CgLightColors, v );
						cgGLSetParameter4fv(CgLightColor, (float*)&LightColors[v * 4]);
					}

					for( DWORD v = 0; v < scene->props[i]->subsets[s]->Textures.size(); v++)
					{
						CGparameter CgVSTexture = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->VertexProgram, scene->props[i]->subsets[s]->Textures[v].first.c_str() );
						cgGLSetTextureParameter( CgVSTexture, scene->props[i]->subsets[s]->Textures[v].second->Image );
						cgGLEnableTextureParameter( CgVSTexture );
						CGparameter CgFSTexture = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->FragmentProgram, scene->props[i]->subsets[s]->Textures[v].first.c_str() );
						cgGLSetTextureParameter( CgFSTexture, scene->props[i]->subsets[s]->Textures[v].second->Image );
						cgGLEnableTextureParameter( CgFSTexture );
					}

					for( DWORD v = 0; v < scene->props[i]->subsets[s]->Variables.size(); v++)
					{
						CGparameter CgVSVariable = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->VertexProgram, scene->props[i]->subsets[s]->Variables[v].first.c_str() );
						cgSetParameterValuefr( CgVSVariable, scene->props[i]->subsets[s]->Variables[v].second.size(), (float*)&scene->props[i]->subsets[s]->Variables[v].second );
						CGparameter CgFSVariable = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->FragmentProgram, scene->props[i]->subsets[s]->Variables[v].first.c_str() );
						cgSetParameterValuefr( CgFSVariable, scene->props[i]->subsets[s]->Variables[v].second.size(), (float*)&scene->props[i]->subsets[s]->Variables[v].second );
					}

					glBindBufferARB( GL_ARRAY_BUFFER, scene->props[i]->mesh->VertexBuffer );
					glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, scene->props[i]->mesh->IndexBuffers[s] );

					// Set vertex attributes
					glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (0)) );
					glVertexAttribPointerARB( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (12)) );
					glVertexAttribPointerARB( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (24)) );
					glVertexAttribPointerARB( 3, 4, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (32)) );
					glVertexAttribPointerARB( 4, 4, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (48)) );

					// Enable vertex attributes
					glEnableVertexAttribArrayARB( 0 );
					glEnableVertexAttribArrayARB( 1 );
					glEnableVertexAttribArrayARB( 2 );
					glEnableVertexAttribArrayARB( 3 );
					glEnableVertexAttribArrayARB( 4 );

					// Bind vertex and fragment programs
					cgGLBindProgram( scene->props[i]->subsets[s]->ShaderProgram->VertexProgram );
					cgGLBindProgram( scene->props[i]->subsets[s]->ShaderProgram->FragmentProgram );

					cgGLEnableProfile( CG_PROFILE_GPU_VP );
					cgGLEnableProfile( CG_PROFILE_GPU_FP );

					int BufferSize;
					glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &BufferSize);
					glDrawElements( GL_TRIANGLES, BufferSize / sizeof(DWORD), GL_UNSIGNED_INT, 0 );

					cgGLDisableProfile( CG_PROFILE_GPU_VP );
					cgGLDisableProfile( CG_PROFILE_GPU_FP );

					// Disable vertex attributes
					glDisableVertexAttribArrayARB( 0 );
					glDisableVertexAttribArrayARB( 1 );
					glDisableVertexAttribArrayARB( 2 );
					glDisableVertexAttribArrayARB( 3 );
					glDisableVertexAttribArrayARB( 4 );

					for( DWORD v = 0; v < scene->props[i]->subsets[s]->Textures.size(); v++)
					{
						CGparameter CgVSTexture = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->VertexProgram, scene->props[i]->subsets[s]->Textures[v].first.c_str() );
						cgGLDisableTextureParameter( CgVSTexture );
						CGparameter CgFSTexture = cgGetNamedParameter( scene->props[i]->subsets[s]->ShaderProgram->FragmentProgram, scene->props[i]->subsets[s]->Textures[v].first.c_str() );
						cgGLDisableTextureParameter( CgFSTexture );
					}
				}
			}
		}
	}

	glDisable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
	glDisable( GL_LIGHTING );
	glEnable(GL_TEXTURE_2D);
//	glDisable( GL_BLEND );

	// Get viewport
	GLint iViewport[4];
    glGetIntegerv( GL_VIEWPORT, iViewport );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	// Set up the orthographic projection
    glOrtho( iViewport[0], iViewport[0] + iViewport[2], iViewport[1] + iViewport[3], iViewport[1], -1, 1 );

	for(UINT i = 0; i < Interfaces.size(); i++)
	{
		if(Interfaces[i]->Enabled)
		{
			RECT AdjustedRect;

			// Check if sprite rect left is relative or absolute and set AdjustedRect left accordingly
			if( Interfaces[i]->Rect.w > 0 && Interfaces[i]->Rect.w < 1 )
			{
				AdjustedRect.left = (LONG)((iViewport[0] + iViewport[2]) * Interfaces[i]->Rect.w);
			}
			else
			{
				AdjustedRect.left = (LONG)Interfaces[i]->Rect.w;
			}

			// Check if sprite rect top is relative or absolute and set AdjustedRect top accordingly
			if( Interfaces[i]->Rect.x > 0 && Interfaces[i]->Rect.x < 1 )
			{
				AdjustedRect.top = (LONG)((iViewport[1] + iViewport[3]) * Interfaces[i]->Rect.x);
			}
			else
			{
				AdjustedRect.top = (LONG)Interfaces[i]->Rect.x;
			}

			// Check if sprite rect right is relative or absolute and set AdjustedRect right accordingly
			if( Interfaces[i]->Rect.y > 0 && Interfaces[i]->Rect.y < 1 )
			{
				AdjustedRect.right = (LONG)((iViewport[0] + iViewport[2]) * Interfaces[i]->Rect.y);
			}
			else
			{
				AdjustedRect.right = (LONG)Interfaces[i]->Rect.y;
			}

			// Check if sprite rect bottom is relative or absolute and set AdjustedRect bottom accordingly
			if( Interfaces[i]->Rect.z > 0 && Interfaces[i]->Rect.z < 1 )
			{
				AdjustedRect.bottom = (LONG)((iViewport[1] + iViewport[3]) * Interfaces[i]->Rect.z);
			}
			else
			{
				AdjustedRect.bottom = (LONG)Interfaces[i]->Rect.z;
			}

			glMatrixMode( GL_MODELVIEW );
			glPushMatrix();
			glLoadIdentity();

			glBindTexture( GL_TEXTURE_2D, Interfaces[i]->Texture->Image );

			// Draw Interface
			glBegin( GL_QUADS );
				glTexCoord2i( 0, 1 );
				glVertex2i( AdjustedRect.left, AdjustedRect.top );
				glTexCoord2i( 1, 1 );
				glVertex2i( AdjustedRect.right, AdjustedRect.top );
				glTexCoord2i( 1, 0 );
				glVertex2i( AdjustedRect.right, AdjustedRect.bottom );
				glTexCoord2i( 0, 0 );
				glVertex2i( AdjustedRect.left, AdjustedRect.bottom );
			glEnd();
		}
	}

	SwapBuffers(hDC);
}

void Ovgl::RenderTarget::SetFullscreen( bool state )
{
	//SwapChain->SetFullscreenState( state, NULL);
}

bool Ovgl::RenderTarget::GetFullscreen()
{
	BOOL state = false;
	//SwapChain->GetFullscreenState( &state, NULL);
	return !!state;
}

void Ovgl::Interface::UpdateText()
{
	RECT WindowRect;
	GetWindowRect( RenderTarget->hWnd, &WindowRect );

	RECT AdjustedRect;

	// Check if sprite rect left is relative or absolute and set AdjustedRect left accordingly
	if( Rect.w > 0 && Rect.w < 1 )
	{
		AdjustedRect.left = (LONG)((WindowRect.right - WindowRect.left) * Rect.w);
	}
	else
	{
		AdjustedRect.left = (LONG)Rect.w;
	}

	// Check if sprite rect top is relative or absolute and set AdjustedRect top accordingly
	if( Rect.x > 0 && Rect.x < 1 )
	{
		AdjustedRect.top = (LONG)((WindowRect.bottom - WindowRect.top) * Rect.x);
	}
	else
	{
		AdjustedRect.top = (LONG)Rect.x;
	}

	// Check if sprite rect right is relative or absolute and set AdjustedRect right accordingly
	if( Rect.y > 0 && Rect.y < 1 )
	{
		AdjustedRect.right = (LONG)((WindowRect.right - WindowRect.left) * Rect.y);
	}
	else
	{
		AdjustedRect.right = (LONG)Rect.y;
	}

	// Check if sprite rect bottom is relative or absolute and set AdjustedRect bottom accordingly
	if( Rect.z > 0 && Rect.z < 1 )
	{
		AdjustedRect.bottom = (LONG)((WindowRect.bottom - WindowRect.top) * Rect.z);
	}
	else
	{
		AdjustedRect.bottom = (LONG)Rect.z;
	}
	
	RECT TextRect;
	TextRect.left = 0;
	TextRect.top = 0;
	TextRect.right = (LONG)(AdjustedRect.right - AdjustedRect.left);
	TextRect.bottom = (LONG)(AdjustedRect.bottom - AdjustedRect.top);
	HDC hDC = CreateCompatibleDC(NULL);
	GLubyte* pSrcData;
	BITMAPINFO bmi = { sizeof( BITMAPINFOHEADER ), TextRect.right, TextRect.bottom, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
	HBITMAP hTempBmp = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS, (void**)&pSrcData, NULL, 0 );
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hTempBmp );
	HFONT NewFont = CreateFontA( CSize, 0, 0, 0, (FW_BOLD * Bold), Italic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 0, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, Font.c_str() );
	HBRUSH NewBrush = CreateSolidBrush(0);
	SelectObject( hDC, NewFont );
	SelectObject( hDC, NewBrush );
	SetTextColor( hDC, RGB(255,255,255) );
	SetBkColor( hDC, 0);
	DrawTextA(hDC, Text.c_str(), Text.size(), &TextRect, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
	GdiFlush();
	DeleteObject(NewBrush);
	DeleteObject(NewFont);	
	DeleteObject(hTempBmp);

	GLubyte* pTexels = new GLubyte[TextRect.right * TextRect.bottom * 4];
	for( LONG row = 0; row < TextRect.bottom; row++ )
	{
		for( LONG col = 0; col < TextRect.right; col++ )
		{
			pTexels[(row * (TextRect.right * 4)) + (col * 4) + 0] = 255;
			pTexels[(row * (TextRect.right * 4)) + (col * 4) + 1] = 255;
			pTexels[(row * (TextRect.right * 4)) + (col * 4) + 2] = 255;
			pTexels[(row * (TextRect.right * 4)) + (col * 4) + 3] = pSrcData[ ((row * (TextRect.right * 4)) + (col * 4)) ];
		}
	}

	// Create OpenGL texture
	glGenTextures( 1, &Texture->Image );
	glBindTexture( GL_TEXTURE_2D, Texture->Image );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, TextRect.right, TextRect.bottom, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexels );
	glBindTexture( GL_TEXTURE_2D, NULL );
	DeleteDC(hDC);
}

Ovgl::Interface* Ovgl::RenderTarget::CreateSprite( Ovgl::Texture* Texture, Ovgl::Vector4* rect )
{
	Ovgl::Interface* Sprite = new Ovgl::Interface;
	Sprite->Texture = Texture;
	Sprite->Enabled = true;
	Sprite->Over = false;
	Sprite->Font = "";
	Sprite->Rect = *rect;
	Sprite->RenderTarget = this;
	Interfaces.push_back( Sprite );
	return Sprite;
}
	
Ovgl::Interface* Ovgl::RenderTarget::CreateText( const std::string& text, Ovgl::Vector4* rect )
{
	Ovgl::Interface* Text = new Ovgl::Interface;
		// Create new texture
	Text->Texture = new Ovgl::Texture;
	Text->Text = text;
	Text->Font = "Arial";
	Text->Size = 24;
	Text->CSize = 24;
	Text->Bold = false;
	Text->Italic = false;
	Text->Rect = *rect;
	Text->Enabled = true;
	Text->Over = false;
	Text->RenderTarget = this;
	Text->UpdateText();
	Interfaces.push_back( Text );
	return Text;
}
