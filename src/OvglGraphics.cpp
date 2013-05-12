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
uint32_t max_level( uint32_t width, uint32_t height )
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

Ovgl::Rect WindowAdjustedRect( Window* window, URect* rect )
{
    // Get the window's rect
    Ovgl::Rect WindowRect;
    SDL_GetWindowPosition( window->sdl_window, &WindowRect.left, &WindowRect.top );
    SDL_GetWindowSize( window->sdl_window, &WindowRect.right, &WindowRect.bottom );
    WindowRect.right += WindowRect.left;
    WindowRect.bottom += WindowRect.top;

    Ovgl::Rect adjustedrect;
    adjustedrect.left = ((WindowRect.right - WindowRect.left) * rect->left.scale) + rect->left.offset;
    adjustedrect.top = ((WindowRect.bottom - WindowRect.top) * rect->top.scale) + rect->left.offset;
    adjustedrect.right = ((WindowRect.right - WindowRect.left) * rect->right.scale) + rect->right.offset;
    adjustedrect.bottom = ((WindowRect.bottom - WindowRect.top) * rect->bottom.scale) + rect->bottom.offset;

    return adjustedrect;
}

Ovgl::Rect RenderTargetAdjustedRect( RenderTarget* rendertarget, URect* rect )
{
    // Get the rendertarget's rect
    Ovgl::Rect RenderTargetRect;

    RenderTargetRect = WindowAdjustedRect( rendertarget->window, &rendertarget->rect );

    Ovgl::Rect adjustedrect;
    adjustedrect.left = ((RenderTargetRect.right - RenderTargetRect.left) * rect->left.scale) + rect->left.offset;
    adjustedrect.top = ((RenderTargetRect.bottom - RenderTargetRect.top) * rect->top.scale) + rect->top.offset;
    adjustedrect.right = ((RenderTargetRect.right - RenderTargetRect.left) * rect->right.scale) + rect->right.offset;
    adjustedrect.bottom = ((RenderTargetRect.bottom - RenderTargetRect.top) * rect->bottom.scale) + rect->bottom.offset;

    return adjustedrect;
}

Ovgl::Rect TextureAdjustedRect( Texture* texture, URect* rect )
{
    // Get the window's rect
    Ovgl::Rect WindowRect;

    glBindTexture( GL_TEXTURE_2D, texture->Image );
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
    on_key_down = NULL;
    on_key_up = NULL;
    on_mouse_move = NULL;
    on_mouse_down = NULL;
    on_mouse_up = NULL;
    on_mouse_over = NULL;
    on_mouse_out = NULL;
    window = hWindow;
    hTex = NULL;
    view = NULL;
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
    window->render_targets.push_back(this);
};

RenderTarget::RenderTarget( Context* pcontext, Texture* hTexture, const URect& viewport, uint32_t flags )
{
    context = pcontext;
    on_key_down = NULL;
    on_key_up = NULL;
    on_mouse_move = NULL;
    on_mouse_down = NULL;
    on_mouse_up = NULL;
    on_mouse_over = NULL;
    on_mouse_out = NULL;
    window = NULL;
    hTex = hTexture;
    view = NULL;
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
};

RenderTarget::~RenderTarget()
{
    for( uint32_t r = 0; r < window->render_targets.size(); r++)
    {
        if(window->render_targets[r] == this)
        {
            window->render_targets.erase( window->render_targets.begin() + r );
        }
    }
}

