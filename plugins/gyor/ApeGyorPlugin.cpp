#include "ApeGyorPlugin.h"

ape::ApeGyorPlugin::ApeGyorPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::ApeGyorPlugin::~ApeGyorPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeGyorPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::ApeGyorPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeBackground();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeGyorPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		mUserInputMacroPose.userPosition = ape::Vector3(459.301, 205.316, -75.8723);
		mUserInputMacroPose.userOrientation = ape::Quaternion(-0.593951, 0.0556969, -0.799066, -0.0749312);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
		mUserInputMacroPose.userPosition = ape::Vector3(-106.345, 235.75, -468.079);
		mUserInputMacroPose.userOrientation = ape::Quaternion(0.0745079, -0.00571105, -0.994289, -0.0762119);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
		mUserInputMacroPose.userPosition = ape::Vector3(-533.586, 232.092, -66.402);
		mUserInputMacroPose.userOrientation = ape::Quaternion(0.543329, -0.041646, -0.836036, -0.0640818);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeGyorPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeGyorPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeGyorPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeGyorPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
