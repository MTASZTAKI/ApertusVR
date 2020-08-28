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
	mModelsIDs = std::multimap<std::string, std::string>();
	mFileGeometryNamesScales = std::map<std::string, ape::Vector3>();
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
				std::string filePath = representation.get_file();
				std::string fileExtension = filePath.substr(filePath.find_last_of("."));
				if (fileExtension != ".jpg" && fileExtension != ".png" && fileExtension != ".JPG" && fileExtension != ".PNG")
				{
					if (auto node = mpSceneManager->createNode(asset.get_id(), true, mpCoreConfig->getNetworkGUID()).lock())
					{
						float unitScale = *representation.get_unit() / 0.01f;
						if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(asset.get_id(), ape::Entity::Type::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							//APE_LOG_DEBUG("fileGeometry: " << asset.get_id() << " filename: " << filePath);
							fileGeometry->setUnitScale(unitScale);
							fileGeometry->setParentNode(node);
							fileGeometry->setFileName(filePath);
						}
					}
				}
			}
		}
	}
}

std::string ape::VLFTSceneLoaderPlugin::findGeometryNameByModelName(std::string modelName)
{
	for (auto modelID : mModelsIDs)
	{
		if (modelName == modelID.second)
		{
			if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(modelID.first).lock()))
			{
				return fileGeometry->getName();
			}
		}
	}
	if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(modelName).lock()))
	{
		return fileGeometry->getName();
	}
	else
	{
		return std::string();
	}
}

void ape::VLFTSceneLoaderPlugin::setInitialState()
{
	auto nodes = mpSceneManager->getNodes();
	for (auto nodeWP : nodes)
	{
		if (auto node = nodeWP.second.lock())
		{
			node->setInitalState();
		}
	}
}

void ape::VLFTSceneLoaderPlugin::cloneGeometry(ape::FileGeometrySharedPtr fileGeometry, std::string id, ape::NodeSharedPtr parentNode)
{
	if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity(id, ape::Entity::Type::GEOMETRY_CLONE, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		geometryClone->setSourceGeometryGroupName(fileGeometry->getName());
		geometryClone->setParentNode(parentNode);
		//APE_LOG_DEBUG("clone: " << geometryClone->getName() << " source: " << fileGeometry->getName());
	}
}

void ape::VLFTSceneLoaderPlugin::parseModelsAndNodes()
{
	for (auto asset : mScene.get_assets())
	{
		if (auto node = mpSceneManager->createNode(asset.get_id(), true, mpCoreConfig->getNetworkGUID()).lock())
		{
			//APE_LOG_DEBUG("createNode: " << asset.get_id());
			std::weak_ptr<std::string> model = asset.get_model();
			if (model.lock())
			{
				auto fileGeometryName = findGeometryNameByModelName(*asset.get_model());
				//APE_LOG_DEBUG("findGeometryNameByModelName: " << fileGeometryName << " model: " << *asset.get_model());
				if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(fileGeometryName).lock()))
				{
					cloneGeometry(fileGeometry, asset.get_id(), node);
				}
				else
				{
					if (auto pureNode = mpSceneManager->getNode(*asset.get_model()).lock())
					{
						pureNode->setParentNode(node);
						//APE_LOG_DEBUG("pureNode: " << *asset.get_model() << " attached to: " << asset.get_id());
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
					//APE_LOG_DEBUG("parentNode: " << parentNode->getName() << " childNode: " << node->getName());
					node->setParentNode(parentNode);
				}
				else
				{
					//APE_LOG_DEBUG("parentNode not found: " << *asset.get_placement_rel_to());
				}
				std::weak_ptr<std::vector<double>> positionWP = asset.get_position();
				if (positionWP.lock())
				{
					std::vector<double> position = *asset.get_position();
					ape::Vector3 apePosition(position[0], position[1], position[2]);
					//APE_LOG_DEBUG("apePosition: " << apePosition.toString());
					node->setPosition(apePosition);
				}
				std::weak_ptr<std::vector<double>> orientationWP = asset.get_rotation();
				if (orientationWP.lock())
				{
					std::vector<double> orientation = *asset.get_rotation();
					ape::Quaternion apeOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);
					//APE_LOG_DEBUG("apeOrientation: " << apeOrientation.toString());
					node->setOrientation(apeOrientation);
				}
			}
			else
			{
				//APE_LOG_DEBUG("node not found: " << asset.get_id());
			}
		}
	}
}

void ape::VLFTSceneLoaderPlugin::parseModelsIDs()
{
	for (auto asset : mScene.get_assets())
	{
		std::weak_ptr<std::string> model = asset.get_model();
		if (model.lock())
		{
			mModelsIDs.insert(std::make_pair(*asset.get_model(), asset.get_id()));
		}
	}
}

void ape::VLFTSceneLoaderPlugin::parseVisibleNodes()
{
	for (auto asset : mScene.get_assets())
	{
		bool isAssetVisible = false;
		for (auto scene : mScene.get_scene())
		{
			if (asset.get_id() == scene)
			{
				if (auto node = mpSceneManager->getNode(asset.get_id()).lock())
				{
					//APE_LOG_DEBUG("visible: " << asset.get_id());
					isAssetVisible = true;
				}
			}
		}
		if (!isAssetVisible)
		{
			if (auto node = mpSceneManager->getNode(asset.get_id()).lock())
			{
				//APE_LOG_DEBUG("not visible: " << asset.get_id());
				node->setChildrenVisibility(false);
			}
		}
	}
}

void ape::VLFTSceneLoaderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeVLFTSceneLoaderPlugin.json";
	mApeVLFTSceneLoaderPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
//	mScene = nlohmann::json::parse(mApeVLFTSceneLoaderPluginConfigFile);
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTSceneLoaderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	parseModelsIDs();
	parseRepresentations();
	parseModelsAndNodes();
	parsePlacementRelTo();
	parseVisibleNodes();
	setInitialState();
	fclose(mApeVLFTSceneLoaderPluginConfigFile);
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
