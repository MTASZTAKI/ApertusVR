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
	mGeometryScale = std::map<std::string, float>();
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

void ape::VLFTSceneLoaderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeVLFTSceneLoaderPlugin.json";
	FILE* apeVLFTSceneLoaderPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	mScene = nlohmann::json::parse(apeVLFTSceneLoaderPluginConfigFile);
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
					if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(asset.get_id(), ape::Entity::Type::GEOMETRY_FILE).lock()))
					{
						fileGeometry->setFileName(fileFullPathStr);
						mGeometryScale[asset.get_id()] = *representation.get_unit() / 0.01f;
					}
				}
			}
		}
	}
	for (auto asset : mScene.get_assets())
	{
		std::weak_ptr<std::string> model = asset.get_model();
		if (model.lock())
		{
			if (auto node = mpSceneManager->createNode(asset.get_id()).lock())
			{
				if (auto parentNode = mpSceneManager->getNode(*asset.get_placement_rel_to()).lock())
				{
					APE_LOG_DEBUG("parentNode: " << parentNode->getName());
					node->setParentNode(parentNode);
				}
				std::weak_ptr<std::vector<double>> position = asset.get_position();
				if (position.lock())
				{
					ape::Vector3 apePosition;
					int index = 0;
					for (auto value : *asset.get_position())
					{
						if (index = 0)
							apePosition.x = value;
						if (index = 1)
							apePosition.y = value;
						if (index = 2)
							apePosition.z = value;
						index++;
					}
					node->setPosition(apePosition);
					float unit = mGeometryScale[*asset.get_model()];
					node->setScale(ape::Vector3(unit, unit, unit));
					if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(*asset.get_model()).lock()))
					{
						if (auto fileGeometryParentNode = fileGeometry->getParentNode().lock())
						{
							if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity(asset.get_id(), ape::Entity::Type::GEOMETRY_CLONE).lock()))
							{
								APE_LOG_DEBUG("clone: " << geometryClone->getName());
								geometryClone->setSourceGeometry(fileGeometry);
								geometryClone->setParentNode(node);
							}
						}
						else
						{
							APE_LOG_DEBUG("node: " << node->getName());
							fileGeometry->setParentNode(node);
						}
					}
				}
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTSceneLoaderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
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
