#include <fstream>
#include <stdint.h>
#include "apeELearningPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ape::apeELearningPlugin::apeELearningPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mNodeNamesHotSpots = std::map<std::string, quicktype::Hotspot>();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mGameURLResourcePath = std::map<std::string, std::string>();
	mRooms = std::vector<quicktype::Room>();
	mUserDeadZone = ape::Vector3(10, 10, 10);
	mSphereGeometryLeft = ape::FileGeometryWeakPtr();
	mSphereGeometryRight = ape::FileGeometryWeakPtr();
	mCurrentRoomID = -1;
	APE_LOG_FUNC_LEAVE();
}

ape::apeELearningPlugin::~apeELearningPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

ape::FileGeometryWeakPtr ape::apeELearningPlugin::createSphere(std::string cameraName, std::string sphereNodeName, std::string meshName, unsigned int visibility)
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
				return sphereMeshFile;
			}
		}
	}
}

void ape::apeELearningPlugin::createRoomTextures()
{
	if (auto sphereGeometryLeft = mSphereGeometryLeft.lock())
	{
		if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(sphereGeometryLeft->getName() + "_Material", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			material->setAmbientColor(ape::Color(1.0f, 1.0f, 1.0f));
			material->setDiffuseColor(ape::Color(1.0f, 1.0f, 1.0f));
			material->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
			if (auto texture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity(sphereGeometryLeft->getName() + "_Texture", ape::Entity::TEXTURE_MANUAL).lock()))
			{
				texture->setParameters(8192, 4096, ape::Texture::PixelFormat::R8G8B8A8, ape::Texture::Usage::DYNAMIC_WRITE_ONLY, false, false);
				material->setPassTexture(texture);
				sphereGeometryLeft->setMaterial(material);
			}
		}
	}
	if (auto sphereGeometryRight = mSphereGeometryRight.lock())
	{
		if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(sphereGeometryRight->getName() + "_Material", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			material->setAmbientColor(ape::Color(1.0f, 1.0f, 1.0f));
			material->setDiffuseColor(ape::Color(1.0f, 1.0f, 1.0f));
			material->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
			if (auto texture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity(sphereGeometryRight->getName() + "_Texture", ape::Entity::TEXTURE_MANUAL).lock()))
			{
				texture->setParameters(8192, 4096, ape::Texture::PixelFormat::R8G8B8A8, ape::Texture::Usage::DYNAMIC_WRITE_ONLY, false, false);
				material->setPassTexture(texture);
				sphereGeometryRight->setMaterial(material);
			}
		}
	}
}

void ape::apeELearningPlugin::createHotSpots()
{
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeELearningPlugin.json";
	FILE* apeELearningConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	quicktype::Welcome config = nlohmann::json::parse(apeELearningConfigFile);
	for (auto const& room : config.get_rooms())
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
							if (auto planeGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity(hotspot.get_id(), ape::Entity::Type::GEOMETRY_PLANE).lock()))
							{
								planeGeometry->setParameters(ape::Vector2(1, 1), ape::Vector2(hotspot.get_src_height(), hotspot.get_src_width()), ape::Vector2(1, 1));
								planeGeometry->setParentNode(node);
								planeGeometry->setMaterial(material);
							}
						}
					}
					node->setChildrenVisibility(false);
					mNodeNamesHotSpots[node->getName()] = hotspot;
				}
			}
		}
		mRooms.push_back(room);
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
}

void ape::apeELearningPlugin::createOverlayBrowser()
{
	mpSceneMakerMacro->makeOverlayBrowser("http://www.apertusvr.org");
	//mpApeUserInputMacro->showOverlayBrowser(false);
}

void ape::apeELearningPlugin::loadNextRoom()
{
	mCurrentRoomID++;
	if (mCurrentRoomID == mRooms.size())
		mCurrentRoomID = 0;
	loadRoomTextures();
	loadHotSpots();
}

