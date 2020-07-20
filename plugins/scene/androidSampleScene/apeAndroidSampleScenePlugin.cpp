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
#include "apeIConeGeometry.h"
#include "apeICloneGeometry.h"

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
		light->setSpecularColor(ape::Color(0.7f,0.7f,0.7f,0.8));
	}

	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light02", ape::Entity::LIGHT, false, "androidSampleScene").lock()))
	{
		light->setLightType(ape::Light::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(1,-1,0));
		light->setDiffuseColor(ape::Color(0.6f,0.6f,0.6f));
		light->setSpecularColor(ape::Color(0.7f,0.7f,0.7f,0.8));
	}

	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light03", ape::Entity::LIGHT, false, "androidSampleScene").lock()))
	{
		light->setLightType(ape::Light::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(0,-1,0));
		light->setDiffuseColor(ape::Color(0.6f,0.6f,0.6f));
		light->setSpecularColor(ape::Color(0.7f,0.7f,0.7f,0.8));
	}

	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light04", ape::Entity::LIGHT, false, "androidSampleScene").lock()))
	{
		light->setLightType(ape::Light::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(0,-1,1));
		light->setDiffuseColor(ape::Color(0.6f,0.6f,0.6f));
		light->setSpecularColor(ape::Color(0.7f,0.7f,0.7f,0.8));
	}

	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light06", ape::Entity::LIGHT, false, "androidSampleScene").lock()))
	{
		light->setLightType(ape::Light::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(0,-1,-1));
		light->setDiffuseColor(ape::Color(0.6f,0.6f,0.6f));
		light->setSpecularColor(ape::Color(0.7f,0.7f,0.7f,0.8));
	}

	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light05", ape::Entity::LIGHT, false, "androidSampleScene").lock()))
	{
		light->setLightType(ape::Light::POINT);
        light->setLightAttenuation(LightAttenuation(1000,100,50,10));
		light->setDiffuseColor(ape::Color(1.0f,1.0f,1.0f));
		light->setSpecularColor(ape::Color(1.0f,1.0f,1.0f,1.0));

		if (auto node = mpSceneManager->createNode("pointLightNode",false,"androidSampleScene").lock())
		{
			node->setPosition(ape::Vector3(-10,2,3));
			light->setParentNode(node);
		}

	}

}

