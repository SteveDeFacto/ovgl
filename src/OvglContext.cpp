/**
 * @file OvglContext.cpp
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
 * @brief This is the core of the library.
 */

#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglResource.h"
#include "OvglGraphics.h"
#include "OvglAudio.h"
#include "OvglMesh.h"
#include "OvglScene.h"
#include "OvglWindow.h"
#include "OvglSkeleton.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <FreeImage.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype2/freetype.h>
#include <freetype2/ftoutln.h>
#include <freetype2/fttrigon.h>
#include <freetype2/ftglyph.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace Ovgl
{
void buildDefaultMedia( Context* context )
{
	SDL_GL_MakeCurrent(context->contextWindow, context->glContext);
	context->defaultMedia = new ResourceManager(context, "");

	Shader* defaultEffect = new Shader;
	Shader* skyboxEffect = new Shader;
	Shader* blurEffect = new Shader;
	Shader* bloomEffect = new Shader;
	Shader* addEffect = new Shader;
	Shader* brightnessEffect = new Shader;
	Shader* motionBlurEffect = new Shader;

	defaultEffect->mLibrary = context->defaultMedia;
	skyboxEffect->mLibrary = context->defaultMedia;
	blurEffect->mLibrary = context->defaultMedia;
	bloomEffect->mLibrary = context->defaultMedia;
	addEffect->mLibrary = context->defaultMedia;
	brightnessEffect->mLibrary = context->defaultMedia;
	motionBlurEffect->mLibrary = context->defaultMedia;

	// Define debugging variables
	CGerror error;
	const char* string;

	std::string shader;

	// Create shader string.
	shader =
		"struct VS_INPUT"
		"{"
		"	float3 pos                  : ATTR0;"
		"	float3 norm                 : ATTR1;"
		"	float2 tex                  : ATTR2;"
		"	float4 bw                   : ATTR3;"
		"	float4 bi                   : ATTR4;"
		"};"

		"struct FS_INPUT"
		"{"
		"	float4 posVS                : POSITION;"
		"	float4 posWS                : TEXCOORD0;"
		"	float2 tex                  : TEXCOORD1;"
		"	float4 norm                 : TEXCOORD2;"
		"};"

		"struct FS_OUTPUT"
		"{"
		"	float4 color                : COLOR;"
		"};"

		"float4 Ambient = float4( 0.0f, 0.0f, 0.0f, 1.0f );"
		"float4 Diffuse = float4( 0.75f, 0.75f, 0.75f, 1.0f );"
		"float EMI = 0.1f;"
		"float LightCount               : LIGHTCOUNT;"
		"float4 ViewPos                 : VIEWPOS;"
		"float4 Lights[16]              : LIGHTS;"
		"float4 LightColors[16]         : LIGHTCOLORS;"
		"float4x4 World                 : WORLD;"
		"float4x4 ViewProj              : VIEWPROJ;"
		"float4x4 Bones[128]            : BONES;"
		"uniform sampler2D txDiffuse;"
		"uniform samplerCUBE txEnvironment;"

		"FS_INPUT VS( VS_INPUT In )"
		"{"
		"	FS_INPUT Out;"
		"	float4x4 skinTransform = 0;"
		"	float4x4 normTransform = 0;"
		"	skinTransform += Bones[In.bi.x] * In.bw.x;"
		"	skinTransform += Bones[In.bi.y] * In.bw.y;"
		"	skinTransform += Bones[In.bi.z] * In.bw.z;"
		"	skinTransform += Bones[In.bi.w] * In.bw.w;"
		"	normTransform = skinTransform;"
		"	normTransform[3].x = 0;"
		"	normTransform[3].y = 0;"
		"	normTransform[3].z = 0;"
		"	Out.posVS = mul(float4(In.pos, 1), skinTransform);"
		"	Out.posWS = Out.posVS;"
		"	Out.norm = mul(float4(In.norm, 1), normTransform);"
		"	Out.tex = In.tex;"
		"	Out.posVS = mul(Out.posVS, ViewProj);"
		"	return Out;"
		"}"

		"FS_OUTPUT FS( FS_INPUT In )"
		"{"
		"	FS_OUTPUT Out;"
		"	float4 light = float4( 0, 0, 0, 0 );"
		"	for(float i = 0; i < LightCount; i++)"
		"	{"
		"		float4 lightDir = Lights[i] - In.posWS;"
		"		float4 NdotL = saturate(dot(In.norm, normalize(lightDir)));"
		"		float4 attenuation = 1 / length(lightDir);"
		"		light += LightColors[i] * NdotL * attenuation * 10;"
		"	}"
		"	float4 envColor = texCUBE( txEnvironment, reflect( normalize( In.posWS.xyz - ViewPos.xyz ), In.norm.xyz ) ) * EMI;"
		"	float4 texColor = tex2D( txDiffuse, In.tex );"
		"	Out.color = ( (texColor + envColor) * Diffuse) * (light + Ambient);"
		"	Out.color.w = min(1.0, Out.color.w);"
		"	return Out;"
		"}"

		"technique t0"
		"{"
		"   pass p0"
		"   {"
		"      VertexProgram = compile gp4vp VS();"
		"      FragmentProgram = compile gp4fp FS();"
		"   }"
		"}";

	defaultEffect->effect = cgCreateEffect(context->cgContext, shader.c_str(), NULL);
	string = cgGetLastErrorString(&error);
	if(error)
	{
		fprintf(stderr, "Error: %s\n", string);
		string = cgGetLastListing(context->cgContext);
		fprintf(stderr, "Compiler: %s\n", string);
	}

	shader =
		"struct VS_INPUT"
		"{"
		"	float3 pos              : ATTR0;"
		"	float3 norm             : ATTR1;"
		"	float2 tex              : ATTR2;"
		"	float4 bw               : ATTR3;"
		"	float4 bi               : ATTR4;"
		"};"

		"struct FS_INPUT"
		"{"
		"  float4 pos               : POSITION;"
		"  float3 tex               : TEXCOORD0;"
		"};"

		"struct FS_OUTPUT"
		"{"
		"  float4 color             : COLOR;"
		"};"

		"uniform samplerCUBE txSkybox;"
		"float4x4 View;"
		"float4x4 Projection;"

		"FS_INPUT VS( VS_INPUT In )"
		"{"
		"	FS_INPUT Out;"
		"	Out.pos = mul( float4( mul( In.pos.xyz, (float3x3)View), 1 ), Projection );"
		"	Out.tex = In.pos.xyz;"
		"	return Out;"
		"}"

		"FS_OUTPUT FS( FS_INPUT In)"
		"{"
		"	FS_OUTPUT Out;"
		"	Out.color = texCUBE(txSkybox, In.tex);"
		"	return Out;"
		"}"

		"technique t0"
		"{"
		"   pass p0"
		"   {"
		"      VertexProgram = compile gp4vp VS();"
		"      FragmentProgram = compile gp4fp FS();"
		"   }"
		"}";

	skyboxEffect->effect = cgCreateEffect(context->cgContext, shader.c_str(), NULL);
	string = cgGetLastErrorString(&error);
	if(error)
	{
		fprintf(stderr, "Error: %s\n", string);
		string = cgGetLastListing(context->cgContext);
		fprintf(stderr, "Compiler: %s\n", string);
	}

	shader =
		"struct FS_INPUT"
		"{"
		"  float3 pos               : POSITION;"
		"  float2 tex               : TEXCOORD0;"
		"};"

		"struct FS_OUTPUT"
		"{"
		"  float4 color             : COLOR;"
		"};"

		"float PixelKernel[13] ="
		"{"
		"	-6,"
		"	-5,"
		"	-4,"
		"	-3,"
		"	-2,"
		"	-1,"
		"	0,"
		"	1,"
		"	2,"
		"	3,"
		"	4,"
		"	5,"
		"	6,"
		"};"

		"static const float BlurWeights[13] ="
		"{"
		"	0.002216,"
		"	0.008764,"
		"	0.026995,"
		"	0.064759,"
		"	0.120985,"
		"	0.176033,"
		"	0.199471,"
		"	0.176033,"
		"	0.120985,"
		"	0.064759,"
		"	0.026995,"
		"	0.008764,"
		"	0.002216,"
		"};"

		"float2 direction;"

		"uniform sampler2D txDiffuse;"

		"FS_OUTPUT FS( FS_INPUT In)"
		"{"
		"	FS_OUTPUT Out;"
		"	float2 samp = In.tex;"
		"	for (int i = 0; i < 13; i++)"
		"	{"
		"		samp = In.tex + (direction * PixelKernel[i]);"
		"		Out.color += tex2D(txDiffuse, samp) * BlurWeights[i];"
		"	}"
		"	return Out;"
		"}"

		"technique t0"
		"{"
		"   pass p0"
		"   {"
		"      FragmentProgram = compile gp4fp FS();"
		"   }"
		"}";

	blurEffect->effect = cgCreateEffect(context->cgContext, shader.c_str(), NULL);
	string = cgGetLastErrorString(&error);
	if(error)
	{
		fprintf(stderr, "Error: %s\n", string);
		string = cgGetLastListing(context->cgContext);
		fprintf(stderr, "Compiler: %s\n", string);
	}

	shader =
		"struct FS_INPUT"
		"{"
		"  float3 pos               : POSITION;"
		"  float2 tex               : TEXCOORD0;"
		"};"

		"struct FS_OUTPUT"
		"{"
		"  float4 color             : COLOR;"
		"};"

		"float Luminance = 1.0f;"

		"uniform sampler2D txDiffuse;"

		"FS_OUTPUT FS( FS_INPUT In)"
		"{"
		"	FS_OUTPUT Out;"
		"	Out.color = tex2D(txDiffuse, In.tex);"
		"	Out.color = Out.color - Luminance;"
		"	Out.color = max(float4(0.0, 0.0, 0.0, 0.0), Out.color);"
		"	return Out;"
		"}"

		"technique t0"
		"{"
		"   pass p0"
		"   {"
		"      FragmentProgram = compile gp4fp FS();"
		"   }"
		"}";

	bloomEffect->effect = cgCreateEffect( context->cgContext, shader.c_str(), NULL );
	string = cgGetLastErrorString( &error );
	if( error )
	{
		fprintf(stderr, "Error: %s\n", string);
		string = cgGetLastListing( context->cgContext );
		fprintf(stderr, "Compiler: %s\n", string);
	}

	shader =
		"struct FS_INPUT"
		"{"
		"  float3 pos               : POSITION;"
		"  float2 tex               : TEXCOORD0;"
		"};"

		"struct FS_OUTPUT"
		"{"
		"  float4 color             : COLOR;"
		"};"

		"uniform sampler2D txDiffuse1;"
		"uniform sampler2D txDiffuse2;"

		"FS_OUTPUT FS( FS_INPUT In)"
		"{"
		"	FS_OUTPUT Out;"
		"	Out.color = tex2D(txDiffuse1, In.tex);"
		"	Out.color += tex2D(txDiffuse2, In.tex);"
		"	Out.color.w = 1.0;"
		"	return Out;"
		"}"

		"technique t0"
		"{"
		"   pass p0"
		"   {"
		"      FragmentProgram = compile gp4fp FS();"
		"   }"
		"}";

	addEffect->effect = cgCreateEffect( context->cgContext, shader.c_str(), NULL );
	string = cgGetLastErrorString( &error );
	if(error)
	{
		fprintf( stderr, "Error: %s\n", string );
		string = cgGetLastListing( context->cgContext );
		fprintf( stderr, "Compiler: %s\n", string );
	}

	shader =
		"struct FS_INPUT"
		"{"
		"  float3 pos               : POSITION;"
		"  float2 tex               : TEXCOORD0;"
		"};"

		"struct FS_OUTPUT"
		"{"
		"  float4 color             : COLOR;"
		"};"

		"uniform sampler2D txDiffuse;"
		"float Brightness;"

		"FS_OUTPUT FS( FS_INPUT In)"
		"{"
		"	FS_OUTPUT Out;"
		"	Out.color = tex2D(txDiffuse, In.tex);"
		"	Out.color = Out.color * Brightness;"
		"	Out.color.w = 1.0;"
		"	return Out;"
		"}"

		"technique t0"
		"{"
		"   pass p0"
		"   {"
		"      FragmentProgram = compile gp4fp FS();"
		"   }"
		"}";

	brightnessEffect->effect = cgCreateEffect( context->cgContext, shader.c_str(), NULL );
	string = cgGetLastErrorString( &error );
	if(error)
	{
		fprintf(stderr, "Error: %s\n", string);
		string = cgGetLastListing( context->cgContext );
		fprintf(stderr, "Compiler: %s\n", string);
	}

	shader =
		"struct FS_INPUT"
		"{"
		"  float3 pos               : POSITION;"
		"  float2 tex               : TEXCOORD0;"
		"};"

		"struct FS_OUTPUT"
		"{"
		"  float4 color             : COLOR;"
		"};"

		"float g_numSamples = 4;"
		"uniform sampler2D sceneSampler;"
		"uniform sampler2D depthTexture;"
		"float4x4 g_ViewProjectionInverseMatrix;"
		"float4x4 g_previousViewProjectionMatrix;"

		"FS_OUTPUT FS( FS_INPUT In)"
		"{"
		"	FS_OUTPUT Out;"
		"   float2 texCoord = In.tex;"
		"   float zOverW = tex2D(depthTexture, texCoord);"
		"   float4 H = float4(texCoord.x * 2 - 1, (1 - texCoord.y) * 2 - 1, zOverW, 1);"
		"   float4 D = mul(H, g_ViewProjectionInverseMatrix);"
		"   float4 worldPos = D / D.w;"
		"   float4 currentPos = H;"
		"   float4 previousPos = mul(worldPos, g_previousViewProjectionMatrix);"
		"   previousPos /= previousPos.w;"
		"   float2 velocity = ((currentPos - previousPos) / 16.f);"
		"   float4 color = tex2D(sceneSampler, texCoord);"
		"   texCoord += velocity;"
		"   for(int i = 1; i < g_numSamples; ++i, texCoord += velocity)"
		"   {"
		"       float4 currentColor = tex2D(sceneSampler, texCoord);"
		"       color += currentColor;"
		"   }"
		"   float4 finalColor = color / g_numSamples;"
		"	Out.color = finalColor;"
		"	return Out;"
		"}"

		"technique t0"
		"{"
		"   pass p0"
		"   {"
		"      FragmentProgram = compile gp4fp FS();"
		"   }"
		"}";

	motionBlurEffect->effect = cgCreateEffect( context->cgContext, shader.c_str(), NULL );
	string = cgGetLastErrorString(&error);
	if(error)
	{
		fprintf( stderr, "Error: %s\n", string );
		string = cgGetLastListing( context->cgContext );
		fprintf( stderr, "Compiler: %s\n", string );
	}

	context->defaultMedia->shaders.push_back( defaultEffect );
	context->defaultMedia->shaders.push_back( skyboxEffect );
	context->defaultMedia->shaders.push_back( blurEffect );
	context->defaultMedia->shaders.push_back( bloomEffect );
	context->defaultMedia->shaders.push_back( addEffect );
	context->defaultMedia->shaders.push_back( brightnessEffect );
	context->defaultMedia->shaders.push_back( motionBlurEffect );

	// Create Default Material
	Material* defaultMaterial = new Material;

	defaultMaterial->shaderProgram = defaultEffect;
	defaultMaterial->mLibrary = context->defaultMedia;
	defaultMaterial->setEffectTexture("txDiffuse", defaultMaterial->mLibrary->createTexture( 256, 256) );
	defaultMaterial->setEffectTexture("txEnvironment", defaultMaterial->mLibrary->createCubemap( 256, 256) );
	defaultMaterial->noZBuffer = false;
	defaultMaterial->noZWrite = false;
	defaultMaterial->postRender = false;
	context->defaultMedia->materials.push_back(defaultMaterial);

	// Create Sky Box
	std::vector< Vertex > vertices(8);
	std::vector< Face > faces(12);
	std::vector< uint32_t > attributes(12);
	vertices[0].position = Vector3( -0.5f, -0.5f, -0.5f );
	vertices[0].weight[0] = 1.0f;
	vertices[1].position = Vector3( 0.5f, -0.5f, -0.5f );
	vertices[1].weight[0] = 1.0f;
	vertices[2].position = Vector3( -0.5f, 0.5f, -0.5f);
	vertices[2].weight[0] = 1.0f;
	vertices[3].position = Vector3( 0.5f, 0.5f, -0.5f );
	vertices[3].weight[0] = 1.0f;
	vertices[4].position = Vector3( -0.5f, -0.5f, 0.5f );
	vertices[4].weight[0] = 1.0f;
	vertices[5].position = Vector3( 0.5f, -0.5f, 0.5f );
	vertices[5].weight[0] = 1.0f;
	vertices[6].position = Vector3( -0.5f, 0.5f, 0.5f );
	vertices[6].weight[0] = 1.0f;
	vertices[7].position = Vector3( 0.5f, 0.5f, 0.5f );
	vertices[7].weight[0] = 1.0f;
	faces[0].indices[1] = 0;
	faces[0].indices[1] = 1;
	faces[0].indices[2] = 2;
	faces[1].indices[0] = 2;
	faces[1].indices[1] = 1;
	faces[1].indices[2] = 3;
	faces[2].indices[0] = 6;
	faces[2].indices[1] = 5;
	faces[2].indices[2] = 4;
	faces[3].indices[0] = 7;
	faces[3].indices[1] = 5;
	faces[3].indices[2] = 6;
	faces[4].indices[0] = 4;
	faces[4].indices[1] = 1;
	faces[4].indices[2] = 0;
	faces[5].indices[0] = 5;
	faces[5].indices[1] = 1;
	faces[5].indices[2] = 4;
	faces[6].indices[0] = 2;
	faces[6].indices[1] = 3;
	faces[6].indices[2] = 6;
	faces[7].indices[0] = 6;
	faces[7].indices[1] = 3;
	faces[7].indices[2] = 7;
	faces[8].indices[0] = 4;
	faces[8].indices[1] = 0;
	faces[8].indices[2] = 2;
	faces[9].indices[0] = 6;
	faces[9].indices[1] = 4;
	faces[9].indices[2] = 2;
	faces[10].indices[0] = 5;
	faces[10].indices[1] = 3;
	faces[10].indices[2] = 1;
	faces[11].indices[0] = 7;
	faces[11].indices[1] = 3;
	faces[11].indices[2] = 5;

	attributes[0] = 0;
	attributes[1] = 0;
	attributes[2] = 0;
	attributes[3] = 0;
	attributes[4] = 0;
	attributes[5] = 0;
	attributes[6] = 0;
	attributes[7] = 0;
	attributes[8] = 0;
	attributes[9] = 0;
	attributes[10] = 0;
	attributes[11] = 0;

	Mesh* mesh = new Mesh;
	mesh->skeleton = new Skeleton;
	mesh->mediaLibrary = context->defaultMedia;
	mesh->vertices = vertices;
	mesh->faces = faces;
	mesh->attributes = attributes;
	mesh->indexBuffers = 0;
	mesh->vertexBuffer = 0;
	Bone* bone = new Bone;
	bone->matrix = matrixIdentity();
	bone->length = 1.0f;
	bone->mesh = new Mesh;
	bone->convex = NULL;
	mesh->skeleton->bones.push_back( bone );
	mesh->update();
	SDL_GL_MakeCurrent( NULL, NULL );
	context->defaultMedia->meshes.push_back( mesh );
}

Context::Context( uint32_t flags )
{
	gQuit = false;

	// Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	contextWindow = SDL_CreateWindow( "ContextWindow", 0, 0, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN );
	glContext = SDL_GL_CreateContext( contextWindow );
	SDL_GL_MakeCurrent(contextWindow, glContext);
	if(!glContext)
	{
		fprintf( stderr, "Error: %s\n", "Could not create GL Context." );
	}

	// Initialize GLEW
//	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf( stderr, "Error: %s\n", glewGetErrorString(err) );
	}

	// Initialize CG
	cgContext = cgCreateContext();
	if(!cgContext)
	{
		fprintf( stderr, "Error: %s\n", cgGetErrorString( cgGetError() ) );
	}
	cgGLRegisterStates( cgContext );

	// Initialize OpenAL
	alDevice = alcOpenDevice( NULL );
	alContext = alcCreateContext( alDevice, NULL );
	alcMakeContextCurrent( alContext );

	// Initialize Bullet
	physicsConfiguration = new btDefaultCollisionConfiguration();
	physicsDispatcher = new btCollisionDispatcher( physicsConfiguration );
	btVector3 worldMin( -1000, -1000, -1000 );
	btVector3 worldMax( 1000, 1000, 1000 );
	physicsBroadphase = new btAxisSweep3( worldMin, worldMax );
	physicsSolver = new btSequentialImpulseConstraintSolver;

	// Initialize FFMPEG
	av_register_all();

	// Initialize FreeType
	if(FT_Init_FreeType( &ftLibrary ))
	{
		fprintf( stderr, "Error occured while initializing FreeType.\n");
	}

	// Initialize FreeImage
	FreeImage_Initialise();

	// Build the default media.
	buildDefaultMedia( this );
}

