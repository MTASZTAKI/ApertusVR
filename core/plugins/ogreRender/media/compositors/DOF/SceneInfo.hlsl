struct VERTEX_IN
{
 float4 position :POSITION;
};

struct VERTEX_OUT
{
 float4 position :	POSITION;
 float3 wPos	 :	TEXCOORD0;
 float4 hPos	 :	TEXCOORD2;
};

VERTEX_OUT vs(VERTEX_IN IN,
	      uniform float4x4 ModelViewProj,
	      uniform float4x4 Model)
{
  
  VERTEX_OUT OUT = (VERTEX_OUT) 0;

  OUT.position = OUT.hPos = mul(ModelViewProj, IN.position);
  OUT.wPos = mul(Model, IN.position).xyz;
  
  return OUT;
}

float4 ps(VERTEX_OUT IN):COLOR0
{
 float hZ = IN.hPos.z / IN.hPos.w;
 return float4(hZ, IN.wPos);
}