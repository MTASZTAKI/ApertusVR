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

#ifndef APE_JSBIND_MANAGER_H
#define APE_JSBIND_MANAGER_H

#include <string>
#include <iostream>
#include "nbind/nbind.h"
#include "nbind/api.h"
#include "ApeJsBindColor.h"
#include "ApeJsBindDegree.h"
#include "ApeJsBindQuaternion.h"
#include "ApeJsBindRadian.h"
#include "ApeJsBindVector3.h"
#include "ApeIScene.h"
#include "ApeISystemConfig.h"
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
public:
	JsBindManager()
	{
		mpScene = Ape::IScene::getSingletonPtr();
		mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	}

	NodeJsPtr createNode(std::string name)
	{
		return NodeJsPtr(mpScene->createNode(name));
	}

	bool getNode(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getNode()" << std::endl;

		auto entityWeakPtr = mpScene->getNode(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto node = std::dynamic_pointer_cast<Ape::INode>(entity))
			{
				done(false, NodeJsPtr(entityWeakPtr));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	bool getUserNode(nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getUserNode()" << std::endl;

		auto nodeWeakPtr = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName);
		if (auto node = nodeWeakPtr.lock())
		{
			done(false, NodeJsPtr(nodeWeakPtr));
			return true;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	LightJsPtr createLight(std::string name)
	{
		std::cout << "JsBindManager::createLight()" << std::endl;
		return LightJsPtr(mpScene->createEntity(name, Ape::Entity::LIGHT));
	}

	bool getLight(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getText()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto textGeometry = std::dynamic_pointer_cast<Ape::ILight>(entity))
			{
				done(false, LightJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	TextJsPtr createText(std::string name)
	{
		std::cout << "JsBindManager::createText()" << std::endl;
		return TextJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_TEXT));
	}

	bool getText(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getText()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto textGeometry = std::dynamic_pointer_cast<Ape::ITextGeometry>(entity))
			{
				done(false, TextJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	IndexedFaceSetJsPtr createIndexedFaceSet(std::string name)
	{
		std::cout << "JsBindManager::createIndexedFaceSet()" << std::endl;
		return IndexedFaceSetJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_INDEXEDFACESET));
	}

	bool getIndexedFaceSet(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getIndexedFaceSet()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto indexedFaceSet = std::dynamic_pointer_cast<Ape::IIndexedFaceSetGeometry>(entity))
			{
				done(false, IndexedFaceSetJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	IndexedLineSetJsPtr createIndexedLineSet(std::string name)
	{
		std::cout << "JsBindManager::createIndexedLineSet()" << std::endl;
		return IndexedLineSetJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_INDEXEDLINESET));
	}

	bool getIndexedLineSet(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getIndexedLineSet()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto indexedLineSet = std::dynamic_pointer_cast<Ape::IIndexedLineSetGeometry>(entity))
			{
				done(false, IndexedLineSetJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	BoxJsPtr createBox(std::string name)
	{
		std::cout << "JsBindManager::createBox()" << std::endl;
		return BoxJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_BOX));
	}

	bool getBox(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getBox()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto box = std::dynamic_pointer_cast<Ape::IBoxGeometry>(entity))
			{
				done(false, BoxJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	FileGeometryJsPtr createFileGeometry(std::string name)
	{
		std::cout << "JsBindManager::createFileGeometry()" << std::endl;
		return FileGeometryJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_FILE));
	}

	bool getFileGeometry(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getFileGeometry()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto box = std::dynamic_pointer_cast<Ape::IFileGeometry>(entity))
			{
				done(false, FileGeometryJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	ManualMaterialJsPtr createManualMaterial(std::string name)
	{
		std::cout << "JsBindManager::createManualMaterial()" << std::endl;
		return ManualMaterialJsPtr(mpScene->createEntity(name, Ape::Entity::MATERIAL_MANUAL));
	}

	bool getManualMaterial(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getManualMaterial()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto manualMaterial = std::dynamic_pointer_cast<Ape::IManualMaterial>(entity))
			{
				done(false, ManualMaterialJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	PbsPassJsPtr createPbsPass(std::string name)
	{
		std::cout << "JsBindManager::createPbsPass()" << std::endl;
		return PbsPassJsPtr(mpScene->createEntity(name, Ape::Entity::PASS_PBS));
	}

	bool getPbsPass(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getPbsPass()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto pbsPass = std::dynamic_pointer_cast<Ape::IPbsPass>(entity))
			{
				done(false, PbsPassJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	ManualPassJsPtr createManualPass(std::string name)
	{
		std::cout << "JsBindManager::createManualPass()" << std::endl;
		return ManualPassJsPtr(mpScene->createEntity(name, Ape::Entity::PASS_MANUAL));
	}

	bool getManualPass(std::string name, nbind::cbFunction &done)
	{
		std::cout << "JsBindManager::getManualPass()" << std::endl;

		if (auto entity = mpScene->getEntity(name).lock())
		{
			if (auto ManualPass = std::dynamic_pointer_cast<Ape::IManualPass>(entity))
			{
				done(false, ManualPassJsPtr(mpScene->getEntity(name)));
				return true;
			}

			done(true, std::string("Dynamic cast failed!"));
			return false;
		}

		done(true, std::string("Return value of getEntity() is nullptr!"));
		return false;
	}

	std::string getFolderPath()
	{
		std::cout << "JsBindManager::getFolderPath()" << std::endl;
		return mpSystemConfig->getFolderPath();
	}

private:
	Ape::IScene* mpScene;
	Ape::ISystemConfig* mpSystemConfig;
};

NBIND_CLASS(JsBindManager)
{
	construct<>();

	method(createNode);
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
