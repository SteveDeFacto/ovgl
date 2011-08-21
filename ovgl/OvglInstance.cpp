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
	Ovgl::Shader* BlurEffect = new Ovgl::Shader;
	Ovgl::Shader* BloomEffect = new Ovgl::Shader;
	Ovgl::Shader* AddEffect = new Ovgl::Shader;

	DefaultEffect->MLibrary = inst->DefaultMedia;
	SkyboxEffect->MLibrary = inst->DefaultMedia;
	BlurEffect->MLibrary = inst->DefaultMedia;
	BloomEffect->MLibrary = inst->DefaultMedia;
	AddEffect->MLibrary = inst->DefaultMedia;

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
	"	float4 posVS				: POSITION;"
	"	float4 posWS				: TEXCOORD0;"
	"	float2 tex					: TEXCOORD1;"
	"	float4 norm					: TEXCOORD2;"
	"};"

	"struct FS_OUTPUT"
	"{"
	"	float4 color				: COLOR;"
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
	"float4x4 Bones[64]				: BONES;"
	"uniform sampler2D txDiffuse;"
	"uniform samplerCUBE txEnvironment;"

	"FS_INPUT VS( VS_INPUT In )"
	"{"
	"	FS_INPUT Out;"
	"	float4x4 skinTransform = 0;"
	"	float3x3 normTransform = 0;"
	"	skinTransform += Bones[In.bi.x] * In.bw.x;"
	"	skinTransform += Bones[In.bi.y] * In.bw.y;"
	"	skinTransform += Bones[In.bi.z] * In.bw.z;"
	"	skinTransform += Bones[In.bi.w] * In.bw.w;"
	"	normTransform += (float3x3)(Bones[In.bi.x] * In.bw.x);"
	"	normTransform += (float3x3)(Bones[In.bi.y] * In.bw.y);"
	"	normTransform += (float3x3)(Bones[In.bi.z] * In.bw.z);"
	"	normTransform += (float3x3)(Bones[In.bi.w] * In.bw.w);"
	"	Out.posVS = mul(float4(In.pos, 1), skinTransform);"
	"	Out.posWS = Out.posVS;"
	"	Out.norm = float4(In.norm, 1);"
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
	"		float4 attenuation = 1/length(lightDir);"
	"		light += LightColors[i] * NdotL * attenuation * 10;"
	"	}"
	"	float4 envColor = texCUBE( txEnvironment, reflect( normalize( In.posWS.xyz - ViewPos.xyz ), In.norm.xyz ) ) * EMI;"
	"	float4 texColor = tex2D( txDiffuse, In.tex );"
	"	Out.color = ( (texColor + envColor) * Diffuse) * (light + Ambient);"
	"	Out.color.w = min(1.0, Out.color.w);"
	"	return Out;"
	"}";
	DefaultEffect->VertexProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), inst->CgVertexProfile, "VS", NULL );
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

	DefaultEffect->FragmentProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), inst->CgFragmentProfile, "FS", NULL );
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

	SkyboxEffect->VertexProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), inst->CgVertexProfile, "VS", NULL );
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

	SkyboxEffect->FragmentProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), inst->CgFragmentProfile, "FS", NULL );
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

	"struct FS_INPUT"
	"{"
	"  float4 pos				: POSITION;"
	"  float2 tex				: TEXCOORD0;"
	"};"

	"struct FS_OUTPUT"
	"{"
	"  float4 color				: COLOR;"
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
	"}";

	BlurEffect->VertexProgram = NULL;

	BlurEffect->FragmentProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), inst->CgFragmentProfile, "FS", NULL );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( BlurEffect->FragmentProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	BlurEffect->GeometryProgram = NULL;

	shader =
	"struct FS_INPUT"
	"{"
	"  float4 pos				: POSITION;"
	"  float2 tex				: TEXCOORD0;"
	"};"

	"struct FS_OUTPUT"
	"{"
	"  float4 color				: COLOR;"
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
	"}";

	BloomEffect->VertexProgram = NULL;

	BloomEffect->FragmentProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), inst->CgFragmentProfile, "FS", NULL );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( BloomEffect->FragmentProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	BloomEffect->GeometryProgram = NULL;

	shader =
	"struct FS_INPUT"
	"{"
	"  float4 pos				: POSITION;"
	"  float2 tex				: TEXCOORD0;"
	"};"

	"struct FS_OUTPUT"
	"{"
	"  float4 color				: COLOR;"
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
	"}";

	AddEffect->VertexProgram = NULL;

	AddEffect->FragmentProgram = cgCreateProgram( inst->CgContext, CG_SOURCE, shader.c_str(), inst->CgFragmentProfile, "FS", NULL );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}
	cgGLLoadProgram( AddEffect->FragmentProgram );
	string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		OutputDebugStringA( string );
	}

	AddEffect->GeometryProgram = NULL;

	inst->DefaultMedia->Shaders.push_back( DefaultEffect );
	inst->DefaultMedia->Shaders.push_back( SkyboxEffect );
	inst->DefaultMedia->Shaders.push_back( BlurEffect );
	inst->DefaultMedia->Shaders.push_back( BloomEffect );
	inst->DefaultMedia->Shaders.push_back( AddEffect );

	// Create Default Material
	Ovgl::Material* DefaultMaterial = new Ovgl::Material;
	
	DefaultMaterial->ShaderProgram = DefaultEffect;
	DefaultMaterial->MLibrary = inst->DefaultMedia;
	DefaultMaterial->set_texture("txDiffuse", DefaultMaterial->MLibrary->CreateTexture( 256, 256) );
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
	mesh->ml = inst->DefaultMedia;
	mesh->vertices = vertices;
	mesh->faces = faces;
	mesh->attributes = attributes;
	mesh->IndexBuffers = NULL;
	mesh->VertexBuffer = NULL;
	Ovgl::Bone* bone = new Ovgl::Bone;
	bone->matrix = Ovgl::MatrixIdentity();
	bone->length = 1.0f;
	bone->mesh = new Ovgl::Mesh;
	bone->convex = NULL;
	mesh->bones.push_back(bone);
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
	rendertarget->debugMode = false;
	rendertarget->bloom = 4;
	rendertarget->motionBlur = true;
	rendertarget->multiSample = true;

	int pixelFormat;
	UINT numFormats;
	float fAttributes[] = {0,0};

	int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,24,
		WGL_ALPHA_BITS_ARB,0,
		WGL_DEPTH_BITS_ARB,0,
		WGL_STENCIL_BITS_ARB,0,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		0, 0};
	wglChoosePixelFormatARB( rendertarget->hDC, iAttributes, fAttributes,1, &pixelFormat, &numFormats);
	SetPixelFormat( rendertarget->hDC, pixelFormat, NULL );
	wglMakeCurrent( rendertarget->hDC, hRC );
	SwapBuffers(rendertarget->hDC);

	// Multi sample framebuffer
	glGenFramebuffers(1, &rendertarget->MultiSampleFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, rendertarget->MultiSampleFrameBuffer);

	// Multi sample colorbuffer
	glGenRenderbuffers(1, &rendertarget->ColorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, rendertarget->ColorBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA16F, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rendertarget->ColorBuffer);

	// Multi sample depthbuffer
	glGenRenderbuffers(1, &rendertarget->DepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, rendertarget->DepthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rendertarget->DepthBuffer);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		OutputDebugString( L"Unable to create multi sample frame buffer" );
	}

	// Effect framebuffer
	glGenFramebuffers(1, &rendertarget->EffectFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, rendertarget->EffectFrameBuffer);

	// Create and bind texture
	glGenTextures(1, &rendertarget->PrimaryTex);
	glBindTexture(GL_TEXTURE_2D, rendertarget->PrimaryTex);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenTextures(1, &rendertarget->SecondaryTex);
	glBindTexture(GL_TEXTURE_2D, rendertarget->SecondaryTex);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &rendertarget->PrimaryBloomTex);
	glBindTexture(GL_TEXTURE_2D, rendertarget->PrimaryBloomTex);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (WindowRect.right - WindowRect.left)/4, (WindowRect.bottom - WindowRect.top)/4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &rendertarget->SecondaryBloomTex);
	glBindTexture(GL_TEXTURE_2D, rendertarget->SecondaryBloomTex);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (WindowRect.right - WindowRect.left)/4, (WindowRect.bottom - WindowRect.top)/4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		OutputDebugString( L"Unable to create effect frame buffer" );
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderTargets.push_back(rendertarget);
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
	HWND temphWindow = CreateWindowA( "OvglWinClass", "", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wcex.hInstance, NULL );
	instance->hWnd = temphWindow;
	// Set window pixel format
	instance->hDC = GetDC( temphWindow );
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	int iFormat = ChoosePixelFormat( instance->hDC, &pfd );
	SetPixelFormat( instance->hDC, iFormat, NULL );

	// Create GL context
	HGLRC tempContext = wglCreateContext( instance->hDC );
	wglMakeCurrent( instance->hDC, tempContext );
	instance->hRC = tempContext;
	glewInit();

	// Create a simple hidden window so we can create a GL context from it
	instance->hWnd = CreateWindowA( "OvglWinClass", "", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wcex.hInstance, NULL );

	instance->hDC = GetDC( instance->hWnd );

	int pixelFormat;
	UINT numFormats;
	float fAttributes[] = {0,0};

	int iAttributes[] = 
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 24,
		WGL_ALPHA_BITS_ARB, 0,
		WGL_DEPTH_BITS_ARB, 0,
		WGL_STENCIL_BITS_ARB, 0,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		0, 0
	};
	wglChoosePixelFormatARB( instance->hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
	SetPixelFormat( instance->hDC, pixelFormat, NULL );

	if( atof((const char*)glGetString(GL_VERSION)) >= 3.1 )
	{
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			0
		};
		instance->hRC = wglCreateContextAttribsARB( instance->hDC, 0, attribs );
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( tempContext );
		DestroyWindow( temphWindow );
		wglMakeCurrent( instance->hDC, instance->hRC );
	}
	else
	{
		OutputDebugString( L"OpenGL 3.1 is not supported." );
	}
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_CULL_FACE);

	// Initialize CG
	instance->CgContext = cgCreateContext();
	instance->CgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	instance->CgFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

	// Initialize OpenAL
	ALCdevice *device = alcOpenDevice(NULL);
	ALCcontext *context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	// Initialize Bullet
	instance->PhysicsConfiguration = new btDefaultCollisionConfiguration();
	instance->PhysicsDispatcher = new	btCollisionDispatcher(instance->PhysicsConfiguration);
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	instance->PhysicsBroadphase = new btAxisSweep3(worldMin,worldMax);
	instance->PhysicsSolver = new btSequentialImpulseConstraintSolver;

	// Build the default media.
	BuildDefaultMedia( instance );

	// Initialize FbxSdk
	instance->FBXManager = KFbxSdkManager::Create();

	return instance;
}

