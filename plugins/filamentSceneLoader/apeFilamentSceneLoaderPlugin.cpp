#include <fstream>
#include <stdint.h>
#include "apeFilamentSceneLoaderPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <string>



ape::FilamentSceneLoaderPlugin::FilamentSceneLoaderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&FilamentSceneLoaderPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mModelsIDs = std::multimap<std::string, std::string>();
	mFileGeometryNamesScales = std::map<std::string, ape::Vector3>();
	mStopCalled = false;
	APE_LOG_FUNC_LEAVE();
}

ape::FilamentSceneLoaderPlugin::~FilamentSceneLoaderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mApeEntities.clear();
	mApeNodes.clear();
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentSceneLoaderPlugin::eventCallBack(const ape::Event & event)
{
	
}

/*void ape::FilamentSceneLoaderPlugin::parseGltfModel(std::string filePath)
{
	tinygltf::TinyGLTF gltf_ctx;
	std::string err;
	std::string warn;
	std::string fileExtension = filePath.substr(filePath.find_last_of("."));
    std::size_t pos = filePath.find("./");
    if (pos == std::string::npos)
        filePath = "../../samples/virtualLearningFactory/"+filePath;
	bool ret = false;
	if (fileExtension.compare(".glb") == 0) {
		std::cout << "Reading binary glTF" << std::endl;
		// assume binary glTF.
        ret = gltf_ctx.LoadBinaryFromFile(&mGltfModel[filePath], &err, &warn,
			filePath.c_str());
	}
	else {
		std::cout << "Reading ASCII glTF" << std::endl;
		// assume ascii glTF.
		ret =
        gltf_ctx.LoadASCIIFromFile(&mGltfModel[filePath], &err, &warn, filePath.c_str());
	}

	if (!warn.empty()) {
		APE_LOG_DEBUG("warning: " << warn << " filename: " << filePath);
	}

	if (!err.empty()) {
		APE_LOG_DEBUG("Err: " << err << " filename: " << filePath);
	}

	if (!ret) {
		APE_LOG_DEBUG("Failed to parse glTF: " << filePath);
	}
	mGltfModel[filePath].nodes;
	gltf_ctx.WriteGltfSceneToFile(&mGltfModel[filePath], filePath);
}*/

/*void ape::FilamentSceneLoaderPlugin::parseRepresentations()
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

				if (fileExtension == ".glb"  || fileExtension == ".gltf")
				{
                    parseGltfModel(filePath);
					if (auto node = mpSceneManager->createNode(asset.get_id(), true, mpCoreConfig->getNetworkGUID()).lock())
					{
						mApeNodes.push_back(asset.get_id());
						float unitScale = *representation.get_unit() / 0.01f;
                        if(mpSceneManager->getEntity(asset.get_id()).lock()){
                            ;
                        }
						else if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(asset.get_id(), ape::Entity::Type::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							mApeEntities.push_back(asset.get_id());
							fileGeometry->setUnitScale(unitScale);
							fileGeometry->setFileName(filePath);
                            fileGeometry->setParentNode(node);
						}
					}
				}
			}
		}
	}
}*/

/*std::string ape::FilamentSceneLoaderPlugin::findGeometryNameByModelName(std::string modelName)
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
}*/

void ape::FilamentSceneLoaderPlugin::setInitialState()
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

void ape::FilamentSceneLoaderPlugin::cloneGeometry(ape::FileGeometrySharedPtr fileGeometry, std::string id, ape::NodeSharedPtr parentNode)
{
	if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity(id, ape::Entity::Type::GEOMETRY_CLONE, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		mApeEntities.push_back(id);
		geometryClone->setSourceGeometry(fileGeometry);
		geometryClone->setParentNode(parentNode);
	}
}

/*void ape::FilamentSceneLoaderPlugin::parseModelsAndNodes()
{
	for (auto asset : mScene.get_assets())
	{
            bool exists = false;
            size_t ind = 0;
            std::string gltfName = "";
            if (auto node = mpSceneManager->createNode(asset.get_id(), true, mpCoreConfig->getNetworkGUID()).lock())
            {
				mApeNodes.push_back(asset.get_id());
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
}*/

/*void ape::FilamentSceneLoaderPlugin::parsePlacementRelTo()
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
}*/

/*void ape::FilamentSceneLoaderPlugin::parseModelsIDs()
{
	for (auto asset : mScene.get_assets())
	{
		std::weak_ptr<std::string> model = asset.get_model();
		if (model.lock())
		{
			mModelsIDs.insert(std::make_pair(*asset.get_model(), asset.get_id()));
		}
	}
}*/

