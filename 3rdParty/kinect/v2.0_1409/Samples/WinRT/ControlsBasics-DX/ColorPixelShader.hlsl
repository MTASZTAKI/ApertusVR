//------------------------------------------------------------------------------
// <copyright file="ColorPixelShader.hlsl" company="Microsoft">
//     Color pixel Shader
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

cbuffer constantBuffer : register(b0)
{
    matrix  transform;
    float4  color;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return color;
}