#include "ApeGyorPathPlanningPlugin.h"

Ape::ApeGyorPathPlanningPlugin::ApeGyorPathPlanningPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPathPlanningPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new Ape::SceneMakerMacro();
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
}

void Ape::ApeGyorPathPlanningPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeGround();
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
