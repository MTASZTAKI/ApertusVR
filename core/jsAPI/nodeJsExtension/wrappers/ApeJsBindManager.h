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
#include "ApeIScene.h"
#include "ApeJsBindIndexedFaceSetGeometryImpl.h"
#include "ApeIndexedLineSetGeometryJsBind.h"
#include "ApeJsBindLightImpl.h"
#include "ApeJsBindNodeImpl.h"
#include "ApeJsBindTextGeometryImpl.h"
#include "ApeManualMaterialJsBind.h"
#include "ApePbsPassJsBind.h"

#ifdef NBIND_CLASS

class JsBindManager
{
public:
	JsBindManager()
	{
		mpScene = Ape::IScene::getSingletonPtr();
	}

	NodeJsPtr createNode(std::string name)
	{
		return NodeJsPtr(mpScene->createNode(name));
	}

	bool getNode(std::string name, nbind::cbFunction &done)
	{
		std::cout << "getNode()" << std::endl;

		auto entityWeakPtr = mpScene->getNode(name);
		if (auto entity = entityWeakPtr.lock())
		{
			if (auto textGeometry = std::dynamic_pointer_cast<Ape::INode>(entity))
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

	LightJsPtr createLight(std::string name)
	{
		std::cout << "createLight()" << std::endl;
		return LightJsPtr(mpScene->createEntity(name, Ape::Entity::LIGHT));
	}

	bool getLight(std::string name, nbind::cbFunction &done)
	{
		std::cout << "getText()" << std::endl;

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
		std::cout << "createText()" << std::endl;
		return TextJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_TEXT));
	}

	bool getText(std::string name, nbind::cbFunction &done)
	{
		std::cout << "getText()" << std::endl;

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
		std::cout << "createIndexedFaceSet()" << std::endl;
		return IndexedFaceSetJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_INDEXEDFACESET));
	}

	bool getIndexedFaceSet(std::string name, nbind::cbFunction &done)
	{
		std::cout << "getIndexedFaceSet()" << std::endl;

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
		std::cout << "createIndexedLineSet()" << std::endl;
		return IndexedLineSetJsPtr(mpScene->createEntity(name, Ape::Entity::GEOMETRY_INDEXEDLINESET));
	}

	bool getIndexedLineSet(std::string name, nbind::cbFunction &done)
	{
		std::cout << "getIndexedLineSet()" << std::endl;

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

	ManualMaterialJsPtr createManualMaterial(std::string name)
	{
		std::cout << "createManualMaterial()" << std::endl;
		return ManualMaterialJsPtr(mpScene->createEntity(name, Ape::Entity::MATERIAL_MANUAL));
	}

	bool getManualMaterial(std::string name, nbind::cbFunction &done)
	{
		std::cout << "getManualMaterial()" << std::endl;

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
		std::cout << "createPbsPass()" << std::endl;
		return PbsPassJsPtr(mpScene->createEntity(name, Ape::Entity::PASS_PBS));
	}

	bool getPbsPass(std::string name, nbind::cbFunction &done)
	{
		std::cout << "getPbsPass()" << std::endl;

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

private:
	Ape::IScene* mpScene;
};

NBIND_CLASS(JsBindManager)
{
	construct<>();

	method(createNode);
	method(getNode);

	method(createLight);
	method(getLight);

	method(createText);
	method(getText);

	method(createIndexedFaceSet);
	method(getIndexedFaceSet);

	method(createIndexedLineSet);
	method(getIndexedLineSet);

	method(createManualMaterial);
	method(getManualMaterial);

	method(createPbsPass);
	method(getPbsPass);
}

#endif

#endif
