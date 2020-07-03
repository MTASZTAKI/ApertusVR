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

#include "apeAndroidSampleScenePlugin.h"
#include "android/log.h"
#include "apeILight.h"
#include "apeIPlaneGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIManualMaterial.h"


ape::AndroidSampleScenePlugin::AndroidSampleScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	__android_log_print(ANDROID_LOG_INFO, "cpplog", "Hello Android Sample Scene");
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::AndroidSampleScenePlugin::~AndroidSampleScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	__android_log_print(ANDROID_LOG_INFO, "cpplog", "Bye Android Sample Scene");
	APE_LOG_FUNC_LEAVE();
}

void ape::AndroidSampleScenePlugin::Init() 
{
	APE_LOG_FUNC_ENTER();
	__android_log_print(ANDROID_LOG_INFO, "cpplog", "Init Android Sample Scene");
	initRootNode();
	initLights();
	initGeometries();
	if(auto rootNodeShared = mRootNodeWeak.lock())
	{
		rootNodeShared->setPosition(ape::Vector3(0.f,1.f,0.f));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::AndroidSampleScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	__android_log_print(ANDROID_LOG_INFO, "cpplog", "Run Android Sample Scene");
	APE_LOG_FUNC_LEAVE();
}

void ape::AndroidSampleScenePlugin::initRootNode()
{
	if (auto rootNodeShared = mpSceneManager->createNode("root", false, "androidSampleScene").lock())
	{
		rootNodeShared->setPosition(ape::Vector3(0, 0, 0));
		rootNodeShared->setScale(ape::Vector3(0.5, 0.5, 0.5));
		mRootNodeWeak = rootNodeShared;
	}
}

void ape::AndroidSampleScenePlugin::initLights()
{
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light01", ape::Entity::LIGHT, false, "androidSampleScene").lock()))
	{
		light->setLightType(ape::Light::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(-1,-1,0));
		light->setDiffuseColor(ape::Color(0.6f,0.6f,0.6f));
		light->setSpecularColor(ape::Color(1.0f,1.0f,1.0f));
	}

	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light02", ape::Entity::LIGHT, false, "androidSampleScene").lock()))
	{
		light->setLightType(ape::Light::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(1,-1,0));
		light->setDiffuseColor(ape::Color(0.6f,0.6f,0.6f));
		light->setSpecularColor(ape::Color(1.0f,1.0f,1.0f));
	}
}

void ape::AndroidSampleScenePlugin::initGeometries()
{
	// plane
	if(auto planeNode = mpSceneManager->createNode("plane01Node",false,"androidSampleScene").lock())
	{
		planeNode->setParentNode(mRootNodeWeak);
		planeNode->setPosition(ape::Vector3(0,0,0));

		if (auto plane = std::static_pointer_cast<ape::IPlaneGeometry>(
				mpSceneManager->createEntity("plane01", ape::Entity::GEOMETRY_PLANE, false,
											 "androidSampleScene").lock()))
		{
			plane->setParameters(ape::Vector2(),ape::Vector2(100,100),ape::Vector2());
			plane->setParentNode(planeNode);

			if (auto material = std::static_pointer_cast<ape::IManualMaterial>(
					mpSceneManager->createEntity("plane01Material",ape::Entity::MATERIAL_MANUAL,
							false,"androidSampleScene").lock()))
			{
				material->setAmbientColor(ape::Color(0.2f,0.2f,0.2f));
				material->setDiffuseColor(ape::Color(0.4f,0.4f,0.4f));
				material->setSpecularColor(ape::Color(0.5f,0.5f,0.5f));

				plane->setMaterial(material);
			}
		}
	}

	// Vibrating_Bowl_Clip
	if (auto node = mpSceneManager->createNode("vibratingBowlClipNode",false,"androidSampleScene").lock()) {
		node->setPosition(ape::Vector3(-10, 0, 0));
		node->setParentNode(mRootNodeWeak);

		if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
				mpSceneManager->createEntity("Vibrating_Bowl_Clip", ape::Entity::GEOMETRY_FILE,
											 false, "androidSampleScene").lock()))
		{
			fileGeom->setUnitScale(0.01f);
			fileGeom->setFileName("VibratingBowl/Vibrating_Bowl_Clip.obj");
			fileGeom->setParentNode(node);
		}
	}

	// Haas
	if (auto node = mpSceneManager->createNode("GripperNode",false,"androidSampleScene").lock())
	{
		node->setPosition(ape::Vector3(10,0,0));
		node->setParentNode(mRootNodeWeak);

		if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
				mpSceneManager->createEntity("UR_Gripper_1", ape::Entity::GEOMETRY_FILE,
											 false, "androidSampleScene").lock()))
		{
			fileGeom->setFileName("UniversalRobot/UR_Gripper_1.obj");
			fileGeom->setParentNode(node);
		}
	}
}

void ape::AndroidSampleScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();	
}

void ape::AndroidSampleScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();	
}

void ape::AndroidSampleScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();	
}

void ape::AndroidSampleScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();	
}