//------------------------------------------------------------------------------
// <copyright file="KinectUserViewerVertexShader.hlsl" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

struct VertexShaderInput
{
    float2 pos : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct VertexShaderOutput
{
    float2 texcoord : TEXCOORD0;
    float4 pos : SV_POSITION;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.texcoord = input.texcoord;
    return output;
}
