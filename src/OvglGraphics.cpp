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
 * @brief This part of the library handles everything related to graphics.
 */

#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglResource.h"
#include "OvglGraphics.h"
#include "OvglScene.h"
#include "OvglAudio.h"
#include "OvglMesh.h"
#include "OvglWindow.h"
#include "OvglSkeleton.h"
#include <GL/glew.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

namespace Ovgl
{

// This function is used to find the maximum number of mipmaps that can be produced from the given resolution.
uint32_t maxLevel( uint32_t width, uint32_t height )
{
	uint32_t max = 0;
	uint32_t highest = 0;
	if( width > height )
	{
		highest = width;
	}
	else
	{
		highest = height;
	}
	while( highest > 1)
	{
		highest = highest / 2;
		max = max + 1;
	}
	return max;
}

Ovgl::Rect windowAdjustedRect( Window* window, URect* rect )
{
	// Get the window's rect
	Ovgl::Rect windowRect;
	SDL_GetWindowPosition( window->sdlWindow, &windowRect.left, &windowRect.top );
	SDL_GetWindowSize( window->sdlWindow, &windowRect.right, &windowRect.bottom );
	windowRect.right += windowRect.left;
	windowRect.bottom += windowRect.top;

	Ovgl::Rect adjustedRect;
	adjustedRect.left = ((windowRect.right - windowRect.left) * rect->left.scale) + rect->left.offset;
	adjustedRect.top = ((windowRect.bottom - windowRect.top) * rect->top.scale) + rect->left.offset;
	adjustedRect.right = ((windowRect.right - windowRect.left) * rect->right.scale) + rect->right.offset;
	adjustedRect.bottom = ((windowRect.bottom - windowRect.top) * rect->bottom.scale) + rect->bottom.offset;

	return adjustedRect;
}

Ovgl::Rect renderTargetAdjustedRect( RenderTarget* rendertarget, URect* rect )
{
	// Get the rendertarget's rect
	Ovgl::Rect RenderTargetRect;

	RenderTargetRect = windowAdjustedRect( rendertarget->window, &rendertarget->rect );

	Ovgl::Rect adjustedrect;
	adjustedrect.left = ((RenderTargetRect.right - RenderTargetRect.left) * rect->left.scale) + rect->left.offset;
	adjustedrect.top = ((RenderTargetRect.bottom - RenderTargetRect.top) * rect->top.scale) + rect->top.offset;
	adjustedrect.right = ((RenderTargetRect.right - RenderTargetRect.left) * rect->right.scale) + rect->right.offset;
	adjustedrect.bottom = ((RenderTargetRect.bottom - RenderTargetRect.top) * rect->bottom.scale) + rect->bottom.offset;

	return adjustedrect;
}

Ovgl::Rect textureAdjustedRect( Texture* texture, URect* rect )
{
	// Get the window's rect
	Ovgl::Rect WindowRect;

	glBindTexture( GL_TEXTURE_2D, texture->image );
	GLint width, height;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );

	WindowRect.left = 0;
	WindowRect.top = 0;
	WindowRect.right = width;
	WindowRect.bottom = height;

	Ovgl::Rect adjustedrect;
	adjustedrect.left = ((WindowRect.right - WindowRect.left) * rect->left.scale) + rect->left.offset;
	adjustedrect.top = ((WindowRect.bottom - WindowRect.top) * rect->top.scale) + rect->left.offset;
	adjustedrect.right = ((WindowRect.right - WindowRect.left) * rect->right.scale) + rect->right.offset;
	adjustedrect.bottom = ((WindowRect.bottom - WindowRect.top) * rect->bottom.scale) + rect->bottom.offset;
	return adjustedrect;
}

RenderTarget::RenderTarget( Context* pcontext, Window* hWindow, const URect& viewport, uint32_t flags )
{
	context = pcontext;
	onKeyDown = NULL;
	onKeyUp = NULL;
	onMouseMove = NULL;
	onMouseDown = NULL;
	onMouseUp = NULL;
	onMouseOver = NULL;
	onMouseOut = NULL;
	window = hWindow;
	hTex = NULL;
	view = NULL;
	debugMode = false;
	autoLuminance = false;
	bloom = 0;
	motionBlur = false;
	multiSample = false;
	eyeLuminance = 0.0f;
	rect = viewport;
	multiSampleFrameBuffer = 0;
	effectFrameBuffer = 0;
	colorBuffer = 0;
	depthBuffer = 0;
	primaryTex = 0;
	secondaryTex = 0;
	primaryBloomTex = 0;
	secondaryBloomTex = 0;
	update();
	window->renderTargets.push_back(this);
};

RenderTarget::RenderTarget( Context* pContext, Texture* hTexture, const URect& viewport, uint32_t flags )
{
	context = pContext;
	onKeyDown = NULL;
	onKeyUp = NULL;
	onMouseMove = NULL;
	onMouseDown = NULL;
	onMouseUp = NULL;
	onMouseOver = NULL;
	onMouseOut = NULL;
	window = NULL;
	hTex = hTexture;
	view = NULL;
	debugMode = false;
	autoLuminance = true;
	bloom = 4;
	motionBlur = true;
	multiSample = true;
	eyeLuminance = 0.0f;
	rect = viewport;
	multiSampleFrameBuffer = 0;
	effectFrameBuffer = 0;
	colorBuffer = 0;
	depthBuffer = 0;
	primaryTex = 0;
	secondaryTex = 0;
	primaryBloomTex = 0;
	secondaryBloomTex = 0;
	update();
};

RenderTarget::~RenderTarget()
{
	for( uint32_t r = 0; r < window->renderTargets.size(); r++)
	{
		if(window->renderTargets[r] == this)
		{
			window->renderTargets.erase( window->renderTargets.begin() + r );
		}
	}
}

