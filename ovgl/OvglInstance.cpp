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
#include "OvglGraphics.h"
#include "OvglAudio.h"
#include "OvglMesh.h"
#include "OvglScene.h"

//void ErrorExit(LPTSTR lpszFunction) 
//{ 
//    // Retrieve the system error message for the last-error code
//
//    LPVOID lpMsgBuf;
//    LPVOID lpDisplayBuf;
//    DWORD dw = GetLastError(); 
//
//    FormatMessage(
//        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//        FORMAT_MESSAGE_FROM_SYSTEM |
//        FORMAT_MESSAGE_IGNORE_INSERTS,
//        NULL,
//        dw,
//        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//        (LPTSTR) &lpMsgBuf,
//        0, NULL );
//
//    // Display the error message and exit the process
//
//    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
//	StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf); 
//    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
//
//    LocalFree(lpMsgBuf);
//    LocalFree(lpDisplayBuf);
//    ExitProcess(dw); 
//}

Ovgl::Effect* BuildDefaultEffect( Ovgl::Instance* inst )
{
	Ovgl::Effect* effect = new Ovgl::Effect;
	effect->Inst = inst;

	// Create shader string.
	std::string shader = 
	"float4 Ambient = float4( 0.47f, 0.47f, 0.47f, 1.0f );"
	"float4 Diffuse = float4( 0.5f, 0.5f, 0.5f, 1.0f );"
	"int Light_Count			: LIGHTCOUNT;"
	"float4 Lights[255]			: LIGHTARRAY;"
	"float4 LightColors[255]	: LIGHTCOLORARRAY;"
	"float4x4 World				: WORLD;"
	"float4x4 View				: WORLDVIEW;"
	"float4x4 Projection		: PROJECTION;"
	"float4x4 Bones[255]		: BONEARRAY;"
	"struct VS_INPUT"
	"{"
	"	float3 pos				: POSITION;"
	"	float3 norm				: NORMAL;"
	"	float2 tex				: TEXCOORD0;"
	"	float4 bw				: TEXCOORD1;"
	"	float4 bi				: TEXCOORD2;"
	"};"
	"struct PS_INPUT"
	"{"
	"	float4 pos				: SV_POSITION;"
	"	float4 pos2				: POSITION;"
	"	float2 tex				: TEXCOORD0;"
	"	float4 norm				: NORMAL;"
	"};"
	"PS_INPUT VS( VS_INPUT In )"
	"{"
	"	PS_INPUT Out = (PS_INPUT)0;"
	"   float4x4 skinTransform = 0;"
	"   skinTransform += Bones[In.bi.x] * In.bw.x;"
	"   skinTransform += Bones[In.bi.y] * In.bw.y;"
	"   skinTransform += Bones[In.bi.z] * In.bw.z;"
	"   skinTransform += Bones[In.bi.w] * In.bw.w;"
	"   Out.pos = mul(float4(In.pos, 1), skinTransform);"
	"	Out.pos2 = Out.pos;"
	"	Out.norm = normalize(mul(float4(In.norm, 0), skinTransform));"
	"   Out.pos = mul(mul(Out.pos, View), Projection);"
	"	Out.tex = In.tex;"
	"	return Out;"
	"}"
	"float4 PS( PS_INPUT In) : SV_Target"
	"{"
	"	float4 color = float4( 0, 0, 0, 0 );"
	"	for(int i = 0; i < Light_Count; i++)"
	"	{"
	"		float4 lightDir = Lights[i] - In.pos2;"
	"		float4 NdotL = saturate(dot(In.norm, normalize(lightDir)));"
	"		float4 attenuation = 1/length(lightDir);"
	"		color += LightColors[i] * NdotL * attenuation * 10;"
	"	}"    
	"	color.w = 1;"
	"	return color * Diffuse;"
	"}"
	"technique10 Render"
	"{"
	"	pass P0"
	"	{"
	"		SetVertexShader( CompileShader( vs_4_0, VS() ) );"
	"		SetGeometryShader( NULL );"
	"		SetPixelShader( CompileShader( ps_4_0, PS() ) );"
	"	}"
	"}";

	//  Create effect.
	D3DX10CreateEffectFromMemory( shader.c_str(), shader.size(), NULL, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, inst->D3DDevice, NULL, NULL, &effect->SFX, NULL, NULL );
	
	// Get shader variables.
	effect->Technique = effect->SFX->GetTechniqueByName( "Render" );
	effect->Shadow_Maps = effect->SFX->GetVariableBySemantic( "SHADOWMAPS" )->AsShaderResource();
	effect->Cube_Views = effect->SFX->GetVariableBySemantic( "CUBEVIEWS" )->AsMatrix();
	effect->Light_Count = effect->SFX->GetVariableBySemantic( "LIGHTCOUNT" )->AsScalar();
	effect->Lights = effect->SFX->GetVariableBySemantic( "LIGHTARRAY" )->AsVector();
	effect->Light_Colors = effect->SFX->GetVariableBySemantic( "LIGHTCOLORARRAY" )->AsVector();
    effect->Bones = effect->SFX->GetVariableBySemantic( "BONEARRAY" )->AsMatrix();
    effect->View = effect->SFX->GetVariableBySemantic( "WORLDVIEW" )->AsMatrix();
	effect->Projection = effect->SFX->GetVariableBySemantic( "PROJECTION" )->AsMatrix();

	// Get technique.
	D3D10_PASS_DESC PassDesc;
	effect->Technique->GetPassByIndex( 0 )->GetDesc( &PassDesc );

	// Create vertex layout desc.
	D3D10_INPUT_ELEMENT_DESC vertexlayout[] =
	{
		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,	24,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	32,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	48,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create vertex layout.
	inst->D3DDevice->CreateInputLayout( vertexlayout, sizeof(vertexlayout)/sizeof(vertexlayout[0]), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &effect->Layout );

	//Return effect.
	return effect;
}

Ovgl::Effect* Ovgl::Instance::CreateEffect( const std::string& file )
{
	Ovgl::Effect* effect = new Ovgl::Effect;
	effect->Inst = this;
	
	//  Create effect.
	std::wstring wfilename;
	wfilename.assign( file.begin(), file.end() );
	D3DX10CreateEffectFromFile( wfilename.c_str(), NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, D3DDevice, NULL, NULL, &effect->SFX, NULL, NULL );
	
	// Get shader variables.
	effect->Technique = effect->SFX->GetTechniqueByName( "Render" );
	effect->Shadow_Maps = effect->SFX->GetVariableBySemantic( "SHADOWMAPS" )->AsShaderResource();
	effect->Cube_Views = effect->SFX->GetVariableBySemantic( "CUBEVIEWS" )->AsMatrix();
	effect->Light_Count = effect->SFX->GetVariableBySemantic( "LIGHTCOUNT" )->AsScalar();
	effect->Lights = effect->SFX->GetVariableBySemantic( "LIGHTARRAY" )->AsVector();
	effect->Light_Colors = effect->SFX->GetVariableBySemantic( "LIGHTCOLORARRAY" )->AsVector();
    effect->Bones = effect->SFX->GetVariableBySemantic( "BONEARRAY" )->AsMatrix();
    effect->View = effect->SFX->GetVariableBySemantic( "WORLDVIEW" )->AsMatrix();
	effect->Projection = effect->SFX->GetVariableBySemantic( "PROJECTION" )->AsMatrix();

	// Get technique.
	D3D10_PASS_DESC PassDesc;
	effect->Technique->GetPassByIndex( 0 )->GetDesc( &PassDesc );

	// Create vertex layout desc.
	D3D10_INPUT_ELEMENT_DESC vertexlayout[] =
	{
		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,	24,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	32,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	48,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create vertex layout.
	D3DDevice->CreateInputLayout( vertexlayout, sizeof(vertexlayout)/sizeof(vertexlayout[0]), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &effect->Layout );

	//Add Effect to array
	Effects.push_back( effect );

	//Return effect.
	return effect;
}

Ovgl::Scene* Ovgl::Instance::CreateScene( const std::string& file, Ovgl::Matrix44* offset, DWORD flags )
{
	Ovgl::Scene* scene = new Ovgl::Scene;
	scene->Inst = this;
	NxSceneDesc sceneDesc;
	sceneDesc.gravity.set( 0.0f, -9.8f, 0.0f );
	if( PhysX->getHWVersion() != NX_HW_VERSION_NONE)
		#ifdef _DEBUG
			sceneDesc.simType = NX_SIMULATION_SW;
		#else
			sceneDesc.simType = NX_SIMULATION_HW;
		#endif
	scene->physics_scene = PhysX->createScene(sceneDesc);
	NxMaterial* defaultMaterial = scene->physics_scene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
	if( !file.empty() )
	{
		scene->Load(file, 0);
	}
	this->Scenes.push_back(scene);
	return scene;
};

Ovgl::RenderTarget* Ovgl::Instance::CreateRenderTarget( HWND hWnd, RECT* rect, DWORD flags )
{
	RECT WindowRect;
	GetWindowRect( hWnd, &WindowRect );
	Ovgl::RenderTarget* rendertarget = new Ovgl::RenderTarget;
	rendertarget->Inst = this;
	rendertarget->RenderTargetView = NULL;
	rendertarget->DepthStencilView = NULL;
	rendertarget->hWnd = hWnd;
	rendertarget->ViewPort = new D3D10_VIEWPORT;
	ZeroMemory( rendertarget->ViewPort, sizeof(D3D10_VIEWPORT) );
	rendertarget->ViewPort->MinDepth = 0.0f;
	rendertarget->ViewPort->MaxDepth = 1.0f;
	if( rect != NULL )
	{
		rendertarget->ViewPort->TopLeftX = rect->left;
		rendertarget->ViewPort->TopLeftY = rect->top;
		rendertarget->ViewPort->Width = rect->right - rect->left;
		rendertarget->ViewPort->Height = rect->bottom - rect->top;
	}
	else
	{
		rendertarget->ViewPort->TopLeftX = 0;
		rendertarget->ViewPort->TopLeftY = 0;
		rendertarget->ViewPort->Width = WindowRect.right - WindowRect.left;
		rendertarget->ViewPort->Height = WindowRect.bottom - WindowRect.top;
	}
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = WindowRect.right - WindowRect.left;
	sd.BufferDesc.Height = WindowRect.bottom - WindowRect.top;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	Factory->CreateSwapChain( D3DDevice, &sd, &rendertarget->SwapChain);
	ID3D10Texture2D* pBackBuffer;
	rendertarget->SwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBackBuffer );
	D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &rendertarget->RenderTargetView );
	pBackBuffer->Release();
	ID3D10Texture2D* DepthStencil;
	D3D10_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.Width = WindowRect.right - WindowRect.left;
	DepthStencilDesc.Height = WindowRect.bottom - WindowRect.top;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Usage = D3D10_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;
	D3DDevice->CreateTexture2D( &DepthStencilDesc, NULL, &DepthStencil );
	D3DDevice->CreateDepthStencilView( DepthStencil, NULL, &rendertarget->DepthStencilView );
	DepthStencil->Release();
	this->RenderTargets.push_back(rendertarget);
	return rendertarget;
};

