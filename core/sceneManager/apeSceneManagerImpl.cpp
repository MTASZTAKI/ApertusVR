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
#include "apeCloneGeometryImpl.h"
#include "apeFileMaterialImpl.h"
#include "apeCameraImpl.h"
#include "apeManualMaterialImpl.h"
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
	APE_LOG_FUNC_LEAVE();
}

ape::SceneManagerImpl::~SceneManagerImpl()
{
	mNodes.clear();
	mEntities.clear();
	delete (ape::SceneNetworkImpl*)mpSceneNetwork;
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

ape::NodeWeakPtr ape::SceneManagerImpl::createNode(std::string name, bool replicate, std::string ownerID)
{
	APE_LOG_FUNC_ENTER();
	auto node = std::make_shared<ape::NodeImpl>(name, replicate, ownerID, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
	mNodes.insert(std::make_pair(name, node));
	((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::NODE_CREATE));
	if (replicate)
	{
		if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
			replicaManager->Reference(node.get());
	}
	APE_LOG_FUNC_LEAVE();
	return node;
}

void ape::SceneManagerImpl::destroySceneNetwork()
{
	((ape::SceneNetworkImpl*)mpSceneNetwork)->mDestructionBegun = true;
	/*std::this_thread::sleep_for(std::chrono::milliseconds(100));
	((ape::SceneNetworkImpl*)mpSceneNetwork)->leave();*/
	//for (auto mnode : mNodes) {
	//	auto node = std::static_pointer_cast<ape::NodeImpl>(mnode.second);
	//	((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(mnode.first, ape::Event::Type::NODE_DELETE));
	//	if (node->isReplicated())
	//	{
	//		if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
	//			replicaManager->Dereference(node.get());
	//	}
	//}
	//mNodes.clear();
	//mEntities.clear();
	//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	if (((ape::SceneNetworkImpl*)mpSceneNetwork)->mRunReplicaPeerListenThread.joinable())
		((ape::SceneNetworkImpl*)mpSceneNetwork)->mRunReplicaPeerListenThread.join();
	if (((ape::SceneNetworkImpl*)mpSceneNetwork)->mConnectToRoomThread.joinable())
		((ape::SceneNetworkImpl*)mpSceneNetwork)->mConnectToRoomThread.join();
	delete  (ape::SceneNetworkImpl*)mpSceneNetwork;
	
}

void ape::SceneManagerImpl::createSceneNetwork()
{
	mpSceneNetwork = new ape::SceneNetworkImpl();
}

void ape::SceneManagerImpl::deleteNode(std::string name)
{
	auto node = std::static_pointer_cast<ape::NodeImpl>(mNodes[name]);
	((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::NODE_DELETE));
	if (node->isReplicated())
	{
		if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
			replicaManager->Dereference(node.get());
	}
	mNodes.erase(name);
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

ape::EntityWeakPtr ape::SceneManagerImpl::createEntity(std::string name, ape::Entity::Type type, bool replicate, std::string ownerID)
{
	APE_LOG_FUNC_ENTER();
	switch (type) 
	{
		case ape::Entity::LIGHT:
		{
			APE_LOG_TRACE("type: LIGHT");
			auto entity = std::make_shared<ape::LightImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::LIGHT_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_TEXT:
		{
			APE_LOG_TRACE("type: GEOMETRY_TEXT");
			auto entity = std::make_shared<ape::TextGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TEXT_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
                {
                    replicaManager->Reference(entity.get());
                }
					
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_FILE:
		{
			APE_LOG_TRACE("type: GEOMETRY_FILE");
			auto entity = std::make_shared<ape::FileGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_FILE_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_PLANE:
		{
			APE_LOG_TRACE("type: GEOMETRY_PLANE");
			auto entity = std::make_shared<ape::PlaneGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_PLANE_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_BOX:
		{
			APE_LOG_TRACE("type: GEOMETRY_BOX");
			auto entity = std::make_shared<ape::BoxGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_BOX_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_CONE:
		{
			APE_LOG_TRACE("type: GEOMETRY_CONE");
			auto entity = std::make_shared<ape::ConeGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CONE_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_CYLINDER:
		{
			APE_LOG_TRACE("type: GEOMETRY_CYLINDER");
			auto entity = std::make_shared<ape::CylinderGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CYLINDER_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_SPHERE:
		{
			APE_LOG_TRACE("type: GEOMETRY_SPHERE");
			auto entity = std::make_shared<ape::SphereGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_SPHERE_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_TORUS:
		{
			APE_LOG_TRACE("type: GEOMETRY_TORUS");
			auto entity = std::make_shared<ape::TorusGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TORUS_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_TUBE:
		{
			APE_LOG_TRACE("type: GEOMETRY_TUBE");
			auto entity = std::make_shared<ape::TubeGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TUBE_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_INDEXEDFACESET:
		{
			APE_LOG_TRACE("type: GEOMETRY_INDEXEDFACESET");
			auto entity = std::make_shared<ape::IndexedFaceSetGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_INDEXEDFACESET_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_INDEXEDLINESET:
		{
			APE_LOG_TRACE("type: GEOMETRY_INDEXEDLINESET");
			auto entity = std::make_shared<ape::IndexedLineSetGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_INDEXEDLINESET_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::GEOMETRY_CLONE:
		{
			APE_LOG_TRACE("type: GEOMETRY_CLONE");
			auto entity = std::make_shared<ape::CloneGeometryImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CLONE_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::MATERIAL_FILE:
		{
			APE_LOG_TRACE("type: MATERIAL_FILE");
			auto entity = std::make_shared<ape::FileMaterialImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::MATERIAL_FILE_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::MATERIAL_MANUAL:
		{
			APE_LOG_TRACE("type: MATERIAL_MANUAL");
			auto entity = std::make_shared<ape::ManualMaterialImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::MATERIAL_MANUAL_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::TEXTURE_MANUAL:
		{
			APE_LOG_TRACE("type: TEXTURE_MANUAL");
			auto entity = std::make_shared<ape::ManualTextureImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_MANUAL_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::TEXTURE_FILE:
		{
			APE_LOG_TRACE("type: TEXTURE_FILE");
			auto entity = std::make_shared<ape::FileTextureImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_FILE_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::POINT_CLOUD:
		{
			APE_LOG_TRACE("type: POINT_CLOUD");
			auto entity = std::make_shared<ape::PointCloudImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::POINT_CLOUD_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::BROWSER:
		{
			APE_LOG_TRACE("type: BROWSER");
			auto entity = std::make_shared<ape::BrowserImpl>(name, replicate, ownerID,  ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::BROWSER_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
			return entity;
		}
		case ape::Entity::RIGIDBODY:
		{
			APE_LOG_TRACE("type: RIGIDBODY");
			auto entity = std::make_shared<ape::RigidBodyImpl>(name, replicate, ownerID, ((ape::SceneNetworkImpl*)mpSceneNetwork)->isReplicaHost());
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::RIGIDBODY_CREATE));
			if (replicate)
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Reference(entity.get());
			}
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
		case ape::Entity::TEXTURE_UNIT:
		{
			APE_LOG_TRACE("type: TEXTURE_UNIT");
			auto entity = std::make_shared<ape::UnitTextureImpl>(name);
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_UNIT_CREATE));
			return entity;
		}
		case ape::Entity::GEOMETRY_RAY:
		{
			APE_LOG_TRACE("type: GEOMETRY_RAY");
			auto entity = std::make_shared<ape::RayGeometryImpl>(name);
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_RAY_CREATE));
			return entity;
		}
		case ape::Entity::SKY:
		{
			APE_LOG_TRACE("type: SKY");
			auto entity = std::make_shared<ape::SkyImpl>(name);
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::SKY_CREATE));
			return entity;
		}
		case ape::Entity::WATER:
		{
			APE_LOG_TRACE("type: WATER");
			auto entity = std::make_shared<ape::WaterImpl>(name);
			mEntities.insert(std::make_pair(name, entity));
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::WATER_CREATE));
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
	if (mEntities.find(name) == mEntities.end())
		return;
    ape::Entity::Type type = mEntities[name]->getType();
	switch (type) 
	{
		case ape::Entity::LIGHT:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::LIGHT_DELETE));
			auto entity = std::static_pointer_cast<ape::LightImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_TEXT:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TEXT_DELETE));
			auto entity = std::static_pointer_cast<ape::TextGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_FILE:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_FILE_DELETE));
			auto entity = std::static_pointer_cast<ape::FileGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_PLANE:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_PLANE_DELETE));
			auto entity = std::static_pointer_cast<ape::PlaneGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_BOX:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_BOX_DELETE));
			auto entity = std::static_pointer_cast<ape::BoxGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_CONE:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CONE_DELETE));
			auto entity = std::static_pointer_cast<ape::ConeGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_CYLINDER:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CYLINDER_DELETE));
			auto entity = std::static_pointer_cast<ape::CylinderGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_SPHERE:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_SPHERE_DELETE));
			auto entity = std::static_pointer_cast<ape::SphereGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_TORUS:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TORUS_DELETE));
			auto entity = std::static_pointer_cast<ape::TorusGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_TUBE:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_TUBE_DELETE));
			auto entity = std::static_pointer_cast<ape::TubeGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_INDEXEDFACESET:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_INDEXEDFACESET_DELETE));
			auto entity = std::static_pointer_cast<ape::IndexedFaceSetGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_INDEXEDLINESET:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_INDEXEDLINESET_DELETE));
			auto entity = std::static_pointer_cast<ape::IndexedLineSetGeometryImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::GEOMETRY_CLONE:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::GEOMETRY_CLONE_DELETE));
			auto entity = std::static_pointer_cast<ape::CloneGeometryImpl>(mEntities[name]);
			if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
				replicaManager->Dereference(entity.get());
		}
			break;
		case ape::Entity::MATERIAL_FILE:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::MATERIAL_FILE_DELETE));
			auto entity = std::static_pointer_cast<ape::FileMaterialImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::MATERIAL_MANUAL:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::MATERIAL_MANUAL_DELETE));
			auto entity = std::static_pointer_cast<ape::ManualMaterialImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::TEXTURE_MANUAL:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::TEXTURE_MANUAL_DELETE));
			auto entity = std::static_pointer_cast<ape::ManualTextureImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::BROWSER:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::BROWSER_DELETE));
			auto entity = std::static_pointer_cast<ape::BrowserImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
			break;
		case ape::Entity::RIGIDBODY:
		{
			((ape::EventManagerImpl*)mpEventManager)->fireEvent(ape::Event(name, ape::Event::Type::RIGIDBODY_DELETE));
			auto entity = std::static_pointer_cast<ape::BrowserImpl>(mEntities[name]);
			if (entity->isReplicated())
			{
				if (auto replicaManager = ((ape::SceneNetworkImpl*)mpSceneNetwork)->getReplicaManager().lock())
					replicaManager->Dereference(entity.get());
			}
		}
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
		case ape::Entity::INVALID:
			break;
		default:
			break;
	}
    mEntities.erase(name);
}

