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

#ifndef APE_JSBIND_MANAGER_H
#define APE_JSBIND_MANAGER_H

#include <iostream>
#include <map>
#include <string>
#include "apeIEventManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeUserInputMacro.h"
#include "apeBoxGeometryJsBind.h"
#include "apeFileGeometryJsBind.h"
#include "apeIndexedLineSetGeometryJsBind.h"
#include "apeJsBindColor.h"
#include "apeJsBindDegree.h"
#include "apeJsBindEuler.h"
#include "apeJsBindIndexedFaceSetGeometryImpl.h"
#include "ApeJsBindRigidBodyImpl.h"
#include "ApeJsBindCloneGeometryImpl.h"
#include "apeJsBindLightImpl.h"
#include "apeJsBindMatrix4.h"
#include "apeJsBindNodeImpl.h"
#include "apeJsBindQuaternion.h"
#include "apeJsBindRadian.h"
#include "apeJsBindTextGeometryImpl.h"
#include "apeJsBindVector3.h"
#include "apeJsBindVector2.h"
#include "apeManualMaterialJsBind.h"
#include "apePointCloudJsBind.h"
#include "ApeFileTextureJsBind.h"
#include "apePlaneGeometryJsBind.h"
#include "nbind/nbind.h"
#include "nbind/api.h"

#ifdef NBIND_CLASS

class JsBindManager
{
	enum ErrorType
	{
		DYN_CAST_FAILED,
		NULLPTR
	};

	std::map<ErrorType, std::string> mErrorMap;

public:
	JsBindManager()
	{
		APE_LOG_FUNC_ENTER();
		//std::cout << "JsBindManager: " << std::endl;
		mpSceneManager = ape::ISceneManager::getSingletonPtr();
		mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
		mpEventManager = ape::IEventManager::getSingletonPtr();
		mErrorMap.insert(std::pair<ErrorType, std::string>(DYN_CAST_FAILED, "Dynamic cast failed!"));
		mErrorMap.insert(std::pair<ErrorType, std::string>(NULLPTR, "Return value is nullptr!"));
		APE_LOG_FUNC_LEAVE();
	}

	/*void start(std::string configFolderPath)
	{
		APE_LOG_FUNC_ENTER();
		ape::System::Start(configFolderPath.c_str(), true);
		APE_LOG_FUNC_LEAVE();
	}

	void stop()
	{
		APE_LOG_FUNC_ENTER();
		ape::System::Stop();
		APE_LOG_FUNC_LEAVE();
	}*/

	NodeJsPtr createNode(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		return NodeJsPtr(mpSceneManager->createNode(name));
	}

	void getNodes(nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		auto nodes = mpSceneManager->getNodes();
		std::vector<NodeJsPtr> nodeJsPtrVec;
		for (auto node : nodes)
		{
			nodeJsPtrVec.push_back(NodeJsPtr(node.second));
		}
		done(false, nodeJsPtrVec);
		APE_LOG_FUNC_LEAVE();
	}

	

	void getNodesNames(nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		auto nodes = mpSceneManager->getNodes();
		std::vector<std::string> nodeNameVec;
		for (auto node : nodes)
		{
			nodeNameVec.push_back(node.second.lock()->getName());
		}
		done(false, nodeNameVec);
		APE_LOG_FUNC_LEAVE();
	}

