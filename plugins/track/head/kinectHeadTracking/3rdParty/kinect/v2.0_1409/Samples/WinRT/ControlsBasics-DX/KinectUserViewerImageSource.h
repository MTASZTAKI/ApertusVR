//------------------------------------------------------------------------------
// <copyright file="KinectUserViewerImageSource.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "pch.h"
#include <dxgi.h> 
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "windows.ui.xaml.media.dxinterop.h"

#include <robuffer.h>
#include <collection.h>

#include <sstream>
#include <iostream>
#include <Windows.h>

using namespace WindowsPreview::Kinect;
using namespace std;

namespace Microsoft { 
	namespace Kinect {
		namespace Toolkit {
			namespace Input {
				struct PixelShaderConstants
				{
					float depthNearMM;
					float depthFarMM;
					float trackedIndex;
					float padding[5];
					float backgroundNearColor[4];
					float backgroundFarColor[4];
					float playerNearColor[4];
					float playerFarColor[4];
					float playerActiveNearColor[4];
					float playerActiveFarColor[4];
				};

				class KinectUserViewerImageSource
				{
				public:
					KinectUserViewerImageSource(POINT upperLeft, float pixelWidth, float pixelHeight, bool isOpaque);
					void UpdateNui(DepthFrame^ depthFrame, BodyIndexFrame^ bodyIndexFrame, UINT64 engagedBodyIndex);
					ID3D11ShaderResourceView* GetShaderResourceView();
					void SetVisibility(bool visible);
					void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, D3D_FEATURE_LEVEL featureLevel);
					void RenderNuiToTexture();

				private:
					void Clear();
					void CreateDeviceResources();
					void DestroyDeviceResources();

					void BeginDraw();
					void EndDraw();

					void DrawUserViewer();

					POINT                                               m_offset;

					// Direct3D device
					Microsoft::WRL::ComPtr<ID3D11Device>                m_d3dDevice;
					Microsoft::WRL::ComPtr<ID3D11DeviceContext>         m_d3dContext;

					Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_depthTexture;
					Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_depthSRV;
					Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_depthSamplerState;

					Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_bodyIndexTexture;
					Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_bodyIndexSRV;
					Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_bodyIndexSamplerState;
					
					Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_userViewerTexture;
					Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_userViewerTextureRTV;
					Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_userViewerTextureSRV;
					Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_inputLayout;
					Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
					Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
					Microsoft::WRL::ComPtr<ID3D11Buffer>                m_vertexBuffer;
					Microsoft::WRL::ComPtr<ID3D11Buffer>                m_pixelShaderConstantBuffer;

					PixelShaderConstants                                m_pixelShaderConstantBufferData;

					long                                               m_width;
					long                                               m_height;
				};
			}
		}
	}
}