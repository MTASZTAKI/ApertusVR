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
	mUserDeadZone = ape::Vector3(30, 30, 30);
	mSphereGeometryLeft = ape::FileGeometryWeakPtr();
	mSphereGeometryRight = ape::FileGeometryWeakPtr();
	mCurrentRoomID = -1;
	mMouseMovedValueAbs = ape::Vector2();
	mMouseScrolledValue = 0;
	mOverlayBrowserCursor = ape::UserInputMacro::OverlayBrowserCursor();
	mControllerNode = ape::NodeWeakPtr();
	mLastHmdPosition = ape::Vector3();
	mLastHmdOrientation = ape::Quaternion();
	mIsTouchPadPressed = false;
	mCurrentRotationPoses = std::vector<RotationPose>();
	mCurrentRotationPoseID = -1;
	mIsForwardMovementEnabled = false;
	mCurrentSphereAngle = 0;
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
			//sphereNode->setScale(ape::Vector3(100, 100, 100));
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
		for (auto const& room : mConfig.get_rooms())
		{
			if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(room.get_id() + "_Material_Left", ape::Entity::MATERIAL_MANUAL).lock()))
			{
				material->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
				if (auto texture = std::static_pointer_cast<ape::IFileTexture>(mpSceneManager->createEntity(room.get_id() + "_Texture_Left", ape::Entity::TEXTURE_FILE).lock()))
				{
					auto stringPos = room.get_texture().find_last_of("_") + 1;
					std::string textureFileName = room.get_texture().substr(0, stringPos) + "Top.png";
					texture->setFileName(textureFileName);
					material->setTexture(texture);
				}
			}
		}
	}
	if (auto sphereGeometryRight = mSphereGeometryRight.lock())
	{
		for (auto const& room : mConfig.get_rooms())
		{
			if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(room.get_id() + "_Material_Right", ape::Entity::MATERIAL_MANUAL).lock()))
			{
				material->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
				if (auto texture = std::static_pointer_cast<ape::IFileTexture>(mpSceneManager->createEntity(room.get_id() + "_Texture_Right", ape::Entity::TEXTURE_FILE).lock()))
				{
					auto stringPos = room.get_texture().find_last_of("_") + 1;
					std::string textureFileName = room.get_texture().substr(0, stringPos) + "Top.png";
					texture->setFileName(textureFileName);
					material->setTexture(texture);
				}
			}
		}
	}
}

