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
#include "apeIFileGeometry.h"
#include "apeIIndexedLineSetGeometry.h"
#include "apeICloneGeometry.h"
#include "apeDoubleQueue.h"
#include "apeMatrix4.h"

ape::PluginManagerImpl* gpPluginManagerImpl;
ape::EventManagerImpl* gpEventManagerImpl;
ape::LogManagerImpl* gpLogManagerImpl;
ape::SceneManagerImpl* gpSceneManagerImpl;
ape::CoreConfigImpl* gpCoreConfigImpl; 
ape::DoubleQueue<std::pair<std::string, int>> mEventDoubleQueue;

void ape::System::Start(const char* configFolderPath, bool isBlocking, std::function<void()> userThreadFunction, int step_interval)
{
	std::cout << "apertusVR - Your open source AR/VR engine for science, education and industry" << std::endl;
	std::cout << "Build Target Platform: " << APE_PLATFORM_STRING << std::endl;
	gpLogManagerImpl = new LogManagerImpl();
	gpCoreConfigImpl = new CoreConfigImpl(std::string(configFolderPath));
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
	mEventDoubleQueue.push(std::pair(subjectName, eventType));
	//cb(&subjectName[0], eventType);
}

void ApeSystemStart(char* configFolderPath)
{
	gpLogManagerImpl = new ape::LogManagerImpl();
	gpCoreConfigImpl = new ape::CoreConfigImpl(std::string(configFolderPath));
	gpEventManagerImpl = new ape::EventManagerImpl();
	gpSceneManagerImpl = new ape::SceneManagerImpl();
	//gpSceneManagerImpl->createNode(std::string("StartNode"), true, gpCoreConfigImpl->getNetworkGUID());
}

void ApeSystemStop()
{
	delete gpEventManagerImpl;
	delete gpSceneManagerImpl;
	//delete gpPluginManagerImpl;
	delete gpCoreConfigImpl;
	delete gpLogManagerImpl;
}

void stringToCharPTR(std::string str, char* charPtr) {
	for (int i = 0; i < str.length(); i++)
	{
		charPtr[i] = str[i];
	}
	charPtr[str.length()] = '\0';
}

bool ApeSceneManager_GetEventNumber(int* eventNumber) {
	if (mEventDoubleQueue.sizePush() > 0) {
		mEventDoubleQueue.swap();
	}
	eventNumber[0] = mEventDoubleQueue.sizePop();
	return true;
}

bool ApeSceneManager_GetEvents(char** subjectNames, int* eventTypes, int* eventNumber) {
	int i = 0;
	while (!mEventDoubleQueue.emptyPop() && i < eventNumber[0]) {
		auto apeEvent = mEventDoubleQueue.front();
		stringToCharPTR(apeEvent.first, subjectNames[i]);
		eventTypes[i] = apeEvent.second;
		i++;
		mEventDoubleQueue.pop();
	}
	return true;
}

void ApeEventManager_RegisterCallback(ANSWERCB fp)
{
	cb = fp;
	gpEventManagerImpl->connectEvent(ape::Event::Group::NODE, std::bind(ApeEventListener, std::placeholders::_1));
	//gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::NODE, std::bind(ApeEventListener, std::placeholders::_1));
	//gpEventManagerImpl->connectEvent(ape::Event::Group::LIGHT, std::bind(ApeEventListener, std::placeholders::_1));
	//gpEventManagerImpl->connectEvent(ape::Event::Group::CAMERA, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(ApeEventListener, std::placeholders::_1));
	/*gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(ApeEventListener, std::placeholders::_1));*/
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_CLONE, std::bind(ApeEventListener, std::placeholders::_1));
	/*gpEventManagerImpl->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::SKY, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::WATER, std::bind(ApeEventListener, std::placeholders::_1));
	gpEventManagerImpl->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(ApeEventListener, std::placeholders::_1));*/
}


bool ApeSceneManager_GetIndexedFaceSet_GetVerticesSize(char* name, int* size)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		*size = static_cast<int>(indexedFaceSet->getParameters().coordinates.size());
		return true;
	}
	return false;
}

bool ApeSceneManager_GetIndexedFaceSet_GetVertices(char* name, float* vertices)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto apeVertices = indexedFaceSet->getParameters().coordinates;
		for (int i = 0; i < apeVertices.size(); i++)
		{
			vertices[i] = apeVertices[i];
		}
		return true;
	}
	return false;
}

