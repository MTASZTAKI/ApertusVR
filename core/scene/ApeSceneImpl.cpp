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
#include "ApeFileMaterialImpl.h"
#include "ApeCameraImpl.h"
#include "ApeScenePropertyImpl.h"

template<> Ape::IScene* Ape::Singleton<Ape::IScene>::msSingleton = 0;

Ape::SceneImpl::SceneImpl()
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpSceneSessionImpl = ((Ape::SceneSessionImpl*)Ape::ISceneSession::getSingletonPtr());
	mReplicaManager = mpSceneSessionImpl->getReplicaManager();
	mNodes = Ape::NodeSharedPtrNameMap();
	mEntities = Ape::EntitySharedPtrNameMap();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	if (mpSceneSessionImpl->isHost())
		createProperty();
	msSingleton = this;
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
	auto node = std::make_shared<Ape::NodeImpl>(name, mpSceneSessionImpl->isHost());
	mNodes.insert(std::make_pair(name, node));
	mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::NODE_CREATE));
	if (auto replicaManager = mReplicaManager.lock())
		replicaManager->Reference(node.get());
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

Ape::EntityWeakPtr Ape::SceneImpl::createEntity(std::string name, std::string parentNodeName, Ape::Entity::Type type)
{
	switch (type) 
	{
		case Ape::Entity::LIGHT:
		{
			auto light = std::make_shared<Ape::LightImpl>(name, parentNodeName, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, light));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::LIGHT_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(light.get());
			return light;
		}
		case Ape::Entity::GEOMETRY_TEXT:
		{
			auto geometryText = std::make_shared<Ape::TextGeometryImpl>(name, parentNodeName, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, geometryText));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_TEXT_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(geometryText.get());
			return geometryText;
		}
		case Ape::Entity::GEOMETRY_FILE:
		{
			auto geometryFile = std::make_shared<Ape::FileGeometryImpl>(name, parentNodeName, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, geometryFile));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::GEOMETRY_FILE_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(geometryFile.get());
			return geometryFile;
		}
		case Ape::Entity::MATERIAL_FILE:
		{
			auto materialFile = std::make_shared<Ape::FileMaterialImpl>(name, parentNodeName, mpSceneSessionImpl->isHost());
			mEntities.insert(std::make_pair(name, materialFile));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::MATERIAL_FILE_CREATE));
			if (auto replicaManager = mReplicaManager.lock())
				replicaManager->Reference(materialFile.get());
			return materialFile;
		}
		case Ape::Entity::CAMERA:
		{
			auto camera = std::make_shared<Ape::CameraImpl>(name, parentNodeName);
			mEntities.insert(std::make_pair(name, camera));
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::CAMERA_CREATE));
			return camera;
		}
		case Ape::Entity::INVALID:
			return Ape::EntityWeakPtr();
		default:
			return Ape::EntityWeakPtr();
	}
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
		case Ape::Entity::MATERIAL_FILE:
			mpEventManagerImpl->fireEvent(Ape::Event(name, Ape::Event::Type::MATERIAL_FILE_DELETE));
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

Ape::ScenePropertyWeakPtr Ape::SceneImpl::getProperty()
{
	return mProperty;
}

Ape::ScenePropertyWeakPtr Ape::SceneImpl::createProperty()
{
	mProperty = std::make_shared<Ape::ScenePropertyImpl>("SceneProperty", mpSceneSessionImpl->isHost());
	if (auto replicaManager = mReplicaManager.lock())
		replicaManager->Reference((Ape::ScenePropertyImpl*)mProperty.get());
	return mProperty;
}

