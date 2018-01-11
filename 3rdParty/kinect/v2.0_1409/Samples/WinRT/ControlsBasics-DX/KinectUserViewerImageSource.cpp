//------------------------------------------------------------------------------
// <copyright file="KinectUserViewerImageSource.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "pch.h"
#include "KinectUserViewerImageSource.h"
#include "KinectUserViewerPixelShader.h"
#include "KinectUserViewerVertexShader.h"

using namespace Platform;
using namespace Microsoft::Kinect::Toolkit::Input;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;

#define DEPTHIR_WIDTH 512
#define DEPTHIR_HEIGHT 424

struct VertexPositionTexcoord
{
	XMFLOAT2 pos;
	XMFLOAT2 texcoord;
};

//--------------------------------------------------------------------------------------
// Name: ColorToFloat4(UINT color, _Out_writes_(4) float* pColor)
// Desc: Splits a color defined in HEX into a float4
//--------------------------------------------------------------------------------------
void ColorToFloat4(UINT color, _Out_writes_(4) float* pColor)
{
	pColor[0] = ((color >> 16) & 0xFF) / 255.0f;
	pColor[1] = ((color >> 8) & 0xFF) / 255.0f;
	pColor[2] = (color & 0xFF) / 255.0f;
	pColor[3] = 1.0f;
}

//--------------------------------------------------------------------------------------
// Name: KinectUserViewerImageSource(POINT upperLeft, float pixelWidth, float pixelHeight, bool isOpaque)
// Desc: Constructor
//--------------------------------------------------------------------------------------
KinectUserViewerImageSource::KinectUserViewerImageSource(POINT upperLeft, float pixelWidth, float pixelHeight, bool isOpaque) :
 m_width((long)pixelWidth),
 m_height((long)pixelHeight)
{
	m_offset = upperLeft;
	m_pixelShaderConstantBufferData.depthNearMM = 500;
	m_pixelShaderConstantBufferData.depthFarMM = 8000;
	m_pixelShaderConstantBufferData.trackedIndex = -1;
	ColorToFloat4(0x355959, m_pixelShaderConstantBufferData.backgroundNearColor);
	ColorToFloat4(0x101010, m_pixelShaderConstantBufferData.backgroundFarColor);
	ColorToFloat4(0x3D7A73, m_pixelShaderConstantBufferData.playerNearColor);
	ColorToFloat4(0x285E66, m_pixelShaderConstantBufferData.playerFarColor);
	ColorToFloat4(0x5DC21E, m_pixelShaderConstantBufferData.playerActiveNearColor);
	ColorToFloat4(0x107C10, m_pixelShaderConstantBufferData.playerActiveFarColor);
}

//--------------------------------------------------------------------------------------
// Name: Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
// Desc: Sets D3D Context, D3D Device
//--------------------------------------------------------------------------------------
void KinectUserViewerImageSource::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, D3D_FEATURE_LEVEL featureLevel)
{
	// Stop initialization if feature level is below D3D 10_0. We require D3D 10_0 or higher.
	// On lower feature levels, kinectUserViewer will simply not display anything.
	if (featureLevel < D3D_FEATURE_LEVEL_10_0)
	{
		return;
	}

	m_d3dContext = context;
	m_d3dDevice = device;

	CreateDeviceResources();
}