bool ApeSceneManager_GetIndexedFaceSet_GetIndicesSize(char* name, int* size)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		*size = static_cast<int>(indexedFaceSet->getParameters().indices.size());
		return true;
	}
	return false;
}

bool ApeSceneManager_GetIndexedFaceSet_GetIndices(char* name, int* indices)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto apeIndices = indexedFaceSet->getParameters().indices;
		for (int i = 0; i < apeIndices.size(); i++)
		{
			indices[i] = apeIndices[i];
		}
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


bool ApeSceneManager_GetIndexedFaceSet_GetParent(char* name, char* parent)
{
	if (auto indexedFaceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto apeParent = indexedFaceSet->getParentNode().lock())
		{
			
			auto apeParentName = apeParent->getName();
			stringToCharPTR(apeParentName, parent);
			/*for (int i = 0; i < apeParentName.length(); i++)
			{
				parent[i] = apeParentName[i];
			}
			parent[apeParentName.length()] = '\0';*/
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

bool ApeSceneManager_GetNode_GetRotation(char* name, float* rotation)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		float m_pi = 3.14159265359;
		auto q = node->getOrientation();
		float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
		float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		rotation[0] = std::atan2(sinr_cosp, cosr_cosp)*180.0f/ m_pi;
		float sinp = 2.0f * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
			rotation[1] = std::copysign(m_pi / 2, sinp) * 180.1f / m_pi; // use 90 degrees if out of range
		else
			rotation[1] = std::asin(sinp) * 180.0f / m_pi;

		float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		rotation[2] = std::atan2(siny_cosp, cosy_cosp) * 180.0f / m_pi;

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
			auto apeParentName = apeParent->getName();
			stringToCharPTR(apeParentName, parent);
			return true;
		}
	}
	return false;
}

bool ApeSceneManager_CreateNode(char* name)
{
	gpSceneManagerImpl->createNode(std::string(name), true, gpCoreConfigImpl->getNetworkGUID());

	return true;
}

bool ApeSceneManager_DeleteNode(char* name)
{
	gpSceneManagerImpl->deleteNode(std::string(name));
	return true;
}

bool ApeSceneManager_GetNode_DetachFromParent(char* name)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		node->detachFromParentNode();
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_GetCreator(char* name, char* creator)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		stringToCharPTR(node->getCreator(), creator);
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_GetChildrenVisibility(char* name, int* visible)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		if (node->getChildrenVisibility())
			visible[0] = 1;
		else
			visible[0] = 0;
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_GetVisibility(char* name, int* visible)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		if (node->isVisible())
			visible[0] = 1;
		else
			visible[0] = 0;
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_SetVisibility(char* name, int* visible)
{
	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		if (visible[0] == 1)
			node->setVisible(true);
		else
			node->setVisible(false);
		return true;
	}
	return false;
}

bool ApeSceneManager_GetText_GetCaption(char* name, char* caption)
{
	if (auto apeTextGeometry = std::static_pointer_cast<ape::ITextGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto apeCaption = apeTextGeometry->getCaption();
		stringToCharPTR(apeCaption, caption);
		/*for (int i = 0; i < apeCaption.length(); i++)
		{
			caption[i] = apeCaption[i];
		}
		caption[apeCaption.length()] = '\0';*/
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_SetPosition(char* name, float* position)
{

	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		node->setPosition(ape::Vector3(position[0], position[1], position[2]));
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_SetOrientation(char* name, float* orientation)
{

	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		node->setOrientation(ape::Quaternion(orientation[0], orientation[1], orientation[2], orientation[3]));
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_SetScale(char* name, float* scale)
{

	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		node->setScale(ape::Vector3(scale[0], scale[1], scale[2]));
		return true;
	}
	return false;
}

bool ApeSceneManager_GetNode_SetParentNode(char* name, char* parent)
{

	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		if (auto parentNode = gpSceneManagerImpl->getNode(std::string(parent)).lock())
		{
			node->setParentNode(parentNode);
			return true;
		}
		
	}
	return false;
}

bool ApeSceneManager_GetNode_SetChildrenVisiblity(char* name, int* visible)
{

	if (auto node = gpSceneManagerImpl->getNode(std::string(name)).lock())
	{
		if (visible[0] == 1)
			node->setChildrenVisibility(true);
		else
			node->setChildrenVisibility(false);
		return true;
	}
	return false;
}


bool ApeSceneManager_GetFileGeometry_CreateFileGeometry(char* name)
{
	gpSceneManagerImpl->createEntity(std::string(name), ape::Entity::GEOMETRY_FILE, true, gpCoreConfigImpl->getNetworkGUID());
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		return true;
	}
	return false;
}

bool ApeSceneManager_GetFileGeometry_GetParentNode(char* name, char* parent)
{
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto parentNode = geometryFile->getParentNode().lock()) {
			
			auto apeParentName = parentNode->getName();
			stringToCharPTR(apeParentName, parent);
			return true;
		}
	}
	return false;
}

