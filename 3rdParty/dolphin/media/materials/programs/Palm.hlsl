// Palm CG vertex and fragment program - For Hydrax demo application
// Xavier Verguín González

void main_vp( // In
             float4 iPosition         : POSITION,
             float3 iNormal           : NORMAL,
             float2 iUv               : TEXCOORD0,
             // Out
             out float4 oPosition     : POSITION,
             out float3 oPosition_    : TEXCOORD0,
             out float3 oNormal       : TEXCOORD1,
             out float2 oUv           : TEXCOORD2,
             // Uniform
             uniform float    uTime,
             uniform float4x4 uWorld,
             uniform float4x4 uWorldViewProj)
{
   if(iPosition.z>2)
   {
      uTime += mul(uWorld, iPosition-float4(0,0,2,0)).x/100;
      iPosition.xz += float2(sin(uTime), cos(uTime))*iPosition.z/100;
   }

   oPosition  = mul(uWorldViewProj, iPosition);
   oPosition_ = iPosition.xyz;
   oNormal    = iNormal;
   oUv        = iUv;
}

void main_fp( // In
             float3 iPosition  : TEXCOORD0,
             float3 iNormal    : TEXCOORD1,
             float2 iUv        : TEXCOORD2,
             // Out
             out float4 oColor : COLOR,
             // Uniform
             uniform float3    uEyePosition,
             uniform float3    uLightPosition,
             uniform float3    uLightDiffuse,
             uniform float3    uLightSpecular,
	         uniform sampler2D uTexture)
{
    float3 ViewVector = normalize(iPosition-uEyePosition);
    float3 LightDir   = normalize(uLightPosition-iPosition);

    float  Specular       = dot(iNormal, normalize(ViewVector + LightDir));
	float4 DiffTexture    = tex2D(uTexture, iUv);

	float3 Final          = DiffTexture*uLightDiffuse + uLightSpecular*Specular/2*saturate(2/iPosition.z);

	oColor = float4(saturate(Final),DiffTexture.w);
}


