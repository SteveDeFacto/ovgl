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

#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglMedia.h"
#include "OvglGraphics.h"
#include "OvglScene.h"
#include "OvglAudio.h"
#include "OvglMesh.h"
#include "OvglWindow.h"
#include "OvglSkeleton.h"

namespace Ovgl
{

	// This function is used to find the maximum number of mipmaps that can be produced from the given resolution.
	uint32_t MaxLevel( uint32_t width, uint32_t height)
	{
		uint32_t max = 0;
		uint32_t highest = 0;
		if( width > height)
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

    Ovgl::Rect WindowAdjustedRect( Window* window, URect* rect)
	{
        // Get the window's rect
        Ovgl::Rect WindowRect;

        WindowRect.left = window->hWnd->getPosition().x;
        WindowRect.top = window->hWnd->getPosition().y;
        WindowRect.right = window->hWnd->getPosition().x + window->hWnd->getSize().x;
        WindowRect.bottom = window->hWnd->getPosition().y + window->hWnd->getSize().y;

        Ovgl::Rect adjustedrect;
        adjustedrect.left = ((WindowRect.right - WindowRect.left) * rect->left.scale) + rect->left.offset;
        adjustedrect.top = ((WindowRect.bottom - WindowRect.top) * rect->top.scale) + rect->left.offset;
        adjustedrect.right = ((WindowRect.right - WindowRect.left) * rect->right.scale) + rect->right.offset;
        adjustedrect.bottom = ((WindowRect.bottom - WindowRect.top) * rect->bottom.scale) + rect->bottom.offset;

        return adjustedrect;
	}

    Ovgl::Rect TextureAdjustedRect( Texture* texture, URect* rect)
	{
        // Get the window's rect
        Ovgl::Rect WindowRect;

        glBindTexture(GL_TEXTURE_2D, texture->Image);
        GLint width, height;
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        WindowRect.left =0;
        WindowRect.top =0;
        WindowRect.right = width;
        WindowRect.bottom = height;

        Ovgl::Rect adjustedrect;
        adjustedrect.left = ((WindowRect.right - WindowRect.left) * rect->left.scale) + rect->left.offset;
        adjustedrect.top = ((WindowRect.bottom - WindowRect.top) * rect->top.scale) + rect->left.offset;
        adjustedrect.right = ((WindowRect.right - WindowRect.left) * rect->right.scale) + rect->right.offset;
        adjustedrect.bottom = ((WindowRect.bottom - WindowRect.top) * rect->bottom.scale) + rect->bottom.offset;
        return adjustedrect;
	}

    RenderTarget::RenderTarget( Instance* Instance, Window* hWindow, const URect& viewport, uint32_t flags )
	{
		Inst = Instance;
		hWin = hWindow;
		hTex = NULL;
		View = NULL;
		debugMode = false;
        autoLuminance = false;
        bloom = 0;
        motionBlur = false;
        multiSample = false;
		eye_luminance = 0.0f;
        rect = viewport;
        MultiSampleFrameBuffer = 0;
        EffectFrameBuffer = 0;
        ColorBuffer = 0;
        DepthBuffer = 0;
        PrimaryTex = 0;
        SecondaryTex = 0;
        PrimaryBloomTex = 0;
        SecondaryBloomTex = 0;
        Update();
		hWin->RenderTargets.push_back(this);
	};

    RenderTarget::RenderTarget( Instance* Instance, Texture* hTexture, const URect& viewport, uint32_t flags )
	{
		Inst = Instance;
		hWin = NULL;
		hTex = hTexture;
		View = NULL;
		debugMode = false;
		autoLuminance = true;
		bloom = 4;
		motionBlur = true;
		multiSample = true;
		eye_luminance = 0.0f;
        rect = viewport;
        MultiSampleFrameBuffer = 0;
        EffectFrameBuffer = 0;
        ColorBuffer = 0;
        DepthBuffer = 0;
        PrimaryTex = 0;
        SecondaryTex = 0;
        PrimaryBloomTex = 0;
        SecondaryBloomTex = 0;
        Update();
        Inst->hWnd->setActive(false);
	};

	void RenderTarget::Release()
	{
		for( uint32_t r = 0; r < hWin->RenderTargets.size(); r++)
		{
			if(hWin->RenderTargets[r] == this)
			{
				hWin->RenderTargets.erase( hWin->RenderTargets.begin() + r );
			}
		}
		delete this;
	}

	void RenderTarget::RenderMesh( const Mesh& mesh, const Matrix44& matrix, std::vector< Matrix44 >& pose, std::vector< Material* >& materials, bool PostRender )
	{
        Matrix44 viewProj = (MatrixInverse( Vector4(0,0,0,0), View->getPose() ) * View->projMat);
        Matrix44 worldMat = (matrix * viewProj );
        glLoadMatrixf((float*)&worldMat);

        float LightCount = (float)View->scene->lights.size();
        std::vector< float > mLights;
        std::vector< float > LightColors;
        for( uint32_t l = 0; l < View->scene->lights.size(); l++)
        {
            mLights.push_back( View->scene->lights[l]->getPose()._41 );
            mLights.push_back( View->scene->lights[l]->getPose()._42 );
            mLights.push_back( View->scene->lights[l]->getPose()._43 );
            mLights.push_back( 1.0f );
            LightColors.push_back( View->scene->lights[l]->color.x );
            LightColors.push_back( View->scene->lights[l]->color.y );
            LightColors.push_back( View->scene->lights[l]->color.z );
            LightColors.push_back( 1.0f );
        }

        for( uint32_t s = 0; s < mesh.subset_count; s++)
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

            CGparameter CgWorldMatrix = cgGetNamedEffectParameter( materials[s]->ShaderProgram->effect, "World" );
            Matrix44 tWorldMat = MatrixTranspose(worldMat);
            cgGLSetMatrixParameterfc( CgWorldMatrix, (float*)&tWorldMat );
            CGparameter CgViewProjMatrix = cgGetNamedEffectParameter( materials[s]->ShaderProgram->effect, "ViewProj" );
            Matrix44 tViewProj = MatrixTranspose(viewProj);
            cgGLSetMatrixParameterfc( CgViewProjMatrix, (float*)&tViewProj );
            CGparameter CgViewPos= cgGetNamedEffectParameter( materials[s]->ShaderProgram->effect, "ViewPos" );
            cgGLSetParameter4f( CgViewPos, View->getPose()._41, View->getPose()._42, View->getPose()._43, View->getPose()._44 );

            CGparameter CgBoneMatrices = cgGetNamedEffectParameter( materials[s]->ShaderProgram->effect, "Bones" );
            for( uint32_t v = 0; v < pose.size(); v++)
            {
                CGparameter CgBone = cgGetArrayParameter( CgBoneMatrices, v );
                Matrix44 tPose = MatrixTranspose(pose[v]);
                cgGLSetMatrixParameterfc(CgBone, (float*)&tPose);
            }

            CGparameter CgLightCount = cgGetNamedEffectParameter( materials[s]->ShaderProgram->effect, "LightCount" );
            cgGLSetParameter1f( CgLightCount, LightCount );

            CGparameter CgLights = cgGetNamedEffectParameter( materials[s]->ShaderProgram->effect, "Lights" );
            CGparameter CgLight;
            for( uint32_t v = 0; v < mLights.size() / 4; v++)
            {
                CgLight=cgGetArrayParameter( CgLights, v );
                cgGLSetParameter4fv(CgLight, (float*)&mLights[v * 4]);
            }

            CGparameter CgLightColors = cgGetNamedEffectParameter( materials[s]->ShaderProgram->effect, "LightColors" );
            CGparameter CgLightColor;
            for( uint32_t v = 0; v < LightColors.size() / 4; v++)
            {
                CgLightColor=cgGetArrayParameter( CgLightColors, v );
                cgGLSetParameter4fv(CgLightColor, (float*)&LightColors[v * 4]);
            }

            for( uint32_t v = 0; v < materials[s]->Textures.size(); v++)
            {
                CGparameter CgTexture = materials[s]->Textures[v].first;
                cgGLSetTextureParameter( CgTexture, materials[s]->Textures[v].second->Image );
                cgGLEnableTextureParameter( CgTexture );
            }

            for( uint32_t v = 0; v < materials[s]->Variables.size(); v++)
            {
                CGparameter CgVariable = materials[s]->Variables[v].first;
                cgSetParameterValuefr( CgVariable, materials[s]->Variables[v].second.size(), (float*)&materials[s]->Variables[v].second[0] );
            }

            glBindBufferARB( GL_ARRAY_BUFFER, mesh.vertex_buffer );
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffers[s] );

			// Set vertex attributes
			glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
			glVertexAttribPointerARB( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );
			glVertexAttribPointerARB( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (24)) );
			glVertexAttribPointerARB( 3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (32)) );
			glVertexAttribPointerARB( 4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (48)) );

			// Enable vertex attributes
			glEnableVertexAttribArrayARB( 0 );
			glEnableVertexAttribArrayARB( 1 );
			glEnableVertexAttribArrayARB( 2 );
			glEnableVertexAttribArrayARB( 3 );
			glEnableVertexAttribArrayARB( 4 );

            CGtechnique tech = cgGetFirstTechnique( materials[s]->ShaderProgram->effect );
            CGpass pass;
            pass = cgGetFirstPass(tech);
            while (pass)
            {
                cgSetPassState(pass);
                int BufferSize;
                glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &BufferSize);
                glDrawElements( GL_TRIANGLES, BufferSize / sizeof(uint32_t), GL_UNSIGNED_INT, 0 );
                cgResetPassState(pass);
                pass = cgGetNextPass(pass);
            }

			// Disable vertex attributes
			glDisableVertexAttribArrayARB( 0 );
			glDisableVertexAttribArrayARB( 1 );
			glDisableVertexAttribArrayARB( 2 );
			glDisableVertexAttribArrayARB( 3 );
			glDisableVertexAttribArrayARB( 4 );

            glBindBufferARB( GL_ARRAY_BUFFER, 0 );
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, 0 );

			for( uint32_t v = 0; v < materials[s]->Textures.size(); v++)
			{
				CGparameter CgTexture = materials[s]->Textures[v].first;
				cgGLDisableTextureParameter( CgTexture );
			}
		}
	}

	void RenderTarget::AutoLuminance()
	{
        glBindTexture(GL_TEXTURE_2D, PrimaryTex);
        glGenerateMipmap(GL_TEXTURE_2D);
        float luminance;

        GLint width, height;
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        glGetTexImage(GL_TEXTURE_2D, MaxLevel( width, height), GL_LUMINANCE, GL_FLOAT, &luminance);
        eye_luminance = eye_luminance + ((( luminance + 0.5f ) - eye_luminance ) * 0.01f);
        eye_luminance = std::max( 0.5f, std::min( 1.0f, eye_luminance) );

        // Auto luminance effect
        glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);

        // Set texture
        CGparameter CgFSTexture = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[5]->effect, "txDiffuse" );
        cgGLSetTextureParameter( CgFSTexture, PrimaryTex );
        cgGLEnableTextureParameter( CgFSTexture );

        // Set brightness
        CGparameter CgBrightness = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[5]->effect, "Brightness" );
        cgGLSetParameter1f( CgBrightness, 1.0f / eye_luminance );

        CGtechnique tech = cgGetFirstTechnique( Inst->DefaultMedia->Shaders[5]->effect );
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
            cgResetPassState(pass);
            pass = cgGetNextPass(pass);
        }

        cgGLDisableTextureParameter( CgFSTexture );
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderTarget::Bloom()
	{
        CGtechnique tech;
        CGpass pass;

		// Render bloom effect
		glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryBloomTex, 0);

		GLint width, height;
		glBindTexture(GL_TEXTURE_2D, PrimaryBloomTex);
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		glBindTexture(GL_TEXTURE_2D, 0);

		glViewport( 0, 0, width, height );

        CGparameter CgFSTexture = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[3]->effect, "txDiffuse" );
		cgGLSetTextureParameter( CgFSTexture, PrimaryTex );
		cgGLEnableTextureParameter( CgFSTexture );

		// Set vertex attributes
        glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
		glVertexAttribPointerARB( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );

		// Enable vertex attributes
		glEnableVertexAttribArrayARB( 0 );
		glEnableVertexAttribArrayARB( 1 );

        tech = cgGetFirstTechnique( Inst->DefaultMedia->Shaders[3]->effect );
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
            cgResetPassState(pass);
            pass = cgGetNextPass(pass);
        }

        tech = cgGetFirstTechnique( Inst->DefaultMedia->Shaders[3]->effect );
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
            cgResetPassState(pass);
            pass = cgGetNextPass(pass);
        }

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
                CGparameter CgFSTexture2 = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[2]->effect, "txDiffuse" );
				cgGLSetTextureParameter( CgFSTexture2, PrimaryBloomTex );
				cgGLEnableTextureParameter( CgFSTexture2 );
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryBloomTex, 0);
                CGparameter CgFSTexture2 = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[2]->effect, "txDiffuse" );
				cgGLSetTextureParameter( CgFSTexture2, SecondaryBloomTex );
				cgGLEnableTextureParameter( CgFSTexture2 );
			}
            CGparameter CgDirection2 = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[2]->effect, "direction" );
			cgGLSetParameter2f( CgDirection2, x, y );

			// Set vertex attributes
			glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
			glVertexAttribPointerARB( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );

			// Enable vertex attributes
			glEnableVertexAttribArrayARB( 0 );
			glEnableVertexAttribArrayARB( 1 );

            tech = cgGetFirstTechnique( Inst->DefaultMedia->Shaders[2]->effect );
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
                cgResetPassState(pass);
                pass = cgGetNextPass(pass);
            }

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
        CGparameter CgFSTextureA = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[4]->effect, "txDiffuse1" );
		cgGLSetTextureParameter( CgFSTextureA, PrimaryTex);
		cgGLEnableTextureParameter( CgFSTextureA );

        CGparameter CgFSTextureB = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[4]->effect, "txDiffuse2" );
		cgGLSetTextureParameter( CgFSTextureB, PrimaryBloomTex );
		cgGLEnableTextureParameter( CgFSTextureB );

		// Set vertex attributes
		glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
		glVertexAttribPointerARB( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );

		// Enable vertex attributes
		glEnableVertexAttribArrayARB( 0 );
		glEnableVertexAttribArrayARB( 1 );

        tech = cgGetFirstTechnique( Inst->DefaultMedia->Shaders[4]->effect );
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
            cgResetPassState(pass);
            pass = cgGetNextPass(pass);
        }

		// Disable vertex attributes
		glDisableVertexAttribArrayARB( 0 );
		glDisableVertexAttribArrayARB( 1 );

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderTarget::MotionBlur( float x, float y )
	{
		glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SecondaryTex, 0);

        CGparameter CgFSTexture = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[2]->effect, "txDiffuse" );
        cgGLSetTextureParameter( CgFSTexture, PrimaryTex );
        cgGLEnableTextureParameter( CgFSTexture );

        CGparameter CgDirection = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[2]->effect, "direction" );

        cgGLSetParameter2f( CgDirection, x, y );

		// Render a full screen quad
        CGtechnique tech = cgGetFirstTechnique( Inst->DefaultMedia->Shaders[2]->effect );
        CGpass pass = cgGetFirstPass(tech);
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
            cgResetPassState(pass);
            pass = cgGetNextPass(pass);
        }

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

        cgGLDisableTextureParameter( CgFSTexture );

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderTarget::DrawMarker( const Matrix44& matrix )
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Matrix44 mat = (matrix * MatrixInverse( Vector4(0.0f, 0.0f, 0.0f, 0.0f), View->getPose()));
		glLoadMatrixf((float*)&mat);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf((float*)&View->projMat);
		glDisable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		glLineWidth( 1.0f );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glBegin(GL_LINES);
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( 1.0f, 0.0f, 0.0f );
		glEnd();
		glColor3f( 0.0f, 1.0f, 0.0f );
		glBegin(GL_LINES);
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( 0.0f, 1.0f, 0.0f );
		glEnd();
		glColor3f( 0.0f, 0.0f, 1.0f );
		glBegin(GL_LINES);
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( 0.0f, 0.0f, 1.0f );
		glEnd();
	}

	void RenderTarget::Render()
	{
        // Get the window's rect
        Ovgl::Rect WindowRect;
        Ovgl::Rect adjustedrect;

        if(hWin)
        {
            hWin->hWnd->setActive(true);
            WindowRect.left = hWin->hWnd->getPosition().x;
            WindowRect.top = hWin->hWnd->getPosition().y;
            WindowRect.right = hWin->hWnd->getPosition().x + hWin->hWnd->getSize().x;
            WindowRect.bottom = hWin->hWnd->getPosition().y + hWin->hWnd->getSize().y;
            adjustedrect = WindowAdjustedRect( hWin, &rect);
        }
        else
        {
            hWin->hWnd->setActive(true);
            WindowRect.left = 0;
            WindowRect.top = 0;
            glBindTexture(GL_TEXTURE_2D, hTex->Image);
            GLint glwidth, glheight;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &glwidth);
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &glheight);
            WindowRect.right = glwidth;
            WindowRect.bottom = glheight;
            adjustedrect = TextureAdjustedRect( hTex, &rect);
        }

        int width = (int)(adjustedrect.right - adjustedrect.left);
        int height = (int)(adjustedrect.bottom - adjustedrect.top);

        if( View != NULL )
        {
            glBindFramebuffer(GL_FRAMEBUFFER, MultiSampleFrameBuffer);

            // Set the viewport to fit the window
            glViewport( 0, 0, width, height );

            Scene* scene = View->scene;
            Matrix44 viewProj = (MatrixInverse( Vector4(0,0,0,0), View->getPose() ) * View->projMat);

            // Create light arrays
            float LightCount = (float)scene->lights.size();
            std::vector< float > mLights;
            std::vector< float > LightColors;
            for( uint32_t l = 0; l < scene->lights.size(); l++)
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

            if(scene->sky_box)
            {
                // Disable depth test
                glDisable (GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);

                glDisable(GL_MULTISAMPLE_ARB);

                // Set skybox shader View variable
                CGparameter CgView = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[1]->effect, "View" );
                Matrix44 tinvView = MatrixTranspose( MatrixInverse( Vector4(0,0,0,0), View->getPose()) );
                cgGLSetMatrixParameterfc( CgView, (float*)&tinvView );

                // Set skybox shader Projection variable
                CGparameter CgProjection = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[1]->effect, "Projection" );
                Matrix44 tView = MatrixTranspose( View->projMat);
                cgGLSetMatrixParameterfc( CgProjection, (float*)&tView );

                // Set skybox texture
                CGparameter CgFSTexture = cgGetNamedEffectParameter( Inst->DefaultMedia->Shaders[1]->effect, "txSkybox" );
                cgGLSetTextureParameter( CgFSTexture, scene->sky_box->Image );
                cgGLEnableTextureParameter( CgFSTexture );

                // Bind vertex and index buffers
                glBindBufferARB( GL_ARRAY_BUFFER, Inst->DefaultMedia->Meshes[0]->vertex_buffer );
                glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, Inst->DefaultMedia->Meshes[0]->index_buffers[0] );

                // Set vertex attributes
                glVertexAttribPointerARB( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
                glVertexAttribPointerARB( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );
                glVertexAttribPointerARB( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (24)) );
                glVertexAttribPointerARB( 3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (32)) );
                glVertexAttribPointerARB( 4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (48)) );

                // Enable vertex attributes
                glEnableVertexAttribArrayARB( 0 );
                glEnableVertexAttribArrayARB( 1 );
                glEnableVertexAttribArrayARB( 2 );
                glEnableVertexAttribArrayARB( 3 );
                glEnableVertexAttribArrayARB( 4 );

                // Draw skybox
                CGtechnique tech = cgGetFirstTechnique( Inst->DefaultMedia->Shaders[1]->effect );
                CGpass pass = cgGetFirstPass(tech);
                while (pass)
                {
                    cgSetPassState(pass);
                    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
                    cgResetPassState(pass);
                    pass = cgGetNextPass(pass);
                }

                // Disable vertex attributes
                glDisableVertexAttribArrayARB( 0 );
                glDisableVertexAttribArrayARB( 1 );
                glDisableVertexAttribArrayARB( 2 );
                glDisableVertexAttribArrayARB( 3 );
                glDisableVertexAttribArrayARB( 4 );
                glBindBufferARB( GL_ARRAY_BUFFER, 0 );
                glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, 0 );

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

            for( uint32_t PostRender = 0; PostRender < 2; PostRender++ )
            {
                // Render Objects
                for( uint32_t i = 0; i < scene->objects.size(); i++ )
                {
                    std::vector<Matrix44> temp(1);
                    temp[0] = scene->objects[i]->getPose();
                    RenderMesh( *scene->objects[i]->mesh, scene->objects[i]->getPose(), temp, scene->objects[i]->materials, !!PostRender );
                }

                // Render props
                for( uint32_t i = 0; i < scene->props.size(); i++ )
                {
                    RenderMesh( *scene->props[i]->mesh, scene->props[i]->getPose(), scene->props[i]->matrices, scene->props[i]->materials, !!PostRender );
                }

                // Render actors
                for( uint32_t i = 0; i < scene->actors.size(); i++ )
                {
                    if( scene->actors[i]->mesh )
                    {
                        RenderMesh( *scene->actors[i]->mesh, scene->actors[i]->getPose(), scene->actors[i]->pose->matrices, scene->actors[i]->materials, !!PostRender );
                    }
                }
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glColor3f(1.0f, 1.0f, 1.0f);
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
            glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
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
                Matrix44 CamMat = View->getPose();
                Vector2 CamVec = Vector2(sin(CamMat._11*(float)OvglPi) * abs(CamMat._22), sin(CamMat._22*(float)OvglPi) );
                Vector2 CurCamVec = (CamVec - LastCamVec)/20;
                if( CurCamVec.x > 0.001f || CurCamVec.x < -0.001f || CurCamVec.y > 0.001f || CurCamVec.y < -0.001f )
                {
                    MotionBlur( CurCamVec.x, CurCamVec.y );
                }
                LastCamVec = CamVec;
            }

            glViewport( 0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top );

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
                glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hTex->Image, 0);
            }

            // Render to screen
            glBindTexture(GL_TEXTURE_2D, PrimaryTex);
            glBegin( GL_QUADS );
                glTexCoord2i( 1, 1 );
                glVertex2i( adjustedrect.right, adjustedrect.top );
                glTexCoord2i( 0, 1 );
                glVertex2i( adjustedrect.left, adjustedrect.top );
                glTexCoord2i( 0, 0 );
                glVertex2i( adjustedrect.left, adjustedrect.bottom );
                glTexCoord2i( 1, 0 );
                glVertex2i( adjustedrect.right, adjustedrect.bottom );
            glEnd();

            for( int i = 0; i < Interfaces.size(); i++ )
            {
                Ovgl::Rect interfacerect;
                interfacerect.left = ((adjustedrect.right - adjustedrect.left) * Interfaces[i]->rect.left.scale) + Interfaces[i]->rect.left.offset + adjustedrect.left;
                interfacerect.top = ((adjustedrect.bottom - adjustedrect.top) * Interfaces[i]->rect.top.scale) + Interfaces[i]->rect.top.offset + adjustedrect.top;
                interfacerect.right = ((adjustedrect.right - adjustedrect.left) * Interfaces[i]->rect.right.scale) + Interfaces[i]->rect.right.offset + adjustedrect.left;
                interfacerect.bottom = ((adjustedrect.bottom - adjustedrect.top) * Interfaces[i]->rect.bottom.scale) + Interfaces[i]->rect.bottom.offset + adjustedrect.top;
                Interfaces[i]->render( interfacerect );
            }
            hWin->hWnd->setActive(false);
		}
	}

	void RenderTarget::Update()
	{
        hWin->hWnd->setActive(true);
        Ovgl::Rect adjustedrect = WindowAdjustedRect( hWin, &rect);

		int width = (int)(adjustedrect.right - adjustedrect.left);
		int height = (int)(adjustedrect.bottom - adjustedrect.top);

		// Delete FrameBuffers and Textures
        if(MultiSampleFrameBuffer) glDeleteFramebuffers( 1, &MultiSampleFrameBuffer );
        if(EffectFrameBuffer)glDeleteFramebuffers( 1, &EffectFrameBuffer );
        if(ColorBuffer)glDeleteRenderbuffers( 1, &ColorBuffer );
        if(DepthBuffer)glDeleteRenderbuffers( 1, &DepthBuffer );
        if(PrimaryTex)glDeleteTextures(1, &PrimaryTex);
        if(SecondaryTex)glDeleteTextures(1, &SecondaryTex);
        if(PrimaryBloomTex)glDeleteTextures(1, &PrimaryBloomTex);
        if(SecondaryBloomTex)glDeleteTextures(1, &SecondaryBloomTex);

        // Multi sample framebuffer
        glGenFramebuffers(1, &MultiSampleFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, MultiSampleFrameBuffer);

        // Multi sample colorbuffer
        glGenRenderbuffers(1, &ColorBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, ColorBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA16F, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ColorBuffer);

        // Multi sample depthbuffer
        glGenRenderbuffers(1, &DepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, DepthBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT32, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Effect framebuffer
        glGenFramebuffers(1, &EffectFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);

		// Create and bind textures
        glGenTextures(1, &PrimaryTex);
        glBindTexture(GL_TEXTURE_2D, PrimaryTex);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, MaxLevel( width, height));
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);

        glGenTextures(1, &SecondaryTex);
        glBindTexture(GL_TEXTURE_2D, SecondaryTex);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glGenTextures(1, &PrimaryBloomTex);
        glBindTexture(GL_TEXTURE_2D, PrimaryBloomTex);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width/4, height/4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glGenTextures(1, &SecondaryBloomTex);
        glBindTexture(GL_TEXTURE_2D, SecondaryBloomTex);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width/4, height/4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        hWin->hWnd->setActive(false);
	}

    Interface::Interface( Interface* parent, const URect& rect )
    {
        this->rect = rect;
        parent->children.push_back(this);
    }

    Interface::Interface( RenderTarget* parent, const URect& rect )
    {
        this->rect = rect;
        parent->Interfaces.push_back(this);
    }

    Interface::~Interface()
    {

    }

    void Interface::render( const Ovgl::Rect& adjustedrect )
    {
        glBindTexture(GL_TEXTURE_2D, background->Image );
        glBegin( GL_QUADS );
            glTexCoord2i( 1, 1 );
            glVertex2i( adjustedrect.right, adjustedrect.top );
            glTexCoord2i( 0, 1 );
            glVertex2i( adjustedrect.left, adjustedrect.top );
            glTexCoord2i( 0, 0 );
            glVertex2i( adjustedrect.left, adjustedrect.bottom );
            glTexCoord2i( 1, 0 );
            glVertex2i( adjustedrect.right, adjustedrect.bottom );
        glEnd();

        for( int c = 0; c < children.size(); c++ )
        {
            Ovgl::Rect childrect;
            childrect.left = ((adjustedrect.right - adjustedrect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset + adjustedrect.left;
            childrect.top = ((adjustedrect.bottom - adjustedrect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset + adjustedrect.top;
            childrect.right = ((adjustedrect.right - adjustedrect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset + adjustedrect.left;
            childrect.bottom = ((adjustedrect.bottom - adjustedrect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset + adjustedrect.top;
            children[c]->render( childrect );
        }
    }
}
