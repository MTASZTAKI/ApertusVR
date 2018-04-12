/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#include "ApeSceneImpl.h"
#include "ApeNodeImpl.h"
#include "ApeLightImpl.h"
#include "ApeTextGeometryImpl.h"
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
#include "ApeFileMaterialImpl.h"
#include "ApeCameraImpl.h"
#include "ApeManualMaterialImpl.h"
#include "ApePbsPassImpl.h"
#include "ApeManualPassImpl.h"
#include "ApeManualTextureImpl.h"
#include "ApeBrowserImpl.h"
#include "ApeUnitTextureImpl.h"
#include "ApeRayGeometryImpl.h"
#include "ApeSkyImpl.h"
#include "ApeWaterImpl.h"
#include "ApePointCloudImpl.h"

Ape::SceneImpl::SceneImpl()
{
	LOG_FUNC_ENTER();
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpSceneSessionImpl = ((Ape::SceneSessionImpl*)Ape::ISceneSession::getSingletonPtr());
	mReplicaManager = mpSceneSessionImpl->getReplicaManager();
	mNodes = Ape::NodeSharedPtrNameMap();
	mEntities = Ape::EntitySharedPtrNameMap();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	msSingleton = this;
	LOG_FUNC_LEAVE();
}

Ape::SceneImpl::~SceneImpl()
{
	mNodes.clear();
	mEntities.clear();
}

Ape::NodeWeakPtrNameMap Ape::SceneImpl::getNodes()
{
	Ape::NodeWeakPtrNameMap nodes;
	for (auto node : mNodes)
		nodes[node.first] = node.second;
	return nodes;
}

Ape::NodeWeakPtr Ape::SceneImpl::getNode(std::string name)
{
	if (mNodes.find(name) != mNodes.end())
		return mNodes[name];
	else
		return Ape::NodeWeakPtr();	
}

Ape::NodeWeakPtr Ape::SceneImpl::createNode(std::string name)
{
	//LOG_FUNC_ENTER();
	auto node = std::make_shared<Ape::NodeImpl>(name, mpSceneSessionImpl->isHost());
	mNodes.insert(std::make_pair(name, node));
	mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::NODE_CREATE));
	if (auto replicaManager = mReplicaManager.lock())
		replicaManager->Reference(node.get());

	//LOG_FUNC_LEAVE();
	return node;
}

void Ape::SceneImpl::deleteNode(std::string name)
{
	mNodes.erase(name);
	mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::NODE_DELETE));
}

Ape::EntityWeakPtrNameMap Ape::SceneImpl::getEntities()
{
	Ape::EntityWeakPtrNameMap entites;
	for (auto entity : mEntities)
		entites[entity.first] = entity.second;
	return entites;
}

Ape::EntityWeakPtr Ape::SceneImpl::getEntity(std::string name)
{
	if (mEntities.find(name) != mEntities.end())
		return mEntities[name];
	else
		return Ape::EntityWeakPtr();
}

