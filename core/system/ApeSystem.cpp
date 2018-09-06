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

#include <chrono>
#include <random>
#include "ApePlatform.h"
#include "ApeSystem.h"
#include "ApeSystemConfigImpl.h"
#include "ApeMainWindowImpl.h"
#include "ApePluginManagerImpl.h"
#include "ApeEventManagerImpl.h"
#include "ApeLogManagerImpl.h"
#include "ApeSceneManagerImpl.h"
#include "ApeSceneSessionImpl.h"
#include "ApeINode.h"
#include "ApeITextGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApeIManualPass.h"
#include "ApeISphereGeometry.h"
#include "ApeIConeGeometry.h"
#include "ApeIPlaneGeometry.h"

Ape::PluginManagerImpl* gpPluginManagerImpl;
Ape::EventManagerImpl* gpEventManagerImpl;
Ape::LogManagerImpl* gpLogManagerImpl;
Ape::SceneManagerImpl* gpSceneManagerImpl;
Ape::SceneSessionImpl* gpSceneSessionImpl;
Ape::SystemConfigImpl* gpSystemConfigImpl;
Ape::MainWindowImpl* gpMainWindowImpl;

void Ape::System::Start(const char* configFolderPath, int isBlockingMode)
{
	std::cout << "ApertusVR - Your open source AR/VR engine for science, education and industry" << std::endl;
	std::cout << "Build Target Platform: " << APE_PLATFORM_STRING << std::endl;

	gpSystemConfigImpl = new SystemConfigImpl(std::string(configFolderPath));
	std::string uniqueUserNamePrefix = gpSystemConfigImpl->getSceneSessionConfig().uniqueUserNamePrefix;
	std::string delimiter = "-";
	auto tp = std::chrono::system_clock::now();
	auto dur = tp.time_since_epoch();
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
	std::stringstream uniqueUserNodeName;
	uniqueUserNodeName << uniqueUserNamePrefix << delimiter << nanoseconds;
	gpSystemConfigImpl->setGeneratedUniqueUserNodeName(uniqueUserNodeName.str());

	gpMainWindowImpl = new MainWindowImpl();
	gpEventManagerImpl = new EventManagerImpl();
	gpLogManagerImpl = new LogManagerImpl();
	gpPluginManagerImpl = new PluginManagerImpl();
	gpSceneSessionImpl = new SceneSessionImpl();
	gpSceneManagerImpl = new SceneManagerImpl();
	gpSceneSessionImpl->setScene(gpSceneManagerImpl);
	
	if (gpSystemConfigImpl->getMainWindowConfig().creator == "ApeSystem")
		; //TODO open a platform specific window if needed

	gpPluginManagerImpl->CreatePlugins();

	//Must create a userNode by the Ape::System with an unqiue name, or not? Who is the responsible for that? System or a plugin?
	if (auto userNode = gpSceneManagerImpl->createNode(uniqueUserNodeName.str()).lock())
	{
		if (auto headNode = gpSceneManagerImpl->createNode(uniqueUserNodeName.str() + "_HeadNode").lock())
		{
			headNode->setParentNode(userNode);
			if (auto userMaterial = std::static_pointer_cast<Ape::IManualMaterial>(gpSceneManagerImpl->createEntity(uniqueUserNodeName.str() + "_Material", Ape::Entity::MATERIAL_MANUAL).lock()))
			{
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<double> distDouble(0.0, 1.0);
				std::vector<double> randomColors;
				for (int i = 0; i < 3; i++)
					randomColors.push_back(distDouble(gen));
				userMaterial->setDiffuseColor(Ape::Color(randomColors[0], randomColors[1], randomColors[2]));
				userMaterial->setSpecularColor(Ape::Color(randomColors[0], randomColors[1], randomColors[2]));
			}
		}
	}

	gpPluginManagerImpl->InitAndRunPlugins();

	if (isBlockingMode)
		gpPluginManagerImpl->joinPluginThreads();
	else
		gpPluginManagerImpl->detachPluginThreads();
}

void Ape::System::Stop()
{
	delete gpEventManagerImpl;
	delete gpSceneManagerImpl;
	delete gpSceneSessionImpl;
	delete gpPluginManagerImpl;
	delete gpSystemConfigImpl;
	delete gpMainWindowImpl;
}

