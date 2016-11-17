//-----------------------------------------------------------------------------
// Program Type: Fragment shader
// Language: hlsl
// Created by Ogre RT Shader Generator. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#include "FFPLib_Common.hlsl"
#include "SGXLib_PerPixelLighting.hlsl"
#include "FFPLib_Texturing.hlsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

float4	derived_ambient_light_colour;
float4	surface_specular_colour;
float4	surface_emissive_colour;
float4	derived_scene_colour;
float	surface_shininess;
Texture2D	gTextureSampler0 : register(t0);
SamplerState	gTextureSamplerState0 : register(s0);

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: Pixel Program Entry point
//-----------------------------------------------------------------------------
void main
	(
	 in float4	oPos_0 : SV_Position, 
	 in float4	iColor_0 : COLOR, 
	 in float3	iTexcoord3_0 : TEXCOORD0, 
	 in float2	iTexcoord2_1 : TEXCOORD1, 
	 out float4	oColor_0 : SV_Target
	)
{
	float4	lLocalParam_0;
	float4	lPerPixelDiffuse;
	float4	texel_0;
	SamplerData2D	gTextureSampler02D;
	float4	source1;
	float4	source2;

	FFP_Construct(0.0, 0.0, 0.0, 0.0, lLocalParam_0);

	FFP_Assign(iColor_0, oColor_0);

	FFP_Assign(derived_scene_colour, lPerPixelDiffuse);

	FFP_Assign(lPerPixelDiffuse, iColor_0);

	FFP_Assign(iColor_0, oColor_0);

	FFP_Construct_Sampler_Wrapper(gTextureSampler0, gTextureSamplerState0, gTextureSampler02D);

	FFP_SampleTexture(gTextureSampler02D, iTexcoord2_1, texel_0);

	FFP_Assign(texel_0, source1);

	FFP_Assign(iColor_0, source2);

	FFP_Modulate(source1, source2, oColor_0);

	FFP_Add(oColor_0.xyz, lLocalParam_0.xyz, oColor_0.xyz);
}