Ape::EntityWeakPtr Ape::SceneImpl::createEntity(std::string name, Ape::Entity::Type type)
{
	LOG_FUNC_ENTER();
	switch (type) 
	{
		case Ape::Entity::LIGHT:
		{
			LOG_TRACE("type: LIGHT");
			auto entity = std::make_shared<Ape::LightImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::LIGHT_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_TEXT:
		{
			LOG_TRACE("type: GEOMETRY_TEXT");
			auto entity = std::make_shared<Ape::TextGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_TEXT_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_FILE:
		{
			LOG_TRACE("type: GEOMETRY_FILE");
			auto entity = std::make_shared<Ape::FileGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_FILE_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_PLANE:
		{
			LOG_TRACE("type: GEOMETRY_PLANE");
			auto entity = std::make_shared<Ape::PlaneGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_PLANE_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_BOX:
		{
			LOG_TRACE("type: GEOMETRY_BOX");
			auto entity = std::make_shared<Ape::BoxGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_BOX_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_CONE:
		{
			LOG_TRACE("type: GEOMETRY_CONE");
			auto entity = std::make_shared<Ape::ConeGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_CONE_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_CYLINDER:
		{
			LOG_TRACE("type: GEOMETRY_CYLINDER");
			auto entity = std::make_shared<Ape::CylinderGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_CYLINDER_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_SPHERE:
		{
			LOG_TRACE("type: GEOMETRY_SPHERE");
			auto entity = std::make_shared<Ape::SphereGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_SPHERE_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_TORUS:
		{
			LOG_TRACE("type: GEOMETRY_TORUS");
			auto entity = std::make_shared<Ape::TorusGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_TORUS_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_TUBE:
		{
			LOG_TRACE("type: GEOMETRY_TUBE");
			auto entity = std::make_shared<Ape::TubeGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_TUBE_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_INDEXEDFACESET:
		{
			LOG_TRACE("type: GEOMETRY_INDEXEDFACESET");
			auto entity = std::make_shared<Ape::IndexedFaceSetGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_INDEXEDFACESET_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::GEOMETRY_INDEXEDLINESET:
		{
			LOG_TRACE("type: GEOMETRY_INDEXEDLINESET");
			auto entity = std::make_shared<Ape::IndexedLineSetGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_INDEXEDLINESET_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::MATERIAL_FILE:
		{
			LOG_TRACE("type: MATERIAL_FILE");
			auto entity = std::make_shared<Ape::FileMaterialImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::MATERIAL_FILE_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::MATERIAL_MANUAL:
		{
			LOG_TRACE("type: MATERIAL_MANUAL");
			auto entity = std::make_shared<Ape::ManualMaterialImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::MATERIAL_MANUAL_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::PASS_PBS:
		{
			LOG_TRACE("type: PASS_PBS");
			auto entity = std::make_shared<Ape::PbsPassImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::PASS_PBS_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::PASS_MANUAL:
		{
			LOG_TRACE("type: PASS_MANUAL");
			auto entity = std::make_shared<Ape::ManualPassImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::PASS_MANUAL_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::TEXTURE_MANUAL:
		{
			LOG_TRACE("type: TEXTURE_MANUAL");
			auto entity = std::make_shared<Ape::ManualTextureImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::TEXTURE_MANUAL_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::POINT_CLOUD:
		{
			LOG_TRACE("type: POINT_CLOUD");
			auto entity = std::make_shared<Ape::PointCloudImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::POINT_CLOUD_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(entity.get());
			return entity;
		}
		case Ape::Entity::CAMERA:
		{
			LOG_TRACE("type: CAMERA");
			auto entity = std::make_shared<Ape::CameraImpl>(name);
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::CAMERA_CREATE));
			return entity;
		}
		case Ape::Entity::BROWSER:
		{
			LOG_TRACE("type: BROWSER");
			auto entity = std::make_shared<Ape::BrowserImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::BROWSER_CREATE));
			return entity;
		}
		case Ape::Entity::TEXTURE_UNIT:
		{
			LOG_TRACE("type: TEXTURE_UNIT");
			auto entity = std::make_shared<Ape::UnitTextureImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::TEXTURE_UNIT_CREATE));
			return entity;
		}
		case Ape::Entity::GEOMETRY_RAY:
		{
			LOG_TRACE("type: GEOMETRY_RAY");
			auto entity = std::make_shared<Ape::RayGeometryImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_RAY_CREATE));
			return entity;
		}
		case Ape::Entity::SKY:
		{
			LOG_TRACE("type: SKY");
			auto entity = std::make_shared<Ape::SkyImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::SKY_CREATE));
			return entity;
		}
		case Ape::Entity::WATER:
		{
			LOG_TRACE("type: WATER");
			auto entity = std::make_shared<Ape::WaterImpl>(name, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, entity));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::WATER_CREATE));
			return entity;
		}
		case Ape::Entity::INVALID:
		{
			LOG_TRACE("type: INVALID");
			return Ape::EntityWeakPtr();
		}
		default:
		{
			LOG_TRACE("type: default");
			return Ape::EntityWeakPtr();
		}
	}
	LOG_FUNC_LEAVE();
}

void Ape::SceneImpl::deleteEntity(std::string name)
{
	Ape::Entity::Type type = mEntities[name]->getType();
	mEntities.erase(name);
	switch (type) 
	{
		case Ape::Entity::LIGHT:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::LIGHT_DELETE));
			break;
		case Ape::Entity::GEOMETRY_TEXT:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_TEXT_DELETE));
			break;
		case Ape::Entity::GEOMETRY_FILE:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_FILE_DELETE));
			break;
		case Ape::Entity::GEOMETRY_PLANE:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_PLANE_DELETE));
			break;
		case Ape::Entity::GEOMETRY_BOX:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_BOX_DELETE));
			break;
		case Ape::Entity::GEOMETRY_CONE:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_CONE_DELETE));
			break;
		case Ape::Entity::GEOMETRY_CYLINDER:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_CYLINDER_DELETE));
			break;
		case Ape::Entity::GEOMETRY_SPHERE:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_SPHERE_DELETE));
			break;
		case Ape::Entity::GEOMETRY_TORUS:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_TORUS_DELETE));
			break;
		case Ape::Entity::GEOMETRY_TUBE:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_TUBE_DELETE));
			break;
		case Ape::Entity::GEOMETRY_INDEXEDFACESET:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_INDEXEDFACESET_DELETE));
			break;
		case Ape::Entity::GEOMETRY_INDEXEDLINESET:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_INDEXEDLINESET_DELETE));
			break;
		case Ape::Entity::MATERIAL_FILE:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::MATERIAL_FILE_DELETE));
			break;
		case Ape::Entity::MATERIAL_MANUAL:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::MATERIAL_MANUAL_DELETE));
			break;
		case Ape::Entity::PASS_PBS:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::PASS_PBS_DELETE));
			break;
		case Ape::Entity::PASS_MANUAL:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::PASS_MANUAL_DELETE));
			break;
		case Ape::Entity::TEXTURE_MANUAL:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::TEXTURE_MANUAL_DELETE));
			break;
		case Ape::Entity::BROWSER:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::BROWSER_DELETE));
			break;
		case Ape::Entity::TEXTURE_UNIT:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::TEXTURE_UNIT_DELETE));
			break;
		case Ape::Entity::GEOMETRY_RAY:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_RAY_DELETE));
			break;
		case Ape::Entity::SKY:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::SKY_DELETE));
			break;
		case Ape::Entity::WATER:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::WATER_DELETE));
			break;
		case Ape::Entity::POINT_CLOUD:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::POINT_CLOUD_DELETE));
			break;
		case Ape::Entity::CAMERA:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::CAMERA_DELETE));
			break;
		case Ape::Entity::INVALID:
			break;
		default:
			break;
	}
}