bool ApeSceneManager_GetFileGeometry_SetParentNode(char* name, char* parent)
{
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto parentNode = gpSceneManagerImpl->getNode(std::string(parent)).lock())
		{
			geometryFile->setParentNode(parentNode);
			return true;
		}

	}
	return false;
}

bool ApeSceneManager_GetFileGeometry_PlayAnimation(char* name, char* animationID)
{
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		geometryFile->playAnimation(std::string(animationID));
		return true;
	}
	return false;
}

bool ApeSceneManager_GetFileGeometry_StopAnimation(char* name, char* animationID)
{
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		geometryFile->stopAnimation(std::string(animationID));
		return true;
	}
	return false;
}

bool ApeSceneManager_GetFileGeometry_GetRunningAnimation(char* name, char* animationID)
{
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto runningAnim = geometryFile->getRunningAnimation();
		stringToCharPTR(runningAnim, animationID);
		return true;
	}
	return false;
}

bool ApeSceneManager_GetFileGeometry_GetStoppedAnimation(char* name, char* animationID)
{
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto runningAnim = geometryFile->getStoppedAnimation();
		stringToCharPTR(runningAnim, animationID);
		return true;
	}
	return false;
}

bool ApeSceneManager_GetFileGeometry_SetFileName(char* name, char* fileName)
{
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		geometryFile->setFileName(std::string(fileName));
		return true;
	}
	return false;
}

bool ApeSceneManager_GetFileGeometry_GetFileName(char* name, char* fileName)
{
	if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto apeFileName = geometryFile->getFileName();
		stringToCharPTR(apeFileName, fileName);
		return true;
	}
	return false;
}


bool ApeSceneManager_GetFileGeometry_Delete(char* name)
{

	gpSceneManagerImpl->deleteEntity(std::string(name));
	return true;
}

bool ApeSceneManager_GetGeometryIndexedLineSet_CreateIndexedLineSet(char* name)
{
	gpSceneManagerImpl->createEntity(std::string(name), ape::Entity::GEOMETRY_INDEXEDLINESET, true, gpCoreConfigImpl->getNetworkGUID());
	if (auto geometryFile = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		return true;
	}
	return false;
}

bool ApeSceneManager_GetGeometryIndexedLineSet_GetParentNode(char* name, char* parent)
{

	if (auto indexedLineSet = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto parentNode = indexedLineSet->getParentNode().lock())
		{
			auto apeParentNode = parentNode->getName();
			stringToCharPTR(apeParentNode, parent);
			return true;
		}

	}
	return false;
}

bool ApeSceneManager_GetGeometryIndexedLineSet_SetParameters(char* name, float* coord, int* indices, float* colors, int* size)
{

	if (auto indexedLineSet = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto params = indexedLineSet->getParameters();
		auto pCoords = params.getCoordinates();
		auto pIndices = params.getIndices();
		for (size_t i = 0; i < *size; i++)
		{
			pCoords.push_back(coord[i*3]);
			pCoords.push_back(coord[i*3+1]);
			pCoords.push_back(coord[i*3+2]);
			pIndices.push_back(indices[i]);
		}
		indexedLineSet->setParameters(pCoords, pIndices, ape::Color(colors[0],colors[1], colors[2]));
		return true;

	}
	return false;
}

