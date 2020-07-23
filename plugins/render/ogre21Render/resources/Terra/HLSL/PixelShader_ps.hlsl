@insertpiece( SetCrossPlatformSettings )

@property( false )

struct PS_INPUT
{
@insertpiece( Terra_VStoPS_block )
};

float4 main( PS_INPUT inPs
@property( hlms_vpos ), float4 gl_FragCoord : SV_Position@end )
{
	float4 outColour;
	outColour = float4( inPs.uv0.xy, 0.0, 1.0 );
	return outColour;
}

@end
@property( !false )
// START UNIFORM DECLARATION
@insertpiece( PassDecl )
@insertpiece( TerraMaterialDecl )
@insertpiece( TerraInstanceDecl )
@insertpiece( custom_ps_uniformDeclaration )
// END UNIFORM DECLARATION
struct PS_INPUT
{
@insertpiece( Terra_VStoPS_block )
};

Texture2D<float3> terrainNormals : register(t1);
Texture2D<float4> terrainShadows : register(t2);
SamplerState terrainNormalsSamplerState : register(s1);
SamplerState terrainShadowsSamplerState : register(s2);

@property( !hlms_shadowcaster )

@property( hlms_forwardplus )
Buffer<uint> f3dGrid : register(t3);
Buffer<float4> f3dLightList : register(t4);@end

@property( num_textures )Texture2DArray textureMaps[@value( num_textures )] : register(t@value(textureRegStart));@end
@property( envprobe_map )TextureCube	texEnvProbeMap : register(t@value(envMapReg));
SamplerState envMapSamplerState : register(s@value(envMapReg));@end

@foreach( numSamplerStates, n )
	SamplerState samplerState@n : register(s@counter(samplerStateStart));@end


@property( hlms_lights_spot_textured )@insertpiece( DeclQuat_zAxis )
float3 qmul( float4 q, float3 v )
{
	return v + 2.0 * cross( cross( v, q.xyz ) + q.w * v, q.xyz );
}
@end

@property( detail_maps_normal )float3 getTSDetailNormal( SamplerState samplerState, Texture2DArray normalMap, float3 uv )
{
	float3 tsNormal;
@property( signed_int_textures )
	//Normal texture must be in U8V8 or BC5 format!
	tsNormal.xy = normalMap.Sample( samplerState, uv ).xy;
@end @property( !signed_int_textures )
	//Normal texture must be in LA format!
	tsNormal.xy = normalMap.Sample( samplerState, uv ).xw * 2.0 - 1.0;
@end
	tsNormal.z	= sqrt( max( 0, 1.0 - tsNormal.x * tsNormal.x - tsNormal.y * tsNormal.y ) );

	return tsNormal;
}
	@foreach( 4, n )
		@property( normal_weight_detail@n )
			@piece( detail@n_nm_weight_mul ) * material.normalWeights.@insertpiece( detail_swizzle@n )@end
		@end
	@end
@end

@insertpiece( DeclareBRDF )

@insertpiece( DeclShadowMapMacros )
@insertpiece( DeclShadowSamplers )
@insertpiece( DeclShadowSamplingFuncs )

@insertpiece( DeclOutputType )