/*void ape::FilamentSceneLoaderPlugin::parseVisibleNodes()
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
}*/

void ape::FilamentSceneLoaderPlugin::createResourceList()
{
	auto networkConfig = mpCoreConfig->getNetworkConfig();
	auto folder = mSceneDesc.get_context().get_repo_path();
	for (auto sceneNode : mSceneDesc.get_scene()) {
		std::stringstream fileFullPath;
		fileFullPath << "..\\..\\samples\\filamentScene\\rooms\\"<<folder<<"\\"<< sceneNode;
		mModelPaths.push_back(fileFullPath.str());
	}

	APE_LOG_FUNC_ENTER();
	/*for (const auto& entry : std::filesystem::directory_iterator(mpCoreConfig->getConfigFolderPath() + "\\rooms\\" + networkConfig.lobbyConfig.roomName)) {
		if (entry.path().extension() == ".gltf" || entry.path().extension() == ".glb") {
			mModelPaths.push_back(entry.path().u8string());
			APE_LOG_DEBUG("Model file found: " << entry.path());
		}

	}*/
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentSceneLoaderPlugin::parseModels()
{
	APE_LOG_FUNC_ENTER();
	int cnt = 0, posCnt = 0;
	for (auto modelPath : mModelPaths) {
		std::filesystem::path fullPath = modelPath;
		std::string fileName = fullPath.filename().u8string();
		std::string entityName = fileName + "_gltfEntity";
		if (auto node = mpSceneManager->createNode(fileName, true, mpCoreConfig->getNetworkGUID()).lock())
		{
			APE_LOG_DEBUG("File node created: " << fileName);
			mApeNodes.push_back(fileName);
			if (mpSceneManager->getEntity(fileName).lock()) {
				;
			}
			else if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(entityName, ape::Entity::Type::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				APE_LOG_DEBUG("File geometry created: " << entityName);
				mApeEntities.push_back(entityName);
				fileGeometry->setFileName(modelPath);
				fileGeometry->setParentNode(node);
			}
			if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(entityName).lock())) {
				for (size_t i = 0; i < mSceneDesc.get_clones()[cnt]; i++) {
					std::string cloneName = fileName + "_Clone" + std::to_string(i);
					if (auto cloneNode = mpSceneManager->createNode(cloneName, true, mpCoreConfig->getNetworkGUID()).lock())
					{
						if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity(cloneName, ape::Entity::Type::GEOMETRY_CLONE, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							mApeEntities.push_back(cloneName);
							APE_LOG_DEBUG("Geometry clone created: " << cloneName);
							geometryClone->setSourceGeometry(fileGeometry);
						}
						cloneNode->setParentNode(node);
					}
				}	
			}
			node->setChildrenVisibility(true);
			node->setPosition(ape::Vector3(0.0, 0.0, 0.0));

		}
		if (auto node = mpSceneManager->getNode(fileName).lock()) {
			for (size_t i = 0; i < mSceneDesc.get_clones()[cnt]; i++) {
				std::string cloneName = fileName + "_Clone" + std::to_string(i);
				if (auto cloneNode = mpSceneManager->getNode(cloneName).lock()) {
					if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->getEntity(cloneName).lock()))
					{
						geometryClone->setParentNode(cloneNode);
						node->setChildrenVisibility(true);
					}
					auto pos = mSceneDesc.get_positions()[posCnt++];
					cloneNode->setPosition(ape::Vector3(pos[0], pos[1], pos[2]));
				}
			}
		}
		cnt++;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentSceneLoaderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "/apeFilamentSceneLoaderPlugin.json";
	mApeFilamentSceneLoaderPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	mSceneDesc = nlohmann::json::parse(mApeFilamentSceneLoaderPluginConfigFile);

	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentSceneLoaderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	//parseModelsIDs();
	//parseRepresentations();
	/*parseModelsAndNodes();
	parsePlacementRelTo();
	parseVisibleNodes();
	setInitialState();*/
	//fclose(mApeFilamentSceneLoaderPluginConfigFile);
	createResourceList();
	parseModels();
	while (!mStopCalled)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentSceneLoaderPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentSceneLoaderPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	mStopCalled = true;
	mGltfModel.clear();
	for (auto item : mApeEntities)
		mpSceneManager->deleteEntity(item);
	for (auto item : mApeNodes)
		mpSceneManager->deleteNode(item);
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentSceneLoaderPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentSceneLoaderPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
