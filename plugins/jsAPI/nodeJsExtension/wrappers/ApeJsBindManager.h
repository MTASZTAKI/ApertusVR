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

#include <string>
#include <map>
#include <iostream>
#include "nbind/nbind.h"
#include "nbind/api.h"
#include "ApeJsBindColor.h"
#include "ApeJsBindDegree.h"
#include "ApeJsBindEuler.h"
#include "ApeJsBindQuaternion.h"
#include "ApeJsBindRadian.h"
#include "ApeJsBindVector3.h"
#include "ApeJsBindMatrix4.h"
#include "ApeIScene.h"
#include "ApeISystemConfig.h"
#include "ApeIEventManager.h"
#include "ApeJsBindIndexedFaceSetGeometryImpl.h"
#include "ApeIndexedLineSetGeometryJsBind.h"
#include "ApeBoxGeometryJsBind.h"
#include "ApeFileGeometryJsBind.h"
#include "ApeJsBindLightImpl.h"
#include "ApeJsBindNodeImpl.h"
#include "ApeJsBindTextGeometryImpl.h"
#include "ApeManualMaterialJsBind.h"
#include "ApePbsPassJsBind.h"
#include "ApeManualPassJsBind.h"

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
		LOG_FUNC_ENTER()
		mpScene = Ape::IScene::getSingletonPtr();
		mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
		mpEventManager = Ape::IEventManager::getSingletonPtr();
		//mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&JsBindManager::nodeEventCallBack, this, std::placeholders::_1));
		mErrorMap.insert(std::pair<ErrorType, std::string>(DYN_CAST_FAILED, "Dynamic cast failed!"));
		mErrorMap.insert(std::pair<ErrorType, std::string>(NULLPTR, "Return value is nullptr!"));
		LOG_FUNC_LEAVE();
	}

	void nodeEventCallBack(const Ape::Event& ev)
	{
		//LOG(LOG_TYPE_DEBUG, "ev.group: " << ev.group << " ev.subjectName: " << ev.subjectName);
		//std::map<int, nbind::cbFunction*>::iterator it;
		//it = mEventMap.find(ev.group);
		//if (it != mEventMap.end())
		//{
		//	v8::Local<v8::Value> argv[1];
		//	argv[0] = Nan::Null();
		//	LOG(LOG_TYPE_DEBUG, "calling callback function...");
		//	(*it->second)("test");
		//}
	}

	void connectEvent(int group, nbind::cbFunction& cb)
	{
		/*nbind::cbFunction* persistentCallback = new nbind::cbFunction(cb);
		mEventMap.insert(std::pair<int, nbind::cbFunction*>(group, persistentCallback));
		LOG(LOG_TYPE_DEBUG, "function inserted to map. group: " << group);*/
	}

	void start(std::string configFolderPath)
	{
		LOG_FUNC_ENTER();
		Ape::System::Start(configFolderPath.c_str(), true);
		LOG_FUNC_LEAVE();
	}

	void stop()
	{
		LOG_FUNC_ENTER();
		Ape::System::Stop();
		LOG_FUNC_LEAVE();
	}

	NodeJsPtr createNode(std::string name)
	{
		LOG_FUNC_ENTER();
		return NodeJsPtr(mpScene->createNode(name));
	}

	void getNodes(nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		auto nodes = mpScene->getNodes();
		std::vector<NodeJsPtr> nodeJsPtrVec;
		for (auto node : nodes)
		{
			nodeJsPtrVec.push_back(NodeJsPtr(node.second));
		}
		done(false, nodeJsPtrVec);
		LOG_FUNC_LEAVE();
	}

	void getNodesNames(nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		auto nodes = mpScene->getNodes();
		std::vector<std::string> nodeNameVec;
		for (auto node : nodes)
		{
			nodeNameVec.push_back(node.second.lock()->getName());
		}
		done(false, nodeNameVec);
		LOG_FUNC_LEAVE();
	}

	bool getNode(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getNode(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto node = std::dynamic_pointer_cast<Ape::INode>(entity))
			{
				success = true;
				done(!success, NodeJsPtr(entityWeakPtr));
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
		LOG_FUNC_LEAVE();
		return success;
	}

	bool getUserNode(nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto nodeWeakPtr = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName);
		if (auto node = nodeWeakPtr.lock())
		{
			success = true;
			done(!success, NodeJsPtr(nodeWeakPtr));
		}
		else
		{
			success = false;
			done(!success, mErrorMap[ErrorType::NULLPTR]);
		}
		LOG_FUNC_LEAVE();
		return success;
	}

	LightJsPtr createLight(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return LightJsPtr(mpScene->createEntity(name, Ape::Entity::LIGHT));
	}

	bool getLight(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto textGeometry = std::dynamic_pointer_cast<Ape::ILight>(entity))
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
		LOG_FUNC_LEAVE();
		return success;
	}

	TextJsPtr createText(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return TextJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_TEXT));
	}

	bool getText(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto textGeometry = std::dynamic_pointer_cast<Ape::ITextGeometry>(entity))
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
		LOG_FUNC_LEAVE();
		return success;
	}

	IndexedFaceSetJsPtr createIndexedFaceSet(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return IndexedFaceSetJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_INDEXEDFACESET));
	}

	bool getIndexedFaceSet(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto indexedFaceSet = std::dynamic_pointer_cast<Ape::IIndexedFaceSetGeometry>(entity))
			{
				success = true;
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
		LOG_FUNC_LEAVE();
		return success;
	}

	IndexedLineSetJsPtr createIndexedLineSet(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return IndexedLineSetJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_INDEXEDLINESET));
	}

	bool getIndexedLineSet(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto indexedLineSet = std::dynamic_pointer_cast<Ape::IIndexedLineSetGeometry>(entity))
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
		LOG_FUNC_LEAVE();
		return success;
	}

	BoxJsPtr createBox(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return BoxJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_BOX));
	}

	bool getBox(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto box = std::dynamic_pointer_cast<Ape::IBoxGeometry>(entity))
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
		LOG_FUNC_LEAVE();
		return success;
	}

	FileGeometryJsPtr createFileGeometry(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return FileGeometryJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_FILE));
	}

	bool getFileGeometry(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto box = std::dynamic_pointer_cast<Ape::IFileGeometry>(entity))
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
		LOG_FUNC_LEAVE();
		return success;
	}

	ManualMaterialJsPtr createManualMaterial(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return ManualMaterialJsPtr(mpScene->createEntity(name, Ape::Entity::MATERIAL_MANUAL));
	}

	bool getManualMaterial(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto manualMaterial = std::dynamic_pointer_cast<Ape::IManualMaterial>(entity))
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
		LOG_FUNC_LEAVE();
		return success;
	}

	PbsPassJsPtr createPbsPass(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return PbsPassJsPtr(mpScene->createEntity(name, Ape::Entity::PASS_PBS));
	}

	bool getPbsPass(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto pbsPass = std::dynamic_pointer_cast<Ape::IPbsPass>(entity))
			{
				success = true;
				done(!success, PbsPassJsPtr(entityWeakPtr));
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
		LOG_FUNC_LEAVE();
		return success;
	}

	ManualPassJsPtr createManualPass(std::string name)
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return ManualPassJsPtr(mpScene->createEntity(name, Ape::Entity::PASS_MANUAL));
	}

	bool getManualPass(std::string name, nbind::cbFunction &done)
	{
		LOG_FUNC_ENTER();
		bool success = false;
		auto entityWeakPtr = mpScene->getEntity(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto ManualPass = std::dynamic_pointer_cast<Ape::IManualPass>(entity))
			{
				success = true;
				done(!success, ManualPassJsPtr(entityWeakPtr));
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
		LOG_FUNC_LEAVE();
		return success;
	}

	std::string getFolderPath()
	{
		LOG_FUNC_ENTER();
		LOG_FUNC_LEAVE();
		return mpSystemConfig->getFolderPath();
	}

private:
	Ape::IScene* mpScene;
	Ape::ISystemConfig* mpSystemConfig;
	Ape::IEventManager* mpEventManager;

	std::map<int, nbind::cbFunction*> mEventMap;
};

NBIND_CLASS(JsBindManager)
{
	construct<>();

	method(start);
	method(stop);

	method(connectEvent);
	method(nodeEventCallBack);

	method(createNode);
	method(getNodesNames);
	method(getNodes);
	method(getNode);
	method(getUserNode);

	method(createLight);
	method(getLight);

	method(createText);
	method(getText);

	method(createIndexedFaceSet);
	method(getIndexedFaceSet);

	method(createBox);
	method(getBox);

	method(createFileGeometry);
	method(getFileGeometry);

	method(createIndexedLineSet);
	method(getIndexedLineSet);

	method(createManualMaterial);
	method(getManualMaterial);

	method(createPbsPass);
	method(getPbsPass);

	method(createManualPass);
	method(getManualPass);

	method(getFolderPath);
}

#endif

#endif
