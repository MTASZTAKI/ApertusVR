@insertpiece( SetCrossPlatformSettings )

@insertpiece( Common_Matrix_DeclUnpackMatrix4x4 )
@insertpiece( Common_Matrix_DeclUnpackMatrix4x3 )

struct VS_INPUT
{
	float4 vertex : POSITION;
@property( hlms_normal )	float3 normal : NORMAL;@end
@property( hlms_qtangent )	float4 qtangent : NORMAL;@end

@property( normal_map && !hlms_qtangent )
	float3 tangent	: TANGENT;
	@property( hlms_binormal )float3 binormal	: BINORMAL;@end
@end

@property( hlms_skeleton )
	uint4 blendIndices	: BLENDINDICES;
	float4 blendWeights : BLENDWEIGHT;@end

@property( hlms_pose )
	uint vertexId: SV_VertexID;@end

@foreach( hlms_uv_count, n )
	float@value( hlms_uv_count@n ) uv@n : TEXCOORD@n;@end
	uint drawId : DRAWID;
	@insertpiece( custom_vs_attributes )
};

struct PS_INPUT
{
@insertpiece( VStoPS_block )
	float4 gl_Position: SV_Position;

	@pdiv( full_pso_clip_distances, hlms_pso_clip_distances, 4 )
	@pmod( partial_pso_clip_distances, hlms_pso_clip_distances, 4 )
	@foreach( full_pso_clip_distances, n )
		float4 gl_ClipDistance@n : SV_ClipDistance@n;
	@end
	@property( partial_pso_clip_distances )
		float@value( partial_pso_clip_distances ) gl_ClipDistance@value( full_pso_clip_distances ) : SV_ClipDistance@value( full_pso_clip_distances );
	@end
};

// START UNIFORM DECLARATION
@insertpiece( PassDecl )
@property( hlms_skeleton || hlms_shadowcaster || hlms_pose )@insertpiece( InstanceDecl )@end
Buffer<float4> worldMatBuf : register(t0);
@property(hlms_pose)
	Buffer<float4> poseBuf : register(t4);@end
@insertpiece( custom_vs_uniformDeclaration )
// END UNIFORM DECLARATION

@property( hlms_qtangent )
@insertpiece( DeclQuat_xAxis )
@property( normal_map )
@insertpiece( DeclQuat_yAxis )
@end @end

@property( !hlms_skeleton )
@piece( local_vertex )input.vertex@end
@piece( local_normal )normal@end
@piece( local_tangent )tangent@end
@end
@property( hlms_skeleton )
@piece( local_vertex )worldPos@end
@piece( local_normal )worldNorm@end
@piece( local_tangent )worldTang@end
@end

@property( hlms_skeleton )@piece( SkeletonTransform )
	uint _idx = (input.blendIndices[0] << 1u) + input.blendIndices[0]; //blendIndices[0] * 3u; a 32-bit int multiply is 4 cycles on GCN! (and mul24 is not exposed to GLSL...)
		uint matStart = worldMaterialIdx[input.drawId].x >> 9u;
	float4 worldMat[3];
		worldMat[0] = worldMatBuf.Load( int(matStart + _idx + 0u) );
		worldMat[1] = worldMatBuf.Load( int(matStart + _idx + 1u) );
		worldMat[2] = worldMatBuf.Load( int(matStart + _idx + 2u) );
	float4 worldPos;
	worldPos.x = dot( worldMat[0], input.vertex );
	worldPos.y = dot( worldMat[1], input.vertex );
	worldPos.z = dot( worldMat[2], input.vertex );
	worldPos.xyz *= input.blendWeights[0];
	@property( hlms_normal || hlms_qtangent )float3 worldNorm;
	worldNorm.x = dot( worldMat[0].xyz, normal );
	worldNorm.y = dot( worldMat[1].xyz, normal );
	worldNorm.z = dot( worldMat[2].xyz, normal );
	worldNorm *= input.blendWeights[0];@end
	@property( normal_map )float3 worldTang;
	worldTang.x = dot( worldMat[0].xyz, tangent );
	worldTang.y = dot( worldMat[1].xyz, tangent );
	worldTang.z = dot( worldMat[2].xyz, tangent );
	worldTang *= input.blendWeights[0];@end

	@psub( NeedsMoreThan1BonePerVertex, hlms_bones_per_vertex, 1 )
	@property( NeedsMoreThan1BonePerVertex )float4 tmp;
	tmp.w = 1.0;@end //!NeedsMoreThan1BonePerVertex
	@foreach( hlms_bones_per_vertex, n, 1 )
	_idx = (input.blendIndices[@n] << 1u) + input.blendIndices[@n]; //blendIndices[@n] * 3; a 32-bit int multiply is 4 cycles on GCN! (and mul24 is not exposed to GLSL...)
		worldMat[0] = worldMatBuf.Load( int(matStart + _idx + 0u) );
		worldMat[1] = worldMatBuf.Load( int(matStart + _idx + 1u) );
		worldMat[2] = worldMatBuf.Load( int(matStart + _idx + 2u) );
	tmp.x = dot( worldMat[0], input.vertex );
	tmp.y = dot( worldMat[1], input.vertex );
	tmp.z = dot( worldMat[2], input.vertex );
	worldPos.xyz += (tmp * input.blendWeights[@n]).xyz;
	@property( hlms_normal || hlms_qtangent )
	tmp.x = dot( worldMat[0].xyz, normal );
	tmp.y = dot( worldMat[1].xyz, normal );
	tmp.z = dot( worldMat[2].xyz, normal );
	worldNorm += tmp.xyz * input.blendWeights[@n];@end
	@property( normal_map )
	tmp.x = dot( worldMat[0].xyz, tangent );
	tmp.y = dot( worldMat[1].xyz, tangent );
	tmp.z = dot( worldMat[2].xyz, tangent );
	worldTang += tmp.xyz * input.blendWeights[@n];@end
	@end

	worldPos.w = 1.0;
