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
#include "system/apePlatform.h"
#include "system/apeSystem.h"
#include "apeEventManagerImpl.h"
#include "apeLogManagerImpl.h"
#include "apePluginManagerImpl.h"
#include "apeSceneManagerImpl.h"
#include "apeSceneNetworkImpl.h"
#include "apeCoreConfigImpl.h"

ape::PluginManagerImpl* gpPluginManagerImpl;
ape::EventManagerImpl* gpEventManagerImpl;
ape::LogManagerImpl* gpLogManagerImpl;
ape::SceneManagerImpl* gpSceneManagerImpl;
ape::SceneNetworkImpl* gpSceneNetworkImpl;
ape::CoreConfigImpl* gpCoreConfigImpl;

void ape::System::Start(const char* configFolderPath, int isBlockingMode)
{
	std::cout << "apertusVR - Your open source AR/VR engine for science, education and industry" << std::endl;
	std::cout << "Build Target Platform: " << APE_PLATFORM_STRING << std::endl;
	gpCoreConfigImpl = new CoreConfigImpl(std::string(configFolderPath));
	gpEventManagerImpl = new EventManagerImpl();
	gpLogManagerImpl = new LogManagerImpl();
	gpPluginManagerImpl = new PluginManagerImpl();
	gpSceneNetworkImpl = new SceneNetworkImpl();
	gpSceneManagerImpl = new SceneManagerImpl();
	gpSceneNetworkImpl->setScene(gpSceneManagerImpl);
	gpPluginManagerImpl->CreatePlugins();
	gpPluginManagerImpl->InitAndRunPlugins();
	if (isBlockingMode)
		gpPluginManagerImpl->joinPluginThreads();
	else
		gpPluginManagerImpl->detachPluginThreads();
}

void ape::System::Stop()
{
	delete gpEventManagerImpl;
	delete gpSceneManagerImpl;
	delete gpSceneNetworkImpl;
	delete gpPluginManagerImpl;
	delete gpCoreConfigImpl;
}