void RenderTarget::renderMesh( const Mesh& mesh, const Matrix44& matrix, std::vector< Matrix44 >& pose, std::vector< Material* >& materials, bool postRender )
{
	Matrix44 viewProj = (matrixInverse( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), view->getPose() ) * view->projMat);
	Matrix44 worldMat = (matrix * viewProj );
	glLoadMatrixf((float*)&worldMat);

	float lightCount = (float)view->scene->lights.size();
	std::vector< float > mLights;
	std::vector< float > lightColors;
	for( uint32_t l = 0; l < view->scene->lights.size(); l++)
	{
		mLights.push_back( view->scene->lights[l]->getPose()._41 );
		mLights.push_back( view->scene->lights[l]->getPose()._42 );
		mLights.push_back( view->scene->lights[l]->getPose()._43 );
		mLights.push_back( 1.0f );
		lightColors.push_back( view->scene->lights[l]->color.x );
		lightColors.push_back( view->scene->lights[l]->color.y );
		lightColors.push_back( view->scene->lights[l]->color.z );
		lightColors.push_back( 1.0f );
	}

	for( uint32_t s = 0; s < mesh.subsetCount; s++)
		if( postRender == materials[s]->postRender )
		{
			if(materials[s]->noZBuffer)
			{
				glDisable (GL_DEPTH_TEST);
			}
			else
			{
				glEnable (GL_DEPTH_TEST);
			}

			if(materials[s]->noZWrite)
			{
				glDepthMask (GL_FALSE);
			}
			else
			{
				glDepthMask (GL_TRUE);
			}

			CGparameter cgWorldMatrix = cgGetNamedEffectParameter( materials[s]->shaderProgram->effect, "World" );
			Matrix44 tWorldMat = matrixTranspose(worldMat);
			cgGLSetMatrixParameterfc( cgWorldMatrix, (float*)&tWorldMat );
			CGparameter cgViewProjMatrix = cgGetNamedEffectParameter( materials[s]->shaderProgram->effect, "ViewProj" );
			Matrix44 tViewProj = matrixTranspose(viewProj);
			cgGLSetMatrixParameterfc( cgViewProjMatrix, (float*)&tViewProj );
			CGparameter cgViewPos= cgGetNamedEffectParameter( materials[s]->shaderProgram->effect, "ViewPos" );
			cgGLSetParameter4f( cgViewPos, view->getPose()._41, view->getPose()._42, view->getPose()._43, view->getPose()._44 );

			CGparameter cgBoneMatrices = cgGetNamedEffectParameter( materials[s]->shaderProgram->effect, "Bones" );
			for( uint32_t v = 0; v < pose.size(); v++)
			{
				CGparameter cgBone = cgGetArrayParameter( cgBoneMatrices, v );
				Matrix44 tPose = matrixTranspose(pose[v]);
				cgGLSetMatrixParameterfc(cgBone, (float*)&tPose);
			}

			CGparameter cgLightCount = cgGetNamedEffectParameter( materials[s]->shaderProgram->effect, "LightCount" );
			cgGLSetParameter1f( cgLightCount, lightCount );

			CGparameter CgLights = cgGetNamedEffectParameter( materials[s]->shaderProgram->effect, "Lights" );
			CGparameter CgLight;
			for( uint32_t v = 0; v < mLights.size() / 4; v++)
			{
				CgLight=cgGetArrayParameter( CgLights, v );
				cgGLSetParameter4fv(CgLight, (float*)&mLights[v * 4]);
			}

			CGparameter CgLightColors = cgGetNamedEffectParameter( materials[s]->shaderProgram->effect, "LightColors" );
			CGparameter CgLightColor;
			for( uint32_t v = 0; v < lightColors.size() / 4; v++)
			{
				CgLightColor=cgGetArrayParameter( CgLightColors, v );
				cgGLSetParameter4fv(CgLightColor, (float*)&lightColors[v * 4]);
			}

			for( uint32_t v = 0; v < materials[s]->textures.size(); v++)
			{
				CGparameter CgTexture = materials[s]->textures[v].first;
				cgGLSetTextureParameter( CgTexture, materials[s]->textures[v].second->image );
				cgGLEnableTextureParameter( CgTexture );
			}

			for( uint32_t v = 0; v < materials[s]->variables.size(); v++)
			{
				CGparameter CgVariable = materials[s]->variables[v].first;
				cgSetParameterValuefr( CgVariable, materials[s]->variables[v].second.size(), (float*)&materials[s]->variables[v].second[0] );
			}

			glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexBuffer );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffers[s] );

			// Set vertex attributes
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( (char *)NULL + (0) ) );
			glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( (char *)NULL + (12) ) );
			glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( (char *)NULL + (24) ) );
			glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( (char *)NULL + (32) ) );
			glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( (char *)NULL + (48) ) );

			// Enable vertex attributes
			glEnableVertexAttribArray( 0 );
			glEnableVertexAttribArray( 1 );
			glEnableVertexAttribArray( 2 );
			glEnableVertexAttribArray( 3 );
			glEnableVertexAttribArray( 4 );

			CGtechnique tech = cgGetFirstTechnique( materials[s]->shaderProgram->effect );
			CGpass pass;
			pass = cgGetFirstPass(tech);
			while (pass)
			{
				cgSetPassState(pass);
				int BufferSize;
				glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &BufferSize);
				glDrawElements( GL_TRIANGLES, BufferSize / sizeof( uint32_t ), GL_UNSIGNED_INT, 0 );
				cgResetPassState(pass);
				pass = cgGetNextPass(pass);
			}

			// Disable vertex attributes
			glDisableVertexAttribArray( 0 );
			glDisableVertexAttribArray( 1 );
			glDisableVertexAttribArray( 2 );
			glDisableVertexAttribArray( 3 );
			glDisableVertexAttribArray( 4 );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			for( uint32_t v = 0; v < materials[s]->textures.size(); v++)
			{
				CGparameter cgTexture = materials[s]->textures[v].first;
				cgGLDisableTextureParameter( cgTexture );
			}
		}
}