//--------------------------------------------------------------------------------------
// Name: CreateDeviceResources()
// Desc: Initializes hardware dependent resources.
//--------------------------------------------------------------------------------------
void KinectUserViewerImageSource::CreateDeviceResources()
{
	// Load the vertex shader.
	DX::ThrowIfFailed(
		m_d3dDevice->CreateVertexShader(
		KinectUserViewerVertexShader,
		sizeof(KinectUserViewerVertexShader),
		nullptr,
		&m_vertexShader
		)
		);

	// Create input layout for vertex shader.
	const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	DX::ThrowIfFailed(
		m_d3dDevice->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			KinectUserViewerVertexShader,
			sizeof(KinectUserViewerVertexShader),
			&m_inputLayout
		)
	);

	// Load the pixel shader.
	DX::ThrowIfFailed(
		m_d3dDevice->CreatePixelShader(
			KinectUserViewerPixelShader,
			sizeof(KinectUserViewerPixelShader),
			nullptr,
			&m_pixelShader
		)
	);

	// Create the constant buffer.
	const CD3D11_BUFFER_DESC pixelShaderConstantBufferDesc = CD3D11_BUFFER_DESC(sizeof(PixelShaderConstants), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_d3dDevice->CreateBuffer(
			&pixelShaderConstantBufferDesc,
			nullptr,
			&m_pixelShaderConstantBuffer
		)
	);

	// Describe the vertices of the cube.
	VertexPositionTexcoord vertices[] =
	{
		{ XMFLOAT2(-1, -1), XMFLOAT2(0, 1) },
		{ XMFLOAT2(-1, 1), XMFLOAT2(0, 0) },
		{ XMFLOAT2(1, -1), XMFLOAT2(1, 1) },
		{ XMFLOAT2(1, 1), XMFLOAT2(1, 0) },
	};

	// Create the vertex buffer.
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = vertices;
	const CD3D11_BUFFER_DESC vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_d3dDevice->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
		)
	);

	// Create depth texture, shader resource view, sampler state for UserViewerTexture
	{
		D3D11_TEXTURE2D_DESC tex2DDesc = { 0 };
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));

		tex2DDesc.Width = DEPTHIR_WIDTH;
		tex2DDesc.Height = DEPTHIR_HEIGHT;
		tex2DDesc.MipLevels = 1;
		tex2DDesc.ArraySize = 1;
		tex2DDesc.Format = DXGI_FORMAT_R16_UNORM;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		tex2DDesc.Usage = D3D11_USAGE_DYNAMIC;

		DX::ThrowIfFailed(
			m_d3dDevice->CreateTexture2D(
				&tex2DDesc,
				NULL,
				&m_depthTexture
			)
		);

		srvDesc.Format = tex2DDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		DX::ThrowIfFailed(
			m_d3dDevice->CreateShaderResourceView(
				m_depthTexture.Get(),
				&srvDesc,
				&m_depthSRV
			)
		);

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MaxLOD = FLT_MAX;
		DX::ThrowIfFailed(
			m_d3dDevice->CreateSamplerState(
				&samplerDesc,
				&m_depthSamplerState
			)
		);

		// create body index texture, shader resource view, sampler state
		tex2DDesc.Format = DXGI_FORMAT_R8_UNORM;

		DX::ThrowIfFailed(
			m_d3dDevice->CreateTexture2D(
				&tex2DDesc,
				NULL,
				&m_bodyIndexTexture
			)
		);

		srvDesc.Format = tex2DDesc.Format;
		DX::ThrowIfFailed(
			m_d3dDevice->CreateShaderResourceView(
				m_bodyIndexTexture.Get(),
				&srvDesc,
				&m_bodyIndexSRV
			)
		);

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		DX::ThrowIfFailed(
			m_d3dDevice->CreateSamplerState(
				&samplerDesc,
				&m_bodyIndexSamplerState
			)
		);
	}

	{
		D3D11_TEXTURE2D_DESC tex2DDesc = { 0 };
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));

		tex2DDesc.Width = DEPTHIR_WIDTH;
		tex2DDesc.Height = DEPTHIR_HEIGHT;
		tex2DDesc.MipLevels = 1;
		tex2DDesc.ArraySize = 1;
		tex2DDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;

		DX::ThrowIfFailed(
			m_d3dDevice->CreateTexture2D(
				&tex2DDesc,
				NULL,
				&m_userViewerTexture
			)
		);

		renderTargetViewDesc.Format = tex2DDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		
		// Create the render target view.
		DX::ThrowIfFailed(
			m_d3dDevice->CreateRenderTargetView(m_userViewerTexture.Get(), &renderTargetViewDesc, &m_userViewerTextureRTV)
		);

		srvDesc.Format = tex2DDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		DX::ThrowIfFailed(
			m_d3dDevice->CreateShaderResourceView(
				m_userViewerTexture.Get(),
				&srvDesc,
				&m_userViewerTextureSRV
			)
		);
	}
}

//--------------------------------------------------------------------------------------
// Name: DestroyDeviceResources()
// Desc: Destroy device resources.
//--------------------------------------------------------------------------------------
void KinectUserViewerImageSource::DestroyDeviceResources()
{
	m_d3dContext = nullptr;
	m_depthTexture = nullptr;
	m_depthSRV = nullptr;
	m_depthSamplerState = nullptr;
	m_bodyIndexTexture = nullptr;
	m_bodyIndexSRV = nullptr;
	m_bodyIndexSamplerState = nullptr;
	m_inputLayout = nullptr;
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_vertexBuffer = nullptr;
	m_pixelShaderConstantBuffer = nullptr;
	m_d3dDevice = nullptr;
}