bool ApeSceneManager_GetGeometryIndexedLineSet_GetParameters(char* name, float* coord, int* indices, float* colors, int* size)
{

	if (auto indexedLineSet = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto params = indexedLineSet->getParameters();
		auto pCoords = params.getCoordinates();
		auto pIndices = params.getIndices();
		auto pColor = params.getColor();
		coord = new float[pCoords.size()];
		indices = new int[pIndices.size()];
		for (size_t i = 0; i < pIndices.size(); i++)
		{
			coord[i*3] = pCoords[i*3];
			coord[i*3+1] = pCoords[i*3+1];
			coord[i*3+2] = pCoords[i*3+2];
			indices[i] = pIndices[i];
		}
		size[0] = int(pIndices.size());
		colors = new float[4];
		colors[0] = pColor.r;
		colors[1] = pColor.g;
		colors[2] = pColor.b;
		colors[3] = pColor.a;
		return true;

	}
	return false;
}

bool ApeSceneManager_GetGeometryIndexedLineSet_Delete(char* name)
{
	gpSceneManagerImpl->deleteEntity(std::string(name));
	return true;
}

bool ApeSceneManager_GetGeometryClone_CreateGeometryClone(char* name)
{
	gpSceneManagerImpl->createEntity(std::string(name), ape::Entity::GEOMETRY_CLONE, true, gpCoreConfigImpl->getNetworkGUID());
	if (auto geometryFile = std::static_pointer_cast<ape::ICloneGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		return true;
	}
	return false;
}

bool ApeSceneManager_GetGeometryClone_GetParentNode(char* name, char* parent)
{
	if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto parentNode = geometryClone->getParentNode().lock()) {
			
			auto apeParentNode = parentNode->getName();
			stringToCharPTR(apeParentNode, parent);
			return true;
		}
	}
	return false;
}

bool ApeSceneManager_GetGeometryClone_SetParentNode(char* name, char* parent)
{
	if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto parentNode = gpSceneManagerImpl->getNode(std::string(parent)).lock())
		{
			geometryClone->setParentNode(parentNode);
			return true;
		}

	}
	return false;
}


bool ApeSceneManager_GetGeometryClone_GetSourceGeomtry(char* name, char* sourceGeometry)
{
	if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto sourceGeom = std::static_pointer_cast<ape::Geometry>(geometryClone->getSourceGeometry().lock())) {

			auto apeSourceGeometry = sourceGeom->getName();
			stringToCharPTR(apeSourceGeometry, sourceGeometry);
			return true;
		}

	}
	return false;
}

bool ApeSceneManager_GetGeometryClone_SetSourceGeomtry(char* name, char* sourceGeometry)
{
	if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		if (auto sourceGeom = std::static_pointer_cast<ape::Geometry>(gpSceneManagerImpl->getEntity(std::string(sourceGeometry)).lock())) {
			geometryClone->setSourceGeometry(sourceGeom);
			return true;
		}

	}
	return false;
}

bool ApeSceneManager_GetGeometryClone_GetSourceGeomtryGroupName(char* name, char* sourceGeometryGroupName)
{
	if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		auto apeSourceGeometryGroupName = geometryClone->getSourceGeometryGroupName();
		stringToCharPTR(apeSourceGeometryGroupName, sourceGeometryGroupName);
		return true;

	}
	return false;
}

bool ApeSceneManager_GetGeometryClone_SetSourceGeomtryGroupName(char* name, char* sourceGeometryGroupName)
{
	if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		geometryClone->setSourceGeometryGroupName(std::string(sourceGeometryGroupName));
		return true;

	}
	return false;
}

bool ApeSceneManager_GetGeometryClone_Delete(char* name)
{
	gpSceneManagerImpl->deleteEntity(std::string(name));
	return true;
}

bool ApeSceneManager_GetText_CreateTextGeometry(char* name)
{
	gpSceneManagerImpl->createEntity(std::string(name), ape::Entity::GEOMETRY_TEXT, true, gpCoreConfigImpl->getNetworkGUID());
	if (auto geometryFile = std::static_pointer_cast<ape::ITextGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		return true;
	}
	return false;
}

bool ApeSceneManager_GetText_Delete(char* name)
{
	gpSceneManagerImpl->deleteEntity(std::string(name));
	return true;
}
bool ApeSceneManager_GetText_SetCaption(char* name, char* caption) 
{
	if (auto apeTextGeometry = std::static_pointer_cast<ape::ITextGeometry>(gpSceneManagerImpl->getEntity(std::string(name)).lock()))
	{
		apeTextGeometry->setCaption(std::string(caption));
		return true;
	}
	return false;
}