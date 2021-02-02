#include "apeViveSRPlugin.h"

ape::ViveSRPlugin::ViveSRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	//mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ViveSRPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::ViveSRPlugin::~ViveSRPlugin()
{
	APE_LOG_FUNC_ENTER();
	//mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&ViveSRPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::ViveSRPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	APE_LOG_DEBUG("waiting for main window");
	while (mpCoreConfig->getWindowConfig().handle == nullptr && mpCoreConfig->getWindowConfig().device == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("main window was found");
	APE_LOG_DEBUG("init end");
}

void ape::ViveSRPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
