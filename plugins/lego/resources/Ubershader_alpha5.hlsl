struct AppVertexData {
    float4 pos		: POSITION;
    float4 color	: COLOR0;
    float3 normal	: NORMAL;
    float2 txcoord	: TEXCOORD0;
    float3 tangent	: TANGENT0;
    float3 binormal	: BINORMAL0;
};

struct VertexOutput {
    float4 hpos		: POSITION;
    float2 UV		: TEXCOORD0;
    float3 tangent	: TEXCOORD1;
    float3 binormal	: TEXCOORD2;
    float3 normal	: TEXCOORD3;
    float3 lightdir	: TEXCOORD4;
    float4 color	: COLOR0;
    float2 UV2      : TEXCOORD5;
    float3 eyevec   : TEXCOORD6;
    float2 vParallaxOffsetTS : TEXCOORD7;
    float3 SpotDirection    : TEXCOORD8;
   
};

void Ambient_vp(float4 position : POSITION, 
                out float4 oPosition : POSITION,
                out float4 colour    : COLOR,
                uniform float4x4 worldViewProj,
                uniform float4 ambient)
 {
     oPosition = mul(worldViewProj, position);
     colour = ambient;
 } 

VertexOutput view_spaceVS(AppVertexData IN,

    uniform float4x4 WvpXf,
    
    uniform float3 eyeposition,
        
    uniform float TileCount,

    uniform float DetTileCount,
    
    uniform float4 LampPos,

    uniform float3 spotDirection,

    uniform float useHeightmap,

    uniform float Offset_bias

    
) {
    VertexOutput OUT = (VertexOutput)0;
    
    IN.binormal = cross(IN.tangent, IN.normal);
       
    float3x3 rotation = float3x3(IN.tangent, IN.binormal, IN.normal); 

   
    OUT.hpos = mul(WvpXf,IN.pos);
   
    OUT.lightdir = normalize(LampPos.xyz -  (IN.pos * LampPos.w));
   
    OUT.tangent = IN.tangent;
    OUT.binormal = IN.binormal;
    OUT.normal = IN.normal;


    OUT.color = IN.color;
    OUT.UV = TileCount * IN.txcoord.xy;
    OUT.UV2 = DetTileCount * IN.txcoord.xy;

    OUT.eyevec = eyeposition - IN.pos.xyz;    //get the eye vector

    //OUT.eyevec = normalize(mul(rotation, OUT.eyevec));

    //OUT.lightdir = normalize(mul(rotation, OUT.lightdir));

    OUT.SpotDirection = -spotDirection;

    if (useHeightmap==1)
    {

    float3 vViewTS  = mul( rotation, OUT.eyevec );
    
    // Compute initial parallax displacement direction:
    float2 vParallaxDirection = normalize(  vViewTS.xy );
       
    // The length of this vector determines the furthest amount of displacement:
    float fLength         = length( vViewTS );
    float fParallaxLength = sqrt( fLength * fLength - vViewTS.z * vViewTS.z ) / vViewTS.z; 
       
    OUT.vParallaxOffsetTS = vParallaxDirection * fParallaxLength * Offset_bias;
    
    }

    return OUT;
}


/************ PIXEL SHADERS ******************/


float2 LightingFuncGGX_FV(float dotLH, float roughness)
{
    float alpha = roughness*roughness;

    // F
    float F_a, F_b;
    float dotLH5 = pow(1.0f-dotLH,5);
    F_a = 1.0f;
    F_b = dotLH5;

    // V
    float vis;
    float k = alpha/2.0f;
    float k2 = k*k;
    float invK2 = 1.0f-k2;
    vis = rcp(dotLH*dotLH*invK2 + k2);

    return float2(F_a*vis,F_b*vis);
}

float LightingFuncGGX_D(float dotNH, float roughness)
{
    float alpha = roughness*roughness;
    float alphaSqr = alpha*alpha;
    float pi = 3.14159f;
    float denom = dotNH * dotNH *(alphaSqr-1.0) + 1.0f;

    float D = alphaSqr/(pi * denom * denom);
    return D;
}

float LightingFuncGGX_OPT3(float3 N, float3 V, float3 L, float roughness, float F0)
{
    float3 H = normalize(V+L);

    float dotNL = saturate(dot(N,L));
    float dotLH = saturate(dot(L,H));
    float dotNH = saturate(dot(N,H));

    float D = LightingFuncGGX_D(dotNH,roughness);
    float2 FV_helper = LightingFuncGGX_FV(dotLH,roughness);
    float FV = F0*FV_helper.x + (1.0f-F0)*FV_helper.y;
    float specular = dotNL * D * FV;

    return specular;
}

