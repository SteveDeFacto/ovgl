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
#include "OvglGraphics.h"
#include "OvglScene.h"
#include "OvglAudio.h"
#include "OvglMesh.h"



void Ovgl::RenderTarget::Release()
{
	SwapChain->SetFullscreenState( FALSE, NULL );
	SwapChain->Release();
	RenderTargetView->Release();
	DepthStencilView->Release();
	for( UINT r = 0; r < Inst->RenderTargets.size(); r++)
	{
		if(Inst->RenderTargets[r] == this)
		{
			Inst->RenderTargets.erase( Inst->RenderTargets.begin() + r );
		}
	}
	delete this;
}

void Ovgl::RenderTarget::Update()
{
	Ovgl::Scene* scene = view->scene;
	float* viewMat = (float*)&Ovgl::MatrixInverse( &Ovgl::Vector4Set(0,0,0,0), &view->getPose() );
	float* projMat = (float*)&view->projMat;
	int Light_Count = scene->lights.size();
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
	Inst->D3DDevice->ClearRenderTargetView(RenderTargetView, D3DXVECTOR4(0, 0, 1, 1) );
	Inst->D3DDevice->ClearDepthStencilView( DepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );
	Inst->D3DDevice->RSSetViewports( 1, ViewPort );
	Inst->D3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	Inst->D3DDevice->OMSetDepthStencilState(NULL, NULL);
	Inst->D3DDevice->OMSetRenderTargets( 1, &RenderTargetView, DepthStencilView ); 
	for( DWORD i = 0; i < scene->props.size(); i++ )
	{
		for( DWORD s = 0; s < scene->props[i]->mesh->subset_count; s++)
		{
			int Bone_Count = scene->props[i]->mesh->bones.size();
			scene->props[i]->subsets[s]->View->SetMatrix( viewMat );
			scene->props[i]->subsets[s]->Projection->SetMatrix( projMat );
			scene->props[i]->subsets[s]->Light_Count->SetInt( Light_Count );
			if( Light_Count > 0 )
			{
				scene->props[i]->subsets[s]->Lights->SetFloatVectorArray( &mLights[0], 0, Light_Count );
				scene->props[i]->subsets[s]->Light_Colors->SetFloatVectorArray( &LightColors[0], 0, Light_Count );
			}
			scene->props[i]->subsets[s]->Bones->SetMatrixArray( (float*)&scene->props[i]->matrices[0], 0, Bone_Count );
//			scene->props[i]->subsets[s]->Cube_Views->SetMatrixArray( (float*)g_amCubeMapViewAdjust, 0, 6 );
			Inst->D3DDevice->IASetInputLayout( scene->props[i]->subsets[s]->Layout );
			D3D10_TECHNIQUE_DESC techDesc;
			scene->props[i]->subsets[s]->Technique->GetDesc( &techDesc );
			UINT stride = sizeof( Ovgl::Vertex );
			UINT offset = 0;
			Inst->D3DDevice->IASetVertexBuffers( 0, 1, &scene->props[i]->mesh->VertexBuffer, &stride, &offset );
			for( UINT p = 0; p < techDesc.Passes; ++p )
			{
				scene->props[i]->subsets[s]->Technique->GetPassByIndex( p )->Apply(0);
				Inst->D3DDevice->IASetIndexBuffer( scene->props[i]->mesh->IndexBuffers[s], DXGI_FORMAT_R32_UINT, 0 );
				D3D10_BUFFER_DESC Buffer_Desc;
				scene->props[i]->mesh->IndexBuffers[s]->GetDesc(&Buffer_Desc);
				Inst->D3DDevice->DrawIndexed( Buffer_Desc.ByteWidth / sizeof(DWORD), 0, 0 );
			}
		}
	}

	for( DWORD i = 0; i < scene->objects.size(); i++ )
	{
		for( DWORD s = 0; s < scene->objects[i]->mesh->subset_count; s++)
		{
			scene->objects[i]->subsets[s]->View->SetMatrix( viewMat );
			scene->objects[i]->subsets[s]->Projection->SetMatrix( projMat );
			scene->objects[i]->subsets[s]->Light_Count->SetInt( Light_Count );
			if( Light_Count > 0 )
			{
				scene->objects[i]->subsets[s]->Lights->SetFloatVectorArray( &mLights[0], 0, Light_Count );
				scene->objects[i]->subsets[s]->Light_Colors->SetFloatVectorArray( &LightColors[0], 0, Light_Count );
			}
			scene->objects[i]->subsets[s]->Bones->SetMatrixArray( (float*)&scene->objects[i]->getPose(), 0, 1 );
			Inst->D3DDevice->IASetInputLayout( scene->objects[i]->subsets[s]->Layout );
			D3D10_TECHNIQUE_DESC techDesc;
			scene->objects[i]->subsets[s]->Technique->GetDesc( &techDesc );
			UINT stride = sizeof( Ovgl::Vertex );
			UINT offset = 0;
			Inst->D3DDevice->IASetVertexBuffers( 0, 1, &scene->objects[i]->mesh->VertexBuffer, &stride, &offset );
			for( UINT p = 0; p < techDesc.Passes; ++p )
			{
				scene->objects[i]->subsets[s]->Technique->GetPassByIndex( p )->Apply(0);
				Inst->D3DDevice->IASetIndexBuffer( scene->objects[i]->mesh->IndexBuffers[s], DXGI_FORMAT_R32_UINT, 0 );
				D3D10_BUFFER_DESC Buffer_Desc;
				scene->objects[i]->mesh->IndexBuffers[s]->GetDesc(&Buffer_Desc);
				Inst->D3DDevice->DrawIndexed( Buffer_Desc.ByteWidth / sizeof(DWORD), 0, 0 );
			}
		}
	}
	
	SwapChain->Present(1, 0);
}

void Ovgl::RenderTarget::SetFullscreen( bool state )
{
	SwapChain->SetFullscreenState( state, NULL);
}

bool Ovgl::RenderTarget::GetFullscreen()
{
	BOOL state;
	SwapChain->GetFullscreenState( &state, NULL);
	return !!state;
}