void ape::AndroidSampleScenePlugin::initGeometries()
{
    // plane
    if (auto planeNode = mpSceneManager->createNode("plane01Node", false,
                                                    "androidSampleScene").lock())
    {
        planeNode->setParentNode(mRootNodeWeak);
        planeNode->setPosition(ape::Vector3(0, 0, 0));

        if (auto plane = std::static_pointer_cast<ape::IPlaneGeometry>(
                mpSceneManager->createEntity("plane01", ape::Entity::GEOMETRY_PLANE, false,
                                             "androidSampleScene").lock()))
        {
            if (auto material = std::static_pointer_cast<ape::IManualMaterial>(
                    mpSceneManager->createEntity("plane01Material", ape::Entity::MATERIAL_MANUAL,
                                                 false, "androidSampleScene").lock()))
            {
                material->setAmbientColor(ape::Color(0.0f, 0.0f, 0.0f));
                material->setDiffuseColor(ape::Color(0.1f, 0.1f, 0.1f, 1.f));
                material->setSpecularColor(ape::Color(0.2f, 0.2f, 0.2f, 0.01f));

                plane->setMaterial(material);
            }

            plane->setParameters(ape::Vector2(), ape::Vector2(25, 20), ape::Vector2());
            plane->setParentNode(planeNode);
        }
    }

    // cone
    if (auto coneNode = mpSceneManager->createNode("cone01Node",false,"androidSampleScene").lock())
    {
        coneNode->setParentNode(mRootNodeWeak);
        coneNode->setPosition(ape::Vector3(-10,0.0,-10));

        if (auto cone = std::static_pointer_cast<ape::IConeGeometry>(
                mpSceneManager->createEntity("cone01", ape::Entity::Type::GEOMETRY_CONE,
                                             false, "androidSampleScene").lock()))
        {
            if (auto material = std::static_pointer_cast<ape::IManualMaterial>(
                    mpSceneManager->createEntity("cone01Material", ape::Entity::MATERIAL_MANUAL,
                                                 false, "androidSampleScene").lock()))
            {
                material->setAmbientColor(ape::Color(0.0f, 0.0f, 0.0f));
                material->setDiffuseColor(ape::Color(0.1f, 0.2f, 0.8f, 1.f));
                material->setSpecularColor(ape::Color(0.5f, 0.5f, 0.5f, 0.05f));

                cone->setMaterial(material);
            }

            cone->setParameters(3.0f,4.0f,1.0f,ape::Vector2());
            cone->setParentNode(coneNode);
        }
    }

    // Vibrating_Bowl_Clip
    if (auto node = mpSceneManager->createNode("vibratingBowlClipNode", false,
                                               "androidSampleScene").lock()) {
        node->setPosition(ape::Vector3(10, 0, -7));
        node->setOrientation(ape::Quaternion(ape::Degree(45),ape::Vector3(0,1,0)));
        node->setParentNode(mRootNodeWeak);

        if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
                mpSceneManager->createEntity("Vibrating_Bowl_Clip", ape::Entity::GEOMETRY_FILE,
                                             false, "androidSampleScene").lock()))
        {
			fileGeom->setUnitScale(0.01f);
			fileGeom->setFileName("VibratingBowl/Vibrating_Bowl_Clip.obj");
			fileGeom->setParentNode(node);


			if (auto cloneNode = mpSceneManager->createNode("vibratingBowlClipCloneNode", false,
                                                            "androidSampleScene").lock())
			{
				cloneNode->setPosition(ape::Vector3(18, 0, -13));
				cloneNode->setOrientation(ape::Quaternion(ape::Degree(-45), ape::Vector3(0, 1, 0)));
				cloneNode->setParentNode(mRootNodeWeak);

				if (auto cloneGeom = std::static_pointer_cast<ape::ICloneGeometry>(
						mpSceneManager->createEntity("Vibrating_Bowl_Clip_Clone",
													 ape::Entity::GEOMETRY_CLONE,
													 false, "androidSampleScene").lock()))
				{
					cloneGeom->setSourceGeometryGroupName(fileGeom->getName());
					cloneGeom->setParentNode(cloneNode);
				}
			}
		}
    }

    // Gripper
    if (auto node = mpSceneManager->createNode("GripperNode", false, "androidSampleScene").lock()) {
        node->setPosition(ape::Vector3(6, 0, 0));
        node->setParentNode(mRootNodeWeak);

        if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
                mpSceneManager->createEntity("UR_Gripper_1", ape::Entity::GEOMETRY_FILE,
                                             false, "androidSampleScene").lock())) {
            fileGeom->setUnitScale(0.2f);
            fileGeom->setFileName("UniversalRobot/UR_Gripper_1.obj");
            fileGeom->setParentNode(node);
        }
    }

    // Conveyor
    if (auto node = mpSceneManager->createNode("ConveyorNode", false,
                                                    "androidSampleScene").lock()) {
        node->setPosition(ape::Vector3(0, 0, 0));
        node->setParentNode(mRootNodeWeak);

        if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
                mpSceneManager->createEntity("Conveyor_Main", ape::Entity::GEOMETRY_FILE,
                                             false, "androidSampleScene").lock()))
        {
            fileGeom->setUnitScale(0.01f);
            fileGeom->setFileName("Conveyor/Conveyor_Main.obj");
            fileGeom->setParentNode(node);
        }
    }

    // PickAndPlace
    if (auto node = mpSceneManager->createNode("PickAndPlaceNode",false,"androidSampleScene").lock())
	{
    	node->setPosition(ape::Vector3(-7,0,7));
    	node->setParentNode(mRootNodeWeak);
    	node->setOrientation(ape::Quaternion(ape::Degree(45.f),ape::Vector3(0,1,0)));

    	if (auto nodeFloating = mpSceneManager->createNode("PickAndPlayeFloating",false,"androidSampleScene").lock())
    	{
    		nodeFloating->setPosition(ape::Vector3(-1,0,0));
    		nodeFloating->setChildrenVisibility(false);

			if (auto nodeX = mpSceneManager->createNode("PickAndPlaceNodeX", false,
														"androidSampleScene").lock()) {
				nodeX->setPosition(ape::Vector3(0, 0, 0));
				nodeX->setParentNode(nodeFloating);

				if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
						mpSceneManager->createEntity("PickAndPlaceX", ape::Entity::GEOMETRY_FILE,
													 false, "androidSampleScene").lock()))
				{
					fileGeom->setUnitScale(0.02f);
					fileGeom->setFileName("PickAndPlace/Pick_and_Place_Floating_X.obj");
					fileGeom->setParentNode(nodeX);
				}
			}

			if (auto nodeY = mpSceneManager->createNode("PickAndPlaceNodeY", false,
														"androidSampleScene").lock()) {
				nodeY->setPosition(ape::Vector3(0, 0, 0));
				nodeY->setParentNode(nodeFloating);

				if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
						mpSceneManager->createEntity("PickAndPlaceY", ape::Entity::GEOMETRY_FILE,
													 false, "androidSampleScene").lock()))
				{
					fileGeom->setUnitScale(0.02f);
					fileGeom->setFileName("PickAndPlace/Pick_and_Place_Floating_Y.obj");
					fileGeom->setParentNode(nodeY);
				}
			}

			if (auto nodeZ = mpSceneManager->createNode("PickAndPlaceNodeZ", false,
														"androidSampleScene").lock())
			{
				nodeZ->setPosition(ape::Vector3(0, 0, 5));
				nodeZ->setParentNode(nodeFloating);

				if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
						mpSceneManager->createEntity("PickAndPlaceZ", ape::Entity::GEOMETRY_FILE,
													 false, "androidSampleScene").lock())) {
					fileGeom->setUnitScale(0.02f);
					fileGeom->setFileName("PickAndPlace/Pick_and_Place_Floating_Z.obj");
					fileGeom->setParentNode(nodeZ);
				}
			}
		}

		if (auto nodeStatic = mpSceneManager->createNode("PickAndPlaceNodeStatic",false,"androidSampleScene").lock())
		{
			nodeStatic->setPosition(ape::Vector3(-5,0,-5));
			nodeStatic->setParentNode(node);

			if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
					mpSceneManager->createEntity("PickAndPlaceStatic", ape::Entity::GEOMETRY_FILE,
												 false, "androidSampleScene").lock()))
			{
				fileGeom->setUnitScale(0.02f);
				fileGeom->setFileName("PickAndPlace/Pick_and_Place_Static.obj");
				fileGeom->setParentNode(nodeStatic);
			}
		}
	}

    // LateralConveyor
	if (auto node = mpSceneManager->createNode("LateralConveyorNode",false,"androidSampleScene").lock())
	{
		node->setPosition(ape::Vector3(7, 0, 10));
		node->setParentNode(mRootNodeWeak);
		node->setOrientation(ape::Quaternion(ape::Degree(15.f), ape::Vector3(0, 1, 0)));

		if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
				mpSceneManager->createEntity("LateralConveyor", ape::Entity::GEOMETRY_FILE,
											 false, "androidSampleScene").lock()))
		{
			fileGeom->setUnitScale(0.02f);
			fileGeom->setFileName("PickAndPlace/Lateral_Conveyor.obj");
			fileGeom->setParentNode(node);
		}
	}

//	if (auto node = mpSceneManager->createNode("buildingNode",false,"androidSampleScene").lock())
//    {
//        node->setPosition(ape::Vector3(10,0,0));
//        node->setParentNode(mRootNodeWeak);
//
//        if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
//                mpSceneManager->createEntity("Building", ape::Entity::GEOMETRY_FILE,
//                        false, "androidSampleScene").lock()))
//        {
//            fileGeom->setUnitScale(0.001f);
//            fileGeom->setFileName("buildings/buildingP/building.obj");
//        }
//    }
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