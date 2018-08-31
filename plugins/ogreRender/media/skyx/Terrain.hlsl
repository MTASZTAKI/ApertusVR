// --------------------- Terrain material ------------------------

void main_vp( // In
             float4 iPosition         : POSITION,
             float2 iUv               : TEXCOORD0,
             // Out
             out float4 oPosition     : POSITION,
             out float3 oUv           : TEXCOORD0,
             // Uniform
             uniform float4x4 uWorldViewProj)
{
   oPosition = mul(uWorldViewProj, iPosition);
   oUv.xy       = iUv;
   oUv.z = iPosition.y;
}

void main_fp( // In
             float3 iUv        : TEXCOORD0,
             // Out
             out float4 oColor : COLOR,
             // Uniform
             uniform float     uLightY,
	         uniform sampler2D uAmbientMap : register(s0),
	         uniform sampler2D uTex0       : register(s1),
	         uniform sampler2D uTex1       : register(s2))
{
	float AmbientColor = tex2D(uAmbientMap,iUv.xy).r;
    float AmbientLight = AmbientColor * (0.8-0.8*saturate(-(uLightY-0.15)*4)+0.115);
    
    float3 Tex0 = tex2D(uTex0,iUv.xy*1000);
    float3 Tex1 = tex2D(uTex1,iUv.xy*1000);
    
    oColor = float4(AmbientLight * lerp(Tex1, Tex0, saturate(-AmbientColor*4+4.5f*(iUv.z/1000))), 1);
}