//--------------------------------------------------------------------------------------
// Name: RenderNuiToTexture()
// Desc: Begins drawing, allowing updates to content in the specified area.
//--------------------------------------------------------------------------------------
void KinectUserViewerImageSource::RenderNuiToTexture()
{
	if (m_d3dContext == nullptr || m_d3dDevice == nullptr)
	{
		return;
	}

	// Clear Context State
	m_d3dContext->ClearState();

	// Set context's render target.
	m_d3dContext->OMSetRenderTargets(1, m_userViewerTextureRTV.GetAddressOf(), nullptr);

	CD3D11_VIEWPORT viewPort(0.0f, 0.0f, static_cast<float>(DEPTHIR_WIDTH), static_cast<float>(DEPTHIR_HEIGHT));

	// Set the current viewport using the descriptor.
	m_d3dContext->RSSetViewports(1, &viewPort);

	m_d3dContext->UpdateSubresource(
		m_pixelShaderConstantBuffer.Get(),
		0,
		nullptr,
		&m_pixelShaderConstantBufferData,
		0,
		0
		);

	UINT stride = sizeof(VertexPositionTexcoord);
	UINT offset = 0;
	m_d3dContext->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
		);

	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_d3dContext->IASetInputLayout(m_inputLayout.Get());

	m_d3dContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	m_d3dContext->PSSetConstantBuffers(0, 1, m_pixelShaderConstantBuffer.GetAddressOf());
	m_d3dContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	ID3D11ShaderResourceView* pSRVs[2] = { m_depthSRV.Get(), m_bodyIndexSRV.Get() };
	m_d3dContext->PSSetShaderResources(0, 2, pSRVs);

	ID3D11SamplerState* pSamplerStates[2] = { m_depthSamplerState.Get(), m_bodyIndexSamplerState.Get() };
	m_d3dContext->PSSetSamplers(0, 2, pSamplerStates);

	m_d3dContext->Draw(4, 0);

	ID3D11RenderTargetView* pRTV = nullptr;
	m_d3dContext->OMSetRenderTargets(1, &pRTV, nullptr);
}

//--------------------------------------------------------------------------------------
// Name: GetShaderResourceView()
// Desc: Returns pointer to KinectUserViewer Shader Resource View.
//--------------------------------------------------------------------------------------
ID3D11ShaderResourceView* KinectUserViewerImageSource::GetShaderResourceView()
{
	return m_userViewerTextureSRV.Get();
}

//--------------------------------------------------------------------------------------
// Name: UpdateNui(DepthFrame^ depthFrame, BodyIndexFrame^ bodyIndexFrame, UINT64 engagedBodyIndex)
// Desc: Update Nui Textures and updates tracked body index.
//--------------------------------------------------------------------------------------
void KinectUserViewerImageSource::UpdateNui(DepthFrame^ depthFrame, BodyIndexFrame^ bodyIndexFrame, UINT64 engagedBodyIndex)
{
	if (m_d3dDevice == nullptr)
	{
		return;
	}

	// Copy DepthFrame data to Depth Texture
	if (depthFrame != nullptr)
	{
		ComPtr<IUnknown> spBuffer = (IUnknown*)depthFrame->LockImageBuffer();
		ComPtr<IBufferByteAccess> spBufferByteAccess;
		spBuffer.As(&spBufferByteAccess);

		byte* pPixelBuffer = nullptr;
		HRESULT hr = spBufferByteAccess->Buffer(&pPixelBuffer);
		if (SUCCEEDED(hr))
		{
			D3D11_MAPPED_SUBRESOURCE map;
			hr = m_d3dContext->Map(m_depthTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			if (SUCCEEDED(hr) && map.pData != nullptr)
			{
				memcpy(map.pData, pPixelBuffer, DEPTHIR_WIDTH * DEPTHIR_HEIGHT * 2);
				m_d3dContext->Unmap(m_depthTexture.Get(), 0);
			}
		}
	}

	// Copy BodyIndexFrame data to BodyIndex Texture
	if (bodyIndexFrame != nullptr)
	{
		ComPtr<IUnknown> spBuffer = (IUnknown*)bodyIndexFrame->LockImageBuffer();
		ComPtr<IBufferByteAccess> spBufferByteAccess;
		spBuffer.As(&spBufferByteAccess);

		byte* pPixelBuffer = nullptr;
		HRESULT hr = spBufferByteAccess->Buffer(&pPixelBuffer);
		if (SUCCEEDED(hr))
		{
			D3D11_MAPPED_SUBRESOURCE map;
			hr = m_d3dContext->Map(m_bodyIndexTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			if (SUCCEEDED(hr) && map.pData != nullptr)
			{
				memcpy(map.pData, pPixelBuffer, DEPTHIR_WIDTH * DEPTHIR_HEIGHT);
				m_d3dContext->Unmap(m_bodyIndexTexture.Get(), 0);
			}
		}
	}

	m_pixelShaderConstantBufferData.trackedIndex = (float)engagedBodyIndex;
}

//--------------------------------------------------------------------------------------
// Name: SetVisibility(bool visible)
// Desc: Set visibility of Kinect User Viewer.
//--------------------------------------------------------------------------------------
void KinectUserViewerImageSource::SetVisibility(bool visible)
{
	if (visible)
	{
		if (m_d3dDevice == nullptr)
		{
			CreateDeviceResources();
		}
	}
	else
	{
		DestroyDeviceResources();
	}
}
