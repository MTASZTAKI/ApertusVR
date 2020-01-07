#include <fstream>
#include <stdint.h>
#include "apeVLFTAnimationPlayerPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::VLFTAnimationPlayerPlugin::VLFTAnimationPlayerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&VLFTAnimationPlayerPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::VLFTAnimationPlayerPlugin::~VLFTAnimationPlayerPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}



void ape::VLFTAnimationPlayerPlugin::eventCallBack(const ape::Event & event)
{
	
}

void ape::VLFTAnimationPlayerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeVLFTAnimationPlayerPlugin.json";
	FILE* apeVLFTAnimationPlayerPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	mAnimations = nlohmann::json::parse(apeVLFTAnimationPlayerPluginConfigFile);
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
