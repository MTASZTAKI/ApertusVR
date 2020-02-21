#include <fstream>
#include <stdint.h>
#include "apeVLFTSceneLoaderPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::VLFTSceneLoaderPlugin::VLFTSceneLoaderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&VLFTSceneLoaderPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::VLFTSceneLoaderPlugin::~VLFTSceneLoaderPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTSceneLoaderPlugin::eventCallBack(const ape::Event & event)
{
	
}

void ape::VLFTSceneLoaderPlugin::parseRepresentations()
{
	for (auto asset : mScene.get_assets())
	{
		std::weak_ptr<std::vector<quicktype::Representation>> representations = asset.get_representations();
		if (representations.lock())
		{
			for (auto representation : *asset.get_representations())
			{
				std::stringstream fileFullPath;
				std::string filePath = representation.get_file();
				std::size_t found = filePath.find(":");
				if (found != std::string::npos)
				{
					fileFullPath << filePath;
				}
				found = filePath.find("./");
				if (found != std::string::npos)
				{
					fileFullPath << filePath;
				}
				else
				{
					std::stringstream fileFullPathSource;
					fileFullPathSource << APE_SOURCE_DIR << filePath;
					fileFullPath << fileFullPathSource.str();
				}
				std::string fileFullPathStr = fileFullPath.str();
				std::string fileName = fileFullPathStr.substr(fileFullPathStr.find_last_of("/\\") + 1);
				std::string fileExtension = fileFullPathStr.substr(fileFullPathStr.find_last_of("."));
				if (fileExtension != ".jpg" && fileExtension != ".png" && fileExtension != ".JPG" && fileExtension != ".PNG")
				{
					if (auto node = mpSceneManager->createNode(asset.get_id()).lock())
					{
						float unitScale = *representation.get_unit() / 0.01f;
						node->setScale(ape::Vector3(unitScale, unitScale, unitScale));
						APE_LOG_DEBUG("geometryNode: " << unitScale);
						if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(asset.get_id(), ape::Entity::Type::GEOMETRY_FILE).lock()))
						{
							fileGeometry->setParentNode(node);
							fileGeometry->setFileName(fileFullPathStr);
						}
					}
				}
			}
		}
	}
}

void ape::VLFTSceneLoaderPlugin::parseModels()
{
	for (auto asset : mScene.get_assets())
	{
		std::weak_ptr<std::string> model = asset.get_model();
		if (model.lock())
		{
			if (auto node = mpSceneManager->createNode(asset.get_id()).lock())
			{
				if (auto childNode = mpSceneManager->getNode(*asset.get_model()).lock())
				{
					if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(*asset.get_model()).lock()))
					{
						APE_LOG_DEBUG("childNode: " << childNode->getName());
						childNode->setParentNode(node);
					}
				}
			}
		}
	}
}

void ape::VLFTSceneLoaderPlugin::parsePlacementRelTo()
{
	for (auto asset : mScene.get_assets())
	{
		std::weak_ptr<std::string> placementRelTo = asset.get_placement_rel_to();
		if (placementRelTo.lock())
		{
			if (auto node = mpSceneManager->getNode(asset.get_id()).lock())
			{
				if (auto parentNode = mpSceneManager->getNode(*asset.get_placement_rel_to()).lock())
				{
					APE_LOG_DEBUG("parentNode: " << parentNode->getName());
					node->setParentNode(parentNode);
				}
				std::weak_ptr<std::vector<double>> positionWP = asset.get_position();
				if (positionWP.lock())
				{
					std::vector<double> position = *asset.get_position();
					ape::Vector3 apePosition(position[0], position[1], position[2]);
					APE_LOG_DEBUG("apePosition: " << apePosition.toString());
					node->setPosition(apePosition);
				}
				std::weak_ptr<std::vector<double>> orientationWP = asset.get_rotation();
				if (orientationWP.lock())
				{
					std::vector<double> orientation = *asset.get_rotation();
					ape::Quaternion apeOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);
					APE_LOG_DEBUG("apeOrientation: " << apeOrientation.toString());
					node->setOrientation(apeOrientation);
				}
			}
		}
	}
}

void ape::VLFTSceneLoaderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeVLFTSceneLoaderPlugin.json";
	FILE* apeVLFTSceneLoaderPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	mScene = nlohmann::json::parse(apeVLFTSceneLoaderPluginConfigFile);
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTSceneLoaderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	parseRepresentations();
	parseModels();
	parsePlacementRelTo();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTSceneLoaderPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTSceneLoaderPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTSceneLoaderPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTSceneLoaderPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