void RenderTarget::renderAutoLuminance()
{
	glBindTexture(GL_TEXTURE_2D, primaryTex);
	glGenerateMipmap(GL_TEXTURE_2D);
	float luminance;

	GLint width, height;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );

	glGetTexImage( GL_TEXTURE_2D, maxLevel( width, height), GL_LUMINANCE, GL_FLOAT, &luminance );
	eyeLuminance = eyeLuminance + ( ( ( luminance + 0.5f ) - eyeLuminance ) * 0.01f );
	eyeLuminance = std::max( 0.5f, std::min( 1.0f, eyeLuminance ) );

	// Auto luminance effect
	glBindFramebuffer( GL_FRAMEBUFFER, effectFrameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primaryTex, 0 );

	// Set texture
	CGparameter CgFSTexture = cgGetNamedEffectParameter( context->defaultMedia->shaders[5]->effect, "txDiffuse" );
	cgGLSetTextureParameter( CgFSTexture, primaryTex );
	cgGLEnableTextureParameter( CgFSTexture );

	// Set brightness
	CGparameter CgBrightness = cgGetNamedEffectParameter( context->defaultMedia->shaders[5]->effect, "Brightness" );
	cgGLSetParameter1f( CgBrightness, 1.0f / eyeLuminance );

	CGtechnique tech = cgGetFirstTechnique( context->defaultMedia->shaders[5]->effect );
	CGpass pass;
	pass = cgGetFirstPass(tech);
	while (pass)
	{
		cgSetPassState(pass);
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
		cgResetPassState( pass );
		pass = cgGetNextPass( pass );
	}

	cgGLDisableTextureParameter( CgFSTexture );
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::renderBloom()
{
	CGtechnique tech;
	CGpass pass;

	// Render bloom effect
	glBindFramebuffer( GL_FRAMEBUFFER, effectFrameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primaryBloomTex, 0 );

	GLint width, height;
	glBindTexture( GL_TEXTURE_2D, primaryBloomTex );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glViewport( 0, 0, width, height );

	CGparameter CgFSTexture = cgGetNamedEffectParameter( context->defaultMedia->shaders[3]->effect, "txDiffuse" );
	cgGLSetTextureParameter( CgFSTexture, primaryTex );
	cgGLEnableTextureParameter( CgFSTexture );

	// Set vertex attributes
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (0) ) );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (12) ) );

	// Enable vertex attributes
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );

	tech = cgGetFirstTechnique( context->defaultMedia->shaders[3]->effect );
	pass = cgGetFirstPass(tech);
	while (pass)
	{
		cgSetPassState(pass);
		glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( -1.0f,-1.0f, -1.0f );
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( 1.0f,-1.0f, -1.0f );
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f( 1.0f, 1.0f, -1.0f );
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f( -1.0f, 1.0f, -1.0f );
		glEnd();
		cgResetPassState( pass );
		pass = cgGetNextPass( pass );
	}

	tech = cgGetFirstTechnique( context->defaultMedia->shaders[3]->effect );
	pass = cgGetFirstPass( tech );
	while (pass)
	{
		cgSetPassState( pass );
		glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glEnd();
		cgResetPassState( pass );
		pass = cgGetNextPass( pass );
	}

	// Disable vertex attributes
	glDisableVertexAttribArray( 0 );
	glDisableVertexAttribArray( 1 );

	bool flipflop = true;
	for( float i = 0; i < (float)OvglPi; i = i + ( (float)(OvglPi) / bloom ) )
	{
		float x = sin(i) * 0.005f;
		float y = cos(i) * 0.005f;

		if( flipflop )
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, secondaryBloomTex, 0);
			CGparameter cgFSTexture2 = cgGetNamedEffectParameter( context->defaultMedia->shaders[2]->effect, "txDiffuse" );
			cgGLSetTextureParameter( cgFSTexture2, primaryBloomTex );
			cgGLEnableTextureParameter( cgFSTexture2 );
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primaryBloomTex, 0);
			CGparameter cgFSTexture2 = cgGetNamedEffectParameter( context->defaultMedia->shaders[2]->effect, "txDiffuse" );
			cgGLSetTextureParameter( cgFSTexture2, secondaryBloomTex );
			cgGLEnableTextureParameter( cgFSTexture2 );
		}
		CGparameter CgDirection2 = cgGetNamedEffectParameter( context->defaultMedia->shaders[2]->effect, "direction" );
		cgGLSetParameter2f( CgDirection2, x, y );

		// Set vertex attributes
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (0) ) );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (12) ) );

		// Enable vertex attributes
		glEnableVertexAttribArray( 0 );
		glEnableVertexAttribArray( 1 );

		tech = cgGetFirstTechnique( context->defaultMedia->shaders[2]->effect );
		pass = cgGetFirstPass( tech );
		while (pass)
		{
			cgSetPassState( pass );
			glBegin(GL_QUADS);
			glTexCoord2f( 0.0f, 0.0f );
			glVertex3f( -1.0f,-1.0f, -1.0f );
			glTexCoord2f( 1.0f, 0.0f );
			glVertex3f( 1.0f,-1.0f, -1.0f );
			glTexCoord2f( 1.0f, 1.0f );
			glVertex3f( 1.0f, 1.0f, -1.0f );
			glTexCoord2f( 0.0f, 1.0f );
			glVertex3f( -1.0f, 1.0f, -1.0f );
			glEnd();
			cgResetPassState( pass );
			pass = cgGetNextPass( pass );
		}

		// Disable vertex attributes
		glDisableVertexAttribArray( 0 );
		glDisableVertexAttribArray( 1 );
		flipflop = !flipflop;
	}

	// Get texture width and height.
	glBindTexture( GL_TEXTURE_2D, primaryTex );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primaryTex, 0 );
	glViewport( 0, 0, width, height );
	CGparameter CgFSTextureA = cgGetNamedEffectParameter( context->defaultMedia->shaders[4]->effect, "txDiffuse1" );
	cgGLSetTextureParameter( CgFSTextureA, primaryTex );
	cgGLEnableTextureParameter( CgFSTextureA );

	CGparameter CgFSTextureB = cgGetNamedEffectParameter( context->defaultMedia->shaders[4]->effect, "txDiffuse2" );
	cgGLSetTextureParameter( CgFSTextureB, primaryBloomTex );
	cgGLEnableTextureParameter( CgFSTextureB );

	// Set vertex attributes
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (0) ) );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (12) ) );

	// Enable vertex attributes
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );

	tech = cgGetFirstTechnique( context->defaultMedia->shaders[4]->effect );
	pass = cgGetFirstPass( tech );
	while (pass)
	{
		cgSetPassState( pass );
		glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glEnd();
		cgResetPassState( pass );
		pass = cgGetNextPass( pass );
	}

	// Disable vertex attributes
	glDisableVertexAttribArray( 0 );
	glDisableVertexAttribArray( 1 );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void RenderTarget::renderMotionBlur( )
{
	glBindFramebuffer( GL_FRAMEBUFFER, effectFrameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, secondaryTex, 0 );

	CGparameter cgFSTexture = cgGetNamedEffectParameter( context->defaultMedia->shaders[6]->effect, "sceneSampler" );
	cgGLSetTextureParameter( cgFSTexture, primaryTex );
	cgGLEnableTextureParameter( cgFSTexture );

	CGparameter cgFSTexture2 = cgGetNamedEffectParameter( context->defaultMedia->shaders[6]->effect, "depthTexture" );
	cgGLSetTextureParameter( cgFSTexture2, depthTexture );
	cgGLEnableTextureParameter( cgFSTexture2 );

	Matrix44 viewProj = matrixInverse( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), ( view->projMat * matrixInverse( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), view->getPose() ) ) );
	static Matrix44 previous_viewProj;
	CGparameter cgViewProjMatrix = cgGetNamedEffectParameter( context->defaultMedia->shaders[6]->effect, "g_ViewProjectionInverseMatrix" );
	Matrix44 tViewProj = matrixTranspose( viewProj );
	cgGLSetMatrixParameterfc( cgViewProjMatrix, (float*)&tViewProj );

	CGparameter cgPreviousViewProjMatrix = cgGetNamedEffectParameter( context->defaultMedia->shaders[6]->effect, "g_previousViewProjectionMatrix" );
	Matrix44 tPreviousViewProj = matrixTranspose(previous_viewProj );
	cgGLSetMatrixParameterfc( cgPreviousViewProjMatrix, (float*)&tPreviousViewProj );

	CGtechnique tech = cgGetFirstTechnique( context->defaultMedia->shaders[6]->effect );
	CGpass pass;
	pass = cgGetFirstPass( tech );
	while (pass)
	{
		cgSetPassState( pass );
		glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(1.0f,-1.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glEnd();
		cgResetPassState( pass );
		pass = cgGetNextPass( pass );
	}

	cgGLDisableTextureParameter( cgFSTexture );
	cgGLDisableTextureParameter( cgFSTexture2 );

	glBindTexture( GL_TEXTURE_2D, secondaryTex );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primaryTex, 0 );

	glBegin( GL_QUADS );
	glTexCoord2f( 0.0f, 0.0f );
	glVertex3f(-1.0f,-1.0f, -1.0f);
	glTexCoord2f( 1.0f, 0.0f );
	glVertex3f(1.0f,-1.0f, -1.0f);
	glTexCoord2f( 1.0f, 1.0f );
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f( 0.0f, 1.0f );
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	previous_viewProj = view->projMat * matrixInverse( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), view->getPose() );
}