Ovgl::Instance* Ovgl::Create( DWORD flags )
{
	//Create new instance
	Ovgl::Instance* instance = new Ovgl::Instance; 

	// Initialize COM
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Create DirectX device
	D3D10_CREATE_DEVICE_FLAG DeviceFlag;
	#ifdef _DEBUG
		DeviceFlag = D3D10_CREATE_DEVICE_DEBUG;
	#else
		DeviceFlag = D3D10_CREATE_DEVICE_SINGLETHREADED;
	#endif

	D3D10CreateDevice( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, DeviceFlag, D3D10_SDK_VERSION, &instance->D3DDevice );

	// Get factory from D3DDevice.
	IDXGIDevice * pDXGIDevice;
	IDXGIAdapter * pDXGIAdapter;
	instance->D3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&instance->Factory);

	// Get default views.
	instance->D3DDevice->OMGetRenderTargets( 1, &instance->RenderTargetView, NULL );
	instance->D3DDevice->PSGetShaderResources( 0, 1, &instance->ShaderResourceView );

	// Set Rasterizer State.
	D3D10_RASTERIZER_DESC rasterizerState;
    rasterizerState.FillMode = D3D10_FILL_SOLID;
    rasterizerState.CullMode = D3D10_CULL_FRONT;
    rasterizerState.FrontCounterClockwise = true;
    rasterizerState.DepthBias = false;
    rasterizerState.DepthBiasClamp = 0;
    rasterizerState.SlopeScaledDepthBias = 0;
    rasterizerState.DepthClipEnable = true;
    rasterizerState.ScissorEnable = false;
    rasterizerState.MultisampleEnable = false;
    rasterizerState.AntialiasedLineEnable = false;
    instance->D3DDevice->CreateRasterizerState( &rasterizerState, &instance->SolidRasterState );
    rasterizerState.FillMode = D3D10_FILL_WIREFRAME;
    rasterizerState.CullMode = D3D10_CULL_FRONT;
    rasterizerState.FrontCounterClockwise = true;
    rasterizerState.DepthBias = false;
    rasterizerState.DepthBiasClamp = 0;
    rasterizerState.SlopeScaledDepthBias = 0;
    rasterizerState.DepthClipEnable = true;
    rasterizerState.ScissorEnable = false;
    rasterizerState.MultisampleEnable = false;
    rasterizerState.AntialiasedLineEnable = false;
    instance->D3DDevice->CreateRasterizerState( &rasterizerState, &instance->WireFrameRasterState);

	// Set Blend State.
	ID3D10BlendState* BlendState = NULL;
	D3D10_BLEND_DESC BlendStateDesc;
	ZeroMemory(&BlendStateDesc, sizeof(D3D10_BLEND_DESC));
	BlendStateDesc.AlphaToCoverageEnable = false;
	BlendStateDesc.BlendEnable[0] = true;
	BlendStateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	BlendStateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.BlendOp = D3D10_BLEND_OP_ADD;
	BlendStateDesc.SrcBlendAlpha = D3D10_BLEND_ZERO;
	BlendStateDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	BlendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	BlendStateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	instance->D3DDevice->CreateBlendState(&BlendStateDesc, &BlendState);
	instance->D3DDevice->OMSetBlendState(BlendState, D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);

	// Create Global sprite.
	D3DX10CreateSprite( instance->D3DDevice, 0, &instance->MainSprite );

	// Initialize XAudio2
	XAudio2Create( &instance->XAudio2, 0);
	instance->XAudio2->CreateMasteringVoice( &instance->MasteringVoice );
	instance->DeviceDetails = new XAUDIO2_DEVICE_DETAILS;
	instance->XAudio2->GetDeviceDetails( 0, instance->DeviceDetails );
	X3DAudioInitialize( instance->DeviceDetails->OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, instance->X3DAudio );
	instance->DSPSettings = new X3DAUDIO_DSP_SETTINGS;
	ZeroMemory( instance->DSPSettings, sizeof( X3DAUDIO_DSP_SETTINGS ) );
	instance->DSPSettings->SrcChannelCount = 2;
	instance->DSPSettings->DstChannelCount = instance->DeviceDetails->OutputFormat.Format.nChannels;
	DWORD dwSize = (instance->DSPSettings->SrcChannelCount * instance->DSPSettings->DstChannelCount);
	FLOAT32* MatrixCoefficients = new FLOAT32[dwSize];
	ZeroMemory( MatrixCoefficients, sizeof( FLOAT32 ) * dwSize );
	instance->DSPSettings->pMatrixCoefficients = MatrixCoefficients;

	// Create PhyX object.
	NxPhysicsSDKDesc NxDesc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	instance->Allocator = new NxUserAllocatorDefault;
	instance->PhysX = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, instance->Allocator, NULL, NxDesc, &errorCode );

	#ifdef _DEBUG
		instance->PhysX->setParameter(NX_VISUALIZATION_SCALE, 1);
		instance->PhysX->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
		instance->PhysX->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
		instance->PhysX->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);
		instance->PhysX->getFoundationSDK().getRemoteDebugger()->connect ("localhost", 5425);
	#else
		instance->PhysX->setParameter(NX_VISUALIZATION_SCALE, 0);
		instance->PhysX->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 0);
		instance->PhysX->setParameter(NX_VISUALIZE_JOINT_LIMITS, 0);
		instance->PhysX->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 0);
	#endif

	// Create PhysX cooking object
	instance->Cooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	instance->Cooking->NxInitCooking();

	// Create PhysX character controller.
	instance->Manager = NxCreateControllerManager( instance->Allocator );

	// Create temporary arrays to hold mesh data.
	std::vector<Ovgl::Vertex> vertices;
	std::vector<Ovgl::Face> faces;

	// Create Sky Box
	vertices.resize(8);
	faces.resize(12);
	vertices[0].position.x = -0.5f;
	vertices[0].position.y = -0.5f;
	vertices[0].position.z = -0.5f;
	vertices[1].position.x = 0.5f;
	vertices[1].position.y = -0.5f;
	vertices[1].position.z = -0.5f;
	vertices[2].position.x = -0.5f;
	vertices[2].position.y = 0.5f;
	vertices[2].position.z = -0.5f;
	vertices[3].position.x = 0.5f;
	vertices[3].position.y = 0.5f;
	vertices[3].position.z = -0.5f;
	vertices[4].position.x = -0.5f;
	vertices[4].position.y = -0.5f;
	vertices[4].position.z = 0.5f;
	vertices[5].position.x = 0.5f;
	vertices[5].position.y = -0.5f;
	vertices[5].position.z = 0.5f;
	vertices[6].position.x = -0.5f;
	vertices[6].position.y = 0.5f;
	vertices[6].position.z = 0.5f;
	vertices[7].position.x = 0.5f;
	vertices[7].position.y = 0.5f;
	vertices[7].position.z = 0.5f;
	faces[0].indices[0] = 0;
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
	D3D10_BUFFER_DESC Buffer_Desc;
	D3D10_SUBRESOURCE_DATA InitData;
	Buffer_Desc.Usage = D3D10_USAGE_DEFAULT;
	Buffer_Desc.ByteWidth = sizeof( Ovgl::Vertex ) * vertices.size();
	Buffer_Desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;
	InitData.pSysMem = &vertices[0];
	instance->D3DDevice->CreateBuffer( &Buffer_Desc, &InitData, &instance->CubeVertexBuffer );
	Buffer_Desc.Usage = D3D10_USAGE_DEFAULT;
	Buffer_Desc.ByteWidth = sizeof( Ovgl::Face ) * faces.size();
	Buffer_Desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;
	InitData.pSysMem = &faces[0];
	instance->D3DDevice->CreateBuffer( &Buffer_Desc, &InitData, &instance->CubeIndexBuffer );

	// Create pyramid shape.
	vertices.resize(5);
	faces.resize(6);
	vertices[0].position.x = 0.0f;
	vertices[0].position.y = 0.0f;
	vertices[0].position.z = 0.0f;
	vertices[1].position.x = -0.5f;
	vertices[1].position.y = -0.5f;
	vertices[1].position.z = 1.0f;
	vertices[2].position.x = 0.5f;
	vertices[2].position.y = -0.5f;
	vertices[2].position.z = 1.0f;
	vertices[3].position.x = -0.5f;
	vertices[3].position.y = 0.5f;
	vertices[3].position.z = 1.0f;
	vertices[4].position.x = 0.5f;
	vertices[4].position.y = 0.5f;
	vertices[4].position.z = 1.0f;
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
	NxConvexMeshDesc ConvexMeshDesc;    
	ConvexMeshDesc.numVertices = vertices.size();
	ConvexMeshDesc.numTriangles = faces.size();
	ConvexMeshDesc.pointStrideBytes = sizeof(Ovgl::Vertex);
	ConvexMeshDesc.triangleStrideBytes = sizeof(Ovgl::Face);
	ConvexMeshDesc.points = &vertices[0];
	ConvexMeshDesc.triangles = &faces[0];
	ConvexMeshDesc.flags = 0;
	NxStreamDefault buf;
	instance->Cooking->NxCookConvexMesh(ConvexMeshDesc, buf);
	instance->Shapes[0] = instance->PhysX->createConvexMesh( buf );

	// Build the default effect.
	instance->DefaultEffect = BuildDefaultEffect( instance );

	// Initialize FbxSdk
	instance->FBXManager = KFbxSdkManager::Create();

	// Return AxInstance
	return instance;
}

