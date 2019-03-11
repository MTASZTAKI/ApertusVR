/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/


#include "ApeReplicaManagerConnection.h"
#include "ApeNodeImpl.h" 
#include "ApeFileGeometryImpl.h"
#include "ApePlaneGeometryImpl.h"
#include "ApeBoxGeometryImpl.h"
#include "ApeCylinderGeometryImpl.h"
#include "ApeTorusGeometryImpl.h"
#include "ApeTubeGeometryImpl.h"
#include "ApeSphereGeometryImpl.h"
#include "ApeConeGeometryImpl.h"
#include "ApeTextGeometryImpl.h"
#include "ApeIndexedFaceSetGeometryImpl.h" 
#include "ApeIndexedLineSetGeometryImpl.h" 
#include "ApeLightImpl.h" 
#include "ApeFileMaterialImpl.h"
#include "ApeManualMaterialImpl.h"
#include "ApePbsPassImpl.h"
#include "ApeManualPassImpl.h"
#include "ApeBrowserImpl.h"
#include "ApeUnitTextureImpl.h"
#include "ApeRayGeometryImpl.h"
#include "ApeSkyImpl.h"
#include "ApeWaterImpl.h"
#include "ApePointCloudImpl.h"
#include "ApeFileTextureImpl.h"

Ape::ReplicaManagerConnection::ReplicaManagerConnection(const RakNet::SystemAddress &_systemAddress, RakNet::RakNetGUID _guid) : Connection_RM3(_systemAddress, _guid)
{
	mpSceneManagerImpl = ((Ape::SceneManagerImpl*)Ape::ISceneManager::getSingletonPtr());
}

Ape::ReplicaManagerConnection::~ReplicaManagerConnection()
{

}

RakNet::Replica3* Ape::ReplicaManagerConnection::AllocReplica(RakNet::BitStream *allocationIdBitstream, RakNet::ReplicaManager3 *replicaManager3)
{
	//TODO_CORE how to guarantee the unqiue node and entity name in a mulitplayer session? This question belongs to all replicas (nodes and entites)? Or the plugins must consider this regulation?
	RakNet::RakString objectType;
	allocationIdBitstream->Read(objectType);
	//APE_LOG_DEBUG("Received: " << objectType);
	if (objectType == "Node")
	{
		RakNet::RakString nodeName;
		allocationIdBitstream->Read(nodeName);
		//APE_LOG_DEBUG("Received name: " << nodeName.C_String() << std::endl;
		if (auto node = mpSceneManagerImpl->createNode(nodeName.C_String()).lock())
			return ((Ape::NodeImpl*)node.get());
	}
	else
	{
		RakNet::RakString entityName;
		allocationIdBitstream->Read(entityName);
		//APE_LOG_DEBUG("Received name: " << entityName.C_String());
		if (objectType == "FileGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_FILE).lock())
				return ((Ape::FileGeometryImpl*)entity.get());
		}
		else if (objectType == "PlaneGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_PLANE).lock())
				return ((Ape::PlaneGeometryImpl*)entity.get());
		}
		else if (objectType == "BoxGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_BOX).lock())
				return ((Ape::BoxGeometryImpl*)entity.get());
		}
		else if (objectType == "ConeGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_CONE).lock())
				return ((Ape::ConeGeometryImpl*)entity.get());
		}
		else if (objectType == "CylinderGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_CYLINDER).lock())
				return ((Ape::CylinderGeometryImpl*)entity.get());
		}
		else if (objectType == "SphereGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_SPHERE).lock())
				return ((Ape::SphereGeometryImpl*)entity.get());
		}
		else if (objectType == "TorusGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_TORUS).lock())
				return ((Ape::TorusGeometryImpl*)entity.get());
		}
		else if (objectType == "TubeGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_TUBE).lock())
				return ((Ape::TubeGeometryImpl*)entity.get());
		}
		else if (objectType == "TextGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_TEXT).lock())
				return ((Ape::TextGeometryImpl*)entity.get());
		}
		else if (objectType == "IndexedFaceSetGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_INDEXEDFACESET).lock())
				return ((Ape::IndexedFaceSetGeometryImpl*)entity.get());
		}
		else if (objectType == "IndexedLineSetGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::GEOMETRY_INDEXEDLINESET).lock())
				return ((Ape::IndexedLineSetGeometryImpl*)entity.get());
		}
		else if (objectType == "FileMaterial")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::MATERIAL_FILE).lock())
				return ((Ape::FileMaterialImpl*)entity.get());
		}
		else if (objectType == "Light")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::LIGHT).lock())
				return ((Ape::LightImpl*)entity.get());
		}
		else if (objectType == "ManualMaterial")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::MATERIAL_MANUAL).lock())
				return ((Ape::ManualMaterialImpl*)entity.get());
		}
		else if (objectType == "PbsPass")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::PASS_PBS).lock())
				return ((Ape::PbsPassImpl*)entity.get());
		}
		else if (objectType == "ManualPass")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::PASS_MANUAL).lock())
				return ((Ape::ManualPassImpl*)entity.get());
		}
		else if (objectType == "Browser")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::BROWSER).lock())
				return ((Ape::BrowserImpl*)entity.get());
		}
		else if (objectType == "UnitTexture")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::TEXTURE_UNIT).lock())
				return ((Ape::UnitTextureImpl*)entity.get());
		}
		else if (objectType == "FileTexture")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::TEXTURE_FILE).lock())
				return ((Ape::FileTextureImpl*)entity.get());
		}
		else if (objectType == "Sky")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::SKY).lock())
				return ((Ape::SkyImpl*)entity.get());
		}
		else if (objectType == "Water")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::WATER).lock())
				return ((Ape::WaterImpl*)entity.get());
		}
		else if (objectType == "PointCloud")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), Ape::Entity::POINT_CLOUD).lock())
				return ((Ape::PointCloudImpl*)entity.get());
		}
	}
	
	return NULL;
}

