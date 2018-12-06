#include "ApeGyorPathPlanningPlugin.h"

Ape::ApeGyorPathPlanningPlugin::ApeGyorPathPlanningPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPathPlanningPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeGyorPathPlanningPlugin::~ApeGyorPathPlanningPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPathPlanningPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPathPlanningPlugin::eventCallBack(const Ape::Event& event)
{
	/*if (event.type == Ape::Event::Type::NODE_CREATE)
	{
		if (event.subjectName == "JOINT(Rotational)(gripR1)12ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperLeftRootNode = node;
			}
		}
		else if (event.subjectName == "JOINT(Rotational)(gripR3)14ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperLeftEndNode = node;
			}
		}
		else if (event.subjectName == "JOINT(Rotational)(gripR5)16ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperLeftHelperNode = node;
			}
		}
		else if (event.subjectName == "JOINT(Rotational)(gripR1)18ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperRightRootNode = node;
			}
		}
		else if (event.subjectName == "JOINT(Rotational)(gripR3)20ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperRightEndNode = node;
			}
		}
		else if (event.subjectName == "JOINT(Rotational)(gripR5)22ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperRightHelperNode = node;
			}
		}
	}*/
	//else if (event.type == Ape::Event::Type::NODE_ORIENTATION)
	//{
	//	if (event.subjectName == "JOINT(Rotational)(gripR5)22ur10Gripper")
	//	{
	//		if (auto node = mpScene->getNode(event.subjectName).lock())
	//		{
	//			mGripperRightHelperNodeInitialOrientation = node->getOrientation();
	//			//APE_LOG_DEBUG("mGripperRightHelperNodeInitialOrientation: " << mGripperRightHelperNodeInitialOrientation.toString());
	//		}
	//	}
	//	else if (event.subjectName == "JOINT(Rotational)(gripR5)16ur10Gripper")
	//	{
	//		if (auto node = mpScene->getNode(event.subjectName).lock())
	//		{
	//			mGripperLeftHelperNodeInitialOrientation = node->getOrientation();
	//			//APE_LOG_DEBUG("mGripperLeftHelperNodeInitialOrientation: " << mGripperLeftHelperNodeInitialOrientation.toString());
	//		}
	//	}
	//	else if (event.subjectName == "JOINT(Rotational)(gripR1)18ur10Gripper")
	//	{
	//		if (auto node = mpScene->getNode(event.subjectName).lock())
	//		{
	//			mGripperRightRootNodeInitialOrientation = node->getOrientation();
	//			//APE_LOG_DEBUG("mGripperRightRootNodeInitialOrientation: " << mGripperRightRootNodeInitialOrientation.toString());
	//		}
	//	}
	//	else if (event.subjectName == "JOINT(Rotational)(gripR1)12ur10Gripper")
	//	{
	//		if (auto node = mpScene->getNode(event.subjectName).lock())
	//		{
	//			mGripperLeftRootNodeInitialOrientation = node->getOrientation();
	//			//APE_LOG_DEBUG("mGripperLeftRootNodeInitialOrientation: " << mGripperLeftRootNodeInitialOrientation.toString());
	//		}
	//	}
	//	else if (event.subjectName == "JOINT(Rotational)(gripR3)14ur10Gripper")
	//	{
	//		if (auto node = mpScene->getNode(event.subjectName).lock())
	//		{
	//			mGripperLeftEndNodeInitialOrientation = node->getOrientation();
	//			//APE_LOG_DEBUG("mGripperLeftEndNodeInitialOrientation: " << mGripperLeftEndNodeInitialOrientation.toString());
	//		}
	//	}
	//	else if (event.subjectName == "JOINT(Rotational)(gripR3)20ur10Gripper")
	//	{
	//		if (auto node = mpScene->getNode(event.subjectName).lock())
	//		{
	//			mGripperRightEndNodeInitialOrientation = node->getOrientation();
	//			//APE_LOG_DEBUG("mGripperRightEndNodeInitialOrientation: " << mGripperRightEndNodeInitialOrientation.toString());
	//		}
	//	}
	//}
}

void Ape::ApeGyorPathPlanningPlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto planeNode = mpSceneManager->createNode("planeNode").lock())
	{
		planeNode->setPosition(Ape::Vector3(0, -20, 0));
		if (auto planeMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("planeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			planeMaterial->setDiffuseColor(Ape::Color(0.1f, 0.1f, 0.1f));
			planeMaterial->setSpecularColor(Ape::Color(0.3f, 0.3f, 0.2f));
			if (auto plane = std::static_pointer_cast<Ape::IPlaneGeometry>(mpSceneManager->createEntity("plane", Ape::Entity::GEOMETRY_PLANE).lock()))
			{
				plane->setParameters(Ape::Vector2(1, 1), Ape::Vector2(1000, 1000), Ape::Vector2(1, 1));
				plane->setParentNode(planeNode);
				plane->setMaterial(planeMaterial);
			}
		}
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.5f, 0.5f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.7f));
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, -1));
		light->setDiffuseColor(Ape::Color(0.5f, 0.5f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.7f));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPathPlanningPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	//while (!(mGripperRightRootNode.lock() && mGripperLeftRootNode.lock() && mGripperLeftHelperNode.lock() && mGripperRightHelperNode.lock()
	//	&& mGripperRightEndNode.lock() && mGripperLeftEndNode.lock()))
	//{
	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//}
	//mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPathPlanningPlugin::eventCallBack, this, std::placeholders::_1));
	//int gripperMaxValue = 255;
	//int gripperMinValue = 0;
	//int gripperCurrentValue = gripperMinValue;
	//float degreeStep = 45.0f / (float)gripperMaxValue; //max degree (the root joint of the gripper) divided by the max value from the gripper
	//Ape::Vector3 axis(0, 0, 1);

	//if (auto node = mpScene->getNode("JOINT(Rotational)(q1)2ur10Gripper").lock())
	//{
	//	node->setOrientation(Ape::Quaternion(0.707, 0 , 0.707, 0));
	//}
	//if (auto node = mpScene->getNode("JOINT(Rotational)(q4)5ur10Gripper").lock())
	//{
	//	node->setOrientation(Ape::Quaternion(0.707, 0, 0.707, 0));
	//}

	while (true)
	{
		//APE_LOG_DEBUG("gripperCurrentValue: " << gripperCurrentValue);
		/*Ape::Degree degree = gripperCurrentValue * degreeStep;
		Ape::Quaternion orientation;
		orientation.FromAngleAxis(Ape::Radian(degree.toRadian()), axis);
		if (auto gripperLeftRootNode = mGripperLeftRootNode.lock())
		{
			gripperLeftRootNode->setOrientation(mGripperLeftRootNodeInitialOrientation * orientation);
			if (auto gripperLeftEndNode = mGripperLeftEndNode.lock())
			{
				gripperLeftEndNode->setOrientation((mGripperLeftEndNodeInitialOrientation * Ape::Quaternion(0.9980631, 0, 0, -0.0622098)) * gripperLeftRootNode->getOrientation().Inverse());
			}
		}
		if (auto gripperRightRootNode = mGripperRightRootNode.lock())
		{
			gripperRightRootNode->setOrientation(mGripperRightRootNodeInitialOrientation * orientation);
			if (auto gripperRightEndNode = mGripperRightEndNode.lock())
			{
				gripperRightEndNode->setOrientation((mGripperRightEndNodeInitialOrientation * Ape::Quaternion(0.9980631, 0, 0, -0.0622098)) * gripperRightRootNode->getOrientation().Inverse());
			}
		}
		if (auto gripperLeftHelperNode = mGripperLeftHelperNode.lock())
		{
			gripperLeftHelperNode->setOrientation(mGripperLeftHelperNodeInitialOrientation * orientation);
		}
		if (auto gripperRightHelperNode = mGripperRightHelperNode.lock())
		{
			gripperRightHelperNode->setOrientation(mGripperRightHelperNodeInitialOrientation * orientation);
		}
		if (gripperCurrentValue == gripperMaxValue)
		{
			gripperCurrentValue = gripperMinValue;
		}
		else
		{
			gripperCurrentValue++;
		}*/
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPathPlanningPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPathPlanningPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPathPlanningPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPathPlanningPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
