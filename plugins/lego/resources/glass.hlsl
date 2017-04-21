void Ambient_vp(float4 position : POSITION, 
                out float4 oPosition : POSITION,
                out float4 colour    : COLOR,
                uniform float4x4 worldViewProj,
                uniform float4 ambient_p)
 {
     oPosition = mul(worldViewProj, position);
     colour = ambient_p;
 } 


float FresnelEffect (float3 vecView, float3 vecNormal, float fFresnelScale, float fFresnelBias, float fFresnelPower)
{
    float   fNdotE = saturate(dot(vecNormal, vecView));
    float   fFresnel = (pow(fNdotE, fFresnelPower) * fFresnelScale) + fFresnelBias;
    return  fFresnel;
}


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



struct VS_OUTPUT2 {
   float4 Pos:     POSITION;
   float3 normal:  TEXCOORD0;
   float3 viewVec: TEXCOORD1;
   float3 lightdir: TEXCOORD2;
   float4 Attenuation: TEXCOORD3;

};



VS_OUTPUT2 Glass_Effect_Group_ComplexGlass_Object_Vertex_Shader_main(uniform float3 eyeposition,
                                                                     uniform float4x4 worldviewproj,
                                                                     uniform float4 LampPos,
                                                                     uniform float4 lightAttenuation,
                                                                     float4 Pos: POSITION,
                                                                     float3 normal: NORMAL)
{
   VS_OUTPUT2 Out;
   Out.Pos = mul(worldviewproj, Pos);
   Out.normal = normal;
   Out.viewVec = eyeposition - Pos;
   Out.lightdir = normalize(LampPos.xyz -  (Pos * LampPos.w));

   float Dist = distance(mul(worldviewproj, LampPos), mul(worldviewproj, Pos)); 
   Out.Attenuation = saturate(1/(lightAttenuation.y + lightAttenuation.z * Dist + lightAttenuation.w * Dist * Dist));

   return Out;
}


float4 Glass_Effect_Group_ComplexGlass_Object_Pixel_Shader_spec_main(float3 normal: TEXCOORD0,
                                                                float3 viewVec: TEXCOORD1,
                                                                float3 lightvec: TEXCOORD2,
                                                                float4 Attenuation: TEXCOORD3,
                                                                uniform float4 SpecColor,
                                                                uniform float Roughness,
                                                                uniform float F0,
                                                                uniform float  ambient,         
                                                                 uniform float  specmult
                                                                ) : COLOR 
{
   normal = normalize(normal);
  
   normal.z = -normal.z;
   
   viewVec = normalize(viewVec);

   //viewVec.z = -viewVec.z;

   float3 Ln = normalize(lightvec);

   float4 specularcomponent = (LightingFuncGGX_OPT3( normal, viewVec, Ln, Roughness, F0)  *  specmult ) ; 

   
   return specularcomponent*SpecColor;//+Attenuation+ambient;
}

float4 Glass_Effect_Group_ComplexGlass_Object_Pixel_Shader_main(float3 normal: TEXCOORD0,
                                                                float3 viewVec: TEXCOORD1,
                                                                float3 lightvec: TEXCOORD2,
                                                                float4 Attenuation: TEXCOORD3,
                                                                uniform float4 LampColor,
                                                                uniform float  useFresnel,

                                                               

                                                                uniform float  refractionScale,
                                                                uniform float  rainbowScale,    
                                                                uniform float  rainbowSpread,

                                                                uniform float  reflblur,

                                                                uniform float g_fFresnelScale,
                                                                uniform float g_fFresnelBias, 
                                                                uniform float g_fFresnelPower, 
                                                                uniform float4 g_fFresnelColor,

                                                                uniform float4 baseColor,      
                                                                uniform float  ambient,         
                                                                uniform float  indexOfRefractionRatio,
                                                                uniform float  reflectionScale,
                                                                uniform samplerCUBE enviromentsampler : register(s0),
                                                                uniform sampler1D   rainbowsampler : register(s1)) : COLOR 
{
   normal = normalize(normal);

   normal.z = -normal.z;
   
   viewVec = normalize(viewVec);

   viewVec.z = -viewVec.z;

   float3 Ln = normalize(lightvec);

   // Look up the reflection
   float4 reflVec;
   reflVec.xyz = reflect(viewVec, normal);
   
   reflVec.w = -reflblur;

   //reflCubeTexture = texCUBEbias(ReflectionCubeSampler, normalReflection);
   
   float4 reflection = texCUBEbias(enviromentsampler, -reflVec);
   
   //float4 reflection = texCUBE(enviromentsampler, -reflVec.xyz);

   

   // We'll use Snell's refraction law:
   // n  * sin(theta ) = n  * sin(theta )
   //  i            i     r            r

   // sin(theta )
   //          i
   float cosine = dot(viewVec, normal);
   float sine = sqrt(1 - cosine * cosine);

   // sin(theta )
   //          r
   float sine2 = saturate(indexOfRefractionRatio * sine);
   float cosine2 = sqrt(1 - sine2 * sine2);

   
   float3 x = -normal;
  
   float3 y = normalize(cross(cross(viewVec, normal), normal));

   // Refraction
   float4 refrVec;
   refrVec.xyz = x * cosine2 + y * sine2;

   refrVec.w = reflblur;

 
   float4 refraction = texCUBEbias(enviromentsampler, refrVec);

   //float4 refraction = texCUBE(enviromentsampler, refrVec.xyz);

   // Colors refract differently and the difference is more
   // visible the stronger the refraction. We'll fake this
   // effect by adding some rainbowish colors accordingly.
   float4 rainbow = tex1D(rainbowsampler, pow(cosine, rainbowSpread));

   float4 rain = rainbowScale * rainbow * baseColor;
   float4 refl = reflectionScale * reflection * baseColor;
   float4 refr = refractionScale * refraction * baseColor;

   float4 cFresnel = float4(0,0,0,1);
   
   if (useFresnel==1)
    {
       
        float4 cGlance;
        float4 cStraight = g_fFresnelColor;
        float fFresnel = FresnelEffect (viewVec, normal, g_fFresnelScale, g_fFresnelBias, g_fFresnelPower);
       
        cGlance = refl;

        cFresnel = lerp(cGlance, cStraight, fFresnel);



    }
  
   // There is more light reflected at sharp angles and less
   // light refracted. There is more color separation of refracted
   // light at sharper angles
   //return sine * cFresnel + (1 - sine2) * refr + sine2 * rain + (ambient*LampColor);

   return  (cFresnel * LampColor);//+(baseColor*baseColor);
  }


float4 Tint( uniform float4 tintcolor) : COLOR 
{
   return tintcolor;
}











