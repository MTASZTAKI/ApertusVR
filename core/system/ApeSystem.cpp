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
#include "system/ApePlatform.h"
#include "system/ApeSystem.h"
#include "sceneelements/ApeIConeGeometry.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeIManualPass.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeIPlaneGeometry.h"
#include "sceneelements/ApeISphereGeometry.h"
#include "sceneelements/ApeITextGeometry.h"
#include "ApeEventManagerImpl.h"
#include "ApeLogManagerImpl.h"
#include "ApeMainWindowImpl.h"
#include "ApePluginManagerImpl.h"
#include "ApeSceneManagerImpl.h"
#include "ApeSceneSessionImpl.h"
#include "ApeSystemConfigImpl.h"

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

