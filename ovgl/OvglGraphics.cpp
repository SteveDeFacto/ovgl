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

void Ovgl::RenderTarget::RenderMesh( Ovgl::Mesh* mesh, Matrix44& matrix, std::vector< Matrix44 >& pose, std::vector< Material* >& materials, bool PostRender )
{
	Ovgl::Matrix44 viewProj = (Ovgl::MatrixInverse( &Ovgl::Vector4(0,0,0,0), &view->getPose() ) * view->projMat);
	Ovgl::Matrix44 worldMat = (matrix * viewProj );
	glLoadMatrixf((float*)&worldMat);

	float LightCount = (float)view->scene->lights.size();
	std::vector< float > mLights;
	std::vector< float > LightColors;
	for( DWORD l = 0; l < view->scene->lights.size(); l++)
	{
		mLights.push_back( view->scene->lights[l]->getPose()._41 );
		mLights.push_back( view->scene->lights[l]->getPose()._42 );
		mLights.push_back( view->scene->lights[l]->getPose()._43 );
		mLights.push_back( 1.0f );
		LightColors.push_back( view->scene->lights[l]->color.x );
		LightColors.push_back( view->scene->lights[l]->color.y );
		LightColors.push_back( view->scene->lights[l]->color.z );
		LightColors.push_back( 1.0f );
	}

	for( DWORD s = 0; s < mesh->subset_count; s++)
	if( PostRender == materials[s]->PostRender )
	{
		if(materials[s]->NoZBuffer)
		{
			glDisable (GL_DEPTH_TEST);
		}
		else
		{
			glEnable (GL_DEPTH_TEST);
		}

		if(materials[s]->NoZWrite)
		{
			glDepthMask (GL_FALSE);
		}
		else
		{
			glDepthMask (GL_TRUE);
		}

		CGparameter CgWorldMatrix = cgGetNamedParameter( materials[s]->ShaderProgram->VertexProgram, "World" );
		cgGLSetMatrixParameterfc( CgWorldMatrix, (float*)&Ovgl::MatrixTranspose(&worldMat) );
		CGparameter CgViewProjMatrix = cgGetNamedParameter( materials[s]->ShaderProgram->VertexProgram, "ViewProj" );
		cgGLSetMatrixParameterfc( CgViewProjMatrix, (float*)&Ovgl::MatrixTranspose(&viewProj) );
		CGparameter CgViewPos= cgGetNamedParameter( materials[s]->ShaderProgram->FragmentProgram, "ViewPos" );
		cgGLSetParameter4f( CgViewPos, view->getPose()._41, view->getPose()._42, view->getPose()._43, view->getPose()._44 );

		CGparameter CgBoneMatrices = cgGetNamedParameter( materials[s]->ShaderProgram->VertexProgram, "Bones" );
		for( DWORD v = 0; v < pose.size(); v++)
		{
			CGparameter CgBone = cgGetArrayParameter( CgBoneMatrices, v );
			cgGLSetMatrixParameterfc(CgBone, (float*)&Ovgl::MatrixTranspose(&pose[v]));
		}

		CGparameter CgLightCount = cgGetNamedParameter( materials[s]->ShaderProgram->FragmentProgram, "LightCount" );
		cgGLSetParameter1f( CgLightCount, LightCount );

		CGparameter CgLights = cgGetNamedParameter( materials[s]->ShaderProgram->FragmentProgram, "Lights" );
		CGparameter CgLight;
		for( DWORD v = 0; v < mLights.size() / 4; v++)
		{
			CgLight=cgGetArrayParameter( CgLights, v );
			cgGLSetParameter4fv(CgLight, (float*)&mLights[v * 4]);
		}

		CGparameter CgLightColors = cgGetNamedParameter( materials[s]->ShaderProgram->FragmentProgram, "LightColors" );
		CGparameter CgLightColor;
		for( DWORD v = 0; v < LightColors.size() / 4; v++)
		{
			CgLightColor=cgGetArrayParameter( CgLightColors, v );
			cgGLSetParameter4fv(CgLightColor, (float*)&LightColors[v * 4]);
		}

		for( DWORD v = 0; v < materials[s]->Textures.size(); v++)
		{
			CGparameter CgTexture = materials[s]->Textures[v].first;
			cgGLSetTextureParameter( CgTexture, materials[s]->Textures[v].second->Image );
			cgGLEnableTextureParameter( CgTexture );
		}

		for( DWORD v = 0; v < materials[s]->Variables.size(); v++)
		{
			CGparameter CgVariable = materials[s]->Variables[v].first;
			cgSetParameterValuefr( CgVariable, materials[s]->Variables[v].second.size(), (float*)&materials[s]->Variables[v].second[0] );
		}

		glBindBufferARB( GL_ARRAY_BUFFER, mesh->VertexBuffer );
		glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, mesh->IndexBuffers[s] );

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
		cgGLBindProgram( materials[s]->ShaderProgram->VertexProgram );
		cgGLBindProgram( materials[s]->ShaderProgram->FragmentProgram );

		cgGLEnableProfile( Inst->CgVertexProfile );
		cgGLEnableProfile( Inst->CgFragmentProfile );

		int BufferSize;
		glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &BufferSize);
		glDrawElements( GL_TRIANGLES, BufferSize / sizeof(DWORD), GL_UNSIGNED_INT, 0 );

		cgGLDisableProfile( Inst->CgVertexProfile );
		cgGLDisableProfile( Inst->CgFragmentProfile );

		// Disable vertex attributes
		glDisableVertexAttribArrayARB( 0 );
		glDisableVertexAttribArrayARB( 1 );
		glDisableVertexAttribArrayARB( 2 );
		glDisableVertexAttribArrayARB( 3 );
		glDisableVertexAttribArrayARB( 4 );

		for( DWORD v = 0; v < materials[s]->Textures.size(); v++)
		{
			CGparameter CgTexture = materials[s]->Textures[v].first;
			cgGLDisableTextureParameter( CgTexture );
		}
	}
}

