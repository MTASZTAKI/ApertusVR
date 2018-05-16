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
#include "ApeSystem.h"
#include "ApeSystemConfigImpl.h"
#include "ApeMainWindowImpl.h"
#include "ApePluginManagerImpl.h"
#include "ApeEventManagerImpl.h"
#include "ApeLogManagerImpl.h"
#include "ApeSceneImpl.h"
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
Ape::SceneImpl* gpSceneImpl;
Ape::SceneSessionImpl* gpSceneSessionImpl;
Ape::SystemConfigImpl* gpSystemConfigImpl;
Ape::MainWindowImpl* gpMainWindowImpl;

void Ape::System::Start(const char* configFolderPath, int isBlockingMode)
{
	std::cout << "ApertusVR - Your open source AR/VR engine for science, education and industry" << std::endl;
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
	gpSceneImpl = new SceneImpl();
	gpSceneSessionImpl->setScene(gpSceneImpl);
	
	if (gpSystemConfigImpl->getMainWindowConfig().creator == "ApeSystem")
		; //TODO open a platform specific window if needed

	gpPluginManagerImpl->CreatePlugins();

	//Must create a userNode by the Ape::System with an unqiue name, or not? Who is the responsible for that? System or a plugin?
	if (auto userNode = gpSceneImpl->createNode(uniqueUserNodeName.str()).lock())
	{
		if (auto userMaterial = std::static_pointer_cast<Ape::IManualMaterial>(gpSceneImpl->createEntity(userNode->getName() + "_ManualMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<double> distDouble(0.0, 1.0);
			std::vector<double> randomColors;
			for (int i = 0; i < 3; i++)
				randomColors.push_back(distDouble(gen));
			userMaterial->setDiffuseColor(Ape::Color(randomColors[0], randomColors[1], randomColors[2]));
			userMaterial->setSpecularColor(Ape::Color(randomColors[0], randomColors[1], randomColors[2]));
			if (auto userConeNode = gpSceneImpl->createNode(userNode->getName() + "_ConeNode").lock())
			{
				userConeNode->setParentNode(userNode);
				userConeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
				if (auto userCone = std::static_pointer_cast<Ape::IConeGeometry>(gpSceneImpl->createEntity(userNode->getName() + "_ConeGeometry", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					userCone->setParameters(10.0f, 30.0f, 1.0f, Ape::Vector2(1, 1));
					userCone->setParentNode(userConeNode);
					userCone->setMaterial(userMaterial);
				}
			}
		}
		if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(gpSceneImpl->createEntity(userNode->getName() + "_TextGeometry", Ape::Entity::GEOMETRY_TEXT).lock()))
		{
			userNameText->setCaption(uniqueUserNodeName.str());
			userNameText->setOffset(Ape::Vector3(0.0f, 10.0f, 0.0f));
			userNameText->setParentNode(userNode);
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
	delete gpSceneImpl;
	delete gpSceneSessionImpl;
	delete gpPluginManagerImpl;
	delete gpSystemConfigImpl;
	delete gpMainWindowImpl;
}

