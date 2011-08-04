/**
* @file OvglInstance.cpp
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
#include "OvglAudio.h"
#include "OvglMesh.h"
#include "OvglScene.h"

void BuildDefaultMedia( Ovgl::Instance* inst )
{
	inst->DefaultMedia = new Ovgl::MediaLibrary;
	inst->DefaultMedia->Inst = inst;

	Ovgl::Shader* DefaultEffect = new Ovgl::Shader;
	Ovgl::Shader* SkyboxEffect = new Ovgl::Shader;
	Ovgl::Shader* SpriteEffect = new Ovgl::Shader;

	DefaultEffect->MLibrary = inst->DefaultMedia;
	SkyboxEffect->MLibrary = inst->DefaultMedia;
	SpriteEffect->MLibrary = inst->DefaultMedia;

	std::string shader;

	// Create shader string.
	shader =
	"struct VS_INPUT"
	"{"
	"	float3 pos					: ATTR0;"
	"	float3 norm					: ATTR1;"
	"	float2 tex					: ATTR2;"
	"	float4 bw					: ATTR3;"
	"	float4 bi					: ATTR4;"
	"};"

	"struct FS_INPUT"
	"{"
	"  float4 posVS					: POSITION;"
	"  float4 posWS					: TEXCOORD0;"
	"  float2 tex					: TEXCOORD1;"
	"  float4 norm					: TEXCOORD2;"
	"};"

	"struct FS_OUTPUT"
	"{"
	"  float4 color					: COLOR;"
	"};"

	"float4 Ambient = float4( 0.0f, 0.0f, 0.0f, 1.0f );"
	"float4 Diffuse = float4( 0.75f, 0.75f, 0.75f, 1.0f );"
	"float EMI = 0.1f;"
	"float LightCount				: LIGHTCOUNT;"
	"float4 ViewPos					: VIEWPOS;"
	"float4 Lights[16]				: LIGHTS;"
	"float4 LightColors[16]			: LIGHTCOLORS;"
	"float4x4 World					: WORLD;"
	"float4x4 ViewProj				: VIEWPROJ;"
	"float4x4 Bones[64]			: BONES;"
	"uniform sampler2D txDiffuse;"
	"uniform samplerCUBE txEnvironment;"

	"FS_INPUT VS( VS_INPUT In )"
	"{"
	"  FS_INPUT Out;"
	"  float4x4 skinTransform = 0;"
	"  float3x3 normTransform = 0;"
	"  skinTransform += Bones[In.bi.x] * In.bw.x;"
	"  skinTransform += Bones[In.bi.y] * In.bw.y;"
	"  skinTransform += Bones[In.bi.z] * In.bw.z;"
	"  skinTransform += Bones[In.bi.w] * In.bw.w;"
	"  normTransform += (float3x3)(Bones[In.bi.x] * In.bw.x);"
	"  normTransform += (float3x3)(Bones[In.bi.y] * In.bw.y);"
	"  normTransform += (float3x3)(Bones[In.bi.z] * In.bw.z);"
	"  normTransform += (float3x3)(Bones[In.bi.w] * In.bw.w);"
	"  Out.posVS = mul(float4(In.pos, 1), skinTransform);"
	"  Out.posWS = Out.posVS;"
	"  Out.norm = float4(In.norm, 1);"
	"  Out.tex = In.tex;"
	"  Out.posVS = mul(Out.posVS, ViewProj);"
	"  return Out;"
	"}"

	"FS_OUTPUT FS( FS_INPUT In )"
	"{"
	"  FS_OUTPUT Out;"
	"  float4 light = float4( 0, 0, 0, 0 );"
	"  for(float i = 0; i < LightCount; i++)"
	"  {"
	"	float4 lightDir = Lights[i] - In.posWS;"
	"	float4 NdotL = saturate(dot(In.norm, normalize(lightDir)));"
	"	float4 attenuation = 1/length(lightDir);"
	"	light += LightColors[i] * NdotL * attenuation * 10;"
	"  }"
	"  float4 envColor = texCUBE( txEnvironment, reflect( normalize( In.posWS.xyz - ViewPos.xyz ), In.norm.xyz ) ) * EMI;"
	"  float4 texColor = tex2D( txDiffuse, In.tex );"
	"  Out.color = ( (texColor + envColor) * Diffuse) * (light + Ambient);"
	"  return Out;"
	"}";

	DefaultEffect->VertexProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), CG_PROFILE_GPU_VP, "VS", NULL );
	CGerror error;
	const char* string;
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( DefaultEffect->VertexProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	DefaultEffect->FragmentProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), CG_PROFILE_GPU_FP, "FS", NULL );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( DefaultEffect->FragmentProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	DefaultEffect->GeometryProgram = NULL;

	shader =
	"struct VS_INPUT"
	"{"
	"	float3 pos				: ATTR0;"
	"	float3 norm				: ATTR1;"
	"	float2 tex				: ATTR2;"
	"	float4 bw				: ATTR3;"
	"	float4 bi				: ATTR4;"
	"};"

	"struct FS_INPUT"
	"{"
	"  float4 pos				: POSITION;"
	"  float3 tex				: TEXCOORD0;"
	"};"

	"struct FS_OUTPUT"
	"{"
	"  float4 color				: COLOR;"
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
	"}";

	SkyboxEffect->VertexProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), CG_PROFILE_GPU_VP, "VS", NULL );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( SkyboxEffect->VertexProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	SkyboxEffect->FragmentProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), CG_PROFILE_GPU_FP, "FS", NULL );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( SkyboxEffect->FragmentProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	SkyboxEffect->GeometryProgram = NULL;

	shader =
	"struct VS_INPUT"
	"{"
	"	float2 pos				: ATTR0;"
	"	float2 tex				: ATTR1;"
	"};"

	"struct FS_INPUT"
	"{"
	"  float4 pos				: POSITION;"
	"  float3 tex				: TEXCOORD0;"
	"};"

	"struct FS_OUTPUT"
	"{"
	"  float4 color				: COLOR;"
	"};"

	"uniform sampler2D txDiffuse;"
	"float4x4 World;"

	"FS_INPUT VS( VS_INPUT In )"
	"{"
	"	FS_INPUT Out;"
	"	Out.pos = mul( float4( In.pos.xy, -10.0, 0.0 ), World );"
	"	Out.tex = float3( In.tex, 0.0 );"
	"	return Out;"
	"}"

	"FS_OUTPUT FS( FS_INPUT In)"
	"{"
	"	FS_OUTPUT Out;"
	"	Out.color = float4( 1.0, 1.0, 1.0, 1.0 );"
	//"	Out.color = tex2D( txDiffuse, In.tex );"
	"	return Out;"
	"}";

	SpriteEffect->VertexProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), CG_PROFILE_GPU_VP, "VS", NULL );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( SpriteEffect->VertexProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	SpriteEffect->FragmentProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), CG_PROFILE_GPU_FP, "FS", NULL );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( SpriteEffect->FragmentProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	SpriteEffect->GeometryProgram = NULL;

	inst->DefaultMedia->Shaders.push_back( DefaultEffect );
	inst->DefaultMedia->Shaders.push_back( SkyboxEffect );
	inst->DefaultMedia->Shaders.push_back( SpriteEffect );

	// Create Default Material
	Ovgl::Material* DefaultMaterial = new Ovgl::Material;
	DefaultMaterial->ShaderProgram = DefaultEffect;
	DefaultMaterial->NoZBuffer = false;
	DefaultMaterial->NoZWrite = false;
	DefaultMaterial->PostRender = false;
	inst->DefaultMedia->Materials.push_back(DefaultMaterial);

	// Create Sky Box
	std::vector<Ovgl::Vertex> vertices(8);
	std::vector<Ovgl::Face> faces(12);
	std::vector<DWORD> attributes(12);
	vertices[0].position = Ovgl::Vector3Set( -0.5f, -0.5f, -0.5f );
	vertices[0].weight[0] = 1.0f;
	vertices[1].position = Ovgl::Vector3Set( 0.5f, -0.5f, -0.5f );
	vertices[1].weight[0] = 1.0f;
	vertices[2].position = Ovgl::Vector3Set( -0.5f, 0.5f, -0.5f);
	vertices[2].weight[0] = 1.0f;
	vertices[3].position = Ovgl::Vector3Set( 0.5f, 0.5f, -0.5f );
	vertices[3].weight[0] = 1.0f;
	vertices[4].position = Ovgl::Vector3Set( -0.5f, -0.5f, 0.5f );
	vertices[4].weight[0] = 1.0f;
	vertices[5].position = Ovgl::Vector3Set( 0.5f, -0.5f, 0.5f );
	vertices[5].weight[0] = 1.0f;
	vertices[6].position = Ovgl::Vector3Set( -0.5f, 0.5f, 0.5f );
	vertices[6].weight[0] = 1.0f;
	vertices[7].position = Ovgl::Vector3Set( 0.5f, 0.5f, 0.5f );
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

	Ovgl::Mesh* mesh = new Ovgl::Mesh;
	mesh->vertices = vertices;
	mesh->faces = faces;
	mesh->attributes = attributes;
	mesh->IndexBuffers = NULL;
	mesh->VertexBuffer = NULL;
	mesh->Update();
	inst->DefaultMedia->Meshes.push_back(mesh);
}

Ovgl::RenderTarget* Ovgl::Instance::CreateRenderTarget( HWND hWnd, RECT* rect, DWORD flags )
{
	RECT WindowRect;
	GetWindowRect( hWnd, &WindowRect );
	Ovgl::RenderTarget* rendertarget = new Ovgl::RenderTarget;
	rendertarget->Inst = this;
	rendertarget->hWnd = hWnd;
	rendertarget->hDC = GetDC(hWnd);
	rendertarget->view = NULL;
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	int iFormat = ChoosePixelFormat( rendertarget->hDC, &pfd );
	SetPixelFormat( rendertarget->hDC, iFormat, &pfd );
	SwapBuffers( rendertarget->hDC );
	this->RenderTargets.push_back(rendertarget);
	return rendertarget;
};

Ovgl::Instance* Ovgl::Create( DWORD flags )
{
	// Create new instance
	Ovgl::Instance* instance = new Ovgl::Instance; 

	// Initialize COM
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Create a simple hidden window so we can create a GL context from it
    WNDCLASSEX wcex;
	ZeroMemory( &wcex, sizeof( wcex ) );
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = GetModuleHandle( NULL );
	wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.lpszClassName = L"OvglWinClass";
    RegisterClassEx( &wcex );
	instance->hWnd = CreateWindowA( "OvglWinClass", "", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wcex.hInstance, NULL );

	// Set window pixel format
	instance->hDC = GetDC( instance->hWnd );
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	int iFormat = ChoosePixelFormat( instance->hDC, &pfd );
	SetPixelFormat( instance->hDC, iFormat, &pfd );

	// Create GL context
	HGLRC tempContext = wglCreateContext( instance->hDC );
	wglMakeCurrent( instance->hDC, tempContext );
	glewInit();
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		0
	};
	instance->hRC = wglCreateContextAttribsARB( instance->hDC, 0, attribs );
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( tempContext );
	wglMakeCurrent( instance->hDC, instance->hRC );

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_CULL_FACE);
	glFrontFace(GL_CW);

	// Initialize CG
	instance->CgContext = cgCreateContext();

	// Initialize OpenAL
	ALCdevice *device = alcOpenDevice(NULL);
	ALCcontext *context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	// Initialize Bullet
	instance->PhysicsConfiguration = new btDefaultCollisionConfiguration();
	instance->PhysicsDispatcher = new	btCollisionDispatcher(instance->PhysicsConfiguration);
	instance->PhysicsBroadphase = new btDbvtBroadphase();
	instance->PhysicsSolver = new btSequentialImpulseConstraintSolver;

	// Create temporary arrays to hold mesh data.
	std::vector<Ovgl::Vertex> vertices;
	std::vector<Ovgl::Face> faces;

	// Create pyramid shape.
	vertices.resize(5);
	faces.resize(6);
	vertices[0].position = Ovgl::Vector3Set( 0.0f, 0.0f, 0.0f );
	vertices[1].position = Ovgl::Vector3Set( -0.5f, -0.5f, 1.0f );
	vertices[2].position = Ovgl::Vector3Set( 0.5f, -0.5f, 1.0f );
	vertices[3].position = Ovgl::Vector3Set( -0.5f, 0.5f, 1.0f );
	vertices[4].position = Ovgl::Vector3Set( 0.5f, 0.5f, 1.0f);
	faces[0].indices[0] = 2;
	faces[0].indices[1] = 1;
	faces[0].indices[2] = 0;
	faces[1].indices[0] = 4;
	faces[1].indices[1] = 2;
	faces[1].indices[2] = 0;
	faces[2].indices[0] = 3;
	faces[2].indices[1] = 4;
	faces[2].indices[2] = 0;
	faces[3].indices[0] = 1;
	faces[3].indices[1] = 3;
	faces[3].indices[2] = 0;
	faces[4].indices[0] = 3;
	faces[4].indices[1] = 1;
	faces[4].indices[2] = 2;
	faces[5].indices[0] = 2;
	faces[5].indices[1] = 4;
	faces[5].indices[2] = 3;
	instance->Shapes[0] = new btConvexHullShape((float*)&vertices[0], vertices.size(), sizeof(Ovgl::Vertex));

	// Build the default media.
	BuildDefaultMedia( instance );

	// Initialize FbxSdk
	instance->FBXManager = KFbxSdkManager::Create();

	return instance;
}

void Ovgl::Instance::Release()
{
	for( DWORD r = 0; r < RenderTargets.size(); r++ )
	{
		RenderTargets[r]->Release();
	}
	CoUninitialize();
};

void Ovgl::Material::set_variable(const std::string& variable, const std::vector<float>& data )
{
	Variables.push_back( make_pair( variable, data ) );
}

void Ovgl::Material::set_texture(const std::string& variable, Texture* texture)
{
	Textures.push_back( make_pair( variable, texture ) );
}

void Ovgl::Shader::Release()
{
	for( DWORD e = 0; e < MLibrary->Shaders.size(); e++ )
	{
		if( MLibrary->Shaders[e] == this )
		{
			MLibrary->Shaders.erase( MLibrary->Shaders.begin() + e );
		}
	}
	delete this;
}

Ovgl::MediaLibrary* Ovgl::Instance::CreateMediaLibrary( const std::string& file )
{
	Ovgl::MediaLibrary* MediaLibrary = new Ovgl::MediaLibrary;
	MediaLibrary->Inst = this;
	return MediaLibrary;
}