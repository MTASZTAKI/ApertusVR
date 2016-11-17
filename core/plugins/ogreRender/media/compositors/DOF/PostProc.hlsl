struct VS_OUT
{
	float4 hPosition	: POSITION;
	float2 texCoord		: TEXCOORD0;
	float3 wPos			: TEXCOORD1;	
};

static const float2 filterTaps[] =
{
	float2(-0.326212f, -0.405805f),
	float2(-0.840144f, -0.07358f),
	float2(-0.695914f, 0.457137f),
	float2(-0.203345f, 0.620716f),
	float2(0.96234f, -0.194983f),
	float2(0.473434f, -0.480026f),
	float2(0.519456f, 0.767022f),
	float2(0.185461f, -0.893124f),
	float2(0.507431f, 0.064425f),
	float2(0.89642f, 0.412458f),
	float2(-0.32194f, -0.932615f),
	float2(-0.791559f, -0.597705f)
};

float4 DoF_PS(VS_OUT IN,
		uniform float width,
		uniform float height,
		uniform float scale,
		uniform sampler2D Image: register(s0),
		uniform sampler2D SceneInfo: register(s1)
		 ):COLOR
{	
	IN.texCoord += float2(0.5/width, 0.5/height);
	float dx = 1.0f / width;
    float dy = 1.0f / height;

	// Depth based blur
	float4 colorSum = tex2D(Image, IN.texCoord); // Center sample
	float  depth = tex2D(SceneInfo, IN.texCoord).r; // Center sample
	float focalDist = tex2D(SceneInfo, float2(0.5,0.5)).r; 
	float sizeCoC = abs(depth - focalDist) * scale;
	
	for (int i = 0; i < 12; i++) // Filter
	{
     float2 tapCoord = IN.texCoord + filterTaps[i] * float2(dx,dy) * sizeCoC; //Tap coords
     colorSum +=  tex2D(Image, tapCoord);
	}
	return colorSum / 12; // Normalize 
}