void RenderTarget::RenderMesh( const Mesh& mesh, const Matrix44& matrix, std::vector< Matrix44 >& pose, std::vector< Material* >& materials, bool post_render )
{
    Matrix44 viewProj = (MatrixInverse( Vector4(0,0,0,0), view->getPose() ) * view->projMat);
    Matrix44 worldMat = (matrix * viewProj );
    glLoadMatrixf((float*)&worldMat);

    float LightCount = (float)view->scene->lights.size();
    std::vector< float > mLights;
    std::vector< float > LightColors;
    for( uint32_t l = 0; l < view->scene->lights.size(); l++)
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

    for( uint32_t s = 0; s < mesh.subset_count; s++)
        if( post_render == materials[s]->PostRender )
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
            cgGLSetParameter4f( CgViewPos, view->getPose()._41, view->getPose()._42, view->getPose()._43, view->getPose()._44 );

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

            glBindBuffer( GL_ARRAY_BUFFER, mesh.vertex_buffer );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffers[s] );

            // Set vertex attributes
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );
            glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (24)) );
            glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (32)) );
            glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (48)) );

            // Enable vertex attributes
            glEnableVertexAttribArray( 0 );
            glEnableVertexAttribArray( 1 );
            glEnableVertexAttribArray( 2 );
            glEnableVertexAttribArray( 3 );
            glEnableVertexAttribArray( 4 );

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
            glDisableVertexAttribArray( 0 );
            glDisableVertexAttribArray( 1 );
            glDisableVertexAttribArray( 2 );
            glDisableVertexAttribArray( 3 );
            glDisableVertexAttribArray( 4 );

            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

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

    glGetTexImage(GL_TEXTURE_2D, max_level( width, height), GL_LUMINANCE, GL_FLOAT, &luminance);
    eye_luminance = eye_luminance + ((( luminance + 0.5f ) - eye_luminance ) * 0.01f);
    eye_luminance = std::max( 0.5f, std::min( 1.0f, eye_luminance) );

    // Auto luminance effect
    glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);

    // Set texture
    CGparameter CgFSTexture = cgGetNamedEffectParameter( context->default_media->shaders[5]->effect, "txDiffuse" );
    cgGLSetTextureParameter( CgFSTexture, PrimaryTex );
    cgGLEnableTextureParameter( CgFSTexture );

    // Set brightness
    CGparameter CgBrightness = cgGetNamedEffectParameter( context->default_media->shaders[5]->effect, "Brightness" );
    cgGLSetParameter1f( CgBrightness, 1.0f / eye_luminance );

    CGtechnique tech = cgGetFirstTechnique( context->default_media->shaders[5]->effect );
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

    CGparameter CgFSTexture = cgGetNamedEffectParameter( context->default_media->shaders[3]->effect, "txDiffuse" );
    cgGLSetTextureParameter( CgFSTexture, PrimaryTex );
    cgGLEnableTextureParameter( CgFSTexture );

    // Set vertex attributes
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );

    // Enable vertex attributes
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );

    tech = cgGetFirstTechnique( context->default_media->shaders[3]->effect );
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

    tech = cgGetFirstTechnique( context->default_media->shaders[3]->effect );
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
    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );

    bool flipflop = true;
    for( float i = 0; i < (float)OvglPi; i = i + ((float)(OvglPi) / bloom))
    {
        float x = sin(i) * 0.005f;
        float y = cos(i) * 0.005f;

        if( flipflop )
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SecondaryBloomTex, 0);
            CGparameter CgFSTexture2 = cgGetNamedEffectParameter( context->default_media->shaders[2]->effect, "txDiffuse" );
            cgGLSetTextureParameter( CgFSTexture2, PrimaryBloomTex );
            cgGLEnableTextureParameter( CgFSTexture2 );
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryBloomTex, 0);
            CGparameter CgFSTexture2 = cgGetNamedEffectParameter( context->default_media->shaders[2]->effect, "txDiffuse" );
            cgGLSetTextureParameter( CgFSTexture2, SecondaryBloomTex );
            cgGLEnableTextureParameter( CgFSTexture2 );
        }
        CGparameter CgDirection2 = cgGetNamedEffectParameter( context->default_media->shaders[2]->effect, "direction" );
        cgGLSetParameter2f( CgDirection2, x, y );

        // Set vertex attributes
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
        glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );

        // Enable vertex attributes
        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );

        tech = cgGetFirstTechnique( context->default_media->shaders[2]->effect );
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
        glDisableVertexAttribArray( 0 );
        glDisableVertexAttribArray( 1 );
        flipflop = !flipflop;
    }

    // Get texture width and height.
    glBindTexture(GL_TEXTURE_2D, PrimaryTex);
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);
    glViewport( 0, 0, width, height );
    CGparameter CgFSTextureA = cgGetNamedEffectParameter( context->default_media->shaders[4]->effect, "txDiffuse1" );
    cgGLSetTextureParameter( CgFSTextureA, PrimaryTex);
    cgGLEnableTextureParameter( CgFSTextureA );

    CGparameter CgFSTextureB = cgGetNamedEffectParameter( context->default_media->shaders[4]->effect, "txDiffuse2" );
    cgGLSetTextureParameter( CgFSTextureB, PrimaryBloomTex );
    cgGLEnableTextureParameter( CgFSTextureB );

    // Set vertex attributes
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );

    // Enable vertex attributes
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );

    tech = cgGetFirstTechnique( context->default_media->shaders[4]->effect );
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
    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::MotionBlur( )
{
    glBindFramebuffer(GL_FRAMEBUFFER, EffectFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SecondaryTex, 0);

    CGparameter CgFSTexture = cgGetNamedEffectParameter( context->default_media->shaders[6]->effect, "sceneSampler" );
    cgGLSetTextureParameter( CgFSTexture, PrimaryTex );
    cgGLEnableTextureParameter( CgFSTexture );

    CGparameter CgFSTexture2 = cgGetNamedEffectParameter( context->default_media->shaders[6]->effect, "depthTexture" );
    cgGLSetTextureParameter( CgFSTexture2, depth_texture );
    cgGLEnableTextureParameter( CgFSTexture2 );

    Matrix44 viewProj = MatrixInverse( Vector4(0,0,0,0), (view->projMat * MatrixInverse( Vector4(0,0,0,0), view->getPose()) ));
    static Matrix44 previous_viewProj;
    CGparameter CgViewProjMatrix = cgGetNamedEffectParameter( context->default_media->shaders[6]->effect, "g_ViewProjectionInverseMatrix" );
    Matrix44 tViewProj = MatrixTranspose(viewProj);
    cgGLSetMatrixParameterfc( CgViewProjMatrix, (float*)&tViewProj );

    CGparameter CgPreviousViewProjMatrix = cgGetNamedEffectParameter( context->default_media->shaders[6]->effect, "g_previousViewProjectionMatrix" );
    Matrix44 tPreviousViewProj = MatrixTranspose(previous_viewProj);
    cgGLSetMatrixParameterfc( CgPreviousViewProjMatrix, (float*)&tPreviousViewProj );

    CGtechnique tech = cgGetFirstTechnique( context->default_media->shaders[6]->effect );
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
    cgGLDisableTextureParameter( CgFSTexture2 );

    glBindTexture(GL_TEXTURE_2D, SecondaryTex);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);

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
    previous_viewProj = view->projMat * MatrixInverse( Vector4(0,0,0,0), view->getPose());
}