Context::~Context()
{
	for( uint32_t i = 0; i < mediaLibraries.size(); i++ )
	{
		delete mediaLibraries[i];
	}
	for( uint32_t i = 0; i < windows.size(); i++ )
	{
		delete windows[i];
	}
	delete physicsSolver;
	delete physicsBroadphase;
	delete physicsDispatcher;
	delete physicsConfiguration;
	cgDestroyContext( cgContext );
	SDL_GL_DeleteContext( glContext );
	SDL_Quit();
	alcMakeContextCurrent( NULL );
	alcDestroyContext( alContext );
	alcCloseDevice( alDevice );
}

void Material::setEffectVariable( const std::string& variable, const std::vector< float >& data )
{
	CGparameter cgVariable = cgGetNamedEffectParameter( this->shaderProgram->effect, variable.c_str() );
	bool found = false;
	for( uint32_t i = 0; i < variables.size(); i++ )
	{

		if(variables[i].first == cgVariable)
		{
			variables[i].second = data;
			found = true;
		}
	}
	if(!found)
	{
		variables.push_back( std::make_pair( cgVariable, data ) );
	}
}

void Material::setEffectTexture(const std::string& variable, Texture* texture)
{
	CGparameter cgVariable = cgGetNamedEffectParameter( this->shaderProgram->effect, variable.c_str() );
	bool found = false;
	for( uint32_t i = 0; i < textures.size(); i++ )
	{
		if(textures[i].first == cgVariable )
		{
			textures[i].second = texture;
			found = true;
		}
	}
	if(!found)
	{
		textures.push_back( std::make_pair( cgVariable, texture ) );
	}
}

