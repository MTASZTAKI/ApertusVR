#include "ApeTexasEEGPlugin.h"

ape::ApeTexasEEGPlugin::ApeTexasEEGPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ApeTexasEEGPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mScore = 0;
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	APE_LOG_FUNC_LEAVE();
}

ape::ApeTexasEEGPlugin::~ApeTexasEEGPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&ApeTexasEEGPlugin::eventCallBack, this, std::placeholders::_1));
	if (mBubbleManager)
	{
		delete mBubbleManager;
		mBubbleManager = NULL;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTexasEEGPlugin::eventCallBack(const ape::Event& event)
{

}

void ape::ApeTexasEEGPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mGameManager = new TexasEEG::GameManager(mpApeUserInputMacro->getUserNode());
	mGameManager->Start();

	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTexasEEGPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTexasEEGPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTexasEEGPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTexasEEGPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTexasEEGPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