void RenderTarget::DrawMarker( const Matrix44& matrix )
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Matrix44 mat = (matrix * MatrixInverse( Vector4(0.0f, 0.0f, 0.0f, 0.0f), view->getPose()));
    glLoadMatrixf((float*)&mat);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf((float*)&view->projMat);
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
	
    if(window)
    {
        SDL_GetWindowPosition( window->sdl_window, &WindowRect.left, &WindowRect.top );
        SDL_GetWindowSize( window->sdl_window, &WindowRect.right, &WindowRect.bottom );
        WindowRect.right += WindowRect.left;
        WindowRect.bottom += WindowRect.top;
        adjustedrect = WindowAdjustedRect( window, &rect);
        SDL_GL_MakeCurrent( window->sdl_window, context->gl_context);
    }
    else
    {
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

    if( view != NULL )
    {
        glBindFramebuffer(GL_FRAMEBUFFER, MultiSampleFrameBuffer);

        // Set the viewport to fit the window
        glViewport( 0, 0, width, height );

        Scene* scene = view->scene;
        Matrix44 viewProj = (MatrixInverse( Vector4(0, 0, 0, 0), view->getPose() ) * view->projMat);

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

            glDisable(GL_MULTISAMPLE);

            // Set skybox shader View variable
            CGparameter CgView = cgGetNamedEffectParameter( context->default_media->shaders[1]->effect, "View" );
            Matrix44 tinvView = MatrixTranspose( MatrixInverse( Vector4(0,0,0,0), view->getPose()) );
            cgGLSetMatrixParameterfc( CgView, (float*)&tinvView );

            // Set skybox shader Projection variable
            CGparameter CgProjection = cgGetNamedEffectParameter( context->default_media->shaders[1]->effect, "Projection" );
            Matrix44 tView = MatrixTranspose( view->projMat);
            cgGLSetMatrixParameterfc( CgProjection, (float*)&tView );

            // Set skybox texture
            CGparameter CgFSTexture = cgGetNamedEffectParameter( context->default_media->shaders[1]->effect, "txSkybox" );
            cgGLSetTextureParameter( CgFSTexture, scene->sky_box->Image );
            cgGLEnableTextureParameter( CgFSTexture );

            // Bind vertex and index buffers
            glBindBuffer( GL_ARRAY_BUFFER, context->default_media->meshes[0]->vertex_buffer );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, context->default_media->meshes[0]->index_buffers[0] );

            // Set vertex attributes
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)) );
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)) );
            glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (24)) );
            glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (32)) );
            glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (48)) );

            // Enable vertex attributes
            glEnableVertexAttribArray( 0 );
            glEnableVertexAttribArray( 1 );
            glEnableVertexAttribArray( 2 );
            glEnableVertexAttribArray( 3 );
            glEnableVertexAttribArray( 4 );

            // Draw skybox
            CGtechnique tech = cgGetFirstTechnique( context->default_media->shaders[1]->effect );
            CGpass pass = cgGetFirstPass(tech);
            while (pass)
            {
                cgSetPassState(pass);
                glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
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
            glEnable(GL_MULTISAMPLE);
        }
        else
        {
            glDisable(GL_MULTISAMPLE);
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
        glDisable(GL_MULTISAMPLE);
        glDisable (GL_DEPTH_TEST);
        glDepthMask (GL_FALSE);
        glDisable( GL_LIGHTING );
        glEnable(GL_TEXTURE_2D);
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        // Blit MultiSampleTexture to BaseTexture to apply effects.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, MultiSampleFrameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, EffectFrameBuffer);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PrimaryTex, 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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
            MotionBlur();
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


    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_MULTISAMPLE);
	glDisable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
	glDisable( GL_LIGHTING );
	glEnable(GL_TEXTURE_2D);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

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

	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);
	for( uint32_t i = 0; i < Interfaces.size(); i++ )
	{
		Ovgl::Rect interfacerect;
		interfacerect.left = ((adjustedrect.right - adjustedrect.left) * Interfaces[i]->rect.left.scale) + Interfaces[i]->rect.left.offset + adjustedrect.left;
		interfacerect.top = ((adjustedrect.bottom - adjustedrect.top) * Interfaces[i]->rect.top.scale) + Interfaces[i]->rect.top.offset + adjustedrect.top;
		interfacerect.right = ((adjustedrect.right - adjustedrect.left) * Interfaces[i]->rect.right.scale) + Interfaces[i]->rect.right.offset + adjustedrect.left;
		interfacerect.bottom = ((adjustedrect.bottom - adjustedrect.top) * Interfaces[i]->rect.bottom.scale) + Interfaces[i]->rect.bottom.offset + adjustedrect.top;
		Interfaces[i]->render( interfacerect );
	}
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);

    SDL_GL_MakeCurrent( NULL, NULL);
}