void RenderTarget::renderMarker( const Matrix44& matrix )
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Matrix44 mat = ( matrix * matrixInverse( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), view->getPose() ) );
	glLoadMatrixf( (float*)&mat );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glLoadMatrixf( (float*)&view->projMat );
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );
	glLineWidth( 1.0f );

	// Draw X axis
	glColor3f( 1.0f, 0.0f, 0.0f );
	glBegin( GL_LINES );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 1.0f, 0.0f, 0.0f );
	glEnd();

	// Draw Y axis
	glColor3f( 0.0f, 1.0f, 0.0f );
	glBegin( GL_LINES );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 1.0f, 0.0f );
	glEnd();

	// Draw Z axis
	glColor3f( 0.0f, 0.0f, 1.0f );
	glBegin( GL_LINES );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 1.0f );
	glEnd();
}

void RenderTarget::render()
{
	// Get the window's rect
	Ovgl::Rect windowRect;
	Ovgl::Rect adjustedRect;

	if(window)
	{
		SDL_GetWindowPosition( window->sdlWindow, &windowRect.left, &windowRect.top );
		SDL_GetWindowSize( window->sdlWindow, &windowRect.right, &windowRect.bottom );
		windowRect.right += windowRect.left;
		windowRect.bottom += windowRect.top;
		adjustedRect = windowAdjustedRect( window, &rect );
		SDL_GL_MakeCurrent( window->sdlWindow, window->windowContext );
	}
	else
	{
		windowRect.left = 0;
		windowRect.top = 0;
		glBindTexture( GL_TEXTURE_2D, hTex->image );
		GLint glwidth, glheight;
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &glwidth );
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &glheight );
		windowRect.right = glwidth;
		windowRect.bottom = glheight;
		adjustedRect = textureAdjustedRect( hTex, &rect );
	}

	int width = (int)( adjustedRect.right - adjustedRect.left );
	int height = (int)( adjustedRect.bottom - adjustedRect.top );

	if( view != NULL )
	{
		Scene* scene = view->scene;
		
		// Create light arrays and render shadowmaps
		float LightCount = (float)scene->lights.size();
		std::vector< float > mLights;
		std::vector< float > lightColors;
		for( uint32_t l = 0; l < scene->lights.size(); l++)
		{
			Light* light = scene->lights[l];
			mLights.push_back( light->getPose()._41 );
			mLights.push_back( light->getPose()._42 );
			mLights.push_back( light->getPose()._43 );
			mLights.push_back( 1.0f );
			lightColors.push_back( light->color.x );
			lightColors.push_back( light->color.y );
			lightColors.push_back( light->color.z );
			lightColors.push_back( 1.0f );

			glBindFramebuffer( GL_FRAMEBUFFER, light->shadowFrameBuffer );
			for( uint32_t PostRender = 0; PostRender < 2; PostRender++ )
			{
				// Render Objects
				for( uint32_t i = 0; i < scene->objects.size(); i++ )
				{
					std::vector<Matrix44> temp(1);
					temp[0] = scene->objects[i]->getPose();
					light->renderShadow( *scene->objects[i]->mesh, scene->objects[i]->getPose(), temp, !!PostRender );
				}

				// Render props
				for( uint32_t i = 0; i < scene->props.size(); i++ )
				{
					light->renderShadow( *scene->props[i]->mesh, scene->props[i]->getPose(), scene->props[i]->matrices, !!PostRender );
				}

				// Render actors
				for( uint32_t i = 0; i < scene->actors.size(); i++ )
				{
					if( scene->actors[i]->mesh )
					{
						light->renderShadow( *scene->actors[i]->mesh, scene->actors[i]->getPose(), scene->actors[i]->pose->matrices, !!PostRender );
					}
				}
			}
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}

		glBindFramebuffer( GL_FRAMEBUFFER, multiSampleFrameBuffer );

		// Set the viewport to fit the window
		glViewport( 0, 0, width, height );


		// Clear depth buffer
		glDepthMask( GL_TRUE );
		glClear( GL_DEPTH_BUFFER_BIT );

		if( scene->skyBox )
		{
			// Disable depth test
			glDisable( GL_DEPTH_TEST );
			glDepthMask( GL_FALSE );

			glDisable( GL_MULTISAMPLE );

			// Set skybox shader View variable
			CGparameter CgView = cgGetNamedEffectParameter( context->defaultMedia->shaders[1]->effect, "View" );
			Matrix44 tinvView = matrixTranspose( matrixInverse( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), view->getPose() ) );
			cgGLSetMatrixParameterfc( CgView, (float*)&tinvView );

			// Set skybox shader Projection variable
			CGparameter CgProjection = cgGetNamedEffectParameter( context->defaultMedia->shaders[1]->effect, "Projection" );
			Matrix44 tView = matrixTranspose( view->projMat );
			cgGLSetMatrixParameterfc( CgProjection, (float*)&tView );

			// Set skybox texture
			CGparameter CgFSTexture = cgGetNamedEffectParameter( context->defaultMedia->shaders[1]->effect, "txSkybox" );
			cgGLSetTextureParameter( CgFSTexture, scene->skyBox->image );
			cgGLEnableTextureParameter( CgFSTexture );

			// Bind vertex and index buffers
			glBindBuffer( GL_ARRAY_BUFFER, context->defaultMedia->meshes[0]->vertexBuffer );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, context->defaultMedia->meshes[0]->indexBuffers[0] );

			// Set vertex attributes
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (0) ) );
			glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (12) ) );
			glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (24) ) );
			glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (32) ) );
			glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( (char *)NULL + (48) ) );

			// Enable vertex attributes
			glEnableVertexAttribArray( 0 );
			glEnableVertexAttribArray( 1 );
			glEnableVertexAttribArray( 2 );
			glEnableVertexAttribArray( 3 );
			glEnableVertexAttribArray( 4 );

			// Draw skybox
			CGtechnique tech = cgGetFirstTechnique( context->defaultMedia->shaders[1]->effect );
			CGpass pass = cgGetFirstPass( tech );
			while( pass )
			{
				cgSetPassState( pass );
				glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
				cgResetPassState( pass );
				pass = cgGetNextPass( pass );
			}

			// Disable vertex attributes
			glDisableVertexAttribArray( 0 );
			glDisableVertexAttribArray( 1 );
			glDisableVertexAttribArray( 2 );
			glDisableVertexAttribArray( 3 );
			glDisableVertexAttribArray( 4 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			cgGLDisableTextureParameter( CgFSTexture );
		}
		else
		{
			glClearColor( 0.0f, 0.0f, 1.0f, 0.0f );
			glClear( GL_COLOR_BUFFER_BIT );
		}

		// Multisample.
		if( multiSample )
		{
			glEnable( GL_MULTISAMPLE );
		}
		else
		{
			glDisable( GL_MULTISAMPLE );
		}

		for( uint32_t PostRender = 0; PostRender < 2; PostRender++ )
		{
			// Render Objects
			for( uint32_t i = 0; i < scene->objects.size(); i++ )
			{
				std::vector<Matrix44> temp(1);
				temp[0] = scene->objects[i]->getPose();
				renderMesh( *scene->objects[i]->mesh, scene->objects[i]->getPose(), temp, scene->objects[i]->materials, !!PostRender );
			}

			// Render props
			for( uint32_t i = 0; i < scene->props.size(); i++ )
			{
				renderMesh( *scene->props[i]->mesh, scene->props[i]->getPose(), scene->props[i]->matrices, scene->props[i]->materials, !!PostRender );
			}

			// Render actors
			for( uint32_t i = 0; i < scene->actors.size(); i++ )
			{
				if( scene->actors[i]->mesh )
				{
					renderMesh( *scene->actors[i]->mesh, scene->actors[i]->getPose(), scene->actors[i]->pose->matrices, scene->actors[i]->materials, !!PostRender );
				}
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glColor3f( 1.0f, 1.0f, 1.0f );
		glDisable( GL_MULTISAMPLE );
		glDisable( GL_DEPTH_TEST );
		glDepthMask( GL_FALSE );
		glDisable( GL_LIGHTING );
		glEnable( GL_TEXTURE_2D );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		// Blit MultiSampleTexture to BaseTexture to apply effects.
		glBindFramebuffer( GL_READ_FRAMEBUFFER, multiSampleFrameBuffer );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, effectFrameBuffer );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primaryTex, 0 );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );
		glBlitFramebuffer( 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST );
		glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );


		if( autoLuminance )
		{
			renderAutoLuminance();
		}

		if( bloom )
		{
			renderBloom();
		}

		if( motionBlur )
		{
			renderMotionBlur();
		}

		glViewport( 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top );

		// Get viewport
		GLint iViewport[4];
		glGetIntegerv( GL_VIEWPORT, iViewport );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		// Set up the orthographic projection
		glOrtho( iViewport[0], iViewport[0] + iViewport[2], iViewport[1] + iViewport[3], iViewport[1], -1, 1 );

		if(hTex)
		{
			glBindFramebuffer( GL_FRAMEBUFFER, effectFrameBuffer );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hTex->image, 0 );
		}

		// Render to screen
		glBindTexture( GL_TEXTURE_2D, primaryTex );
		glBegin( GL_QUADS );
		glTexCoord2i( 1, 1 );
		glVertex2i( adjustedRect.right, adjustedRect.top );
		glTexCoord2i( 0, 1 );
		glVertex2i( adjustedRect.left, adjustedRect.top );
		glTexCoord2i( 0, 0 );
		glVertex2i( adjustedRect.left, adjustedRect.bottom );
		glTexCoord2i( 1, 0 );
		glVertex2i( adjustedRect.right, adjustedRect.bottom );
		glEnd();
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glColor3f( 1.0f, 1.0f, 1.0f );
	glDisable( GL_MULTISAMPLE);
	glDisable( GL_DEPTH_TEST);
	glDepthMask( GL_FALSE );
	glDisable( GL_LIGHTING );
	glEnable( GL_TEXTURE_2D );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glViewport( 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top );

	// Get viewport
	GLint iViewport[4];
	glGetIntegerv( GL_VIEWPORT, iViewport );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// Set up the orthographic projection
	glOrtho( iViewport[0], iViewport[0] + iViewport[2], iViewport[1] + iViewport[3], iViewport[1], -1, 1 );

	glEnable( GL_BLEND );
	glEnable( GL_STENCIL_TEST );
	for( uint32_t i = 0; i < interfaces.size(); i++ )
	{
		Ovgl::Rect interfaceRect;
		interfaceRect.left = ((adjustedRect.right - adjustedRect.left) * interfaces[i]->rect.left.scale) + interfaces[i]->rect.left.offset + adjustedRect.left;
		interfaceRect.top = ((adjustedRect.bottom - adjustedRect.top) * interfaces[i]->rect.top.scale) + interfaces[i]->rect.top.offset + adjustedRect.top;
		interfaceRect.right = ((adjustedRect.right - adjustedRect.left) * interfaces[i]->rect.right.scale) + interfaces[i]->rect.right.offset + adjustedRect.left;
		interfaceRect.bottom = ((adjustedRect.bottom - adjustedRect.top) * interfaces[i]->rect.bottom.scale) + interfaces[i]->rect.bottom.offset + adjustedRect.top;
		interfaces[i]->render( interfaceRect );
	}
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_BLEND );

	SDL_GL_MakeCurrent( NULL, NULL);
}