void ape::apeELearningPlugin::createHotSpots()
{
	for (auto const& room : mConfig.get_rooms())
	{
		std::weak_ptr<std::vector<quicktype::Hotspot>> hotspots = room.get_hotspots();
		if (hotspots.lock())
		{
			for (auto hotspot : *room.get_hotspots())
			{
				if (auto node = mpSceneManager->createNode(hotspot.get_id()).lock())
				{
					node->setPosition(ape::Vector3(20, hotspot.get_v(), hotspot.get_z()));
					node->setOrientation(ape::Quaternion(0.707, 0.707, 0, 0));
					if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(hotspot.get_id() + "_Material", ape::Entity::MATERIAL_MANUAL).lock()))
					{
						material->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
						material->setCullingMode(ape::Material::CullingMode::NONE_CM);
						auto pos = hotspot.get_textures()[0].find("/") + 1;
						/*std::string textureFileName = mpCoreConfig->getNetworkConfig().resourceLocations[0] + "/" + hotspot.get_textures()[0].substr(pos);
						if (auto texture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity(hotspot.get_id() + "_Texture", ape::Entity::TEXTURE_MANUAL).lock()))
						{
							int width, height, channels;
							unsigned char* rgb_image = stbi_load(textureFileName.c_str(), &width, &height, &channels, STBI_rgb_alpha);
							if (rgb_image)
							{
								texture->setParameters(width, height, ape::Texture::PixelFormat::R8G8B8A8, ape::Texture::Usage::DYNAMIC_WRITE_ONLY, false, false);
								texture->setBuffer(rgb_image);
								material->setPassTexture(texture);
							}*/
						std::string textureFileName = hotspot.get_textures()[0].substr(pos);
						if (auto texture = std::static_pointer_cast<ape::IFileTexture>(mpSceneManager->createEntity(hotspot.get_id() + "_Texture", ape::Entity::TEXTURE_FILE).lock()))
						{
							texture->setFileName(textureFileName);
							material->setTexture(texture);
							if (auto planeGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity(hotspot.get_id() + "_Geometry", ape::Entity::Type::GEOMETRY_PLANE).lock()))
							{
								planeGeometry->setParameters(ape::Vector2(1, 1), ape::Vector2(hotspot.get_src_height() * hotspot.get_width(), hotspot.get_src_width() * hotspot.get_width()), ape::Vector2(1, 1));
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

void ape::apeELearningPlugin::createBrowser()
{
	mpSceneMakerMacro->makeBrowser("mainBrowser", "http://www.apertusvr.org", ape::Vector3(0, 0, -70), ape::Quaternion(1, 0, 0, 0), 102.4, 76.8, 1024, 768);
	if (auto browserNode = mpSceneManager->getNode("mainBrowser").lock())
	{
		browserNode->setChildrenVisibility(false);
		if (auto userNode = mpApeUserInputMacro->getUserNode().lock())
		{
			browserNode->setParentNode(userNode);
			browserNode->rotate(ape::Degree(-90).toRadian(), ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::LOCAL);
			browserNode->rotate(ape::Degree(-90).toRadian(), ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::LOCAL);
		}
		if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity("mainBrowserbrowser").lock()))
		{
			mBrowser = browser;
		}
		mBrowserNode = browserNode;
	}
}

void ape::apeELearningPlugin::loadRoom(std::string name)
{
	mIsForwardMovementEnabled = false;
	for (int i = 0; i < mRooms.size(); i++)
	{
		if (mRooms[i].get_id() == name)
		{
			mCurrentRoomID = i;
			break;
		}
	}
	loadRoomTextures();
	mCurrentRotationPoses.clear();
	mCurrentRotationPoses.resize(0);
	loadHotSpots();
	if (auto userNode = mpApeUserInputMacro->getUserNode().lock())
	{
		userNode->setPosition(ape::Vector3(0, 0, 0));
	}
	rotateSpheres(mRooms[mCurrentRoomID].get_rotation());
	auto graph = mConfig.get_graph();
	for (auto const& graphItem : graph)
	{
		if (graphItem.get_src() == mRooms[mCurrentRoomID].get_id())
		{
			ape::RotationPose rotationPose(graphItem.get_angle(), "room", graphItem.get_dst());
			mCurrentRotationPoses.push_back(rotationPose);
		}
	}
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
						ape::RotationPose rotationPose(20, "hotspot", hotspot.get_id());
						mCurrentRotationPoses.push_back(rotationPose);
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
		if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->getEntity(mRooms[mCurrentRoomID].get_id() + "_Material_Left").lock()))
		{
			sphereGeometryLeft->setMaterial(material);
			APE_LOG_DEBUG("A room is active with texture: " << material->getName());
		}
	}
	if (auto sphereGeometryRight = mSphereGeometryRight.lock())
	{
		if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->getEntity(mRooms[mCurrentRoomID].get_id() + "_Material_Right").lock()))
		{
			sphereGeometryRight->setMaterial(material);
			APE_LOG_DEBUG("A room is active with texture: " << material->getName());
		}
	}
}