void Ovgl::Instance::Release()
{
	for( DWORD i = 0; i < MediaLibraries.size(); i++ )
	{
		MediaLibraries[i]->Release();
	}
	for( DWORD r = 0; r < RenderTargets.size(); r++ )
	{
		RenderTargets[r]->Release();
	}

	DefaultMedia->Release();

	delete PhysicsSolver;
	delete PhysicsBroadphase;
	delete PhysicsDispatcher;
	delete PhysicsConfiguration;

	CoUninitialize();
};

void Ovgl::Material::set_variable(const std::string& variable, const std::vector<float>& data )
{
	bool Found = false;
	for( unsigned int i = 0; i < Variables.size(); i++ )
	{
		if(Variables[i].first.compare(variable) == 0)
		{
			Variables[i].second = data;
			Found = true;
		}
	}
	if(!Found)
	{
		Variables.push_back( make_pair( variable, data ) );
	}
}

void Ovgl::Material::set_texture(const std::string& variable, Texture* texture)
{
	bool Found = false;
	for( unsigned int i = 0; i < Textures.size(); i++ )
	{
		if(Textures[i].first.compare(variable) == 0)
		{
			Textures[i].second = texture;
			Found = true;
		}
	}
	if(!Found)
	{
		Textures.push_back( make_pair( variable, texture ) );
	}
}

void Ovgl::Material::Release()
{
	this->Textures.clear();
	this->Variables.clear();
	delete this;
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
	cgDestroyProgram( VertexProgram );
	cgDestroyProgram( FragmentProgram );
	cgDestroyProgram( GeometryProgram );
	delete this;
}

void Ovgl::Texture::Release()
{
	for( unsigned int i = 0; i < MLibrary->Textures.size(); i++ )
	{
		if( MLibrary->Textures[i] == this )
		{
			MLibrary->Textures.erase( MLibrary->Textures.begin() + i );
		}
	}
	glDeleteTextures(1, &Image);
	delete this;
}

Ovgl::MediaLibrary* Ovgl::Instance::CreateMediaLibrary( const std::string& file )
{
	Ovgl::MediaLibrary* MediaLibrary = new Ovgl::MediaLibrary;
	MediaLibrary->Inst = this;
	MediaLibraries.push_back(MediaLibrary);
	return MediaLibrary;
}