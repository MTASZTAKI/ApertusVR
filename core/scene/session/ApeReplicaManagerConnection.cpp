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


#include "ApeReplicaManagerConnection.h"
#include "ApeNodeImpl.h" 
#include "ApeFileGeometryImpl.h"
#include "ApePrimitiveGeometryImpl.h" 
#include "ApeTextGeometryImpl.h" 
#include "ApeLightImpl.h" 
#include "ApeFileMaterialImpl.h"
#include "ApeScenePropertyImpl.h"

Ape::ReplicaManagerConnection::ReplicaManagerConnection(const RakNet::SystemAddress &_systemAddress, RakNet::RakNetGUID _guid) : Connection_RM3(_systemAddress, _guid)
{
	mpSceneImpl = ((Ape::SceneImpl*)Ape::IScene::getSingletonPtr());
}

Ape::ReplicaManagerConnection::~ReplicaManagerConnection()
{

}

RakNet::Replica3* Ape::ReplicaManagerConnection::AllocReplica(RakNet::BitStream *allocationIdBitstream, RakNet::ReplicaManager3 *replicaManager3)
{
	RakNet::RakString objectType;
	allocationIdBitstream->Read(objectType);
	std::cout << "Received: " << objectType << std::endl;
	if (objectType == "SceneProperty")
	{
		if (auto sceneProperty = mpSceneImpl->createProperty().lock())
			return ((Ape::ScenePropertyImpl*)sceneProperty.get());
	}
	else if (objectType == "Node")
	{
		RakNet::RakString nodeName;
		allocationIdBitstream->Read(nodeName);
		if (auto node = mpSceneImpl->createNode(nodeName.C_String()).lock())
			return ((Ape::NodeImpl*)node.get());
	}
	else if (objectType == "FileGeometry")
	{
		RakNet::RakString fileGeometryName;
		allocationIdBitstream->Read(fileGeometryName);
		RakNet::RakString parentNodeName;
		allocationIdBitstream->Read(parentNodeName);
		if (auto fileGeometry = mpSceneImpl->createEntity(fileGeometryName.C_String(), parentNodeName.C_String(), Ape::Entity::GEOMETRY_FILE).lock())
			return ((Ape::FileGeometryImpl*)fileGeometry.get());
	}
	else if (objectType == "PrimitiveGeometry")
	{
		RakNet::RakString primitiveGeomteryName;
		allocationIdBitstream->Read(primitiveGeomteryName);
		RakNet::RakString parentNodeName;
		allocationIdBitstream->Read(parentNodeName);
		if (auto PrimitiveGeometry = mpSceneImpl->createEntity(primitiveGeomteryName.C_String(), parentNodeName.C_String(), Ape::Entity::GEOMETRY_PRIMITVE).lock())
			return ((Ape::PrimitiveGeometryImpl*)PrimitiveGeometry.get());
	}
	else if (objectType == "TextGeometry")
	{
		RakNet::RakString textGeometryName;
		allocationIdBitstream->Read(textGeometryName);
		RakNet::RakString parentNodeName;
		allocationIdBitstream->Read(parentNodeName);
		if (auto textGeometry = mpSceneImpl->createEntity(textGeometryName.C_String(), parentNodeName.C_String(), Ape::Entity::GEOMETRY_TEXT).lock())
			return ((Ape::TextGeometryImpl*)textGeometry.get());
	}
	else if (objectType == "FileMaterial")
	{
		RakNet::RakString fileMaterialName;
		allocationIdBitstream->Read(fileMaterialName);
		RakNet::RakString parentNodeName;
		allocationIdBitstream->Read(parentNodeName);
		if (auto fileMaterial = mpSceneImpl->createEntity(fileMaterialName.C_String(), parentNodeName.C_String(), Ape::Entity::MATERIAL_FILE).lock())
			return ((Ape::FileMaterialImpl*)fileMaterial.get());
	}
	else if (objectType == "Light")
	{
		RakNet::RakString lightName;
		allocationIdBitstream->Read(lightName);
		RakNet::RakString parentNodeName;
		allocationIdBitstream->Read(parentNodeName);
		if (auto light = mpSceneImpl->createEntity(lightName.C_String(), parentNodeName.C_String(), Ape::Entity::LIGHT).lock())
			return ((Ape::LightImpl*)light.get());
	}
	return NULL;
}