void ape::apeELearningPlugin::loadHotSpots()
{
	std::map<std::string, quicktype::Hotspot>::iterator it;
	for (it = mNodeNamesHotSpots.begin(); it != mNodeNamesHotSpots.end(); it++)
	{
		std::weak_ptr<std::vector<quicktype::Hotspot>> hotspots = mRooms[mCurrentRoomID].get_hotspots();
		if (hotspots.lock())
		{
			for (auto hotspot : *mRooms[mCurrentRoomID].get_hotspots())
			{
				if (hotspot.get_id() == it->second.get_id())
				{
					if (auto node = mpSceneManager->getNode(it->first).lock())
					{
						node->setChildrenVisibility(true);
						APE_LOG_DEBUG("A hotSpot is active: " << hotspot.get_id());
					}
				}
				else
				{
					if (auto node = mpSceneManager->getNode(it->first).lock())
					{
						node->setChildrenVisibility(false);
					}
				}
			}
		}
	}
}

void ape::apeELearningPlugin::loadRoomTextures()
{
	if (auto sphereGeometryLeft = mSphereGeometryLeft.lock())
	{
		auto pos = mRooms[mCurrentRoomID].get_texture().find_last_of("_") + 1;
		std::string textureFileName = mpCoreConfig->getNetworkConfig().resourceLocations[0] + "/" + mRooms[mCurrentRoomID].get_texture().substr(0, pos) + "Top.png";
		if (auto texture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(sphereGeometryLeft->getName() + "_Texture").lock()))
		{
			int width, height, channels;
			unsigned char* rgb_image = stbi_load(textureFileName.c_str(), &width, &height, &channels, STBI_rgb_alpha);
			if (rgb_image)
			{
				texture->setBuffer(rgb_image);
				APE_LOG_DEBUG("A room is active with texture: " << textureFileName);
			}
		}
	}
	if (auto sphereGeometryRight = mSphereGeometryRight.lock())
	{
		auto pos = mRooms[mCurrentRoomID].get_texture().find_last_of("_") + 1;
		std::string textureFileName = mpCoreConfig->getNetworkConfig().resourceLocations[0] + "/" + mRooms[mCurrentRoomID].get_texture().substr(0, pos) + "Bottom.png";
		if (auto texture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(sphereGeometryRight->getName() + "_Texture").lock()))
		{
			int width, height, channels;
			unsigned char* rgb_image = stbi_load(textureFileName.c_str(), &width, &height, &channels, STBI_rgb_alpha);
			if (rgb_image)
			{
				texture->setBuffer(rgb_image);
				APE_LOG_DEBUG("A room is active with texture: " << textureFileName);
			}
		}
	}
}

void ape::apeELearningPlugin::eventCallBack(const ape::Event & event)
{
	if (event.type == ape::Event::Type::CAMERA_CREATE)
	{
		std::size_t found = event.subjectName.find("Left");
		if (found != std::string::npos)
		{
			mSphereGeometryLeft = createSphere(event.subjectName, "sphereNodeLeft", "sphere_left.mesh", 1);
		}
		found = event.subjectName.find("Right");
		if (found != std::string::npos)
		{
			mSphereGeometryRight = createSphere(event.subjectName, "sphereNodeRight", "sphere_right.mesh", 2);
			mUserInputMacroPose = ape::UserInputMacro::ViewPose();
			mUserInputMacroPose.headPosition = ape::Vector3(0, 0, 0);
			mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
			//mpApeUserInputMacro->setUserNodePositionLock(true);
			mpApeUserInputMacro->setHeadNodePositionLock(true);
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
								}
							}
						}
					}
				}
			}
		}
	}
	/*else if (event.type == ape::Event::Type::NODE_POSITION)
	{
		if (auto userNode = mpApeUserInputMacro->getUserNode().lock())
		{
			if (event.subjectName == userNode->getName())
			{
				ape::Vector3 position = userNode->getPosition();
				if (position.x > mUserDeadZone.x)
				{
					;
				}
			}
		}
	}*/
}

void ape::apeELearningPlugin::keyStringEventCallback(const std::string & keyValue)
{
	if (keyValue == "r")
	{
		loadNextRoom();
	}
}

void ape::apeELearningPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mpApeUserInputMacro->registerCallbackForKeyStringValue(std::bind(&apeELearningPlugin::keyStringEventCallback, this, std::placeholders::_1));
	createHotSpots();
	createRoomTextures();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (!mSphereGeometryLeft.lock())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	createOverlayBrowser();
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