void Material::release()
{
	this->textures.clear();
	this->variables.clear();
	delete this;
}

void Shader::release()
{
	for( uint32_t e = 0; e < mLibrary->shaders.size(); e++ )
	{
		if( mLibrary->shaders[e] == this )
		{
			mLibrary->shaders.erase( mLibrary->shaders.begin() + e );
		}
	}
	cgDestroyEffect( effect );
	delete this;
}

void Texture::release()
{
	for( uint32_t i = 0; i < mLibrary->textures.size(); i++ )
	{
		if( mLibrary->textures[i] == this )
		{
			mLibrary->textures.erase( mLibrary->textures.begin() + i );
		}
	}
	glDeleteTextures(1, &image);
	delete this;
}

void Context::start()
{
	uint32_t previousTime = SDL_GetTicks();

	// Main message loop
	while( !gQuit )
	{
		uint32_t currentTime = SDL_GetTicks();
		uint32_t elapsedTime = currentTime - previousTime;
		for( uint32_t ml = 0; ml < mediaLibraries.size(); ml++ )
		{
			for( uint32_t s = 0; s < mediaLibraries[ml]->scenes.size(); s++ )
			{
				mediaLibraries[ml]->scenes[s]->update(elapsedTime);
			}
		}
		for( uint32_t w = 0; w < windows.size(); w++ )
		{
			for( uint32_t r = 0; r < windows[w]->renderTargets.size(); r++ )
			{
				windows[w]->renderTargets[r]->render();
			}
		}
		for( uint32_t w = 0; w < windows.size(); w++ )
		{
			windows[w]->doEvents();
		}
		previousTime = currentTime;
	}
}

UDim::UDim()
{
	this->offset = 0;
	this->scale = 0;
}

UDim::UDim( int offset )
{
	this->offset = offset;
	this->scale = 0.0f;
}

UDim::UDim( float scale )
{
	this->offset = 0;
	this->scale = scale;
}

UDim::UDim( int32_t offset, float scale )
{
	this->offset = offset;
	this->scale = scale;
}

URect::URect()
{
	left = UDim( 0, 0.0f );
	top = UDim( 0, 0.0f );
	right = UDim( 0, 0.0f );
	bottom = UDim( 0, 0.0f );
}

URect::URect( UDim left, UDim top, UDim right, UDim bottom )
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}

Rect::Rect()
{
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
}

Rect::Rect( int32_t left, int32_t top, int32_t right, int32_t bottom )
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}
}
