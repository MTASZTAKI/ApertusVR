//------------------------------------------------------------------------------
// <copyright file="KinectUserViewerPixelShader.hlsl" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

Texture2D DepthTexture : register(t0);
Texture2D BodyIndexTexture : register(t1);

SamplerState DepthSampler : register(s0);
SamplerState BodyIndexSampler : register(s1);

cbuffer constants : register(b0)
{
    float  depthNearMM           : packoffset(c0.x);
    float  depthFarMM            : packoffset(c0.y);
    float  trackedIndex          : packoffset(c0.z);
    float4 backgroundNearColor   : packoffset(c2);
    float4 backgroundFarColor    : packoffset(c3);
    float4 playerNearColor       : packoffset(c4);
    float4 playerFarColor        : packoffset(c5);
    float4 playerActiveNearColor : packoffset(c6);
    float4 playerActiveFarColor  : packoffset(c7);
};

float4 main(float2 uv : TEXCOORD0) : SV_Target
{
    int bodyIndex = BodyIndexTexture.Sample(BodyIndexSampler, uv).x * 255;
    int bitWiseFlag = 0x1 << bodyIndex;

    float4 colorNear = backgroundNearColor;
    float4 colorFar = backgroundFarColor;

    if (bodyIndex < 6)
    {
        if ( (bitWiseFlag & (int)trackedIndex) == bitWiseFlag)
        {
            colorNear = playerActiveNearColor;
            colorFar = playerActiveFarColor;
        }
        else
        {
            colorNear = playerNearColor;
            colorFar = playerNearColor;
        }
    }

    float depthMM = DepthTexture.Sample(DepthSampler, uv).x * 65535;
    if (depthMM < depthNearMM || depthMM > depthFarMM)
    {
        return backgroundFarColor;
    }
    else
    {
        float s = (depthMM - depthNearMM) / (depthFarMM - depthNearMM);
        return colorNear * ( 1 - s) + colorFar * s;
    }
}