void RenderTarget::update()
{
	SDL_GL_MakeCurrent( window->sdlWindow, window->windowContext );
	Ovgl::Rect adjustedRect = windowAdjustedRect( window, &rect );

	int width = (int)(adjustedRect.right - adjustedRect.left);
	int height = (int)(adjustedRect.bottom - adjustedRect.top);

	// Delete FrameBuffers and Textures
	if(multiSampleFrameBuffer) glDeleteFramebuffers( 1, &multiSampleFrameBuffer );
	if(effectFrameBuffer)glDeleteFramebuffers( 1, &effectFrameBuffer );
	if(colorBuffer)glDeleteRenderbuffers( 1, &colorBuffer );
	if(depthBuffer)glDeleteRenderbuffers( 1, &depthBuffer );
	if(colorTexture)glDeleteTextures( 1, &colorTexture );
	if(depthTexture)glDeleteTextures( 1, &depthTexture );
	if(primaryTex)glDeleteTextures( 1, &primaryTex );
	if(secondaryTex)glDeleteTextures( 1, &secondaryTex );
	if(primaryBloomTex)glDeleteTextures( 1, &primaryBloomTex );
	if(secondaryBloomTex)glDeleteTextures( 1, &secondaryBloomTex );

	// Multi sample framebuffer
	glGenFramebuffers( 1, &multiSampleFrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, multiSampleFrameBuffer );

	// Multi sample colorbuffer
	glGenRenderbuffers( 1, &colorBuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, colorBuffer );
	glRenderbufferStorageMultisample( GL_RENDERBUFFER, 4, GL_RGBA16F, width, height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer );

	// Multi sample depthbuffer
	glGenRenderbuffers( 1, &depthBuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer );
	glRenderbufferStorageMultisample( GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT32, width, height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer );

	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	// Effect framebuffer
	glGenFramebuffers( 1, &effectFrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, effectFrameBuffer );

	// Create and bind textures
	glGenTextures( 1, &depthTexture );
	glBindTexture( GL_TEXTURE_2D, depthTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );

	glGenTextures( 1, &primaryTex );
	glBindTexture( GL_TEXTURE_2D, primaryTex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel( width, height ) );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primaryTex, 0 );

	glGenTextures( 1, &secondaryTex );
	glBindTexture( GL_TEXTURE_2D, secondaryTex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	glGenTextures( 1, &primaryBloomTex );
	glBindTexture( GL_TEXTURE_2D, primaryBloomTex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width / 4, height / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	glGenTextures( 1, &secondaryBloomTex );
	glBindTexture( GL_TEXTURE_2D, secondaryBloomTex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width / 4, height / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	SDL_GL_MakeCurrent( NULL, NULL );
}

void RenderTarget::doEvent( Event event )
{
	Ovgl::Rect adjustedRect;
	adjustedRect = windowAdjustedRect( window, &rect );
	switch( event.type )
	{
		case OVGL_KEYDOWN:
			if( onKeyDown )
			{
				onKeyDown( event.key);
			}
			for( uint32_t i = 0; i < interfaces.size(); i++ )
			{
				interfaces[i]->doEvent( event, adjustedRect );
			}
			break;

		case OVGL_KEYUP:
			if( onKeyUp )
			{
				onKeyUp( event.key );
			}
			for( uint32_t i = 0; i < interfaces.size(); i++ )
			{
				interfaces[i]->doEvent( event, adjustedRect );
			}
			break;
		case OVGL_MOUSEMOTION:
			if( event.mouseX > adjustedRect.left && event.mouseX < adjustedRect.right )
				if( event.mouseY > adjustedRect.top && event.mouseY < adjustedRect.bottom )
				{
					event.mouseX -= adjustedRect.left;
					event.mouseY -= adjustedRect.top;
					if(onMouseMove)
					{
						onMouseMove( event.mouseX, event.mouseY );
					}
					for( uint32_t i = 0; i < interfaces.size(); i++ )
					{
						Ovgl::Rect interfaceRect = renderTargetAdjustedRect( this, &interfaces[i]->rect );
						if( event.mouseX > interfaceRect.left && event.mouseX < interfaceRect.right )
							if( event.mouseY > interfaceRect.top && event.mouseY < interfaceRect.bottom )
							{
								Event interfaceEvent = event;
								interfaceEvent.mouseX -= interfaceRect.left;
								interfaceEvent.mouseY -= interfaceRect.top;
								interfaces[i]->doEvent( interfaceEvent, interfaceRect );
							}
					}
				}
			break;
		case OVGL_MOUSEBUTTONDOWN:
			if( event.mouseY > adjustedRect.left && event.mouseX < adjustedRect.right )
				if( event.mouseY > adjustedRect.top && event.mouseY < adjustedRect.bottom )
				{
					event.mouseX -= adjustedRect.left;
					event.mouseY -= adjustedRect.top;
					if(onMouseDown)
					{
						onMouseDown( event.mouseX, event.mouseY, event.button );
					}
					for( uint32_t i = 0; i < interfaces.size(); i++ )
					{
						Ovgl::Rect interfaceRect = renderTargetAdjustedRect( this, &interfaces[i]->rect );
						if( event.mouseX > interfaceRect.left && event.mouseX < interfaceRect.right )
							if( event.mouseY > interfaceRect.top && event.mouseY < interfaceRect.bottom )
							{
								Event interfaceEvent = event;
								interfaceEvent.mouseX -= interfaceRect.left;
								interfaceEvent.mouseY -= interfaceRect.top;
								interfaces[i]->doEvent( interfaceEvent, interfaceRect );
							}
					}
				}
			break;
		case OVGL_MOUSEBUTTONUP:
			if( event.mouseX > adjustedRect.left && event.mouseX < adjustedRect.right )
				if( event.mouseY > adjustedRect.top && event.mouseY < adjustedRect.bottom )
				{
					event.mouseX -= adjustedRect.left;
					event.mouseY -= adjustedRect.top;
					if(onMouseUp)
					{
						onMouseUp( event.mouseX, event.mouseY, event.button );
					}
					for( uint32_t i = 0; i < interfaces.size(); i++ )
					{
						Ovgl::Rect interfaceRect = renderTargetAdjustedRect( this, &interfaces[i]->rect );
						if( event.mouseX > interfaceRect.left && event.mouseX < interfaceRect.right )
							if( event.mouseY > interfaceRect.top && event.mouseY < interfaceRect.bottom )
							{
								Event interfaceEvent = event;
								interfaceEvent.mouseX -= interfaceRect.left;
								interfaceEvent.mouseY -= interfaceRect.top;
								interfaces[i]->doEvent( interfaceEvent, interfaceRect );
							}
					}
				}
			break;

		default:
			break;
	}
}

Interface::Interface( Interface* parent, const URect& rect )
{
	this->rect = rect;
	background = NULL;
	tileX = false;
	tileY = false;
	wordBreak = true;
	color = parent->color;
	align = 0;
	hScroll = 0;
	vScroll = 0;
	onKeyDown = NULL;
	onKeyUp = NULL;
	onMouseMove = NULL;
	onMouseDown = NULL;
	onMouseUp = NULL;
	onMouseOver = NULL;
	onMouseOut = NULL;
	renderTarget = parent->renderTarget;
	parent->children.push_back(this);
}

Interface::Interface( RenderTarget* parent, const URect& rect )
{
	this->rect = rect;
	background = NULL;
	tileX = false;
	tileY = false;
	wordBreak = true;
	color = Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
	textColor = Vector4( 0.0f, 0.0f, 0.0f, 1.0f );
	align = 0;
	hScroll = 0;
	vScroll = 0;
	onKeyDown = NULL;
	onKeyUp = NULL;
	onMouseMove = NULL;
	onMouseDown = NULL;
	onMouseUp = NULL;
	onMouseOver = NULL;
	onMouseOut = NULL;
	renderTarget = parent;
	parent->interfaces.push_back(this);
}

Interface::~Interface()
{

}

void Interface::render( const Ovgl::Rect& adjustedRect )
{
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	if( background )
	{
		glBindTexture(GL_TEXTURE_2D, background->image );

	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0 );
	}

	glColor4f(color.x, color.y, color.z, color.w);

	float tileWidth = 1.0f;
	float tileHeight = 1.0f;

	if(tileX)
	{
		GLint width;
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		tileWidth = (float)(adjustedRect.right - adjustedRect.left) / (float)width;
	}
	if(tileY)
	{
		GLint height;
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		tileHeight = (float)(adjustedRect.bottom - adjustedRect.top) / (float)height;
	}
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glBegin( GL_QUADS );
	glTexCoord2f( tileWidth, 0 );
	glVertex2i( adjustedRect.right, adjustedRect.top );
	glTexCoord2f( 0, 0 );
	glVertex2i( adjustedRect.left, adjustedRect.top );
	glTexCoord2f( 0, tileHeight );
	glVertex2i( adjustedRect.left, adjustedRect.bottom );
	glTexCoord2f( tileWidth, tileHeight );
	glVertex2i( adjustedRect.right, adjustedRect.bottom );
	glEnd();

	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glColor4f(textColor.x, textColor.y, textColor.z, textColor.w);

	int32_t x;
	if(align == 0)
	{
		x = adjustedRect.left;
		int32_t y = adjustedRect.top + vScroll;
		for( uint32_t i = 0; i < text.size(); i++ )
		{
			int32_t wordWidth = 0;
			int32_t charWidth, charHeight;
			if(wordBreak)
			{
				uint32_t wi = i + 1;
				while(text[wi] != 32 && wi < text.size())
				{
					glBindTexture(GL_TEXTURE_2D, font->charSet[ text[wi] ]);
					glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &charWidth);
					wordWidth += charWidth;
					wi++;
				}
			}
			glBindTexture(GL_TEXTURE_2D, font->charSet[ text[i] ]);
			glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &charWidth);
			glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &charHeight);
			if(x + charWidth + wordWidth > adjustedRect.right)
			{
				y += font->size;
				x = adjustedRect.left;
			}
			if(charWidth == 0)
			{
				charWidth = font->size / 2;
			}
			uint32_t charOffset = font->size - font->charOffsets[text[i]];
			glBegin( GL_QUADS );
			glTexCoord2f( 1, 0 );
			glVertex2i( x + charWidth, y + charOffset );
			glTexCoord2f( 0, 0 );
			glVertex2i( x, y + charOffset );
			glTexCoord2f( 0, 1 );
			glVertex2i( x, y + charHeight + charOffset );
			glTexCoord2f( 1, 1 );
			glVertex2i( x + charWidth, y + charHeight + charOffset );
			glEnd();
			x += charWidth + 2;
		}
	}
	else if(align == 1)
	{
		x = adjustedRect.right;
		int32_t y = adjustedRect.top + vScroll;
		for( int32_t i = text.size()-1; i >= 0 ; i-- )
		{
			int32_t wordWidth = 0;
			int32_t charWidth, charHeight;
			if(wordBreak)
			{
				int32_t wi = i - 1;
				while(text[wi] != 32 && wi >= 0)
				{
					glBindTexture(GL_TEXTURE_2D, font->charSet[ text[wi] ]);
					glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &charWidth);
					wordWidth += charWidth;
					wi--;
				}
			}
			glBindTexture(GL_TEXTURE_2D, font->charSet[ text[i] ]);
			glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &charWidth);
			glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &charHeight);
			x -= charWidth + 2;
			if(x - (charWidth + wordWidth) < (int32_t)adjustedRect.left)
			{
				y -= font->size;
				x = adjustedRect.right;
			}
			if(charWidth == 0)
			{
				charWidth = font->size / 2;
			}
			uint32_t charOffset = font->size - font->charOffsets[text[i]];
			glBegin( GL_QUADS );
			glTexCoord2f( 1, 0 );
			glVertex2i( x + charWidth, y + charOffset );
			glTexCoord2f( 0, 0 );
			glVertex2i( x, y + charOffset );
			glTexCoord2f( 0, 1 );
			glVertex2i( x, y + charHeight + charOffset );
			glTexCoord2f( 1, 1 );
			glVertex2i( x + charWidth, y + charHeight + charOffset );
			glEnd();

		}
	}

	for( uint32_t c = 0; c < children.size(); c++ )
	{
		Ovgl::Rect childRect;
		childRect.left = ((adjustedRect.right - adjustedRect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset + adjustedRect.left;
		childRect.top = ((adjustedRect.bottom - adjustedRect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset + adjustedRect.top;
		childRect.right = ((adjustedRect.right - adjustedRect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset + adjustedRect.left;
		childRect.bottom = ((adjustedRect.bottom - adjustedRect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset + adjustedRect.top;
		children[c]->render( childRect );
	}
}

void Interface::doEvent(Event event, const Rect& adjustedRect)
{
	switch (event.type)
	{
		case OVGL_KEYDOWN:
			if(onKeyDown)
			{
				onKeyDown( event.key );
			}
			break;
		case OVGL_KEYUP:
			if(onKeyUp)
			{
				onKeyUp( event.key );
			}
			break;
		case OVGL_MOUSEMOTION:
			if(onMouseMove)
			{
				onMouseMove( event.mouseX, event.mouseY );
			}
			for( uint32_t c = 0; c < children.size(); c++ )
			{
				Ovgl::Rect childRect;
				childRect.left = ((adjustedRect.right - adjustedRect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset ;
				childRect.top = ((adjustedRect.bottom - adjustedRect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset ;
				childRect.right = ((adjustedRect.right - adjustedRect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset ;
				childRect.bottom = ((adjustedRect.bottom - adjustedRect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset ;
				if( event.mouseX > childRect.left && event.mouseX < childRect.right )
					if( event.mouseY > childRect.top && event.mouseY < childRect.bottom )
					{
						Event interfaceEvent = event;
						interfaceEvent.mouseX -= childRect.left;
						interfaceEvent.mouseY -= childRect.top;
						children[c]->doEvent( interfaceEvent, childRect );
					}
			}
			break;
		case OVGL_MOUSEBUTTONDOWN:
			if(onMouseDown)
			{
				onMouseDown( event.mouseX, event.mouseY, event.button );
			}
			for( uint32_t c = 0; c < children.size(); c++ )
			{
				Ovgl::Rect childRect;
				childRect.left = ((adjustedRect.right - adjustedRect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset ;
				childRect.top = ((adjustedRect.bottom - adjustedRect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset ;
				childRect.right = ((adjustedRect.right - adjustedRect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset ;
				childRect.bottom = ((adjustedRect.bottom - adjustedRect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset ;
				if( event.mouseX > childRect.left && event.mouseX < childRect.right )
					if( event.mouseY > childRect.top && event.mouseY < childRect.bottom )
					{
						Event interfaceEvent = event;
						interfaceEvent.mouseX -= childRect.left;
						interfaceEvent.mouseY -= childRect.top;
						children[c]->doEvent(interfaceEvent, childRect);
					}
			}
			break;
		case OVGL_MOUSEBUTTONUP:
			if(onMouseUp)
			{
				onMouseUp( event.mouseX, event.mouseY, event.button );
			}
			for( uint32_t c = 0; c < children.size(); c++ )
			{
				Ovgl::Rect childRect;
				childRect.left = ((adjustedRect.right - adjustedRect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset ;
				childRect.top = ((adjustedRect.bottom - adjustedRect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset ;
				childRect.right = ((adjustedRect.right - adjustedRect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset ;
				childRect.bottom = ((adjustedRect.bottom - adjustedRect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset ;
				if( event.mouseX > childRect.left && event.mouseX < childRect.right )
					if( event.mouseY > childRect.top && event.mouseY < childRect.bottom )
					{
						Event interfaceEvent = event;
						interfaceEvent.mouseX -= childRect.left; interfaceEvent.mouseY -= childRect.top;
						children[c]->doEvent(interfaceEvent, childRect);
					}
			}
			break;

		default:
			break;
	}
}

void Interface::setText( const std::string& text )
{
	this->text = text;
}

}
