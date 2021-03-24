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
#include <thread>
#include "apePlatform.h"
#include "apeSystem.h"
#include "apeEventManagerImpl.h"
#include "apeLogManagerImpl.h"
#include "apePluginManagerImpl.h"
#include "apeSceneManagerImpl.h"
#include "apeCoreConfigImpl.h"
#include "apeINode.h"
#include "apeIIndexedFaceSetGeometry.h"
#include "apeIManualMaterial.h"
#include "apeITextGeometry.h"

ape::PluginManagerImpl* gpPluginManagerImpl;
ape::EventManagerImpl* gpEventManagerImpl;
ape::LogManagerImpl* gpLogManagerImpl;
ape::SceneManagerImpl* gpSceneManagerImpl;
ape::CoreConfigImpl* gpCoreConfigImpl;

void ape::System::Start(const char* configFolderPath, bool isBlocking, std::function<void()> userThreadFunction, int step_interval)
{
	std::cout << "apertusVR - Your open source AR/VR engine for science, education and industry" << std::endl;
	std::cout << "Build Target Platform: " << APE_PLATFORM_STRING << std::endl;
	gpLogManagerImpl = new LogManagerImpl();
	gpCoreConfigImpl = new CoreConfigImpl(configFolderPath);
	gpEventManagerImpl = new EventManagerImpl();
	gpSceneManagerImpl = new SceneManagerImpl();
	gpPluginManagerImpl = new PluginManagerImpl();
	gpPluginManagerImpl->CreatePlugins();
	gpPluginManagerImpl->InitAndRunPlugins();
	if (userThreadFunction)
		gpPluginManagerImpl->registerUserThreadFunction(userThreadFunction);
	if (isBlocking)
		gpPluginManagerImpl->joinThreads();
	else
		gpPluginManagerImpl->detachThreads();

	std::this_thread::sleep_for(std::chrono::milliseconds(step_interval));
	gpPluginManagerImpl->callStepFunc();

}

void ape::System::Stop()
{
	gpPluginManagerImpl->StopPlugins();
	delete gpEventManagerImpl;
	delete gpSceneManagerImpl;
	delete gpPluginManagerImpl;
	delete gpCoreConfigImpl;
	delete gpLogManagerImpl;
}

void ApeEventListener(const ape::Event& event)
{
	auto subjectName = event.subjectName;
	auto eventType = event.type;
	cb(&subjectName[0], eventType);
}

void ApeSystemStart(char* configFolderPath)
{
	gpLogManagerImpl = new ape::LogManagerImpl();
	gpCoreConfigImpl = new ape::CoreConfigImpl(std::string(configFolderPath));
	gpEventManagerImpl = new ape::EventManagerImpl();
	gpSceneManagerImpl = new ape::SceneManagerImpl();
}

void ApeSystemStop()
{
	delete gpEventManagerImpl;
	delete gpSceneManagerImpl;
	delete gpPluginManagerImpl;
	delete gpCoreConfigImpl;
	delete gpLogManagerImpl;
}

void ApeEventManager_RegisterCallback(ANSWERCB fp)
{
	cb = fp;
	gpEventManagerImpl->connectEvent(ape::Event::Group::NODE, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(ApeEventListener, std::placeholders::_1));
}

bool ApeSceneManager_GetIndexedFaceSet_GetSize(char* name, int* size)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		*size = static_cast<int>(indexedFaceSet->getParameters().indices.size());
		return true;
	}
	return false;
}

bool ApeSceneManager_GetIndexedFaceSet_GetVertices(char* name, float* vertices)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		vertices = indexedFaceSet->getParameters().coordinates.data();
		return true;
	}
	return false;
}

bool ApeSceneManager_GetIndexedFaceSet_GetIndices(char* name, int* indices)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		indices = indexedFaceSet->getParameters().indices.data();
		return true;
	}
	return false;
}

bool ApeSceneManager_GetIndexedFaceSet_GetColor(char* name, float* color)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto manualMaterial = std::static_pointer_cast<ape::IManualMaterial>((indexedFaceSet->getParameters().material).lock()))
		{
			auto apeColor = manualMaterial->getDiffuseColor();
			color[0] = apeColor.a;
			color[1] = apeColor.r;
			color[2] = apeColor.g;
			color[3] = apeColor.b;
			return true;
		}
	}
	return false;
}

bool ApeSceneManager_GetNode_GetOrientation(char* name, float* orientation)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		auto apeOrientation = node->getOrientation();
		orientation[0] = apeOrientation.w;
		orientation[1] = apeOrientation.x;
		orientation[2] = apeOrientation.y;
		orientation[3] = apeOrientation.z;
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_GetPosition(char* name, float* position)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		auto apePosition = node->getPosition();
		position[0] = apePosition.x;
		position[1] = apePosition.y;
		position[2] = apePosition.z;
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_GetScale(char* name, float* scale)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		auto apeScale = node->getScale();
		scale[0] = apeScale.x;
		scale[1] = apeScale.y;
		scale[2] = apeScale.z;
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_GetParent(char* name, char* parent)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		if (auto apeParent = node->getParentNode().lock())
		{
			parent = &apeParent->getName()[0];
			return true;
		}
	}
	return false;
}

bool ApeSceneManager_GetText_GetCaption(char* name, char* caption)
{
	if (auto apeTextGeometry = std::static_pointer_cast<ape::ITextGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		caption = &apeTextGeometry->getCaption()[0];
		return true;
	}
	return false;
}

