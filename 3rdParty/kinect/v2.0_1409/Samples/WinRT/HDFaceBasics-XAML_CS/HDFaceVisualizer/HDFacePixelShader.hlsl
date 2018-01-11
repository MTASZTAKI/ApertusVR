//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//	This control is based on the following article: http://msdn.microsoft.com/en-us/library/windows/apps/hh825871.aspx
//	And the code here: http://code.msdn.microsoft.com/windowsapps/XAML-SwapChainPanel-00cb688b
//	
//	HDFacePixelShader.hlsl
//
//*********************************************************


struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 normal: NORMAL;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	const float3 ambient = 0.5f;
	const float4 lightDirection = float4(-0.2f, 0.2f, -0.5f, 1.0f);
	const float4 inputColor = float4(0.5f, 0.5f, 0.0f, 1.0f);
	
	float3 normal = input.normal.xyz;
	float3 lightIntensity = saturate(ambient + dot(lightDirection.xyz, normal));
	float4 finalColor = inputColor * float4(lightIntensity, 1.0f);

	return finalColor;
}