float2 ParallaxOcclusionMapping (float numofminsamples, float numofmaxsamples, float3 vViewWS, float3 vNormalWS,
    float2 vParallaxOffsetTS, float2 texCoord, sampler2D HeightSampler )
{
    float2 dxSize, dySize;
   float2 dx, dy;

   float2 fTexCoordsPerSize = texCoord;
   float4( dxSize, dx ) = ddx( float4( fTexCoordsPerSize, texCoord ) );
   float4( dySize, dy ) = ddy( float4( fTexCoordsPerSize, texCoord ) );
   
  int nNumSteps = (int) lerp( numofmaxsamples, numofminsamples, dot( vViewWS, vNormalWS ) );

  float fCurrHeight = 0.0;
  float fStepSize   = 1.0 / (float) nNumSteps;
  float fPrevHeight = 1.0;
  float fNextHeight = 0.0;

  int    nStepIndex = 0;
  bool   bCondition = true;

  float2 vTexOffsetPerStep = fStepSize * vParallaxOffsetTS;
  float2 vTexCurrentOffset = texCoord;
  float  fCurrentBound     = 1.0;
  float  fParallaxAmount   = 0.0;

  float2 pt1 = 0;
  float2 pt2 = 0;
   
  float2 texOffset2 = 0;

  while ( nStepIndex < nNumSteps ) 
  {
     vTexCurrentOffset -= vTexOffsetPerStep;

     fCurrHeight = tex2Dgrad( HeightSampler, vTexCurrentOffset, dx, dy );

     fCurrentBound -= fStepSize;

     if ( fCurrHeight > fCurrentBound ) 
     {   
        pt1 = float2( fCurrentBound, fCurrHeight );
        pt2 = float2( fCurrentBound + fStepSize, fPrevHeight );

        texOffset2 = vTexCurrentOffset - vTexOffsetPerStep;

        nStepIndex = nNumSteps + 1;
        fPrevHeight = fCurrHeight;
     }
     else
     {
        nStepIndex++;
        fPrevHeight = fCurrHeight;
     }
  }   

  float fDelta2 = pt2.x - pt2.y;
  float fDelta1 = pt1.x - pt1.y;
  
  float fDenominator = fDelta2 - fDelta1;
  
  if ( fDenominator == 0.0f )
  {
     fParallaxAmount = 0.0f;
  }
  else
  {
     fParallaxAmount = (pt1.x * fDelta2 - pt2.x * fDelta1 ) / fDenominator;
  }
  
  float2 vParallaxOffset = vParallaxOffsetTS * (1 - fParallaxAmount );

  // The computed texture offset for the displaced point on the pseudo-extruded surface:
  float2 texSampleBase = texCoord - vParallaxOffset;
  float2 texSample = texSampleBase;

    return texSample;
}

float4  NormalMapWSTransform (float4 texNormal, float3 vecNormalWS, float3 vecBinormalWS, float3 vecTangentWS)
{
  texNormal = texNormal * 2 - 1;
  texNormal = float4((vecNormalWS * texNormal.z) + (texNormal.x * vecTangentWS + texNormal.y * -vecBinormalWS), 1.0);
  return texNormal;
}


float4 normal_mapPS(VertexOutput IN,
		    uniform float3 LampColor,
            uniform float3 SurfaceColor,
            uniform float4 SpecColor,
            uniform float3 AmbiColor,
            uniform float useDiffusemap,
            uniform float useNormalmap,
            uniform float useDetailDiffuseMap,
            uniform float useDetailNormalMap,
            uniform float useSpecCMap,
            uniform float useRoghnessMap,
            uniform float useHeightmap,
            uniform float useReflection,
            uniform float useAO,
            uniform float specMult,
            uniform float Roughness,
            uniform float F0,
            uniform float ReflBlurFactor,
            uniform float4 spotParams,
            uniform float NumofMinSamples,
            uniform float NumofMaxSamples,
            uniform float ReflectionPower,
            uniform sampler2D ColorSampler : register(s0),
            uniform sampler2D ColorDetailSampler : register(s1),
            uniform sampler2D NormalSampler : register(s2),
            uniform sampler2D NormalDetailSampler : register(s3),
            uniform sampler2D SpecularColorSampler : register(s4),
            uniform sampler2D RoughnessSampler : register(s5),
            uniform sampler2D HeightSampler : register(s6),
            uniform sampler2D AOsampler : register(s7),
            uniform samplerCUBE ReflectionCubeSampler : register(s8)            
) : COLOR