@end @end  //SkeletonTransform // !hlms_skeleton

@property( hlms_pose )@piece( PoseTransform )
	// Pose data starts after all 3x4 bone matrices
	uint poseDataStart = (worldMaterialIdx[input.drawId].x >> 9u) @property( hlms_skeleton ) + @value(hlms_bones_per_vertex)u * 3u@end ;

	@psub( MoreThanOnePose, hlms_pose, 1 )
	@property( !MoreThanOnePose )
		float4 poseWeights = worldMatBuf.Load( int(poseDataStart + 1u) );
		float4 posePos = poseBuf.Load( int(input.vertexId @property( hlms_pose_normals )<< 1u@end ) );
		input.vertex += posePos * poseWeights.x;
		@property( hlms_pose_normals && (hlms_normal || hlms_qtangent) )
			float4 poseNormal = poseBuf.Load( int((input.vertexId << 1u) + 1u) );
			normal += poseNormal.xyz * poseWeights.x;
		@end
		@pset( NumPoseWeightVectors, 1 )
	@end @property( MoreThanOnePose )
		// NumPoseWeightVectors = (hlms_pose / 4) + min(hlms_pose % 4, 1)
		@pdiv( NumPoseWeightVectorsA, hlms_pose, 4 )
		@pmod( NumPoseWeightVectorsB, hlms_pose, 4 )
		@pmin( NumPoseWeightVectorsC, NumPoseWeightVectorsB, 1 )
		@padd( NumPoseWeightVectors, NumPoseWeightVectorsA, NumPoseWeightVectorsC)
		float4 poseData = worldMatBuf.Load( int(poseDataStart) );
		uint numVertices = asuint( poseData.y );

		@psub( MoreThanOnePoseWeightVector, NumPoseWeightVectors, 1)
		@property( !MoreThanOnePoseWeightVector )
			float4 poseWeights = worldMatBuf.Load( int(poseDataStart + 1u) );
			@foreach( hlms_pose, n )
				input.vertex += poseBuf.Load( int((input.vertexId + numVertices * @nu) @property( hlms_pose_normals )<< 1u@end ) ) * poseWeights[@n];
				@property( hlms_pose_normals && (hlms_normal || hlms_qtangent) )
				normal += poseBuf.Load( int(((input.vertexId + numVertices * @nu) << 1u) + 1u) ).xyz * poseWeights[@n];
				@end
			@end
		@end @property( MoreThanOnePoseWeightVector )
			float poseWeights[@value(NumPoseWeightVectors) * 4];
			@foreach( NumPoseWeightVectors, n)
				float4 weights@n = worldMatBuf.Load( int(poseDataStart + 1u + @nu) );
				poseWeights[@n * 4 + 0] = weights@n[0];
				poseWeights[@n * 4 + 1] = weights@n[1];
				poseWeights[@n * 4 + 2] = weights@n[2];
				poseWeights[@n * 4 + 3] = weights@n[3];
			@end
			@foreach( hlms_pose, n )
				input.vertex += poseBuf.Load( int((input.vertexId + numVertices * @nu) @property( hlms_pose_normals )<< 1u@end ) ) * poseWeights[@n];
				@property( hlms_pose_normals && (hlms_normal || hlms_qtangent) )
				normal += poseBuf.Load( int(((input.vertexId + numVertices * @nu) << 1u) + 1u) ).xyz * poseWeights[@n];
				@end
			@end
		@end
	@end

	// If hlms_skeleton is defined the transforms will be provided by bones.
	// If hlms_pose is not combined with h lms_skeleton the object's worldMat and worldView have to be set.
	@property( !hlms_skeleton )
		float4 worldMat[3];
		worldMat[0] = worldMatBuf.Load( int(poseDataStart + @value(NumPoseWeightVectors)u + 1u) );
		worldMat[1] = worldMatBuf.Load( int(poseDataStart + @value(NumPoseWeightVectors)u + 2u) );
		worldMat[2] = worldMatBuf.Load( int(poseDataStart + @value(NumPoseWeightVectors)u + 3u) );
		float4 worldPos;
		worldPos.x = dot( worldMat[0], input.vertex );
		worldPos.y = dot( worldMat[1], input.vertex );
		worldPos.z = dot( worldMat[2], input.vertex );
		worldPos.w = 1.0;

		@property( hlms_normal || hlms_qtangent )
		@foreach( 4, n )
			float4 row@n = worldMatBuf.Load( int(poseDataStart + @value(NumPoseWeightVectors)u + 4u + @nu) );@end
		float4x4 worldView = transpose( float4x4( row0, row1, row2, row3 ) );
		@end
	@end