void RenderTarget::Update()
{
    SDL_GL_MakeCurrent( NULL, context );
    Ovgl::Rect adjustedrect = WindowAdjustedRect( window, &rect );

    int width = (int)(adjustedrect.right - adjustedrect.left);
    int height = (int)(adjustedrect.bottom - adjustedrect.top);

    // Delete FrameBuffers and Textures
    if(MultiSampleFrameBuffer) glDeleteFramebuffers( 1, &MultiSampleFrameBuffer );
    if(EffectFrameBuffer)glDeleteFramebuffers( 1, &EffectFrameBuffer );
    if(ColorBuffer)glDeleteRenderbuffers( 1, &ColorBuffer );
    if(DepthBuffer)glDeleteRenderbuffers( 1, &DepthBuffer );
    if(color_texture)glDeleteTextures(1, &color_texture);
    if(depth_texture)glDeleteTextures(1, &depth_texture);
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

    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    glGenTextures(1, &PrimaryTex);
    glBindTexture(GL_TEXTURE_2D, PrimaryTex);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_level( width, height));
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
    SDL_GL_MakeCurrent( NULL, NULL );
}

void RenderTarget::do_event( Event event )
{
    Ovgl::Rect adjusted_rect;
    adjusted_rect = WindowAdjustedRect( window, &rect );
    switch( event.type )
    {
    case OVGL_KEYDOWN:
        if( on_key_down )
        {
            on_key_down( event.key);
        }
        for( uint32_t i = 0; i < Interfaces.size(); i++ )
        {
            Interfaces[i]->do_event( event, adjusted_rect );
        }
        break;

    case OVGL_KEYUP:
        if( on_key_up )
        {
            on_key_up( event.key );
        }
        for( uint32_t i = 0; i < Interfaces.size(); i++ )
        {
            Interfaces[i]->do_event( event, adjusted_rect );
        }
        break;
    case OVGL_MOUSEMOTION:
        if( event.mouse_x > adjusted_rect.left && event.mouse_x < adjusted_rect.right )
            if( event.mouse_y > adjusted_rect.top && event.mouse_y < adjusted_rect.bottom )
            {
                event.mouse_x -= adjusted_rect.left;
                event.mouse_y -= adjusted_rect.top;
                if(on_mouse_move)
                {
                    on_mouse_move( event.mouse_x, event.mouse_y );
                }
                for( uint32_t i = 0; i < Interfaces.size(); i++ )
                {
                    Ovgl::Rect interface_rect = RenderTargetAdjustedRect( this, &Interfaces[i]->rect);
                    if( event.mouse_x > interface_rect.left && event.mouse_x < interface_rect.right )
                        if( event.mouse_y > interface_rect.top && event.mouse_y < interface_rect.bottom )
                        {
                            Event interface_event = event;
                            interface_event.mouse_x -= interface_rect.left;
                            interface_event.mouse_y -= interface_rect.top;
                            Interfaces[i]->do_event(interface_event, interface_rect);
                        }
                }
            }
        break;
    case OVGL_MOUSEBUTTONDOWN:
        if( event.mouse_x > adjusted_rect.left && event.mouse_x < adjusted_rect.right )
            if( event.mouse_y > adjusted_rect.top && event.mouse_y < adjusted_rect.bottom )
            {
                event.mouse_x -= adjusted_rect.left;
                event.mouse_y -= adjusted_rect.top;
                if(on_mouse_down)
                {
                    on_mouse_down( event.mouse_x, event.mouse_y, event.button );
                }
                for( uint32_t i = 0; i < Interfaces.size(); i++ )
                {
                    Ovgl::Rect interface_rect = RenderTargetAdjustedRect( this, &Interfaces[i]->rect );
                    if( event.mouse_x > interface_rect.left && event.mouse_x < interface_rect.right )
                        if( event.mouse_y > interface_rect.top && event.mouse_y < interface_rect.bottom )
                        {
                            Event interface_event = event;
                            interface_event.mouse_x -= interface_rect.left;
                            interface_event.mouse_y -= interface_rect.top;
                            Interfaces[i]->do_event( interface_event, interface_rect );
                        }
                }
            }
        break;
    case OVGL_MOUSEBUTTONUP:
        if( event.mouse_x > adjusted_rect.left && event.mouse_x < adjusted_rect.right )
            if( event.mouse_y > adjusted_rect.top && event.mouse_y < adjusted_rect.bottom )
            {
                event.mouse_x -= adjusted_rect.left;
                event.mouse_y -= adjusted_rect.top;
                if(on_mouse_up)
                {
                    on_mouse_up( event.mouse_x, event.mouse_y, event.button );
                }
                for( uint32_t i = 0; i < Interfaces.size(); i++ )
                {
                    Ovgl::Rect interface_rect = RenderTargetAdjustedRect( this, &Interfaces[i]->rect );
                    if( event.mouse_x > interface_rect.left && event.mouse_x < interface_rect.right )
                        if( event.mouse_y > interface_rect.top && event.mouse_y < interface_rect.bottom )
                        {
                            Event interface_event = event;
                            interface_event.mouse_x -= interface_rect.left;
                            interface_event.mouse_y -= interface_rect.top;
                            Interfaces[i]->do_event( interface_event, interface_rect );
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
    tile_x = false;
    tile_y = false;
    wordbreak = true;
    color = parent->color;
    align = 0;
    hscroll = 0;
    vscroll = 0;
    on_key_down = NULL;
    on_key_up = NULL;
    on_mouse_move = NULL;
    on_mouse_down = NULL;
    on_mouse_up = NULL;
    on_mouse_over = NULL;
    on_mouse_out = NULL;
    rendertarget = parent->rendertarget;
    parent->children.push_back(this);
}

Interface::Interface( RenderTarget* parent, const URect& rect )
{
    this->rect = rect;
    background = NULL;
    tile_x = false;
    tile_y = false;
    wordbreak = true;
    color = Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
	text_color = Vector4( 0.0f, 0.0f, 0.0f, 1.0f );
    align = 0;
    hscroll = 0;
    vscroll = 0;
    on_key_down = NULL;
    on_key_up = NULL;
    on_mouse_move = NULL;
    on_mouse_down = NULL;
    on_mouse_up = NULL;
    on_mouse_over = NULL;
    on_mouse_out = NULL;
    rendertarget = parent;
    parent->Interfaces.push_back(this);
}

Interface::~Interface()
{

}

void Interface::render( const Ovgl::Rect& adjusted_rect )
{
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    if( background )
    {
        glBindTexture(GL_TEXTURE_2D, background->Image );

    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, 0 );
    }

    glColor4f(color.x, color.y, color.z, color.w);

    float tilewidth = 1.0f;
    float tileheight = 1.0f;

    if(tile_x)
    {
        GLint width;
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        tilewidth = (float)(adjusted_rect.right - adjusted_rect.left) / (float)width;
    }
    if(tile_y)
    {
        GLint height;
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        tileheight = (float)(adjusted_rect.bottom - adjusted_rect.top) / (float)height;
    }
    glStencilFunc(GL_ALWAYS, 0x1, 0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glBegin( GL_QUADS );
    glTexCoord2f( tilewidth, 0 );
    glVertex2i( adjusted_rect.right, adjusted_rect.top );
    glTexCoord2f( 0, 0 );
    glVertex2i( adjusted_rect.left, adjusted_rect.top );
    glTexCoord2f( 0, tileheight );
    glVertex2i( adjusted_rect.left, adjusted_rect.bottom );
    glTexCoord2f( tilewidth, tileheight );
    glVertex2i( adjusted_rect.right, adjusted_rect.bottom );
    glEnd();

    glStencilFunc(GL_EQUAL, 0x1, 0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColor4f(text_color.x, text_color.y, text_color.z, text_color.w);

    int32_t x;
    if(align == 0)
    {
        x = adjusted_rect.left;
        int32_t y = adjusted_rect.top + vscroll;
        for( uint32_t i = 0; i < text.size(); i++ )
        {
            int32_t wordwidth = 0;
            int32_t charwidth, charheight;
            if(wordbreak)
            {
                uint32_t wi = i + 1;
                while(text[wi] != 32 && wi < text.size())
                {
                    glBindTexture(GL_TEXTURE_2D, font->charset[ text[wi] ]);
                    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &charwidth);
                    wordwidth += charwidth;
                    wi++;
                }
            }
            glBindTexture(GL_TEXTURE_2D, font->charset[ text[i] ]);
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &charwidth);
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &charheight);
            if(x + charwidth + wordwidth > adjusted_rect.right)
            {
                y += font->size;
                x = adjusted_rect.left;
            }
            if(charwidth == 0)
            {
                charwidth = font->size / 2;
            }
            uint32_t charoffset = font->size - font->charoffsets[text[i]];
            glBegin( GL_QUADS );
            glTexCoord2f( 1, 0 );
            glVertex2i( x + charwidth, y + charoffset );
            glTexCoord2f( 0, 0 );
            glVertex2i( x, y + charoffset );
            glTexCoord2f( 0, 1 );
            glVertex2i( x, y + charheight + charoffset );
            glTexCoord2f( 1, 1 );
            glVertex2i( x + charwidth, y + charheight + charoffset );
            glEnd();
            x += charwidth + 2;
        }
    }
    else if(align == 1)
    {
        x = adjusted_rect.right;
        int32_t y = adjusted_rect.top + vscroll;
        for( int32_t i = text.size()-1; i >= 0 ; i-- )
        {
            int32_t wordwidth = 0;
            int32_t charwidth, charheight;
            if(wordbreak)
            {
                int32_t wi = i - 1;
                while(text[wi] != 32 && wi >= 0)
                {
                    glBindTexture(GL_TEXTURE_2D, font->charset[ text[wi] ]);
                    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &charwidth);
                    wordwidth += charwidth;
                    wi--;
                }
            }
            glBindTexture(GL_TEXTURE_2D, font->charset[ text[i] ]);
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &charwidth);
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &charheight);
            x -= charwidth + 2;
            if(x - (charwidth + wordwidth) < (int32_t)adjusted_rect.left)
            {
                y -= font->size;
                x = adjusted_rect.right;
            }
            if(charwidth == 0)
            {
                charwidth = font->size / 2;
            }
            uint32_t charoffset = font->size - font->charoffsets[text[i]];
            glBegin( GL_QUADS );
            glTexCoord2f( 1, 0 );
            glVertex2i( x + charwidth, y + charoffset );
            glTexCoord2f( 0, 0 );
            glVertex2i( x, y + charoffset );
            glTexCoord2f( 0, 1 );
            glVertex2i( x, y + charheight + charoffset );
            glTexCoord2f( 1, 1 );
            glVertex2i( x + charwidth, y + charheight + charoffset );
            glEnd();

        }
    }

    for( uint32_t c = 0; c < children.size(); c++ )
    {
        Ovgl::Rect child_rect;
        child_rect.left = ((adjusted_rect.right - adjusted_rect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset + adjusted_rect.left;
        child_rect.top = ((adjusted_rect.bottom - adjusted_rect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset + adjusted_rect.top;
        child_rect.right = ((adjusted_rect.right - adjusted_rect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset + adjusted_rect.left;
        child_rect.bottom = ((adjusted_rect.bottom - adjusted_rect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset + adjusted_rect.top;
        children[c]->render( child_rect );
    }
}

void Interface::do_event(Event event, const Rect& adjustedrect)
{
    switch (event.type)
    {
    case OVGL_KEYDOWN:
        if(on_key_down)
        {
            on_key_down( event.key );
        }
        break;
    case OVGL_KEYUP:
        if(on_key_up)
        {
            on_key_up( event.key );
        }
        break;
    case OVGL_MOUSEMOTION:
        if(on_mouse_move)
        {
            on_mouse_move( event.mouse_x, event.mouse_y );
        }
        for( uint32_t c = 0; c < children.size(); c++ )
        {
            Ovgl::Rect childrect;
            childrect.left = ((adjustedrect.right - adjustedrect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset ;
            childrect.top = ((adjustedrect.bottom - adjustedrect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset ;
            childrect.right = ((adjustedrect.right - adjustedrect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset ;
            childrect.bottom = ((adjustedrect.bottom - adjustedrect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset ;
            if( event.mouse_x > childrect.left && event.mouse_x < childrect.right )
                if( event.mouse_y > childrect.top && event.mouse_y < childrect.bottom )
                {
                    Event interface_event = event;
                    interface_event.mouse_x -= childrect.left;
                    interface_event.mouse_y -= childrect.top;
                    children[c]->do_event(interface_event, childrect);
                }
        }
        break;
    case OVGL_MOUSEBUTTONDOWN:
        if(on_mouse_down)
        {
            on_mouse_down( event.mouse_x, event.mouse_y, event.button );
        }
        for( uint32_t c = 0; c < children.size(); c++ )
        {
            Ovgl::Rect childrect;
            childrect.left = ((adjustedrect.right - adjustedrect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset ;
            childrect.top = ((adjustedrect.bottom - adjustedrect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset ;
            childrect.right = ((adjustedrect.right - adjustedrect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset ;
            childrect.bottom = ((adjustedrect.bottom - adjustedrect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset ;
            if( event.mouse_x > childrect.left && event.mouse_x < childrect.right )
                if( event.mouse_y > childrect.top && event.mouse_y < childrect.bottom )
                {
                    Event interface_event = event;
                    interface_event.mouse_x -= childrect.left;
                    interface_event.mouse_y -= childrect.top;
                    children[c]->do_event(interface_event, childrect);
                }
        }
        break;
    case OVGL_MOUSEBUTTONUP:
        if(on_mouse_up)
        {
            on_mouse_up( event.mouse_x, event.mouse_y, event.button );
        }
        for( uint32_t c = 0; c < children.size(); c++ )
        {
            Ovgl::Rect childrect;
            childrect.left = ((adjustedrect.right - adjustedrect.left) * children[c]->rect.left.scale) + children[c]->rect.left.offset ;
            childrect.top = ((adjustedrect.bottom - adjustedrect.top) * children[c]->rect.top.scale) + children[c]->rect.top.offset ;
            childrect.right = ((adjustedrect.right - adjustedrect.left) * children[c]->rect.right.scale) + children[c]->rect.right.offset ;
            childrect.bottom = ((adjustedrect.bottom - adjustedrect.top) * children[c]->rect.bottom.scale) + children[c]->rect.bottom.offset ;
            if( event.mouse_x > childrect.left && event.mouse_x < childrect.right )
                if( event.mouse_y > childrect.top && event.mouse_y < childrect.bottom )
                {
                    Event interface_event = event;
                    interface_event.mouse_x -= childrect.left;
                    interface_event.mouse_y -= childrect.top;
                    children[c]->do_event(interface_event, childrect);
                }
        }
        break;

    default:
        break;
    }
}

void Interface::set_text( const std::string& text )
{
    this->text = text;
}

}