void Ovgl::RenderTarget::AutoLuminance()
{
	glBindTexture(GL_TEXTURE_2D, PrimaryTex);
	glGenerateMipmap(GL_TEXTURE_2D);
	float luminance;
	glGetTexImage(GL_TEXTURE_2D, 10, GL_LUMINANCE, GL_FLOAT, &luminance);
	eye_luminance = std::max( 0.5f, std::min( 1.0f, eye_luminance) ) + (( (luminance + 0.5f) - eye_luminance ) * 0.01f);

	// Render bloom effect
	glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);

	// Set texture
	CGparameter CgFSTexture4 = cgGetNamedParameter( Inst->DefaultMedia->Shaders[2]->FragmentProgram, "txDiffuse" );
	cgGLSetTextureParameter( CgFSTexture4, PrimaryTex );
	cgGLEnableTextureParameter( CgFSTexture4 );

	// Set brightness 
	CGparameter CgBrightness = cgGetNamedParameter( Inst->DefaultMedia->Shaders[5]->FragmentProgram, "Brightness" );
	cgGLSetParameter1f( CgBrightness, 1.0f / eye_luminance );
	
	// Set vertex attributes
	glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (0)) );
	glVertexAttribPointerARB( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (12)) );

	// Enable vertex attributes
	glEnableVertexAttribArrayARB( 0 );
	glEnableVertexAttribArrayARB( 1 );

	// Bind vertex and fragment programs
	cgGLBindProgram( Inst->DefaultMedia->Shaders[5]->FragmentProgram );

	cgGLEnableProfile( Inst->CgFragmentProfile );

	glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	cgGLDisableProfile( Inst->CgFragmentProfile );

	// Disable vertex attributes
	glDisableVertexAttribArrayARB( 0 );
	glDisableVertexAttribArrayARB( 1 );
}

