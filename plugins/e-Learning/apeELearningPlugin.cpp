#include <fstream>
#include "apeELearningPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::apeELearningPlugin::apeELearningPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mNodeNamesHotSpots = std::map<std::string, quicktype::Hotspot>();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mGameURLResourcePath = std::map<std::string, std::string>();
	APE_LOG_FUNC_LEAVE();
}

ape::apeELearningPlugin::~apeELearningPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::createSphere(std::string cameraName, std::string sphereNodeName, std::string meshName, unsigned int visibility)
{
	if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(cameraName).lock()))
	{
		if (auto sphereNode = mpSceneManager->createNode(sphereNodeName).lock())
		{
			if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(meshName, ape::Entity::GEOMETRY_FILE).lock()))
			{
				sphereMeshFile->setFileName(meshName);
				sphereMeshFile->setParentNode(sphereNode);
				sphereMeshFile->setVisibilityFlag(visibility);
				camera->setVisibilityMask(visibility);
			}
		}
	}
}

void ape::apeELearningPlugin::eventCallBack(const ape::Event & event)
{
	if (event.type == ape::Event::Type::CAMERA_WINDOW)
	{
		std::size_t found = event.subjectName.find("Left");
		if (found != std::string::npos)
		{
			//createSphere(event.subjectName, "sphereNodeLeft", "sphere_left.mesh", 1);
		}
		found = event.subjectName.find("Right");
		if (found != std::string::npos)
		{
			//createSphere(event.subjectName, "sphereNodeRight", "sphere_right.mesh", 2);
		}
	}
	else if (event.type == ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA)
	{
		if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			if (auto camera = manualTexture->getSourceCamera().lock())
			{
				std::string cameraName = camera->getName();
				std::size_t found = cameraName.find("Left");
				if (found != std::string::npos)
				{
					createSphere(cameraName, "sphereNodeLeft", "sphere_left.mesh", 1);
				}
				found = cameraName.find("Right");
				if (found != std::string::npos)
				{
					createSphere(cameraName, "sphereNodeRight", "sphere_right.mesh", 2);
					mUserInputMacroPose = ape::UserInputMacro::ViewPose();
					mUserInputMacroPose.headPosition = ape::Vector3(0, 0, 0);
					mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
					mpApeUserInputMacro->setHeadNodePositionLock(true);
				}
			}
		}
	}
	else if (event.type == ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		if (auto rayGeometry = mpApeUserInputMacro->getRayGeometry().lock())
		{
			auto intersections = rayGeometry->getIntersections();
			std::list<ape::EntityWeakPtr> intersectionList;
			std::copy(intersections.begin(), intersections.end(), std::back_inserter(intersectionList));
			bool removeItem = false;
			std::list<ape::EntityWeakPtr>::iterator i = intersectionList.begin();
			for (auto intersection : intersectionList)
			{
				if (auto entity = intersection.lock())
				{
					std::string entityName = entity->getName();
					ape::Entity::Type entityType = entity->getType();
					if (entityType >= ape::Entity::Type::GEOMETRY_FILE && entityType <= ape::Entity::Type::GEOMETRY_RAY)
					{
						auto geometry = std::static_pointer_cast<ape::Geometry>(entity);
						if (auto clickedNode = geometry->getParentNode().lock())
						{
							std::map<std::string, quicktype::Hotspot>::iterator it;
							for (it = mNodeNamesHotSpots.begin(); it != mNodeNamesHotSpots.end(); it++)
							{
								if (clickedNode->getName() == it->first)
								{
									APE_LOG_DEBUG("A hotSpotNode was the clickedNode");
									mpApeUserInputMacro->setOverlayBrowserURL(mGameURLResourcePath[it->second.get_gameurl()]);
									mpApeUserInputMacro->showOverlayBrowser(true);
									//mpSceneMakerMacro->makeOverlayBrowser("https://www.youtube.com/embed/eVV5tUmky6c?vq=hd480&autoplay=1&loop=1&playlist=eVV5tUmky6c");
									//mpSceneMakerMacro->makeOverlayBrowser(mGameURLResourcePath[it->second.get_gameurl()]);
								}
							}
						}
					}
				}
			}
		}
	}
}

void ape::apeELearningPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeELearningPlugin.json";
	FILE* apeELearningConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	quicktype::Welcome config = nlohmann::json::parse(apeELearningConfigFile);
	for(auto const& room: config.get_rooms())
	{
		std::weak_ptr<std::vector<quicktype::Hotspot>> hotspots = room.get_hotspots();
		if (hotspots.lock())
		{
			for (auto hotspot : *room.get_hotspots())
			{
				if (auto node = mpSceneManager->createNode(hotspot.get_id()).lock())
				{
					node->setPosition(ape::Vector3(hotspot.get_h(), hotspot.get_v(), hotspot.get_z()));
					node->setOrientation(ape::Quaternion(1, 0, 0, 0));
					if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(hotspot.get_id() + "_Material", ape::Entity::MATERIAL_MANUAL).lock()))
					{
						material->setAmbientColor(ape::Color(1.0f, 1.0f, 1.0f));
						material->setDiffuseColor(ape::Color(1.0f, 1.0f, 1.0f));
						material->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
						std::string textureFileName;
						auto pos = hotspot.get_textures()[0].find("/") + 1;
						textureFileName = hotspot.get_textures()[0].substr(pos);
						if (auto texture = std::static_pointer_cast<ape::IFileTexture>(mpSceneManager->createEntity(textureFileName, ape::Entity::TEXTURE_FILE).lock()))
						{
							texture->setFileName(textureFileName);
							material->setPassTexture(texture);
							material->setCullingMode(ape::Material::CullingMode::CLOCKWISE);
							material->setSceneBlending(ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
							if (auto planeGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity(hotspot.get_id(), ape::Entity::Type::GEOMETRY_PLANE).lock()))
							{
								planeGeometry->setParameters(ape::Vector2(1, 1), ape::Vector2(hotspot.get_src_height() * 10, hotspot.get_src_width() * 10), ape::Vector2(1, 1));
								planeGeometry->setParentNode(node);
								planeGeometry->setMaterial(material);
							}
						}
					}
					mNodeNamesHotSpots[node->getName()] = hotspot;
				}
			}
		}
	}
	for (auto resourceLocation : mpCoreConfig->getNetworkConfig().resourceLocations)
	{
		auto found = resourceLocation.find("/plugins/e-Learning/");
		if (found != std::string::npos)
		{
			std::map<std::string, quicktype::Hotspot>::iterator it;
			for (it = mNodeNamesHotSpots.begin(); it != mNodeNamesHotSpots.end(); it++)
			{
				auto pos = it->second.get_gameurl().find("/") + 1;
				auto htmlName = it->second.get_gameurl().substr(pos);
				std::string resourcePath = "file:///" + resourceLocation + "/" + htmlName;
				mGameURLResourcePath[it->second.get_gameurl()] = resourcePath;
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeOverlayBrowser("http://www.apertusvr.org");
	mpApeUserInputMacro->showOverlayBrowser(false);
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
