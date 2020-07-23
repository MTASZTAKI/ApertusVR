#include "Ogre_glTF_modelConverter.hpp"
#include "Ogre_glTF_common.hpp"
#include <OgreMesh2.h>
#include <OgreMeshManager2.h>
#include <OgreSubMesh2.h>
#include "Ogre_glTF_internal_utils.hpp"

using namespace Ogre_glTF;

size_t vertexBufferPart::getPartStride() const { return buffer->elementSize() * perVertex; }

modelConverter::modelConverter(tinygltf::Model& input) : model { input } {}

Ogre::VertexBufferPackedVec modelConverter::constructVertexBuffer(const std::vector<vertexBufferPart>& parts) const
{
	Ogre::VertexElement2Vec vertexElements;

	size_t stride { 0 }, strideInElements { 0 };
	size_t vertexCount { 0 }, previousVertexCount { 0 };

	for(const auto& part : parts)
	{
		vertexElements.emplace_back(part.type, part.semantic);
		strideInElements += part.perVertex;
		stride += part.buffer->elementSize() * part.perVertex;
		vertexCount = part.vertexCount;

		//Sanity check
		if(previousVertexCount != 0)
		{
			if(vertexCount != previousVertexCount) throw LoadingError("Part of vertex buffer for the same primitive have different vertex counts!");
		}
		else
			previousVertexCount = vertexCount;
	}

	OgreLog("There will be " + std::to_string(vertexCount) + " vertices with a stride of " + std::to_string(stride) + " bytes");

	geometryBuffer<float> finalBuffer(vertexCount * strideInElements);
	size_t bytesWrittenInCurrentStride { 0 };
	for(size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
	{
		bytesWrittenInCurrentStride = 0;
		for(const auto& part : parts)
		{
			memcpy(finalBuffer.dataAddress() + (bytesWrittenInCurrentStride + vertexIndex * stride),
				   (part.buffer->dataAddress() + (vertexIndex * part.getPartStride())),
				   part.getPartStride());
			bytesWrittenInCurrentStride += part.getPartStride();
		}
	}

	Ogre::VertexBufferPackedVec vec;
	auto vertexBuffer = getVaoManager()->createVertexBuffer(vertexElements, vertexCount, Ogre::BT_IMMUTABLE, finalBuffer.data(), false);

	vec.push_back(vertexBuffer);
	return vec;
}

Ogre::MeshPtr modelConverter::getOgreMesh(const Ogre::String& name)
{
	if(name.empty())
	{
		if(!model.meshes.empty()) {
			return getOgreMesh(0);
		}
		else
		{
			return Ogre::MeshPtr();
		}
	}

	for(size_t meshIdx = 0; meshIdx < model.meshes.size(); ++meshIdx) {
		const auto& mesh = model.meshes[meshIdx];
		if(!mesh.name.empty() && mesh.name == name) {
			return getOgreMesh(meshIdx);
		}
	}
	
	return Ogre::MeshPtr();
}

Ogre::MeshPtr modelConverter::getOgreMesh(size_t meshIdx)
{
	Ogre::Aabb boundingBox;
	auto& mesh = model.meshes[meshIdx];
	OgreLog("Found mesh " + mesh.name + " in glTF file");

	auto ogreMesh = Ogre::MeshManager::getSingleton().getByName(mesh.name);
	if(ogreMesh)
	{
		OgreLog("Found mesh " + mesh.name + " in Ogre::MeshManager(v2)");
		return ogreMesh;
	}

	OgreLog("Loading mesh from glTF file");
	OgreLog("mesh has " + std::to_string(mesh.primitives.size()) + " primitives");
	ogreMesh = Ogre::MeshManager::getSingleton().createManual(mesh.name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	OgreLog("Created mesh on v2 MeshManager");

	for(const auto& primitive : mesh.primitives)
	{
		auto subMesh = ogreMesh->createSubMesh();
		OgreLog("Created one submesh");
		const auto indexBuffer = extractIndexBuffer(primitive.indices);

		std::vector<vertexBufferPart> parts;
		//OgreLog("\tprimitive has : " + std::to_string(primitive.attributes.size()) + " atributes");
		for(const auto& atribute : primitive.attributes)
		{
			//OgreLog("\t " + atribute.first);
			parts.push_back(std::move(extractVertexBuffer(atribute, boundingBox)));
		}

		//Get (if they exist) the blend weights and bone index parts of our vertex array object content
		const auto blendIndicesIt = std::find_if(std::begin(parts), std::end(parts), [](const vertexBufferPart& vertexBufferPart) {
			return (vertexBufferPart.semantic == Ogre::VertexElementSemantic::VES_BLEND_INDICES);
		});

		const auto blendWeightsIt = std::find_if(std::begin(parts), std::end(parts), [](const vertexBufferPart& vertexBufferPart) {
			return (vertexBufferPart.semantic == Ogre::VertexElementSemantic::VES_BLEND_WEIGHTS);
		});

		const auto vertexBuffers = constructVertexBuffer(parts);
		auto vao				 = getVaoManager()->createVertexArrayObject(vertexBuffers, indexBuffer, [&]() -> Ogre::OperationType {
			switch(primitive.mode)
			{
				case TINYGLTF_MODE_LINE: OgreLog("Line List"); return Ogre::OT_LINE_LIST;
				case TINYGLTF_MODE_LINE_LOOP: OgreLog("Line Loop"); return Ogre::OT_LINE_STRIP;
				case TINYGLTF_MODE_POINTS: OgreLog("Points"); return Ogre::OT_POINT_LIST;
				case TINYGLTF_MODE_TRIANGLES: OgreLog("Triangle List"); return Ogre::OT_TRIANGLE_LIST;
				case TINYGLTF_MODE_TRIANGLE_FAN: OgreLog("Trinagle Fan"); return Ogre::OT_TRIANGLE_FAN;
				case TINYGLTF_MODE_TRIANGLE_STRIP: OgreLog("Triangle Strip"); return Ogre::OT_TRIANGLE_STRIP;
				default: OgreLog("Unknown"); throw LoadingError("Can't understand primitive mode!");
			};
		}());

		subMesh->mVao[Ogre::VpNormal].push_back(vao);
		subMesh->mVao[Ogre::VpShadow].push_back(vao);

		if(blendIndicesIt != std::end(parts) && blendWeightsIt != std::end(parts))
		{
			//subMesh->_buildBoneAssignmentsFromVertexData();

			//Get the vertexBufferParts from the two iterators
			//OgreLog("The vertex buffer contains blend weights and indices information!");
			vertexBufferPart& blendIndices = *blendIndicesIt;
			vertexBufferPart& blendWeights = *blendWeightsIt;

			//Debug sanity check, both should be equals
			//OgreLog("Vertex count blendIndex : " + std::to_string(blendIndices.vertexCount));
			//OgreLog("Vertex count blendWeight: " + std::to_string(blendWeights.vertexCount));
			//OgreLog("Vertex element count blendIndex : " + std::to_string(blendIndices.perVertex));
			//OgreLog("Vertex element count blendWeight: " + std::to_string(blendWeights.perVertex));

			//Allocate 2 small arrays to store the bone idexes. (They should be of lenght "4")
			std::vector<Ogre::ushort> vertexBoneIndex(blendIndices.perVertex);
			std::vector<Ogre::Real> vertexBlend(blendWeights.perVertex);

			//Add the attahcments for each bones
			for(Ogre::uint32 vertexIndex = 0; vertexIndex < blendIndices.vertexCount; ++vertexIndex)
			{
				//Fetch the for bone indexes from the buffer
				memcpy(vertexBoneIndex.data(),
					   blendIndices.buffer->dataAddress() + (blendIndices.getPartStride() * vertexIndex),
					   blendIndices.perVertex * sizeof(Ogre::ushort));

				//Fetch the for weights from the buffer
				memcpy(vertexBlend.data(),
					   blendWeights.buffer->dataAddress() + (blendWeights.getPartStride() * vertexIndex),
					   blendWeights.perVertex * sizeof(Ogre::Real));

				//Add the bone assignments to the submesh
				for(size_t i = 0; i < blendIndices.perVertex; ++i)
				{
					auto vba = Ogre::VertexBoneAssignment(vertexIndex, vertexBoneIndex[i], vertexBlend[i]);

					//OgreLog("VertexBoneAssignment: " + std::to_string(i) + " over " + std::to_string(blendIndices.perVertex));
					//OgreLog(std::to_string(vba.vertexIndex));
					//OgreLog(std::to_string(vba.boneIndex));
					//OgreLog(std::to_string(vba.weight));

					subMesh->addBoneAssignment(vba);
				}
			}

			//subMesh->_buildBoneIndexMap();
			subMesh->_compileBoneAssignments();
		}
	}

	ogreMesh->_setBounds(boundingBox, true);
	//OgreLog("Setting 'bounding sphere radius' from bounds : " + std::to_string(boundingBox.getRadius()));

	return ogreMesh;
}

void modelConverter::debugDump() const
{
	std::stringstream gltfContentDump;
	gltfContentDump << "This glTF model has:\n"
					<< model.accessors.size() << " accessors\n"
					<< model.animations.size() << " animations\n"
					<< model.buffers.size() << " buffers\n"
					<< model.bufferViews.size() << " bufferViews\n"
					<< model.materials.size() << " materials\n"
					<< model.meshes.size() << " meshes\n"
					<< model.nodes.size() << " nodes\n"
					<< model.textures.size() << " textures\n"
					<< model.images.size() << " images\n"
					<< model.skins.size() << " skins\n"
					<< model.samplers.size() << " samplers\n"
					<< model.cameras.size() << " cameras\n"
					<< model.scenes.size() << " scenes\n"
					<< model.lights.size() << " lights\n";

	OgreLog(gltfContentDump);
}

bool modelConverter::hasSkins() const { return !model.skins.empty(); }

Ogre::VaoManager* modelConverter::getVaoManager()
{
	//Our class shouldn't be able to exist if Ogre hasn't been initalized with a valid render system. This call should allways succeed.
	return Ogre::Root::getSingletonPtr()->getRenderSystem()->getVaoManager();
}

Ogre::IndexBufferPacked* modelConverter::extractIndexBuffer(int accessorID) const
{
	OgreLog("Extracting index buffer");
	const auto& accessor   = model.accessors[accessorID];
	const auto& bufferView = model.bufferViews[accessor.bufferView];
	auto& buffer		   = model.buffers[bufferView.buffer];
	const auto byteStride  = accessor.ByteStride(bufferView);
	const auto indexCount  = accessor.count;
	Ogre::IndexBufferPacked::IndexType type;

	if(byteStride < 0) throw LoadingError("Can't get valid bytestride from accessor and bufferview. Loading data not possible");

	auto convertTo16Bit { false };
	switch(accessor.componentType)
	{
		default: throw LoadingError("Unrecognized index data format");
		case TINYGLTF_COMPONENT_TYPE_BYTE:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: convertTo16Bit = true;
		case TINYGLTF_COMPONENT_TYPE_SHORT:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		{
			type			= Ogre::IndexBufferPacked::IT_16BIT;
			auto geomBuffer = geometryBuffer<Ogre::uint16>(indexCount);
			if(convertTo16Bit)
				loadIndexBuffer(geomBuffer.data(), buffer.data.data(), indexCount, bufferView.byteOffset + accessor.byteOffset, byteStride);
			else
				loadIndexBuffer(geomBuffer.data(),
								reinterpret_cast<Ogre::uint16*>(buffer.data.data()),
								indexCount,
								bufferView.byteOffset + accessor.byteOffset,
								byteStride);
			return getVaoManager()->createIndexBuffer(type, indexCount, Ogre::BT_IMMUTABLE, geomBuffer.dataAddress(), false);
		}
		case TINYGLTF_COMPONENT_TYPE_INT:;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		{
			type			= Ogre::IndexBufferPacked::IT_32BIT;
			auto geomBuffer = geometryBuffer<Ogre::uint32>(indexCount);
			loadIndexBuffer(
				geomBuffer.data(), reinterpret_cast<Ogre::uint32*>(buffer.data.data()), indexCount, bufferView.byteOffset + accessor.byteOffset, byteStride);
			return getVaoManager()->createIndexBuffer(type, indexCount, Ogre::BT_IMMUTABLE, geomBuffer.dataAddress(), false);
		}
	}
}

size_t modelConverter::getVertexBufferElementsPerVertexCount(int type)
{
	switch(type)
	{
		case TINYGLTF_TYPE_VEC2: return 2;
		case TINYGLTF_TYPE_VEC3: return 3;
		case TINYGLTF_TYPE_VEC4: return 4;
		default: return 0;
	}
}

Ogre::VertexElementSemantic modelConverter::getVertexElementScemantic(const std::string& type)
{
	if(type == "POSITION") return Ogre::VES_POSITION;
	if(type == "NORMAL") return Ogre::VES_NORMAL;
	if(type == "TANGENT") return Ogre::VES_TANGENT;
	if(type == "TEXCOORD_0") return Ogre::VES_TEXTURE_COORDINATES;
	if(type == "TEXCOORD_1") return Ogre::VES_TEXTURE_COORDINATES;
	if(type == "COLOR_0") return Ogre::VES_DIFFUSE;
	if(type == "JOINTS_0") return Ogre::VES_BLEND_INDICES;
	if(type == "WEIGHTS_0") return Ogre::VES_BLEND_WEIGHTS;
	return Ogre::VES_COUNT; //Returning this means returning "invalid" here
}

vertexBufferPart modelConverter::extractVertexBuffer(const std::pair<std::string, int>& attribute, Ogre::Aabb& boundingBox) const
{
	const auto elementScemantic			= getVertexElementScemantic(attribute.first);
	const auto& accessor				= model.accessors[attribute.second];
	const auto& bufferView				= model.bufferViews[accessor.bufferView];
	const auto& buffer					= model.buffers[bufferView.buffer];
	const auto vertexBufferByteLen		= bufferView.byteLength;
	const auto numberOfElementPerVertex = getVertexBufferElementsPerVertexCount(accessor.type);
	const auto elementOffsetInBuffer	= bufferView.byteOffset + accessor.byteOffset;
	size_t bufferLenghtInBufferBasicType { 0 };

	std::unique_ptr<geometryBuffer_base> geomBuffer { nullptr };
	Ogre::VertexElementType elementType {};

	switch(accessor.componentType)
	{
		case TINYGLTF_COMPONENT_TYPE_DOUBLE: throw LoadingError("Double precision not implemented!");
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			bufferLenghtInBufferBasicType = (vertexBufferByteLen / sizeof(float));
			geomBuffer					  = std::make_unique<geometryBuffer<float>>(bufferLenghtInBufferBasicType);
			if(numberOfElementPerVertex == 2) elementType = Ogre::VET_FLOAT2;
			if(numberOfElementPerVertex == 3) elementType = Ogre::VET_FLOAT3;
			if(numberOfElementPerVertex == 4) elementType = Ogre::VET_FLOAT4;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			bufferLenghtInBufferBasicType = (vertexBufferByteLen / sizeof(unsigned short));
			geomBuffer					  = std::make_unique<geometryBuffer<unsigned short>>(bufferLenghtInBufferBasicType);
			if(numberOfElementPerVertex == 2) elementType = Ogre::VET_USHORT2;
			if(numberOfElementPerVertex == 4) elementType = Ogre::VET_USHORT4;
			break;
		default: throw LoadingError("Unrecognized vertex buffer coponent type");
	}

	//if(bufferView.byteStride == 0)
	//	OgreLog("Vertex buffer is 'tightly packed'");

	const auto byteStride				  = accessor.ByteStride(bufferView);
	const auto vertexCount				  = accessor.count;
	const auto vertexElementLenghtInBytes = numberOfElementPerVertex * geomBuffer->elementSize();

	if(byteStride < 0) throw LoadingError("Can't get valid bytestride from accessor and bufferview. Loading data not possible");

	//OgreLog("A vertex element on this buffer is " + std::to_string(vertexElementLenghtInBytes) + " bytes long");
	for(size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
	{
		const auto destOffset   = vertexIndex * vertexElementLenghtInBytes;
		const auto sourceOffset = elementOffsetInBuffer + vertexIndex * byteStride;

		memcpy((geomBuffer->dataAddress() + destOffset), (buffer.data.data() + sourceOffset), vertexElementLenghtInBytes);
	}

	//Update the bounding sizes once, when vertex positions has been read.
	if(elementScemantic == Ogre::VES_POSITION)
	{
		//Convert to float and load into Ogre::Vector3 objects
		std::array<Ogre::Real, 3> floatVector {};
		internal_utils::container_double_to_real(accessor.minValues, floatVector);
		const Ogre::Vector3 minBounds { floatVector.data() };
		internal_utils::container_double_to_real(accessor.maxValues, floatVector);
		const Ogre::Vector3 maxBounds { floatVector.data() };

		OgreLog("Updating bounding box size: ");
		OgreLog("Setting Min size: " + std::to_string(minBounds.x) + " " + std::to_string(minBounds.y) + " " + std::to_string(minBounds.z));
		OgreLog("Setting Max size: " + std::to_string(maxBounds.x) + " " + std::to_string(maxBounds.y) + " " + std::to_string(maxBounds.z));
		boundingBox.merge(Ogre::Aabb::newFromExtents(minBounds, maxBounds));
	}

	//geometryBuffer->_debugContentToLog();
	return { std::move(geomBuffer), elementType, elementScemantic, vertexCount, numberOfElementPerVertex };
}
