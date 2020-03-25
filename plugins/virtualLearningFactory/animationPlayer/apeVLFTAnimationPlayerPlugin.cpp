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
	mpEventManager->connectEvent(ape::Event::Group::BROWSER, std::bind(&VLFTAnimationPlayerPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mTimeStampThreads = std::vector<std::thread>();
	mParsedAnimations = std::vector<Animation>();
	mNodeSpaghettiNode = std::map<std::string, std::string>();
	mClickedNode = ape::NodeWeakPtr();
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

void ape::VLFTAnimationPlayerPlugin::playAnimation(std::string nodeName, unsigned int delay, unsigned int fps, std::vector<ape::Vector3> positions, std::vector<ape::Quaternion> orientations)
{
	nodeName += "_Clone";
	if (auto node = mpSceneManager->getNode(nodeName).lock())
	{
		std::string previousOwner = node->getOwner();
		node->setOwner(mpCoreConfig->getNetworkGUID());
		unsigned int frameTime = (1.0f / fps) * 1000;
		APE_LOG_DEBUG("nodeName: " << nodeName << " delay: " << delay << " fps: " << fps << " frameTime: " << frameTime << " size: " << positions.size());
		std::this_thread::sleep_for(std::chrono::seconds(delay));
		node->setVisible(true);
		if (auto spaghettiLineNode = mpSceneManager->createNode(nodeName + "spaghettiLine", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			mNodeSpaghettiNode[node->getName()] = spaghettiLineNode->getName();
			for (int i = 0; i < positions.size(); i++)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(frameTime));
				auto previousPosition = node->getDerivedPosition();
				node->setPosition(positions[i]);
				node->setOrientation(orientations[i]);
				auto currentPosition = node->getDerivedPosition();
				if (auto spagetthiLineSection = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity(spaghettiLineNode->getName() + std::to_string(i), ape::Entity::GEOMETRY_INDEXEDLINESET, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					ape::GeometryCoordinates coordinates = {
						previousPosition.x, previousPosition.y, previousPosition.z,
						currentPosition.x, currentPosition.y, currentPosition.z, };
					ape::GeometryIndices indices = { 0, 1, -1 };
					ape::Color color(1, 0, 0);
					spagetthiLineSection->setParameters(coordinates, indices, color);
					spagetthiLineSection->setParentNode(spaghettiLineNode);
				}
				//APE_LOG_DEBUG("nodeName: " << nodeName << " positions: " << positions[i].toString() << " orientations: " << orientations[i].toString());
			}
		}
		node->setOwner(previousOwner);
	}
}

void ape::VLFTAnimationPlayerPlugin::eventCallBack(const ape::Event & event)
{
	if (event.type == ape::Event::Type::BROWSER_ELEMENT_CLICK)
	{
		if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			APE_LOG_DEBUG("BROWSER_ELEMENT_CLICK");
			if (browser->getClickedElementName() == "play")
			{
				mTimeStampThreads = std::vector<std::thread>();
				for (auto const& animation : mParsedAnimations)
				{
					mTimeStampThreads.push_back(std::thread(&VLFTAnimationPlayerPlugin::playAnimation, this, animation.nodeName, animation.delay, animation.fps, animation.positions, animation.orientations));
				}
				std::for_each(mTimeStampThreads.begin(), mTimeStampThreads.end(), std::mem_fn(&std::thread::detach));
			}
			else if (browser->getClickedElementName() == "backward")
			{
			}
			else if (browser->getClickedElementName() == "forward")
			{
			}
			else if (browser->getClickedElementName() == "pause")
			{
			}
			else if (browser->getClickedElementName() == "stop")
			{
			}
			else if (browser->getClickedElementName() == "record")
			{
			}
			else if (browser->getClickedElementName() == "bookmark")
			{
			}
			else if (browser->getClickedElementName() == "spaghetti")
			{
				if (auto clickedNode = mClickedNode.lock())
				{
					if (auto spaghettiNode = mpSceneManager->getNode(mNodeSpaghettiNode[clickedNode->getName()]).lock())
					{
						if (spaghettiNode->isVisible())
							spaghettiNode->setVisible(false);
						else
							spaghettiNode->setVisible(true);
					}
				}
			}
			else if (browser->getClickedElementName() == "view")
			{
			}
			else if (browser->getClickedElementName() == "edit")
			{
			}
			else if (browser->getClickedElementName() == "performance")
			{
			}
		}
	}
	else if (event.type == ape::Event::Type::NODE_SHOWBOUNDINGBOX)
	{
		if (auto clickedNode = mpSceneManager->getNode(event.subjectName).lock())
		{
			//APE_LOG_DEBUG("NODE_SHOWBOUNDINGBOX");
			mClickedNode = clickedNode;
		}
	}
}

void ape::VLFTAnimationPlayerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeVLFTAnimationPlayerPlugin.json";
	FILE* apeVLFTAnimationPlayerPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	mAnimations = nlohmann::json::parse(apeVLFTAnimationPlayerPluginConfigFile);
	for (const auto& node : mAnimations.get_nodes())
	{
		for (const auto& action : node.get_actions())
		{
			if (action.get_trigger().get_type() == "timestamp")
			{
				std::string fileNamePath = mpCoreConfig->getConfigFolderPath().substr(0, mpCoreConfig->getConfigFolderPath().find("virtualLearningFactory") + 23) + action.get_event().get_data();
				std::ifstream file(fileNamePath);
				std::string dataCount;
				std::getline(file, dataCount);
				std::string fps;
				std::getline(file, fps);
				std::vector<ape::Vector3> positions;
				std::vector<ape::Quaternion> orientations;
				for (int i = 0; i < atoi(dataCount.c_str()); i++)
				{
					std::string postionData;
					std::getline(file, postionData);
					auto posX = postionData.find_first_of(",") - 1;
					float x = atof(postionData.substr(1, posX).c_str());
					postionData = postionData.substr(posX + 2, postionData.length());
					auto posY = postionData.find_first_of(",") - 1;
					float y = atof(postionData.substr(0, posY).c_str());
					postionData = postionData.substr(posY + 2, postionData.length());
					auto posZ = postionData.find_first_of("]") - 1;
					float z = atof(postionData.substr(0, posZ).c_str());
					positions.push_back(ape::Vector3(x, y, z));
				}
				for (int i = 0; i < atoi(dataCount.c_str()); i++)
				{
					std::string orientationData;
					std::getline(file, orientationData);
					auto posW = orientationData.find_first_of(",") - 1;
					float w = atof(orientationData.substr(1, posW).c_str());
					orientationData = orientationData.substr(posW + 2, orientationData.length());
					auto posX = orientationData.find_first_of(",") - 1;
					float x = atof(orientationData.substr(0, posX).c_str());
					orientationData = orientationData.substr(posX + 2, orientationData.length());
					auto posY = orientationData.find_first_of(",") - 1;
					float y = atof(orientationData.substr(0, posY).c_str());
					orientationData = orientationData.substr(posY + 2, orientationData.length());
					auto posZ = orientationData.find_first_of("]" - 1);
					float z = atof(orientationData.substr(0, posZ).c_str());
					orientations.push_back(ape::Quaternion(w, x, y, z));
				}
				//APE_LOG_DEBUG("nodeName: " << node.get_name() << " delay: " << atoi(action.get_trigger().get_data().c_str()) << " fps: " << atoi(fps.c_str()) << " dataCount: " << dataCount);
				Animation animation;
				animation.nodeName = node.get_name();
				animation.delay = atoi(action.get_trigger().get_data().c_str());
				animation.fps = atoi(fps.c_str());
				animation.positions = positions;
				animation.orientations = orientations;
				mParsedAnimations.push_back(animation);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
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