{
    float3 Vn = normalize(IN.eyevec);
    float3 Tn = normalize(IN.tangent);
    float3 Bn = normalize(IN.binormal);

    float3 Ln = normalize(IN.lightdir);
    float3 Nn = normalize(IN.normal); 

    float2 uv = IN.UV;

    float heightMap = tex2D(HeightSampler, IN.UV.xy);
      
    if(useHeightmap==1)
    {
      uv = ParallaxOcclusionMapping (NumofMinSamples, NumofMaxSamples, Vn, Nn,  //numofmaxsamples 60 , numofminsamples 20,
        IN.vParallaxOffsetTS, IN.UV,HeightSampler);
    }

    float4 texCol;
    
    if (useDiffusemap==1)
    {
        texCol = tex2D(ColorSampler,uv);
    }

    else 
    {
        texCol = float4(1,1,1,1);
    }

    float texAO;

    if (useAO==1)
    {
        texAO = tex2D(AOsampler,uv);
    }

    else 
    {
        texAO = 1;
    }


    float3 texDetCol;

    if (useDetailDiffuseMap==1)
         texDetCol = tex2D(ColorDetailSampler,IN.UV2).xyz;    
    else
    {
        texDetCol = float4(0,0,0,1);
    }
    
    float4 tNorm;

    float3 N;

    if (useNormalmap==1)
    {
        tNorm = tex2D(NormalSampler,uv);
        tNorm = tNorm * 2 - 1;
        tNorm = float4((Nn * tNorm.z) + (tNorm.x * Tn + tNorm.y * Bn), tNorm.w);
        N = normalize(tNorm.rgb);
    }
    else
    {
        tNorm = float4(normalize(IN.normal),1);
        N = normalize(tNorm.rgb);   
    }
    float4 tDetNorm;

    if (useDetailNormalMap==1)
    {
        tDetNorm = tex2D(NormalDetailSampler,IN.UV2);
        tDetNorm = tDetNorm * 2 - 1;
        tDetNorm = float4((Nn * tDetNorm.z) + (tDetNorm.x * Tn + tDetNorm.y * Bn), tDetNorm.w);
        N = normalize(tNorm.rgb + tDetNorm.rgb);
    }
    else
    {
        tDetNorm = float4(normalize(IN.normal),1);    
        N = normalize(tNorm.rgb + tDetNorm.rgb);
    }
    
    float4 texSpecColor;

    if (useSpecCMap==1)
    {
        texSpecColor = SpecColor * tex2D(SpecularColorSampler,uv);
    }
 
    else 
    {
        texSpecColor=SpecColor;
    }
    
    float RoughnessMap = tex2D(RoughnessSampler, uv);

    if (useRoghnessMap==1)
    {
       Roughness=saturate(RoughnessMap);
    }
 
    float att = saturate(dot(Ln,IN.normal));

    float diff = saturate(dot(Ln,N));

    float4 reflCubeTexture;

    if (useReflection==1)
    {
   
    float3 E = -IN.eyevec;

    float4 reflVector;

    reflVector.xyz = reflect(E, N); 

    float envBlend = pow(Roughness, ReflBlurFactor); //need more brain :)

    float4 normalReflection;

    normalReflection.xyz = lerp(N, reflVector, envBlend); 

    //ogre conversion ??? need ? :)
    
    normalReflection.z = - normalReflection.z;

    //ogre conversion end
    
    normalReflection.w = ReflBlurFactor;

    reflCubeTexture = texCUBEbias(ReflectionCubeSampler, normalReflection);

    reflCubeTexture = reflCubeTexture * ReflectionPower;

    }
    else
    {
      reflCubeTexture=texCol;
    }
    
     
    float rho = saturate(dot(normalize(IN.SpotDirection),Ln));
    
    float spotFactor = pow(
    saturate(rho - spotParams.y) / (spotParams.x - spotParams.y), spotParams.z);

    float4 specularcomponent;

    specularcomponent = (LightingFuncGGX_OPT3( N, Vn, Ln, Roughness, F0) * Roughness * specMult * texSpecColor) ; //+ reflCubeTexture;
    
    float3 finalcolor = AmbiColor*(texCol.rgb+texDetCol.rgb) +
        att*((texCol.rgb+texDetCol.rgb + reflCubeTexture)*SurfaceColor*diff+specularcomponent);
    
    return float4(finalcolor.rgb*LampColor*texAO,1.0);
}