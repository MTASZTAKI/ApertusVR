#include <iostream>
#include "ApeGyorPlugin.h"

Ape::ApeGyorPlugin::ApeGyorPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mGripperLeftRootNode = Ape::NodeWeakPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeGyorPlugin::~ApeGyorPlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE)
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
				node->setInheritOrientation(false);
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
				node->setInheritOrientation(false);
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
	}
	else if (event.type == Ape::Event::Type::NODE_ORIENTATION)
	{
		if (event.subjectName == "JOINT(Rotational)(gripR5)22ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperRightHelperNodeInitialOrientation = node->getOrientation();
				//LOG(LOG_TYPE_DEBUG, "mGripperRightHelperNodeInitialOrientation: " << mGripperRightHelperNodeInitialOrientation.toString());
			}
		}
		else if (event.subjectName == "JOINT(Rotational)(gripR5)16ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperLeftHelperNodeInitialOrientation = node->getOrientation();
				//LOG(LOG_TYPE_DEBUG, "mGripperLeftHelperNodeInitialOrientation: " << mGripperLeftHelperNodeInitialOrientation.toString());
			}
		}
		else if (event.subjectName == "JOINT(Rotational)(gripR1)18ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperRightRootNodeInitialOrientation = node->getOrientation();
				//LOG(LOG_TYPE_DEBUG, "mGripperRightRootNodeInitialOrientation: " << mGripperRightRootNodeInitialOrientation.toString());
			}
		}
		else if (event.subjectName == "JOINT(Rotational)(gripR1)12ur10Gripper")
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				mGripperLeftRootNodeInitialOrientation = node->getOrientation();
				//LOG(LOG_TYPE_DEBUG, "mGripperLeftRootNodeInitialOrientation: " << mGripperLeftRootNodeInitialOrientation.toString());
			}
		}
	}
}

void Ape::ApeGyorPlugin::Init()
{
	LOG_FUNC_ENTER();

	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto planeNode = mpScene->createNode("planeNode").lock())
	{
		planeNode->setPosition(Ape::Vector3(0, -20, 0));
		if (auto planeMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("planeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			planeMaterial->setDiffuseColor(Ape::Color(0.1f, 0.1f, 0.1f));
			planeMaterial->setSpecularColor(Ape::Color(0.3f, 0.3f, 0.2f));
			if (auto plane = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("plane", Ape::Entity::GEOMETRY_PLANE).lock()))
			{
				plane->setParameters(Ape::Vector2(1, 1), Ape::Vector2(1000, 1000), Ape::Vector2(1, 1));
				plane->setParentNode(planeNode);
				plane->setMaterial(planeMaterial);
			}
		}
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.5f, 0.5f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.7f));
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, -1));
		light->setDiffuseColor(Ape::Color(0.5f, 0.5f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.7f));
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		if (auto node = mGripperLeftEndNode.lock())
		{
			node->rotate(Ape::Degree(-90).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::LOCAL);
			node->rotate(Ape::Degree(-20).toRadian(), Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::LOCAL);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	while (true)
	{
		if (auto node = mGripperRightEndNode.lock())
		{
			node->rotate(Ape::Degree(90).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::LOCAL);
			node->rotate(Ape::Degree(-20).toRadian(), Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::LOCAL);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	while (!(mGripperRightRootNode.lock() && mGripperLeftRootNode.lock() && mGripperLeftHelperNode.lock() && mGripperRightHelperNode.lock()))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	int gripperMaxValue = 255;
	int gripperMinValue = 0;
	int gripperCurrentValue = gripperMinValue;
	float degreeStep = 45.0f / (float)gripperMaxValue; //max degree (the root joint of the gripper) divided by the max value from the gripper
	Ape::Vector3 axis(0, 0, 1);
	while (true)
	{
		gripperCurrentValue++;
		LOG(LOG_TYPE_DEBUG, "gripperCurrentValue: " << gripperCurrentValue);
		Ape::Degree degree = gripperCurrentValue * degreeStep;
		Ape::Quaternion orientation;
		orientation.FromAngleAxis(Ape::Radian(degree.toRadian()), axis);
		if (auto node = mGripperLeftRootNode.lock())
		{
			node->setOrientation(mGripperLeftRootNodeInitialOrientation * orientation);
		}
		if (auto node = mGripperRightRootNode.lock())
		{
			node->setOrientation(mGripperRightRootNodeInitialOrientation * orientation);
		}
		if (auto node = mGripperLeftHelperNode.lock())
		{
			node->setOrientation(mGripperLeftHelperNodeInitialOrientation * orientation);
		}
		if (auto node = mGripperRightHelperNode.lock())
		{
			node->setOrientation(mGripperRightHelperNodeInitialOrientation * orientation);
		}
		if (gripperCurrentValue > gripperMaxValue)
		{
			gripperCurrentValue = gripperMinValue;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Step()
{

}

void Ape::ApeGyorPlugin::Stop()
{

}

void Ape::ApeGyorPlugin::Suspend()
{

}

void Ape::ApeGyorPlugin::Restart()
{

}
