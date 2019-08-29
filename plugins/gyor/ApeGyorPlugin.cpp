#include "apeGyorPlugin.h"

ape::apeGyorPlugin::apeGyorPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&apeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::apeGyorPlugin::~apeGyorPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGyorPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::apeGyorPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeGround();
	mpSceneMakerMacro->makeLit();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGyorPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		/*mUserInputMacroPose.userPosition = ape::Vector3(459.301, 205.316, -75.8723);
		mUserInputMacroPose.userOrientation = ape::Quaternion(-0.593951, 0.0556969, -0.799066, -0.0749312);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
		mUserInputMacroPose.userPosition = ape::Vector3(-106.345, 235.75, -468.079);
		mUserInputMacroPose.userOrientation = ape::Quaternion(0.0745079, -0.00571105, -0.994289, -0.0762119);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
		mUserInputMacroPose.userPosition = ape::Vector3(-533.586, 232.092, -66.402);
		mUserInputMacroPose.userOrientation = ape::Quaternion(0.543329, -0.041646, -0.836036, -0.0640818);
		mpApeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);*/
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGyorPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGyorPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGyorPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGyorPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
