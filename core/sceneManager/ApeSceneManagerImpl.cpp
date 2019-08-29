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

#include "apeSceneManagerImpl.h"
#include "apeNodeImpl.h"
#include "apeLightImpl.h"
#include "apeTextGeometryImpl.h"
#include "apeFileGeometryImpl.h"
#include "apePlaneGeometryImpl.h"
#include "apeBoxGeometryImpl.h"
#include "apeCylinderGeometryImpl.h"
#include "apeTorusGeometryImpl.h"
#include "apeTubeGeometryImpl.h"
#include "apeSphereGeometryImpl.h"
#include "apeConeGeometryImpl.h"
#include "apeTextGeometryImpl.h" 
#include "apeIndexedFaceSetGeometryImpl.h" 
#include "apeIndexedLineSetGeometryImpl.h" 
#include "apeFileMaterialImpl.h"
#include "apeCameraImpl.h"
#include "apeManualMaterialImpl.h"
#include "apePbsPassImpl.h"
#include "apeManualPassImpl.h"
#include "apeManualTextureImpl.h"
#include "apeBrowserImpl.h"
#include "apeUnitTextureImpl.h"
#include "apeRayGeometryImpl.h"
#include "apeSkyImpl.h"
#include "apeWaterImpl.h"
#include "apePointCloudImpl.h"
#include "apeFileTextureImpl.h"
#include "apeSceneNetworkImpl.h"
#include "apeEventManagerImpl.h"
#include "apeRigidBodyImpl.h"

ape::SceneManagerImpl::SceneManagerImpl()
{
	APE_LOG_FUNC_ENTER();
	msSingleton = this;
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpSceneNetwork = new ape::SceneNetworkImpl();
	mNodes = ape::NodeSharedPtrNameMap();
	mEntities = ape::EntitySharedPtrNameMap();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mUniqueID = mpCoreConfig->getNetworkConfig().uniqueID;
	APE_LOG_FUNC_LEAVE();
}

ape::SceneManagerImpl::~SceneManagerImpl()
{
	mNodes.clear();
	mEntities.clear();
}

ape::NodeWeakPtrNameMap ape::SceneManagerImpl::getNodes()
{
	ape::NodeWeakPtrNameMap nodes;
	for (auto node : mNodes)
		nodes[node.first] = node.second;
	return nodes;
}

ape::NodeWeakPtr ape::SceneManagerImpl::getNode(std::string name)
{
	if (mNodes.find(name) != mNodes.end())
		return mNodes[name];
	else
		return ape::NodeWeakPtr();	
}

ape::NodeWeakPtr ape::SceneManagerImpl::createNode(std::string name)
{
	APE_LOG_FUNC_ENTER();
	auto node = std::make_shared<ape::NodeImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
	mNodes.insert(std::make_pair(name, node));
	((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::NODE_CREATE));
	if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
		replicaManager->Reference(node.get());
	APE_LOG_FUNC_LEAVE();
	return node;
}

void ape::SceneManagerImpl::deleteNode(std::string name)
{
	mNodes.erase(name);
	((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::NODE_DELETE));
}

ape::EntityWeakPtrNameMap ape::SceneManagerImpl::getEntities()
{
	ape::EntityWeakPtrNameMap entites;
	for (auto entity : mEntities)
		entites[entity.first] = entity.second;
	return entites;
}

ape::EntityWeakPtr ape::SceneManagerImpl::getEntity(std::string name)
{
	if (mEntities.find(name) != mEntities.end())
		return mEntities[name];
	else
		return ape::EntityWeakPtr();
}