Ovgl::AudioBuffer* Ovgl::Instance::CreateAudioBuffer( const std::string& file )
{
	Ovgl::AudioBuffer* buffer = new Ovgl::AudioBuffer;
	buffer->Inst = this;
	OggVorbis_File info;
	FILE* f = NULL;
	fopen_s( &f, file.c_str(), "rb" );
	if ( f == NULL )
	{
		std::wstring wfile;
		wfile = L"Ovgl::Instance::CreateAudioBuffer was unable to open the file ";
		wfile.append( file.begin(), file.end() );
		OutputDebugString( wfile.c_str() );
		return NULL;
	}
	ov_open_callbacks(f, &info, NULL, 0, OV_CALLBACKS_DEFAULT);
	vorbis_info *vi = ov_info(&info, -1);
	buffer->format = new WAVEFORMATEX;
	memset( buffer->format, 0, sizeof(WAVEFORMATEX) );
	buffer->format->cbSize          = sizeof(WAVEFORMATEX);
	buffer->format->nChannels       = vi->channels;
	buffer->format->wBitsPerSample  = 16;
	buffer->format->nSamplesPerSec  = vi->rate;
	buffer->format->nAvgBytesPerSec = buffer->format->nSamplesPerSec * buffer->format->nChannels * 2;
	buffer->format->nBlockAlign     = 2 * buffer->format->nChannels;
	buffer->format->wFormatTag      = 1;
	buffer->data.resize( vi->channels * 2 * (UINT)ov_pcm_total( &info, -1 ));
	DWORD pos = 0;
	int sec = 0;
	int ret = 1;
	while( ret && pos < buffer->data.size() )
	{
		ret = ov_read( &info, &buffer->data[0] + pos, buffer->data.size() - pos, 0, 2, 1, &sec );
		pos += ret;
	}
	return buffer;
};