	bool getNode(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto nodeWeakPtr = mpSceneManager->getNode(name);
		if (auto nodeSP = nodeWeakPtr.lock())
		{
			if (auto node = std::dynamic_pointer_cast<ape::INode>(nodeSP))
			{
				success = true;
				done(!success, NodeJsPtr(nodeWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	bool getUserNode(nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto nodeWeakPtr = ape::UserInputMacro::getSingletonPtr()->getUserNode();
		if (auto node = nodeWeakPtr.lock())
		{
			//APE_LOG_DEBUG("getUserNode: " << node->getName());
			success = true;
			done(!success, NodeJsPtr(nodeWeakPtr));
		}
		else
		{
			//APE_LOG_DEBUG("getUserNode: false");
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	void getOtherUserNodeNames(nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		auto nodes = mpSceneManager->getNodes();
		std::vector<std::string> otherUserNodeNames;
		for (auto node : nodes)
		{
			if (auto nodeSP = node.second.lock())
			{
				std::string nodeName = nodeSP->getName();
				std::size_t pos = nodeName.find("_HeadNode");
				if (pos != std::string::npos)
				{
					if (auto userNode = nodeSP->getParentNode().lock())
					{
						if (userNode->getName() != ape::UserInputMacro::getSingletonPtr()->getUserNode().lock()->getName())
						{
							APE_LOG_DEBUG("getOtherUserNodeNames: " << userNode->getName());
							otherUserNodeNames.push_back(userNode->getName());
						}
					}
				}
			}
		}
		done(false, otherUserNodeNames);
		APE_LOG_FUNC_LEAVE();
	}

	LightJsPtr createLight(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return LightJsPtr(mpSceneManager->createEntity(name, ape::Entity::LIGHT));
	}

	bool getLight(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto textGeometry = std::dynamic_pointer_cast<ape::ILight>(entity))
			{
				success = true;
				done(!success, LightJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	TextJsPtr createText(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return TextJsPtr(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_TEXT));
	}

	bool getText(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto textGeometry = std::dynamic_pointer_cast<ape::ITextGeometry>(entity))
			{
				success = true;
				done(!success, TextJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	IndexedFaceSetJsPtr createIndexedFaceSet(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		/*printf("\nCREATING INDEXED FACE SET GEOMETRY!\n");*/
		APE_LOG_FUNC_LEAVE();
		return IndexedFaceSetJsPtr(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_INDEXEDFACESET));
	}

	bool getIndexedFaceSet(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
 		/*printf("\nname: %s\n",name.c_str());*/
		if (auto entity = entityWeakPtr.lock())
		{
			/*printf("\nauto entity = entityWeakPtr.lock()\n");*/
			if (auto indexedFaceSet = std::dynamic_pointer_cast<ape::IIndexedFaceSetGeometry>(entity))
			{
				success = true;
				/*printf("\nsuccess = true\n");*/
				done(!success, IndexedFaceSetJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	CloneGeometryJsPtr createCloneGeometry(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return CloneGeometryJsPtr(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_CLONE));
	}

	bool getCloneGeometry(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto indexedFaceSet = std::dynamic_pointer_cast<ape::IIndexedFaceSetGeometry>(entity))
			{
				success = true;
				done(!success, CloneGeometryJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	RigidBodyJsPtr createRigidBody(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return RigidBodyJsPtr(mpSceneManager->createEntity(name, ape::Entity::RIGIDBODY));
	}

	bool getRigidBody(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto rigidBody = std::dynamic_pointer_cast<ape::IRigidBody>(entity))
			{
				success = true;
				done(!success, RigidBodyJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	IndexedLineSetJsPtr createIndexedLineSet(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return IndexedLineSetJsPtr(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_INDEXEDLINESET));
	}

	bool getIndexedLineSet(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto indexedLineSet = std::dynamic_pointer_cast<ape::IIndexedLineSetGeometry>(entity))
			{
				success = true;
				done(!success, IndexedLineSetJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	BoxJsPtr createBox(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return BoxJsPtr(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_BOX));
	}

	bool getBox(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto box = std::dynamic_pointer_cast<ape::IBoxGeometry>(entity))
			{
				success = true;
				done(!success, BoxJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	PlaneJsPtr createPlane(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return PlaneJsPtr(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_PLANE));
	}

	bool getPlane(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto plane = std::dynamic_pointer_cast<ape::IPlaneGeometry>(entity))
			{
				success = true;
				done(!success, PlaneJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	FileGeometryJsPtr createFileGeometry(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return FileGeometryJsPtr(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_FILE));
	}

	bool getFileGeometry(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto box = std::dynamic_pointer_cast<ape::IFileGeometry>(entity))
			{
				success = true;
				done(!success, FileGeometryJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	FileTextureJsPtr createFileTexture(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return FileTextureJsPtr(mpSceneManager->createEntity(name, ape::Entity::TEXTURE_FILE));
	}

	bool getFileTexture(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto fileTexture = std::dynamic_pointer_cast<ape::IFileTexture>(entity))
			{
				success = true;
				done(!success, FileTextureJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	ManualMaterialJsPtr createManualMaterial(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return ManualMaterialJsPtr(mpSceneManager->createEntity(name, ape::Entity::MATERIAL_MANUAL));
	}

	bool getManualMaterial(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto manualMaterial = std::dynamic_pointer_cast<ape::IManualMaterial>(entity))
			{
				success = true;
				done(!success, ManualMaterialJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	PointCloudJsPtr createPointCloud(std::string name)
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return PointCloudJsPtr(mpSceneManager->createEntity(name, ape::Entity::POINT_CLOUD));
	}

	bool getPointCloud(std::string name, nbind::cbFunction &done)
	{
		APE_LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpSceneManager->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto PointCloud = std::dynamic_pointer_cast<ape::IPointCloud>(entity))
			{
				success = true;
				done(!success, PointCloudJsPtr(entityWeakPtr));
			}
			else
			{
				success = false;
				done(!success, mErrorMap[ErrorType::DYN_CAST_FAILED]);
			}
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		APE_LOG_FUNC_LEAVE();
		return success;
	}

	std::string getFolderPath()
	{
		APE_LOG_FUNC_ENTER();
		APE_LOG_FUNC_LEAVE();
		return mpCoreConfig->getConfigFolderPath();
	}

private:
	ape::ISceneManager* mpSceneManager;
	ape::ICoreConfig* mpCoreConfig;
	ape::IEventManager* mpEventManager;
	std::map<int, nbind::cbFunction*> mEventMap;
};

NBIND_CLASS(JsBindManager)
{
	construct<>();

	/*method(start);
	method(stop);*/

	method(createNode);
	method(getNodesNames);
	method(getNodes);
	method(getNode);
	method(getUserNode);
	method(getOtherUserNodeNames);

	method(createLight);
	method(getLight);

	method(createText);
	method(getText);

	method(createIndexedFaceSet);
	method(getIndexedFaceSet);

	method(createRigidBody);
	method(getRigidBody);

	method(createBox);
	method(getBox);

	method(createPlane);
	method(getPlane);

	method(createFileGeometry);
	method(getFileGeometry);

	method(createCloneGeometry);
	method(getCloneGeometry);

	method(createIndexedLineSet);
	method(getIndexedLineSet);

	method(createFileTexture);
	method(getFileTexture);

	method(createManualMaterial);
	method(getManualMaterial);

	method(createPointCloud);
	method(getPointCloud);

	method(getFolderPath);
}

#endif

#endif