ape::EntityWeakPtr ape::SceneManagerImpl::createEntity(std::string name, ape::Entity::Type type)
{
	APE_LOG_FUNC_ENTER();
	switch (type) 
	{
		case ape::Entity::LIGHT:
		{
			APE_LOG_TRACE("type: LIGHT");
			auto entity = std::make_shared<ape::LightImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::LIGHT_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_TEXT:
		{
			APE_LOG_TRACE("type: GEOMETRY_TEXT");
			auto entity = std::make_shared<ape::TextGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TEXT_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_FILE:
		{
			APE_LOG_TRACE("type: GEOMETRY_FILE");
			auto entity = std::make_shared<ape::FileGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_FILE_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_PLANE:
		{
			APE_LOG_TRACE("type: GEOMETRY_PLANE");
			auto entity = std::make_shared<ape::PlaneGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_PLANE_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_BOX:
		{
			APE_LOG_TRACE("type: GEOMETRY_BOX");
			auto entity = std::make_shared<ape::BoxGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_BOX_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_CONE:
		{
			APE_LOG_TRACE("type: GEOMETRY_CONE");
			auto entity = std::make_shared<ape::ConeGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CONE_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_CYLINDER:
		{
			APE_LOG_TRACE("type: GEOMETRY_CYLINDER");
			auto entity = std::make_shared<ape::CylinderGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CYLINDER_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_SPHERE:
		{
			APE_LOG_TRACE("type: GEOMETRY_SPHERE");
			auto entity = std::make_shared<ape::SphereGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_SPHERE_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_TORUS:
		{
			APE_LOG_TRACE("type: GEOMETRY_TORUS");
			auto entity = std::make_shared<ape::TorusGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TORUS_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_TUBE:
		{
			APE_LOG_TRACE("type: GEOMETRY_TUBE");
			auto entity = std::make_shared<ape::TubeGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TUBE_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_INDEXEDFACESET:
		{
			APE_LOG_TRACE("type: GEOMETRY_INDEXEDFACESET");
			auto entity = std::make_shared<ape::IndexedFaceSetGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_INDEXEDFACESET_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::GEOMETRY_INDEXEDLINESET:
		{
			APE_LOG_TRACE("type: GEOMETRY_INDEXEDLINESET");
			auto entity = std::make_shared<ape::IndexedLineSetGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_INDEXEDLINESET_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::MATERIAL_FILE:
		{
			APE_LOG_TRACE("type: MATERIAL_FILE");
			auto entity = std::make_shared<ape::FileMaterialImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::MATERIAL_FILE_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::MATERIAL_MANUAL:
		{
			APE_LOG_TRACE("type: MATERIAL_MANUAL");
			auto entity = std::make_shared<ape::ManualMaterialImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::MATERIAL_MANUAL_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::PASS_PBS:
		{
			APE_LOG_TRACE("type: PASS_PBS");
			auto entity = std::make_shared<ape::PbsPassImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::PASS_PBS_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::PASS_MANUAL:
		{
			APE_LOG_TRACE("type: PASS_MANUAL");
			auto entity = std::make_shared<ape::ManualPassImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::PASS_MANUAL_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::TEXTURE_MANUAL:
		{
			APE_LOG_TRACE("type: TEXTURE_MANUAL");
			auto entity = std::make_shared<ape::ManualTextureImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_MANUAL_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::TEXTURE_FILE:
		{
			APE_LOG_TRACE("type: TEXTURE_FILE");
			auto entity = std::make_shared<ape::FileTextureImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_FILE_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::POINT_CLOUD:
		{
			APE_LOG_TRACE("type: POINT_CLOUD");
			auto entity = std::make_shared<ape::PointCloudImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::POINT_CLOUD_CREATE));
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case ape::Entity::CAMERA:
		{
			APE_LOG_TRACE("type: CAMERA");
			auto entity = std::make_shared<ape::CameraImpl>(name);
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::CAMERA_CREATE));
			return entity;
		}
		case ape::Entity::BROWSER:
		{
			APE_LOG_TRACE("type: BROWSER");
			auto entity = std::make_shared<ape::BrowserImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::BROWSER_CREATE));
			return entity;
		}
		case ape::Entity::TEXTURE_UNIT:
		{
			APE_LOG_TRACE("type: TEXTURE_UNIT");
			auto entity = std::make_shared<ape::UnitTextureImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_UNIT_CREATE));
			return entity;
		}
		case ape::Entity::GEOMETRY_RAY:
		{
			APE_LOG_TRACE("type: GEOMETRY_RAY");
			auto entity = std::make_shared<ape::RayGeometryImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_RAY_CREATE));
			return entity;
		}
		case ape::Entity::SKY:
		{
			APE_LOG_TRACE("type: SKY");
			auto entity = std::make_shared<ape::SkyImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::SKY_CREATE));
			return entity;
		}
		case ape::Entity::WATER:
		{
			APE_LOG_TRACE("type: WATER");
			auto entity = std::make_shared<ape::WaterImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::WATER_CREATE));
			return entity;
		}
		case ape::Entity::RIGIDBODY:
		{
			APE_LOG_TRACE("type: RIGIDBODY");
			auto entity = std::make_shared<ape::RigidBodyImpl>(name, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::RIGIDBODY_CREATE));
			return entity;
		}
		case ape::Entity::INVALID:
		{
			APE_LOG_TRACE("type: INVALID");
			return ape::EntityWeakPtr();
		}
		default:
		{
			APE_LOG_TRACE("type: default");
			return ape::EntityWeakPtr();
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::SceneManagerImpl::deleteEntity(std::string name)
{
	ape::Entity::Type type = mEntities[name]->getType();
	mEntities.erase(name);
	switch (type) 
	{
		case ape::Entity::LIGHT:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::LIGHT_DELETE));
			break;
		case ape::Entity::GEOMETRY_TEXT:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TEXT_DELETE));
			break;
		case ape::Entity::GEOMETRY_FILE:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_FILE_DELETE));
			break;
		case ape::Entity::GEOMETRY_PLANE:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_PLANE_DELETE));
			break;
		case ape::Entity::GEOMETRY_BOX:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_BOX_DELETE));
			break;
		case ape::Entity::GEOMETRY_CONE:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CONE_DELETE));
			break;
		case ape::Entity::GEOMETRY_CYLINDER:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CYLINDER_DELETE));
			break;
		case ape::Entity::GEOMETRY_SPHERE:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_SPHERE_DELETE));
			break;
		case ape::Entity::GEOMETRY_TORUS:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TORUS_DELETE));
			break;
		case ape::Entity::GEOMETRY_TUBE:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TUBE_DELETE));
			break;
		case ape::Entity::GEOMETRY_INDEXEDFACESET:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_INDEXEDFACESET_DELETE));
			break;
		case ape::Entity::GEOMETRY_INDEXEDLINESET:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_INDEXEDLINESET_DELETE));
			break;
		case ape::Entity::MATERIAL_FILE:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::MATERIAL_FILE_DELETE));
			break;
		case ape::Entity::MATERIAL_MANUAL:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::MATERIAL_MANUAL_DELETE));
			break;
		case ape::Entity::PASS_PBS:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::PASS_PBS_DELETE));
			break;
		case ape::Entity::PASS_MANUAL:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::PASS_MANUAL_DELETE));
			break;
		case ape::Entity::TEXTURE_MANUAL:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_MANUAL_DELETE));
			break;
		case ape::Entity::BROWSER:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::BROWSER_DELETE));
			break;
		case ape::Entity::TEXTURE_UNIT:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_UNIT_DELETE));
			break;
		case ape::Entity::GEOMETRY_RAY:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_RAY_DELETE));
			break;
		case ape::Entity::SKY:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::SKY_DELETE));
			break;
		case ape::Entity::WATER:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::WATER_DELETE));
			break;
		case ape::Entity::POINT_CLOUD:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::POINT_CLOUD_DELETE));
			break;
		case ape::Entity::CAMERA:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::CAMERA_DELETE));
			break;
		case ape::Entity::RIGIDBODY:
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::RIGIDBODY_DELETE));
			break;
		case ape::Entity::INVALID:
			break;
		default:
			break;
	}
}

