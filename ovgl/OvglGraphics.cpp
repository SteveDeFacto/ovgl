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

void Ovgl::RenderTarget::Render()
{
	RECT WindowRect;
	GetWindowRect( hWnd, &WindowRect );
	D3D10_VIEWPORT ViewPort;
	ViewPort.MinDepth = 0.0f;
	ViewPort.MaxDepth = 1.0f;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.Width = WindowRect.right - WindowRect.left;
	ViewPort.Height = WindowRect.bottom - WindowRect.top;

	if( view != NULL )
	{
		Ovgl::Scene* scene = view->scene;
		Ovgl::Matrix44 viewMat = Ovgl::MatrixInverse( &Ovgl::Vector4Set(0,0,0,0), &view->getPose() );
		Ovgl::Matrix44 projMat = view->projMat;
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
		Inst->D3DDevice->ClearDepthStencilView( DepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

		Inst->D3DDevice->RSSetViewports( 1, &ViewPort );
		Inst->D3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		Inst->D3DDevice->OMSetDepthStencilState( NULL, NULL);
		Inst->D3DDevice->OMSetRenderTargets( 1, &RenderTargetView, NULL ); 
		Inst->SkyboxEffect->View->SetMatrix( (float*)&viewMat );
		Inst->SkyboxEffect->Projection->SetMatrix( (float*)&projMat );
		Inst->D3DDevice->IASetInputLayout( Inst->SkyboxEffect->Layout );
		D3D10_TECHNIQUE_DESC techDesc;
		Inst->SkyboxEffect->Technique->GetDesc( &techDesc );
		UINT stride = sizeof( Ovgl::Vertex );
		UINT offset = 0;
		Inst->D3DDevice->IASetVertexBuffers( 0, 1, &Inst->CubeVertexBuffer, &stride, &offset );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			Inst->SkyboxEffect->Technique->GetPassByIndex( p )->Apply(0);
			Inst->D3DDevice->IASetIndexBuffer( Inst->CubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
			D3D10_BUFFER_DESC Buffer_Desc;
			Inst->CubeIndexBuffer->GetDesc(&Buffer_Desc);
			Inst->D3DDevice->DrawIndexed( Buffer_Desc.ByteWidth / sizeof(DWORD), 0, 0 );
		}

		Inst->D3DDevice->RSSetViewports( 1, &ViewPort );
		Inst->D3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		Inst->D3DDevice->OMSetDepthStencilState(NULL, NULL);
		Inst->D3DDevice->OMSetRenderTargets( 1, &RenderTargetView, DepthStencilView ); 
		for( DWORD i = 0; i < scene->props.size(); i++ )
		{
			for( DWORD s = 0; s < scene->props[i]->mesh->subset_count; s++)
			{
				int Bone_Count = scene->props[i]->mesh->bones.size();
				scene->props[i]->subsets[s]->View->SetMatrix( (float*)&viewMat );
				scene->props[i]->subsets[s]->ViewPos->SetFloatVector( (float*)&view->getPose()[3] );
				scene->props[i]->subsets[s]->Projection->SetMatrix( (float*)&projMat );
				scene->props[i]->subsets[s]->Light_Count->SetInt( Light_Count );
				if( Light_Count > 0 )
				{
					scene->props[i]->subsets[s]->Lights->SetFloatVectorArray( &mLights[0], 0, Light_Count );
					scene->props[i]->subsets[s]->Light_Colors->SetFloatVectorArray( &LightColors[0], 0, Light_Count );
				}
				scene->props[i]->subsets[s]->Bones->SetMatrixArray( (float*)&scene->props[i]->matrices[0], 0, Bone_Count );
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
				scene->objects[i]->subsets[s]->View->SetMatrix( (float*)&viewMat );
				scene->objects[i]->subsets[s]->ViewPos->SetFloatVector( (float*)&view->getPose()[3] );
				scene->objects[i]->subsets[s]->Projection->SetMatrix( (float*)&projMat );
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
	}









	std::vector<D3DX10_SPRITE*> Sprites;
	for(UINT i = 0; i < Interfaces.size(); i++)
	{
		if(Interfaces[i]->Enabled)
		{
			RECT AdjustedRect;

			// Check if sprite rect left is relative or absolute and set AdjustedRect left accordingly
			if( Interfaces[i]->Rect.w > 0 && Interfaces[i]->Rect.w < 1 )
			{
				AdjustedRect.left = (LONG)(ViewPort.Width * Interfaces[i]->Rect.w);
			}
			else
			{
				AdjustedRect.left = (LONG)Interfaces[i]->Rect.w;
			}

			// Check if sprite rect top is relative or absolute and set AdjustedRect top accordingly
			if( Interfaces[i]->Rect.x > 0 && Interfaces[i]->Rect.x < 1 )
			{
				AdjustedRect.top = (LONG)(ViewPort.Height * Interfaces[i]->Rect.x);
			}
			else
			{
				AdjustedRect.top = (LONG)Interfaces[i]->Rect.x;
			}

			// Check if sprite rect right is relative or absolute and set AdjustedRect right accordingly
			if( Interfaces[i]->Rect.y > 0 && Interfaces[i]->Rect.y < 1 )
			{
				AdjustedRect.right = (LONG)(ViewPort.Width * Interfaces[i]->Rect.y);
			}
			else
			{
				AdjustedRect.right = (LONG)Interfaces[i]->Rect.y;
			}

			// Check if sprite rect bottom is relative or absolute and set AdjustedRect bottom accordingly
			if( Interfaces[i]->Rect.z > 0 && Interfaces[i]->Rect.z < 1 )
			{
				AdjustedRect.bottom = (LONG)(ViewPort.Height * Interfaces[i]->Rect.z);
			}
			else
			{
				AdjustedRect.bottom = (LONG)Interfaces[i]->Rect.z;
			}

			// Get sprite location and size
			float SpriteLeft, SpriteTop, SpriteWidth, SpriteHeight;
			SpriteWidth = (float)(AdjustedRect.right - AdjustedRect.left);
			SpriteHeight = (float)(AdjustedRect.bottom - AdjustedRect.top);
			SpriteLeft = (float)(AdjustedRect.left + (SpriteWidth / 2));
			SpriteTop =  (float)(ViewPort.Height - (AdjustedRect.top + (SpriteHeight / 2)));

			// Set world matrix
			D3DXMATRIX matScaling, matTranslation;
			D3DXMatrixScaling(&matScaling, SpriteWidth, SpriteHeight, 1.0f );
			D3DXMatrixTranslation( &matTranslation, SpriteLeft, SpriteTop, 0.0f);
			Interfaces[i]->Sprite->matWorld = matScaling * matTranslation;

			// Add sprite to list of sprites to be rendered
			Sprites.push_back( Interfaces[i]->Sprite );
		}
	}

	if(Sprites.size())
	{
		Inst->D3DDevice->RSSetViewports( 1, &ViewPort );
		Inst->D3DDevice->RSSetState(Inst->SolidRasterState);
		D3DXMATRIX Projection;
		D3DXMATRIX Scaling;
		D3DXMatrixOrthoOffCenterLH(&Projection, 0.0f, (float)ViewPort.Width, 0.0f, (float)ViewPort.Height, 0.0f, 10.0f);
		Inst->MainSprite->SetProjectionTransform( &Projection );
		Inst->MainSprite->Begin( D3DX10_SPRITE_SORT_TEXTURE );
		Inst->D3DDevice->OMSetRenderTargets( 1, &RenderTargetView, NULL );
		Inst->D3DDevice->PSSetShaderResources( 0, 1, &Inst->ShaderResourceView );
		Inst->MainSprite->DrawSpritesImmediate( Sprites[0], Sprites.size(), 0, 0 );
		Inst->MainSprite->End();
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
	byte* pSrcData;
	BITMAPINFO bmi = { sizeof( BITMAPINFOHEADER ), TextRect.right, -TextRect.bottom, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
	HBITMAP hTempBmp = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS, (void**)&pSrcData, NULL, 0 );
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hTempBmp );
	HFONT NewFont = CreateFontA( CSize, 0, 0, 0, (FW_BOLD * Bold), Italic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 0, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, Font.c_str() );
	HBRUSH NewBrush = CreateSolidBrush(0);
	SelectObject( hDC, NewFont );
	SelectObject( hDC, NewBrush );
	SetTextColor( hDC, RGB(0,0,255) );
	SetBkColor( hDC, 0);
	DrawTextA(hDC, Text.c_str(), Text.size(), &TextRect, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
	GdiFlush();
	DeleteObject(NewBrush);
	DeleteObject(NewFont);	
	DeleteObject(hTempBmp);
	ID3D10Texture2D* pTexture;
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.Width = TextRect.right;
	desc.Height = TextRect.bottom;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	RenderTarget->Inst->D3DDevice->CreateTexture2D( &desc, NULL, &pTexture );
	D3D10_MAPPED_TEXTURE2D mappedTex;
	pTexture->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );

	UCHAR* pTexels = (UCHAR*)mappedTex.pData;
	for( UINT row = 0; row < desc.Height; row++ )
	{
		UINT rowStart = row * mappedTex.RowPitch;
		for( UINT col = 0; col < desc.Width; col++ )
		{
			UINT colStart = col * 4;
			pTexels[rowStart + colStart + 0] = 255;
			pTexels[rowStart + colStart + 1] = 255;
			pTexels[rowStart + colStart + 2] = 255;
			pTexels[rowStart + colStart + 3] = pSrcData[((row * desc.Width) + col) * 4 ];
		}
	}
	pTexture->Unmap( D3D10CalcSubresource(0, 0, 1) );
	DeleteDC(hDC);
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	ID3D10ShaderResourceView *srvTexture = NULL;
	RenderTarget->Inst->D3DDevice->CreateShaderResourceView( pTexture, &srvDesc, &srvTexture );
	pTexture->Release();
	if(Sprite->pTexture) Sprite->pTexture->Release();
	Sprite->pTexture = srvTexture;
}

Ovgl::Interface* Ovgl::RenderTarget::CreateSprite( const std::string& file, Ovgl::Vector4* rect )
{
	Ovgl::Interface* Sprite = new Ovgl::Interface;
	Sprite->Sprite = new D3DX10_SPRITE;
	ID3D10ShaderResourceView* srvTexture;
	D3DX10CreateShaderResourceViewFromFileA(Inst->D3DDevice, file.c_str(), NULL, NULL, &srvTexture, NULL);
	Sprite->Sprite->pTexture = srvTexture;
	Sprite->Sprite->TexCoord.x = 0;
	Sprite->Sprite->TexCoord.y = 0;
	Sprite->Sprite->TexSize.x = 1;
	Sprite->Sprite->TexSize.y = 1;
	Sprite->Sprite->TextureIndex = 0;
	Sprite->Sprite->ColorModulate = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
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
	Text->Sprite = new D3DX10_SPRITE;
	Text->Text = text;
	Text->Font = "Arial";
	Text->Size = 24;
	Text->CSize = 24;
	Text->Bold = false;
	Text->Italic = false;
	Text->Rect = *rect;
	Text->Sprite->pTexture = NULL;
	Text->Sprite->TextureIndex = 0;
	Text->Sprite->TexCoord.x = 0;
	Text->Sprite->TexCoord.y = 0;
	Text->Sprite->TexSize.x = 1;
	Text->Sprite->TexSize.y = 1;
	Text->Sprite->ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXMATRIX matScaling, matTranslation;
	D3DXMatrixScaling(&matScaling, 256.0f, 256.0f, 1.0f );
	D3DXMatrixTranslation( &matTranslation, 0.0f, 0.0f, 0.0f );
	Text->Sprite->matWorld = matScaling * matTranslation;
	Text->Enabled = true;
	Text->Over = false;
	Text->RenderTarget = this;
	Text->UpdateText();
	Interfaces.push_back( Text );
	return Text;
}
