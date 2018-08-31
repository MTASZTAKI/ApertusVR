//------------------------------------------------------------------------------
// <copyright file="VertexShader.hlsl" company="Microsoft">
//     Vertex Shader
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

cbuffer constantBuffer : register(b0)
{
    matrix  transform;
    float4  color;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = mul(input.Pos, transform);
    output.Tex = input.Tex;

    return output;
}