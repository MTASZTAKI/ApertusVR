//------------------------------------------------------------------------------
// <copyright file="SpritePixelShader.hlsl" company="Microsoft">
//     Sprite Pixel Shader
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

Texture2D    txSprite  : register(t0);
SamplerState samSprite : register(s0);

cbuffer constantBuffer
{
    matrix transform;
    float4 targetSpriteIndex;
    float4 tint;
};

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target
{
    float4 output;
    float4 textureData = txSprite.Sample(samSprite , input.TexCoord);
    output.rgb = textureData.r;
    output.a = textureData.g;
    output.rgb *= tint.rgb;
    return output;
}