void Ovgl::Instance::Release()
{
	delete Allocator;
	delete DSPSettings->pMatrixCoefficients;
	delete DeviceDetails;
	delete DSPSettings;
	for( DWORD r = 0; r < RenderTargets.size(); r++ )
	{
		RenderTargets[r]->Release();
	}
	for( DWORD s = 0; s < Scenes.size(); s++ )
	{
		Scenes[s]->Release();
	}
	for( DWORD e = 0; e < Scenes.size(); e++ )
	{
		Effects[e]->Release();
	}
	for( DWORD m = 0; m < Meshes.size(); m++ )
	{
		Meshes[m]->Release();
	}
	MasteringVoice->DestroyVoice();
	XAudio2->StopEngine();
	XAudio2->Release();
	Factory->Release();
	D3DDevice->ClearState();
	MainSprite->Release();
	D3DDevice->Release();
	CoUninitialize();
};

void Ovgl::Effect::set_variable(const std::string& variable, UINT count, float data[] )
{
	SFX->GetVariableByName( variable.c_str() )->SetRawValue( data, 0, count * 4 );
}

void Ovgl::Effect::set_texture(const std::string& variable, const std::string& file)
{
	ID3D10ShaderResourceView* srvTexture;
	std::wstring wfilename;
	wfilename.assign(file.begin(), file.end());
	D3DX10CreateShaderResourceViewFromFile( Inst->D3DDevice, wfilename.c_str(), NULL, NULL, &srvTexture, NULL);
	SFX->GetVariableByName( variable.c_str() )->AsShaderResource()->SetResource(srvTexture);
}

void Ovgl::Effect::Release()
{
	Layout->Release();
	SFX->Release();
	for( DWORD e = 0; e < Inst->Effects.size(); e++ )
	{
		if( Inst->Effects[e] == this )
		{
			Inst->Effects.erase( Inst->Effects.begin() + e );
		}
	}
	delete this;
}