#include "ApeGyorPlugin.h"

Ape::ApeGyorPlugin::ApeGyorPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpApeUserInputMacro = Ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = Ape::UserInputMacro::ViewPose();
	mpSceneMakerMacro = new Ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeGyorPlugin::~ApeGyorPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::eventCallBack(const Ape::Event& event)
{
	
}

void Ape::ApeGyorPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeBackground();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		mUserInputMacroPose.userPosition = Ape::Vector3(459.301, 205.316, -75.8723);
		mUserInputMacroPose.userOrientation = Ape::Quaternion(-0.593951, 0.0556969, -0.799066, -0.0749312);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
		mUserInputMacroPose.userPosition = Ape::Vector3(-106.345, 235.75, -468.079);
		mUserInputMacroPose.userOrientation = Ape::Quaternion(0.0745079, -0.00571105, -0.994289, -0.0762119);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
		mUserInputMacroPose.userPosition = Ape::Vector3(-533.586, 232.092, -66.402);
		mUserInputMacroPose.userOrientation = Ape::Quaternion(0.543329, -0.041646, -0.836036, -0.0640818);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
