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


#include "apeReplicaManagerConnection.h"
#include "apeNodeImpl.h" 
#include "apeFileGeometryImpl.h"
#include "apePlaneGeometryImpl.h"
#include "apeBoxGeometryImpl.h"
#include "apeCylinderGeometryImpl.h"
#include "apeTorusGeometryImpl.h"
#include "apeTubeGeometryImpl.h"
#include "apeSphereGeometryImpl.h"
#include "apeConeGeometryImpl.h"
#include "apeCloneGeometryImpl.h"
#include "apeTextGeometryImpl.h"
#include "apeIndexedFaceSetGeometryImpl.h" 
#include "apeIndexedLineSetGeometryImpl.h" 
#include "apeLightImpl.h" 
#include "apeFileMaterialImpl.h"
#include "apeManualMaterialImpl.h"
#include "apeBrowserImpl.h"
#include "apeUnitTextureImpl.h"
#include "apeRayGeometryImpl.h"
#include "apeSkyImpl.h"
#include "apeWaterImpl.h"
#include "apePointCloudImpl.h"
#include "apeFileTextureImpl.h"
#include "apeManualTextureImpl.h"

ape::ReplicaManagerConnection::ReplicaManagerConnection(const RakNet::SystemAddress &_systemAddress, RakNet::RakNetGUID _guid) : Connection_RM3(_systemAddress, _guid)
{
	mpSceneManagerImpl = ((ape::SceneManagerImpl*)ape::ISceneManager::getSingletonPtr());
	mConnectionGUID = _guid.ToString();
}

ape::ReplicaManagerConnection::~ReplicaManagerConnection()
{

}

RakNet::Replica3* ape::ReplicaManagerConnection::AllocReplica(RakNet::BitStream *allocationIdBitstream, RakNet::ReplicaManager3 *replicaManager3)
{
	RakNet::RakString objectType;
	allocationIdBitstream->Read(objectType);
	//APE_LOG_DEBUG("Received: " << objectType);
	if (objectType == "Node")
	{
		RakNet::RakString nodeName;
		allocationIdBitstream->Read(nodeName);
		RakNet::RakString ownerID;
		allocationIdBitstream->Read(ownerID);
		//APE_LOG_DEBUG("Received name: " << nodeName.C_String() << std::endl;
		if (auto node = mpSceneManagerImpl->createNode(nodeName.C_String(), true, ownerID.C_String()).lock())
			return ((ape::NodeImpl*)node.get());
	}
	else if (objectType == "ManualTexture")
	{
		RakNet::RakString entityName;
		allocationIdBitstream->Read(entityName);
		RakNet::RakString ownerID;
		allocationIdBitstream->Read(ownerID);
		//APE_LOG_DEBUG("Received name: " << entityName.C_String());
		ape::ManualTextureParameters manualTextureParameters;
		allocationIdBitstream->Read(manualTextureParameters);
		if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::TEXTURE_MANUAL, true, ownerID.C_String()).lock()))
		{
			manualTexture->setParameters(manualTextureParameters.width, manualTextureParameters.height, manualTextureParameters.pixelFormat, manualTextureParameters.usage, manualTextureParameters.gammaCorrection, manualTextureParameters.useFsaa, manualTextureParameters.serializeBuffer);
			return ((ape::ManualTextureImpl*)manualTexture.get());
		}
	}
	else
	{
		RakNet::RakString entityName;
		allocationIdBitstream->Read(entityName);
		RakNet::RakString ownerID;
		allocationIdBitstream->Read(ownerID);
		//APE_LOG_DEBUG("Received name: " << entityName.C_String());
		if (objectType == "FileGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_FILE, true, ownerID.C_String()).lock())
				return ((ape::FileGeometryImpl*)entity.get());
		}
		else if (objectType == "CloneGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_CLONE, true, ownerID.C_String()).lock())
				return ((ape::CloneGeometryImpl*)entity.get());
		}
		else if (objectType == "PlaneGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_PLANE, true, ownerID.C_String()).lock())
				return ((ape::PlaneGeometryImpl*)entity.get());
		}
		else if (objectType == "BoxGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_BOX, true, ownerID.C_String()).lock())
				return ((ape::BoxGeometryImpl*)entity.get());
		}
		else if (objectType == "ConeGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_CONE, true, ownerID.C_String()).lock())
				return ((ape::ConeGeometryImpl*)entity.get());
		}
		else if (objectType == "CylinderGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_CYLINDER, true, ownerID.C_String()).lock())
				return ((ape::CylinderGeometryImpl*)entity.get());
		}
		else if (objectType == "SphereGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_SPHERE, true, ownerID.C_String()).lock())
				return ((ape::SphereGeometryImpl*)entity.get());
		}
		else if (objectType == "TorusGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_TORUS, true, ownerID.C_String()).lock())
				return ((ape::TorusGeometryImpl*)entity.get());
		}
		else if (objectType == "TubeGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_TUBE, true, ownerID.C_String()).lock())
				return ((ape::TubeGeometryImpl*)entity.get());
		}
		else if (objectType == "TextGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_TEXT, true, ownerID.C_String()).lock())
				return ((ape::TextGeometryImpl*)entity.get());
		}
		else if (objectType == "IndexedFaceSetGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_INDEXEDFACESET, true, ownerID.C_String()).lock())
				return ((ape::IndexedFaceSetGeometryImpl*)entity.get());
		}
		else if (objectType == "IndexedLineSetGeometry")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::GEOMETRY_INDEXEDLINESET, true, ownerID.C_String()).lock())
				return ((ape::IndexedLineSetGeometryImpl*)entity.get());
		}
		else if (objectType == "FileMaterial")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::MATERIAL_FILE, true, ownerID.C_String()).lock())
				return ((ape::FileMaterialImpl*)entity.get());
		}
		else if (objectType == "Light")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::LIGHT, true, ownerID.C_String()).lock())
				return ((ape::LightImpl*)entity.get());
		}
		else if (objectType == "ManualMaterial")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::MATERIAL_MANUAL, true, ownerID.C_String()).lock())
				return ((ape::ManualMaterialImpl*)entity.get());
		}
		else if (objectType == "Browser")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::BROWSER, true, ownerID.C_String()).lock())
				return ((ape::BrowserImpl*)entity.get());
		}
		else if (objectType == "FileTexture")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::TEXTURE_FILE, true, ownerID.C_String()).lock())
				return ((ape::FileTextureImpl*)entity.get());
		}
		else if (objectType == "PointCloud")
		{
			if (auto entity = mpSceneManagerImpl->createEntity(entityName.C_String(), ape::Entity::POINT_CLOUD, true, ownerID.C_String()).lock())
				return ((ape::PointCloudImpl*)entity.get());
		}
	}
	
	return NULL;
}

