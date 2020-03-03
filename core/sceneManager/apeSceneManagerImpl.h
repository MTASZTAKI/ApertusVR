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

#ifndef APE_SCENEMANAGERIMPL_H
#define APE_SCENEMANAGERIMPL_H

#ifdef _WIN32
#ifdef BUILDING_APE_SCENEMANAGER_DLL
#define APE_SCENEMANAGER_DLL_EXPORT __declspec(dllexport)
#else
#define APE_SCENEMANAGER_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define APE_SCENEMANAGER_DLL_EXPORT 
#endif

#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeILogManager.h"
#include "apeIEventManager.h"

namespace ape
{

	class APE_SCENEMANAGER_DLL_EXPORT SceneManagerImpl : public ape::ISceneManager
	{ 
	public:
		SceneManagerImpl();

		~SceneManagerImpl();

		ape::NodeWeakPtrNameMap getNodes() override;

		ape::NodeWeakPtr getNode(std::string name) override;

		ape::NodeWeakPtr createNode(std::string name, bool replicate, std::string ownerID) override;

		ape::EntityWeakPtrNameMap getEntities() override;

		ape::EntityWeakPtr getEntity(std::string name) override;

		ape::EntityWeakPtr createEntity(std::string name, ape::Entity::Type type, bool replicate, std::string ownerID) override;

		void deleteNode(std::string name) override;

		void deleteEntity(std::string name) override;

	private:
		ape::IEventManager* mpEventManager;

		ape::NodeSharedPtrNameMap mNodes;

		ape::EntitySharedPtrNameMap mEntities;

		ape::ICoreConfig* mpCoreConfig;

		ape::ISceneNetwork* mpSceneNetwork;

		std::string mUniqueID;
	};
}
#endif
