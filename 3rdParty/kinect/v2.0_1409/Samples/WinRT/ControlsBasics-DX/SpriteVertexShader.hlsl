//------------------------------------------------------------------------------
// <copyright file="SpriteVertexShader.hlsl" company="Microsoft">
//     Sprite Vertex Shader
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

cbuffer constantBuffer : register(b0)
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

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = mul(input.Position, transform);
    output.TexCoord.x = targetSpriteIndex.x + input.TexCoord.x * targetSpriteIndex.z;  //targetSpriteIndex.z = sprite width
    output.TexCoord.y = targetSpriteIndex.y + input.TexCoord.y * targetSpriteIndex.w;  //targetSpriteIndex.w = sprite height
    return output;
}