void ape::apeELearningPlugin::rotateSpheres(int angle)
{
	mCurrentSphereAngle = angle;
	auto angleRad = ape::Radian(ape::Degree(angle).toRadian());
	auto axis = ape::Vector3(0, 1, 0);
	auto orientation = ape::Quaternion();
	orientation.FromAngleAxis(angleRad, axis);
	if (auto sphereGeometryLeft = mSphereGeometryLeft.lock())
	{
		if (auto shpereNode = sphereGeometryLeft->getParentNode().lock())
		{
			shpereNode->setOrientation(orientation);
		}
	}
	if (auto sphereGeometryRight = mSphereGeometryRight.lock())
	{
		if (auto shpereNode = sphereGeometryRight->getParentNode().lock())
		{
			shpereNode->setOrientation(orientation);
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
			mpApeUserInputMacro->getUserNode().lock()->setPosition(ape::Vector3(0, 0, 0));
			mpApeUserInputMacro->getUserNode().lock()->setOrientation(ape::Quaternion(1, 0, 0, 0));
			mpApeUserInputMacro->getHeadNode().lock()->setPosition(ape::Vector3(0, 0, 0));
			mpApeUserInputMacro->getHeadNode().lock()->setOrientation(ape::Quaternion(1, 0, 0, 0));
			if (auto controllerNode = mpSceneManager->createNode("htcVive_Controller_Node").lock())
			{
				controllerNode->setScale(ape::Vector3(10, 10, 10));
				std::string controller3DModelFileName = mpCoreConfig->getNetworkConfig().resourceLocations[0] + "/scene.gltf";
				if (auto model = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(controller3DModelFileName, ape::Entity::GEOMETRY_FILE).lock()))
				{
					model->setParentNode(controllerNode);
					model->setFileName(controller3DModelFileName);
				}
				if (auto rayNode = mpSceneManager->createNode("rayNode").lock())
				{
					if (auto rayGeometry = std::static_pointer_cast<ape::IRayGeometry>(mpSceneManager->createEntity("rayQuery", ape::Entity::GEOMETRY_RAY).lock()))
					{
						rayGeometry->setIntersectingEnabled(true);
						rayGeometry->setParentNode(rayNode);
						mRayGeometry = rayGeometry;
					}
					rayNode->setParentNode(controllerNode);
					if (auto laserLine = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity("laserLine", ape::Entity::GEOMETRY_INDEXEDLINESET).lock()))
					{
						ape::GeometryCoordinates coordinates = {0, 0, 0, 0, 400, 0 };
						ape::GeometryIndices indices = { 0, 1, -1 };
						ape::Color color(1, 0, 0);
						laserLine->setParameters(coordinates, indices, color);
						laserLine->setParentNode(rayNode);
						rayNode->setOrientation(ape::Quaternion(0.707, -0.707, 0, 0));
					}
				}
				mControllerNode = controllerNode;
			}
		}
	}
	else if (event.type == ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION called");
		if (auto rayGeometry = mRayGeometry.lock())
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
							//APE_LOG_DEBUG("ClickedNode: " << clickedNode->getName());
							std::map<std::string, quicktype::Hotspot>::iterator it;
							for (it = mNodeNamesHotSpots.begin(); it != mNodeNamesHotSpots.end(); it++)
							{
								if (clickedNode->getName() == it->first && clickedNode->getChildrenVisibility())
								{
									APE_LOG_DEBUG("A hotSpotNode was the clickedNode");
									if (auto browser = mBrowser.lock())
									{
										browser->setURL(mGameURLResourcePath[it->second.get_gameurl()]);
										if (auto browserNode =mBrowserNode.lock())
										{
											browserNode->setChildrenVisibility(true);
										}
									}
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	else if (event.type == ape::Event::Type::NODE_POSITION)
	{
		if (auto userNode = mpApeUserInputMacro->getUserNode().lock())
		{
			if (event.subjectName == userNode->getName())
			{
				ape::Vector3 position = userNode->getPosition();
				//APE_LOG_DEBUG("position.length()" << position.length() << " mUserDeadZone.length()" << mUserDeadZone.length());
				if (position.length() > mUserDeadZone.length())
				{
					if (mCurrentRotationPoses[mCurrentRotationPoseID].type == "room")
						loadRoom(mCurrentRotationPoses[mCurrentRotationPoseID].name);
				}
			}
		}
	}
}

void ape::apeELearningPlugin::keyPressedStringEventCallback(const std::string & keyValue)
{
	if (keyValue == "esc")
	{
		mpApeUserInputMacro->setOverlayBrowserURL("");
		mpApeUserInputMacro->showOverlayBrowser(false);
	}
}

void ape::apeELearningPlugin::mousePressedStringEventCallback(const std::string & keyValue)
{
	if (keyValue == "left")
	{
		if (mpApeUserInputMacro->isOverlayBrowserShowed())
		{
			mOverlayBrowserCursor.cursorClick = true;
			mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
			mpApeUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
		}
		else
		{
			mpApeUserInputMacro->rayQuery(ape::Vector3(mMouseMovedValueAbs.x, mMouseMovedValueAbs.y, 0));
		}
	}
}

void ape::apeELearningPlugin::mouseReleasedStringEventCallback(const std::string & keyValue)
{
	if (mpApeUserInputMacro->isOverlayBrowserShowed())
	{
		mOverlayBrowserCursor.cursorClick = false;
		mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
		mpApeUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
	}
}

void ape::apeELearningPlugin::mouseMovedCallback(const ape::Vector2 & mouseMovedValueRel, const ape::Vector2 & mouseMovedValueAbs)
{
	mMouseMovedValueAbs = mouseMovedValueAbs;
	if (mpApeUserInputMacro->isOverlayBrowserShowed())
	{
		ape::Vector2 cursorTexturePosition;
		cursorTexturePosition.x = (float)-mMouseMovedValueAbs.x / (float)mpCoreConfig->getWindowConfig().width;
		cursorTexturePosition.y = (float)-mMouseMovedValueAbs.y / (float)mpCoreConfig->getWindowConfig().height;
		ape::Vector2 cursorBrowserPosition;
		cursorBrowserPosition.x = (float)mMouseMovedValueAbs.x / (float)mpCoreConfig->getWindowConfig().width;
		cursorBrowserPosition.y = (float)mMouseMovedValueAbs.y / (float)mpCoreConfig->getWindowConfig().height;
		mOverlayBrowserCursor.cursorBrowserPosition = cursorBrowserPosition;
		mOverlayBrowserCursor.cursorTexturePosition = cursorTexturePosition;
		mOverlayBrowserCursor.cursorScrollPosition = ape::Vector2(0, mMouseScrolledValue);
		mOverlayBrowserCursor.cursorClick = false;
		mpApeUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
	}
}

void ape::apeELearningPlugin::mouseScrolledCallback(const int & mouseScrolledValue)
{
	mMouseScrolledValue = mMouseScrolledValue;
}

void ape::apeELearningPlugin::controllerMovedValueCallback(const ape::Vector3 & controllerPosition, const ape::Quaternion & controllerOrientation, const ape::Vector3 & controllerScale)
{
	if (auto controllerNode = mControllerNode.lock())
	{
		ape::Vector3 position = (controllerPosition * controllerScale) - mLastHmdPosition;
		controllerNode->setPosition(position);
		controllerNode->setOrientation(controllerOrientation);
		//APE_LOG_DEBUG("position: " << position.toString());
	}
}

void ape::apeELearningPlugin::hmdMovedEventCallback(const ape::Vector3& hmdMovedValuePos, const ape::Quaternion& hmdMovedValueOri, const ape::Vector3& hmdMovedValueScl)
{
	mLastHmdPosition = hmdMovedValuePos * hmdMovedValueScl;
	mLastHmdOrientation = hmdMovedValueOri;
	if (auto userNode = mpApeUserInputMacro->getUserNode().lock())
	{
		userNode->setOrientation(hmdMovedValueOri);
		//userNode->setPosition(mLastHmdPosition);
	}
	//APE_LOG_DEBUG("mLastHmdPosition: " << mLastHmdPosition.toString());
}

void ape::apeELearningPlugin::controllerTouchpadPressedValue(const ape::Vector2& axis)
{
	//APE_LOG_DEBUG("controllerTouchpadPressedValue: " << axis.toString());
	mIsTouchPadPressed = true;
	if (axis.y > 0.666 && std::abs(axis.x) < 0.333)
	{
		APE_LOG_DEBUG("UP");
		if (mCurrentRotationPoses[mCurrentRotationPoseID].type == "room")
		{
			mIsForwardMovementEnabled = true;
		}
	}
	else if (axis.x > 0.666 && std::abs(axis.y) < 0.333)
	{
		APE_LOG_DEBUG("RIGHT");
		findClosestRotationPose(mCurrentSphereAngle, "RIGHT");
		if (mCurrentRotationPoseID > -1 && mCurrentRotationPoseID < mCurrentRotationPoses.size())
		{
			rotateSpheres(mCurrentRotationPoses[mCurrentRotationPoseID].angle + 1);
		}
	}
	else if (axis.x < -0.666 && std::abs(axis.y) < 0.333)
	{
		APE_LOG_DEBUG("LEFT");
		findClosestRotationPose(mCurrentSphereAngle, "LEFT");
		if (mCurrentRotationPoseID > -1 && mCurrentRotationPoseID < mCurrentRotationPoses.size())
		{
			rotateSpheres(mCurrentRotationPoses[mCurrentRotationPoseID].angle - 1);
		}
	}
}

void ape::apeELearningPlugin::controllerTouchpadReleasedValue(const ape::Vector2 & axis)
{
	mIsTouchPadPressed = false;
}

void ape::apeELearningPlugin::controllerButtonPressedStringValue(const std::string & buttonValue)
{
	APE_LOG_DEBUG("controllerButtonPressedStringValue: " << buttonValue);
	if (buttonValue == "Grip")
	{
		if (auto browserNode = mBrowserNode.lock())
		{
			browserNode->setChildrenVisibility(false);
		}
	}
	else if (buttonValue == "Trigger")
	{
		if (auto rayGeomtery = mRayGeometry.lock())
			rayGeomtery->fireIntersectionQuery();
	}
	else if (buttonValue == "Menu")
	{

	}
}

void ape::apeELearningPlugin::findClosestRotationPose(int currentSphereAngle, std::string command)
{
	auto currentSphereAngleRotatePose = ape::RotationPose(currentSphereAngle, "sphere", "sphere");
	mCurrentRotationPoses.push_back(currentSphereAngleRotatePose);
	std::sort(mCurrentRotationPoses.begin(), mCurrentRotationPoses.end(), std::greater<ape::RotationPose>());
	for (int i = 0; i < mCurrentRotationPoses.size(); i++)
	{
		if (mCurrentRotationPoses[i].type == "sphere")
		{
			if (command == "RIGHT")
			{
				if (i - 1 > -1)
				{
					mCurrentRotationPoseID = i - 1;
					APE_LOG_DEBUG("RIGHT");
				}
				mCurrentRotationPoses.erase(mCurrentRotationPoses.begin() + i);
				return;
			}
			else if (command == "LEFT")
			{
				if (i + 1 < mCurrentRotationPoses.size())
				{
					mCurrentRotationPoseID = i + 1;
					APE_LOG_DEBUG("LEFT");
				}
				mCurrentRotationPoses.erase(mCurrentRotationPoses.begin() + i);
				return;
			}
		}
	}
}

void ape::apeELearningPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeELearningPlugin.json";
	FILE* apeELearningConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	mConfig = nlohmann::json::parse(apeELearningConfigFile);
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mpApeUserInputMacro->registerCallbackForKeyPressedStringValue(std::bind(&apeELearningPlugin::keyPressedStringEventCallback, this, std::placeholders::_1));
	mpApeUserInputMacro->registerCallbackForMousePressedStringValue(std::bind(&apeELearningPlugin::mousePressedStringEventCallback, this, std::placeholders::_1));
	mpApeUserInputMacro->registerCallbackForMouseReleasedStringValue(std::bind(&apeELearningPlugin::mouseReleasedStringEventCallback, this, std::placeholders::_1));
	mpApeUserInputMacro->registerCallbackForMouseMovedValue(std::bind(&apeELearningPlugin::mouseMovedCallback, this, std::placeholders::_1, std::placeholders::_2));
	mpApeUserInputMacro->registerCallbackForMouseScrolledValue(std::bind(&apeELearningPlugin::mouseScrolledCallback, this, std::placeholders::_1));
	mpApeUserInputMacro->registerCallbackForControllerMovedValue(std::bind(&apeELearningPlugin::controllerMovedValueCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	mpApeUserInputMacro->registerCallbackForHmdMovedValue(std::bind(&apeELearningPlugin::hmdMovedEventCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	mpApeUserInputMacro->registerCallbackForControllerTouchpadPressedValue(std::bind(&apeELearningPlugin::controllerTouchpadPressedValue, this, std::placeholders::_1));
	mpApeUserInputMacro->registerCallbackForControllerTouchpadReleasedValue(std::bind(&apeELearningPlugin::controllerTouchpadReleasedValue, this, std::placeholders::_1));
	mpApeUserInputMacro->registerCallbackForControllerButtonPressedStringValue(std::bind(&apeELearningPlugin::controllerButtonPressedStringValue, this, std::placeholders::_1));
	createHotSpots();
	while (!mSphereGeometryLeft.lock())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	createRoomTextures();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	createBrowser();
	loadRoom(mConfig.get_start_room());
	while (true)
	{
		while (mIsForwardMovementEnabled)
		{
			if (auto userNode = mpApeUserInputMacro->getUserNode().lock())
			{
				userNode->translate(ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
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
