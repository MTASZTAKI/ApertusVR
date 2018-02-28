//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//	This control is based on the following article: http://msdn.microsoft.com/en-us/library/windows/apps/hh825871.aspx
//	And the code here: http://code.msdn.microsoft.com/windowsapps/XAML-SwapChainPanel-00cb688b
//
//	HDFaceVertexShader.hlsl
//	
//*********************************************************

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
	float3 normal: NORMAL;
};

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
	float3 normal: NORMAL;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
	float4 pos = float4(input.pos, 1.0f);

	matrix finalMatrix = mul(model, view);
	finalMatrix = mul(finalMatrix, projection);
	pos = mul(pos, finalMatrix);
    output.pos = pos;

	float3 normal = input.normal;
	output.normal = mul(normal, ((float3x3) model));

    return output;
}