void Ovgl::RenderTarget::Bloom()
{
	// Render bloom effect
	glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryBloomTex, 0);

	GLint width, height;
	glBindTexture(GL_TEXTURE_2D, PrimaryBloomTex);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport( 0, 0, width, height );

	CGparameter CgFSTexture = cgGetNamedParameter( Inst->DefaultMedia->Shaders[3]->FragmentProgram, "txDiffuse" );
	cgGLSetTextureParameter( CgFSTexture, PrimaryTex );
	cgGLEnableTextureParameter( CgFSTexture );

	// Set vertex attributes
	glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (0)) );
	glVertexAttribPointerARB( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (12)) );

	// Enable vertex attributes
	glEnableVertexAttribArrayARB( 0 );
	glEnableVertexAttribArrayARB( 1 );

	// Bind vertex and fragment programs
	cgGLBindProgram( Inst->DefaultMedia->Shaders[3]->FragmentProgram );

	cgGLEnableProfile( Inst->CgFragmentProfile );

	glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	cgGLDisableProfile( Inst->CgFragmentProfile );

	// Disable vertex attributes
	glDisableVertexAttribArrayARB( 0 );
	glDisableVertexAttribArrayARB( 1 );

	bool flipflop = true;
	for( float i = 0; i < (float)OvglPi; i = i + ((float)(OvglPi) / bloom))
	{
		float x = sin(i) * 0.005f;
		float y = cos(i) * 0.005f;

		if( flipflop )
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SecondaryBloomTex, 0);
			CGparameter CgFSTexture2 = cgGetNamedParameter( Inst->DefaultMedia->Shaders[2]->FragmentProgram, "txDiffuse" );
			cgGLSetTextureParameter( CgFSTexture2, PrimaryBloomTex );
			cgGLEnableTextureParameter( CgFSTexture2 );
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryBloomTex, 0);
			CGparameter CgFSTexture2 = cgGetNamedParameter( Inst->DefaultMedia->Shaders[2]->FragmentProgram, "txDiffuse" );
			cgGLSetTextureParameter( CgFSTexture2, SecondaryBloomTex );
			cgGLEnableTextureParameter( CgFSTexture2 );
		}
		CGparameter CgDirection2 = cgGetNamedParameter( Inst->DefaultMedia->Shaders[2]->FragmentProgram, "direction" );
		cgGLSetParameter2f( CgDirection2, x, y );

		// Set vertex attributes
		glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (0)) );
		glVertexAttribPointerARB( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (12)) );

		// Enable vertex attributes
		glEnableVertexAttribArrayARB( 0 );
		glEnableVertexAttribArrayARB( 1 );

		// Bind vertex and fragment programs
		cgGLBindProgram( Inst->DefaultMedia->Shaders[2]->FragmentProgram );

		cgGLEnableProfile( Inst->CgFragmentProfile );

		glBegin(GL_QUADS);
			glTexCoord2f( 0.0f, 0.0f );
			glVertex3f(-1.0f,-1.0f, -1.0f);
			glTexCoord2f( 1.0f, 0.0f );
			glVertex3f(1.0f,-1.0f, -1.0f);
			glTexCoord2f( 1.0f, 1.0f );
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f( 0.0f, 1.0f );
			glVertex3f(-1.0f, 1.0f, -1.0f);
		glEnd();

		cgGLDisableProfile( Inst->CgFragmentProfile );

		// Disable vertex attributes
		glDisableVertexAttribArrayARB( 0 );
		glDisableVertexAttribArrayARB( 1 );
		flipflop = !flipflop;
	}

	// Get texture width and height.
	glBindTexture(GL_TEXTURE_2D, PrimaryTex);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);
	glViewport( 0, 0, width, height );
	CGparameter CgFSTextureA = cgGetNamedParameter( Inst->DefaultMedia->Shaders[4]->FragmentProgram, "txDiffuse1" );
	cgGLSetTextureParameter( CgFSTextureA, PrimaryTex);
	cgGLEnableTextureParameter( CgFSTextureA );

	CGparameter CgFSTextureB = cgGetNamedParameter( Inst->DefaultMedia->Shaders[4]->FragmentProgram, "txDiffuse2" );
	cgGLSetTextureParameter( CgFSTextureB, PrimaryBloomTex );
	cgGLEnableTextureParameter( CgFSTextureB );

	// Set vertex attributes
	glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (0)) );
	glVertexAttribPointerARB( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (12)) );

	// Enable vertex attributes
	glEnableVertexAttribArrayARB( 0 );
	glEnableVertexAttribArrayARB( 1 );

	// Bind vertex and fragment programs
	cgGLBindProgram( Inst->DefaultMedia->Shaders[4]->FragmentProgram );

	cgGLEnableProfile( Inst->CgFragmentProfile );

	glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	cgGLDisableProfile( Inst->CgFragmentProfile );

	// Disable vertex attributes
	glDisableVertexAttribArrayARB( 0 );
	glDisableVertexAttribArrayARB( 1 );

		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Ovgl::RenderTarget::MotionBlur( float x, float y )
{
	glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SecondaryTex, 0);

	CGparameter CgFSTexture2 = cgGetNamedParameter( Inst->DefaultMedia->Shaders[2]->FragmentProgram, "txDiffuse" );
	cgGLSetTextureParameter( CgFSTexture2, PrimaryTex );
	cgGLEnableTextureParameter( CgFSTexture2 );

	CGparameter CgDirection2 = cgGetNamedParameter( Inst->DefaultMedia->Shaders[2]->FragmentProgram, "direction" );

	// Set vertex attributes
	glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (0)) );
	glVertexAttribPointerARB( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Ovgl::Vertex), ((char *)NULL + (12)) );

	// Enable vertex attributes
	glEnableVertexAttribArrayARB( 0 );
	glEnableVertexAttribArrayARB( 1 );

	// Bind vertex and fragment programs
	cgGLBindProgram( Inst->DefaultMedia->Shaders[2]->FragmentProgram );

	cgGLEnableProfile( Inst->CgFragmentProfile );

	cgGLSetParameter2f( CgDirection2, x, y );
	// Render a full screen quad
	glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	cgGLDisableProfile( Inst->CgFragmentProfile );

	// Disable vertex attributes
	glDisableVertexAttribArrayARB( 0 );
	glDisableVertexAttribArrayARB( 1 );

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);
	glBindTexture(GL_TEXTURE_2D, SecondaryTex);

	// Render a full screen quad
	glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Ovgl::RenderTarget::Render()
{
	// Set render target's window to current 
	wglMakeCurrent( hDC, Inst->hRC );

	// Get the window's rect
	RECT WindowRect;
	GetWindowRect( hWnd, &WindowRect );

	// Set the viewport to fit the window

	if( view != NULL )
	{
		glBindFramebuffer(GL_FRAMEBUFFER, MultiSampleFrameBuffer);
		glViewport( 0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top );
		Ovgl::Scene* scene = view->scene;
		Ovgl::Matrix44 viewProj = (Ovgl::MatrixInverse( &Ovgl::Vector4(0,0,0,0), &view->getPose() ) * view->projMat);

		// Create light arrays
		float LightCount = (float)scene->lights.size();
		std::vector< float > mLights;
		std::vector< float > LightColors;
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

		if(scene->skybox)
		{
			// Disable depth test
			glDisable (GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			glDisable(GL_MULTISAMPLE_ARB);

			// Set skybox shader View variable
			CGparameter CgView = cgGetNamedParameter( Inst->DefaultMedia->Shaders[1]->VertexProgram, "View" );
			cgGLSetMatrixParameterfc( CgView, (float*)&Ovgl::MatrixTranspose( &Ovgl::MatrixInverse( &Ovgl::Vector4(0,0,0,0), &view->getPose()) ) );

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

			cgGLEnableProfile( Inst->CgVertexProfile );
			cgGLEnableProfile( Inst->CgFragmentProfile );

			// Draw skybox
			glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );

			// Disable profiles
			cgGLDisableProfile( Inst->CgVertexProfile );
			cgGLDisableProfile( Inst->CgFragmentProfile );

			// Disable vertex attributes
			glDisableVertexAttribArrayARB( 0 );
			glDisableVertexAttribArrayARB( 1 );
			glDisableVertexAttribArrayARB( 2 );
			glDisableVertexAttribArrayARB( 3 );
			glDisableVertexAttribArrayARB( 4 );

			cgGLDisableTextureParameter( CgFSTexture );
		}
		else
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
			glClear( GL_COLOR_BUFFER_BIT );
		}

		// Multisample.
		if(multiSample)
		{
			glEnable(GL_MULTISAMPLE_ARB);
		}
		else
		{
			glDisable(GL_MULTISAMPLE_ARB);
		}

		for( DWORD PostRender = 0; PostRender < 2; PostRender++ )
		{
			// Render Objects
			for( DWORD i = 0; i < scene->objects.size(); i++ )
			{
				std::vector<Ovgl::Matrix44> temp(1);
				temp[0] = scene->objects[i]->getPose();
				RenderMesh( scene->objects[i]->mesh, scene->objects[i]->getPose(), temp, scene->objects[i]->materials, !!PostRender );
			}

			// Render props
			for( DWORD i = 0; i < scene->props.size(); i++ )
			{
				RenderMesh( scene->props[i]->mesh, scene->props[i]->getPose(), scene->props[i]->matrices, scene->props[i]->materials, !!PostRender );
			}

			// Render actors
			for( DWORD i = 0; i < scene->actors.size(); i++ )
			{
				if( scene->actors[i]->mesh )
				{
					RenderMesh( scene->actors[i]->mesh, scene->actors[i]->getPose(), scene->actors[i]->matrices, scene->actors[i]->materials, !!PostRender );
				}
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	glDisable(GL_MULTISAMPLE_ARB);
	glDisable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
	glDisable( GL_LIGHTING );
	glEnable(GL_TEXTURE_2D);
	glDisable (GL_BLEND);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// Blit MultiSampleTexture to BaseTexture to apply effects.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, MultiSampleFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, EffectFrameBuffer);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);
	glBlitFramebuffer(0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, 0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	if( autoLuminance )
	{
		AutoLuminance();
	}

	if(bloom)
	{
		Bloom();
	}

	if(motionBlur)
	{
		static Ovgl::Vector2 LastCamVec;
		Ovgl::Matrix44 CamMat = view->getPose();
		Ovgl::Vector2 CamVec = Ovgl::Vector2(sin(CamMat._11*(float)OvglPi) * abs(CamMat._22), sin(CamMat._22*(float)OvglPi) );
		Ovgl::Vector2 CurCamVec = (CamVec - LastCamVec)/30;
		if( CurCamVec.x > 0.001f || CurCamVec.x < -0.001f || CurCamVec.y > 0.001f || CurCamVec.y < -0.001f )
		{
			MotionBlur( CurCamVec.x, CurCamVec.y );
		}
		LastCamVec = CamVec;
	}

	// Render to screen
	glBindTexture(GL_TEXTURE_2D, PrimaryTex);
	glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	if(debugMode)
	{
		glDisable (GL_DEPTH_TEST);
		glDepthMask (GL_FALSE);
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf((float*)&(Ovgl::MatrixInverse( &Ovgl::Vector4(0.0f, 0.0f, 0.0f, 0.0f), &view->getPose())));
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf((float*)&view->projMat);
		view->scene->DynamicsWorld->debugDrawWorld();
	}

	glEnable (GL_BLEND);
	glDisable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
	glDisable( GL_LIGHTING );
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	// Get viewport
	GLint iViewport[4];
    glGetIntegerv( GL_VIEWPORT, iViewport );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
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

			glBindTexture( GL_TEXTURE_2D, Interfaces[i]->Texture->Image );

			// Draw Interface
			glBegin( GL_QUADS );
				glTexCoord2i( 1, 1 );
				glVertex2i( AdjustedRect.right, AdjustedRect.top );
				glTexCoord2i( 0, 1 );
				glVertex2i( AdjustedRect.left, AdjustedRect.top );
				glTexCoord2i( 0, 0 );
				glVertex2i( AdjustedRect.left, AdjustedRect.bottom );
				glTexCoord2i( 1, 0 );
				glVertex2i( AdjustedRect.right, AdjustedRect.bottom );
			glEnd();
		}
	}
	SwapBuffers(hDC);
}

void Ovgl::RenderTarget::SetVSync( bool state )
{
	wglSwapIntervalEXT(state);
}

void Ovgl::RenderTarget::SetFullscreen( bool state )
{
	RECT Rect;
	GetWindowRect(hWnd, &Rect);
	DEVMODE dmScreenSettings				= {0};
	dmScreenSettings.dmSize					= sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth			= Rect.right;
	dmScreenSettings.dmPelsHeight			= Rect.bottom;
	dmScreenSettings.dmFields				= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	dmScreenSettings.dmBitsPerPel			= 24;

	if(state)
	{
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
		ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN );
		ShowWindow( hWnd, SW_SHOW );
		SetFocus(hWnd);
	}
	else
	{
		ChangeDisplaySettings( &dmScreenSettings, 0 );
	}
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
