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
	mTimeStampThreads = std::vector<std::thread>();
	APE_LOG_FUNC_LEAVE();
}

ape::VLFTAnimationPlayerPlugin::~VLFTAnimationPlayerPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::playBinFile(std::string name, quicktype::Action action)
{
	auto nodes = mpSceneManager->getNodes();
	auto entities = mpSceneManager->getEntities();
	if (auto node = mpSceneManager->getNode(name).lock())
	{
		std::ifstream binFileStream;
		std::string binFileNamePath = mpCoreConfig->getConfigFolderPath() + action.get_event().get_data();
		binFileStream.open(binFileNamePath, std::ios::in | std::ios::binary);
		long frameCount = 0;
		binFileStream.read(reinterpret_cast<char*>(&frameCount), sizeof(long));
		if (!binFileStream.badbit)
		{
			std::vector<ape::Vector3> positions;
			positions.resize(frameCount);
			std::vector<ape::Quaternion> orientations;
			orientations.resize(frameCount);
			binFileStream.read(reinterpret_cast<char*>(&positions[0]), frameCount * 3 * sizeof(float));
			if (!binFileStream.badbit)
			{
				binFileStream.read(reinterpret_cast<char*>(&orientations[0]), frameCount * 4 * sizeof(float));
				if (!binFileStream.badbit)
				{
					APE_LOG_DEBUG(name << " animation was timed to start at " << action.get_trigger().get_data().c_str() << " seconds after the startup signal");
					std::this_thread::sleep_for(std::chrono::milliseconds(atoi(action.get_trigger().get_data().c_str()) * 1000));
					for (long i = 0; i < frameCount; i++)
					{
						node->setPosition(positions[i]);
						node->setOrientation(orientations[i]);
						std::this_thread::sleep_for(std::chrono::milliseconds(16));
					}
					APE_LOG_DEBUG(name << " animation was played with a frame count " << frameCount);
				}
				else
				{
					APE_LOG_DEBUG("wrong orientations data: " << name);
				}
			}
			else
			{
				APE_LOG_DEBUG("wrong positions data: " << name);
			}
		}
		else
		{
			APE_LOG_DEBUG("wrong bin header: " << name);
		}
	}
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
	std::for_each(mTimeStampThreads.begin(), mTimeStampThreads.end(), std::mem_fn(&std::thread::detach));
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	std::this_thread::sleep_for(std::chrono::milliseconds(30000)); //TODO sign up an startup signal event from the teacher for example click on something....
	for (const auto& node : mAnimations.get_nodes())
	{
		for (const auto& action : node.get_actions())
		{
			if (action.get_trigger().get_type() == "timestamp")
			{
				mTimeStampThreads.push_back(std::thread(&VLFTAnimationPlayerPlugin::playBinFile, this, node.get_name(), action));
			}
		}
	}
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
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