@insertpiece( output_type ) main( PS_INPUT inPs
@property( hlms_vpos ), float4 gl_FragCoord : SV_Position@end )
{
	PS_OUTPUT psOut;
	@insertpiece( custom_ps_preExecution )

	float4 diffuseCol;
	@insertpiece( FresnelType ) F0;
	float ROUGHNESS;

	float3 nNormal;

	@insertpiece( custom_ps_posMaterialLoad )

//Prepare weight map for the detail maps.
@property( detail_weight_map )
	float4 detailWeights = textureMaps[@value( detail_weight_map_idx )].Sample(
									samplerState@value(detail_weight_map_idx),
									float3( inPs.uv0.xy, @value(detail_weight_map_idx_slice) ) );
@end @property( !detail_weight_map )
	float4 detailWeights = float4( 1.0, 1.0, 1.0, 1.0 );
@end

@property( diffuse_map )
	diffuseCol = textureMaps[@value( diffuse_map_idx )].Sample(
									samplerState@value(diffuse_map_idx),
									float3( inPs.uv0.xy, @value(diffuse_map_idx_slice) ) );
@end

	/// Sample detail maps
@foreach( 4, n )
	@property( detail_map@n )
		float3 detailCol@n = textureMaps[@value(detail_map@n_idx)].Sample(
								samplerState@value(detail_map@n_idx),
								float3( inPs.uv0.xy * material.detailOffsetScale[@value(currOffsetDetail)].zw +
										material.detailOffsetScale[@value(currOffsetDetail)].xy,
										@value(detail_map@n_idx_slice) ) ).xyz;
	@end @property( !detail_map@n )
		float3 detailCol@n = float3( 0, 0, 0 );
	@end

	@property( metalness_map@n )
		float metalness@n = textureMaps[@value( metalness_map@n_idx )].Sample(
									samplerState@value(metalness_map@n_idx),
									float3( inPs.uv0.xy * material.detailOffsetScale[@value(currOffsetDetail)].zw +
											material.detailOffsetScale[@value(currOffsetDetail)].xy,
											@value( metalness_map@n_idx_slice ) ) ).x;
	@end @property( !metalness_map@n )
		float metalness@n = 0;
	@end

	@property( roughness_map@n )
		float roughness@n = textureMaps[@value( roughness_map@n_idx )].Sample(
									samplerState@value(roughness_map@n_idx),
									float3( inPs.uv0.xy * material.detailOffsetScale[@value(currOffsetDetail)].zw +
											material.detailOffsetScale[@value(currOffsetDetail)].xy,
											@value( roughness_map@n_idx_slice ) ) ).x;
	@end @property( !roughness_map@n )
		float roughness@n = 0;
	@end

	@add( currOffsetDetail, 1 )
@end

	float metalness =	(metalness0 * detailWeights.x * material.metalness.x +
						 metalness1 * detailWeights.y * material.metalness.y) +
						(metalness2 * detailWeights.z * material.metalness.z +
						 metalness3 * detailWeights.w * material.metalness.w);

	ROUGHNESS =			(roughness0 * detailWeights.x * material.roughness.x +
						 roughness1 * detailWeights.y * material.roughness.y) +
						(roughness2 * detailWeights.z * material.roughness.z +
						 roughness3 * detailWeights.w * material.roughness.w);
	ROUGHNESS = max( ROUGHNESS, 0.02 );

@property( diffuse_map )
	diffuseCol.xyz *=	(detailCol0 * detailWeights.x + detailCol1 * detailWeights.y) +
						(detailCol2 * detailWeights.z + detailCol3 * detailWeights.w);
@end @property( !diffuse_map )
	@property( detail_maps_diffuse )
		diffuseCol.xyz =	(detailCol0 * detailWeights.x + detailCol1 * detailWeights.y) +
							(detailCol2 * detailWeights.z + detailCol3 * detailWeights.w);
	@end @property( !detail_maps_diffuse )
		diffuseCol.xyzw = float4( 1, 1, 1, 1 );
	@end
@end

	/// Apply the material's diffuse over the textures
	diffuseCol.xyz *= material.kD.xyz;

	//Calculate F0 from metalness, and dim kD as metalness gets bigger.
	F0 = lerp( float3( 0.03f, 0.03f, 0.03f ), @insertpiece( kD ).xyz * 3.14159f, metalness );
	@insertpiece( kD ).xyz = @insertpiece( kD ).xyz - @insertpiece( kD ).xyz * metalness;

@property( !detail_maps_normal )
	// Geometric normal
	nNormal = terrainNormals.Sample( terrainNormalsSamplerState, inPs.uv0.xy ).xyz * 2.0f - 1.0f;
	//nNormal.xz = terrainNormals.Sample( terrainNormalsSamplerState, inPs.uv0.xy ).xy;
	//nNormal.y = sqrt( max( 1.0 - nNormal.x * nNormal.x - nNormal.z * nNormal.z, 0.0 ) );
	nNormal = mul( nNormal, (float3x3)passBuf.view );
@end @property( detail_maps_normal )
	float3 geomNormal = terrainNormals.Sample( terrainNormalsSamplerState, inPs.uv0.xy ).xyz * 2.0f - 1.0f;
	geomNormal = mul( geomNormal, (float3x3)passBuf.view );

	//Get the TBN matrix
	float3 viewSpaceUnitX	= passBuf.view[0].xyz;
	float3 vTangent			= normalize( cross( geomNormal, viewSpaceUnitX ) );
	float3 vBinormal		= cross( vTangent, geomNormal );
	float3x3 TBN			= float3x3( vBinormal, vTangent, geomNormal );
@end

	float fTerrainShadow = terrainShadows.Sample( terrainShadowsSamplerState, inPs.uv0.xy ).x;
	@property( !(hlms_pssm_splits || (!hlms_pssm_splits && hlms_num_shadow_map_lights && hlms_lights_directional)) )
		float fShadow = 1.0f;
	@end
	@insertpiece( DoDirectionalShadowMaps )
	fShadow *= fTerrainShadow;

	/// The first iteration must initialize nNormal instead of try to merge with it.
	/// Blend the detail normal maps with the main normal.
@foreach( second_valid_detail_map_nm, n, first_valid_detail_map_nm )
	float3 vDetail = @insertpiece( SampleDetailMapNm@n ) * detailWeights.@insertpiece(detail_swizzle@n);
	nNormal.xy	= vDetail.xy;
	nNormal.z	= vDetail.z + 1.0 - detailWeights.@insertpiece(detail_swizzle@n);@end
@foreach( detail_maps_normal, n, second_valid_detail_map_nm )@property( detail_map_nm@n )
	vDetail = @insertpiece( SampleDetailMapNm@n ) * detailWeights.@insertpiece(detail_swizzle@n);
	nNormal.xy	+= vDetail.xy;
	nNormal.z	*= vDetail.z + 1.0 - detailWeights.@insertpiece(detail_swizzle@n);@end @end

@property( detail_maps_normal )
	nNormal = normalize( mul( nNormal, TBN ) );
@end

	//Everything's in Camera space
@property( hlms_lights_spot || ambient_hemisphere || envprobe_map || hlms_forwardplus )
	float3 viewDir	= normalize( -inPs.pos );
	float NdotV		= saturate( dot( nNormal, viewDir ) );@end

@property( !ambient_fixed )
	float3 finalColour = float3(0, 0, 0);
@end @property( ambient_fixed )
	float3 finalColour = passBuf.ambientUpperHemi.xyz * @insertpiece( kD ).xyz;
@end

	@insertpiece( custom_ps_preLights )

@property( !custom_disable_directional_lights )
@property( hlms_lights_directional )
	finalColour += BRDF( passBuf.lights[0].position.xyz, viewDir, NdotV, passBuf.lights[0].diffuse, passBuf.lights[0].specular, material, nNormal @insertpiece( brdfExtraParams ) ) @insertpiece( DarkenWithShadowFirstLight );
@end
@foreach( hlms_lights_directional, n, 1 )
	finalColour += BRDF( passBuf.lights[@n].position.xyz, viewDir, NdotV, passBuf.lights[@n].diffuse, passBuf.lights[@n].specular, material, nNormal @insertpiece( brdfExtraParams ) )@insertpiece( DarkenWithShadow );@end
@foreach( hlms_lights_directional_non_caster, n, hlms_lights_directional )
	finalColour += BRDF( passBuf.lights[@n].position.xyz, viewDir, NdotV, passBuf.lights[@n].diffuse, passBuf.lights[@n].specular, material, nNormal @insertpiece( brdfExtraParams ) );@end
@end

@property( hlms_lights_point || hlms_lights_spot )	float3 lightDir;
	float fDistance;
	float3 tmpColour;
	float spotCosAngle;@end

	//Point lights
@foreach( hlms_lights_point, n, hlms_lights_directional_non_caster )
	lightDir = passBuf.lights[@n].position.xyz - inPs.pos;
	fDistance= length( lightDir );
	if( fDistance <= passBuf.lights[@n].attenuation.x )
	{
		lightDir *= 1.0 / fDistance;
		tmpColour = BRDF( lightDir, viewDir, NdotV, passBuf.lights[@n].diffuse, passBuf.lights[@n].specular, material, nNormal @insertpiece( brdfExtraParams ) )@insertpiece( DarkenWithShadowPoint );
		float atten = 1.0 / (0.5 + (passBuf.lights[@n].attenuation.y + passBuf.lights[@n].attenuation.z * fDistance) * fDistance );
		finalColour += tmpColour * atten;
	}@end

	//Spot lights
	//spotParams[@value(spot_params)].x = 1.0 / cos( InnerAngle ) - cos( OuterAngle )
	//spotParams[@value(spot_params)].y = cos( OuterAngle / 2 )
	//spotParams[@value(spot_params)].z = falloff
@foreach( hlms_lights_spot, n, hlms_lights_point )
	lightDir = passBuf.lights[@n].position.xyz - inPs.pos;
	fDistance= length( lightDir );
@property( !hlms_lights_spot_textured )	spotCosAngle = dot( normalize( inPs.pos - passBuf.lights[@n].position.xyz ), passBuf.lights[@n].spotDirection );@end
@property( hlms_lights_spot_textured )	spotCosAngle = dot( normalize( inPs.pos - passBuf.lights[@n].position.xyz ), zAxis( passBuf.lights[@n].spotQuaternion ) );@end
	if( fDistance <= passBuf.lights[@n].attenuation.x && spotCosAngle >= passBuf.lights[@n].spotParams.y )
	{
		lightDir *= 1.0 / fDistance;
	@property( hlms_lights_spot_textured )
		float3 posInLightSpace = qmul( spotQuaternion[@value(spot_params)], inPs.pos );
		float spotAtten = texture( texSpotLight, normalize( posInLightSpace ).xy ).x; //TODO
	@end
	@property( !hlms_lights_spot_textured )
		float spotAtten = saturate( (spotCosAngle - passBuf.lights[@n].spotParams.y) * passBuf.lights[@n].spotParams.x );
		spotAtten = pow( spotAtten, passBuf.lights[@n].spotParams.z );
	@end
		tmpColour = BRDF( lightDir, viewDir, NdotV, passBuf.lights[@n].diffuse, passBuf.lights[@n].specular, material, nNormal @insertpiece( brdfExtraParams ) )@insertpiece( DarkenWithShadow );
		float atten = 1.0 / (0.5 + (passBuf.lights[@n].attenuation.y + passBuf.lights[@n].attenuation.z * fDistance) * fDistance );
		finalColour += tmpColour * (atten * spotAtten);
	}@end

@insertpiece( forward3dLighting )

@property( envprobe_map || ambient_hemisphere )
	float3 reflDir = 2.0 * dot( viewDir, nNormal ) * nNormal - viewDir;
	
	@property( envprobe_map )
		float3 envColourS = texEnvProbeMap.SampleLevel( envMapSamplerState, mul( reflDir, passBuf.invViewMatCubemap ), ROUGHNESS * 12.0 ).xyz @insertpiece( ApplyEnvMapScale );
		float3 envColourD = texEnvProbeMap.SampleLevel( envMapSamplerState, mul( nNormal, passBuf.invViewMatCubemap ), 11.0 ).xyz @insertpiece( ApplyEnvMapScale );
		@property( !hw_gamma_read )	//Gamma to linear space
			envColourS = envColourS * envColourS;
			envColourD = envColourD * envColourD;
		@end
	@end
	@property( ambient_hemisphere )
		float ambientWD = dot( passBuf.ambientHemisphereDir.xyz, nNormal ) * 0.5 + 0.5;
		float ambientWS = dot( passBuf.ambientHemisphereDir.xyz, reflDir ) * 0.5 + 0.5;

		@property( envprobe_map )
			envColourS	+= lerp( passBuf.ambientLowerHemi.xyz, passBuf.ambientUpperHemi.xyz, ambientWD );
			envColourD	+= lerp( passBuf.ambientLowerHemi.xyz, passBuf.ambientUpperHemi.xyz, ambientWS );
		@end @property( !envprobe_map )
			float3 envColourS = lerp( passBuf.ambientLowerHemi.xyz, passBuf.ambientUpperHemi.xyz, ambientWD );
			float3 envColourD = lerp( passBuf.ambientLowerHemi.xyz, passBuf.ambientUpperHemi.xyz, ambientWS );
		@end
	@end

	@insertpiece( BRDF_EnvMap )
@end

@property( !hw_gamma_write )
	//Linear to Gamma space
	psOut.colour0.xyz	= sqrt( finalColour );
@end @property( hw_gamma_write )
	psOut.colour0.xyz	= finalColour;
@end

@property( hlms_alphablend )
	@property( use_texture_alpha )
		psOut.colour0.w		= material.F0.w * diffuseCol.w;
	@end @property( !use_texture_alpha )
		psOut.colour0.w		= material.F0.w;
	@end
@end @property( !hlms_alphablend )
	psOut.colour0.w		= 1.0;@end
@end

	@property( debug_pssm_splits )
		psOut.colour0.xyz = lerp( psOut.colour0.xyz, debugPssmSplit.xyz, 0.2f );
	@end

	@insertpiece( custom_ps_posExecution )

@property( !hlms_render_depth_only )
	return psOut;
@end
}
@end