@end @end // PoseTransform

@property( hlms_skeleton )
	@piece( worldViewMat )passBuf.view@end
@end @property( !hlms_skeleton )
    @piece( worldViewMat )worldView@end
@end

@piece( CalculatePsPos )mul( @insertpiece(local_vertex), @insertpiece( worldViewMat ) ).xyz@end

@piece( VertexTransform )
@insertpiece( custom_vs_preTransform )
	//Lighting is in view space
	@property( hlms_normal || hlms_qtangent )outVs.pos		= @insertpiece( CalculatePsPos );@end
	@property( hlms_normal || hlms_qtangent )outVs.normal	= mul( @insertpiece(local_normal), (float3x3)@insertpiece( worldViewMat ) );@end
	@property( normal_map )outVs.tangent	= mul( @insertpiece(local_tangent), (float3x3)@insertpiece( worldViewMat ) );@end
@property( !hlms_dual_paraboloid_mapping )
	outVs.gl_Position = mul( worldPos, passBuf.viewProj );@end
@property( hlms_dual_paraboloid_mapping )
	//Dual Paraboloid Mapping
	outVs.gl_Position.w	= 1.0f;
	@property( hlms_normal || hlms_qtangent )outVs.gl_Position.xyz	= outVs.pos;@end
	@property( !hlms_normal && !hlms_qtangent )outVs.gl_Position.xyz	= @insertpiece( CalculatePsPos );@end
	float L = length( outVs.gl_Position.xyz );
	outVs.gl_Position.z	+= 1.0f;
	outVs.gl_Position.xy	/= outVs.gl_Position.z;
	outVs.gl_Position.z	= (L - NearPlane) / (FarPlane - NearPlane);@end
@end

PS_INPUT main( VS_INPUT input )
{
	PS_INPUT outVs;
	@insertpiece( custom_vs_preExecution )
@property( !hlms_skeleton && !hlms_pose)
	float4x3 worldMat = UNPACK_MAT4x3( worldMatBuf, input.drawId @property( !hlms_shadowcaster )<< 1u@end );
	@property( hlms_normal || hlms_qtangent )
    float4x4 worldView = UNPACK_MAT4( worldMatBuf, (input.drawId << 1u) + 1u );
	@end

	float4 worldPos = float4( mul( input.vertex, worldMat ).xyz, 1.0f );
@end

@property( hlms_qtangent )
	//Decode qTangent to TBN with reflection
	float3 normal	= xAxis( normalize( input.qtangent ) );
	@property( normal_map )
	float3 tangent	= yAxis( input.qtangent );
	outVs.biNormalReflection = sign( input.qtangent.w ); //We ensure in C++ qtangent.w is never 0
	@end
@end @property( !hlms_qtangent && hlms_normal )
	float3 normal	= input.normal;
	@property( normal_map )float3 tangent	= input.tangent;@end
@end

	@insertpiece( PoseTransform )
	@insertpiece( SkeletonTransform )
	@insertpiece( VertexTransform )

	@insertpiece( DoShadowReceiveVS )
	@insertpiece( DoShadowCasterVS )

	/// hlms_uv_count will be 0 on shadow caster passes w/out alpha test
@foreach( hlms_uv_count, n )
	outVs.uv@n = input.uv@n;@end

@property( (!hlms_shadowcaster || alpha_test) && !lower_gpu_overhead )
	outVs.drawId = input.drawId;@end

	@property( hlms_use_prepass_msaa > 1 )
		outVs.zwDepth.xy = outVs.gl_Position.zw;
	@end

@property( hlms_global_clip_planes )
	outVs.gl_ClipDistance0 = dot( float4( worldPos.xyz, 1.0 ), passBuf.clipPlane0.xyzw );
@end

	@insertpiece( custom_vs_posExecution )

	return outVs;
}
