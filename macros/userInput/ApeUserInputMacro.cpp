#include <iostream>
#include "ApeUserInputMacro.h"

Ape::UserInputMacro* Ape::UserInputMacro::mpInstance = 0;

Ape::UserInputMacro::UserInputMacro()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::BROWSER, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_UNIT, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mCameras = std::map<std::string, Ape::CameraWeakPtr>();
	std::string uniqueUserNamePrefix = mpSystemConfig->getSceneSessionConfig().uniqueUserNamePrefix;
	std::string delimiter = "-";
	auto tp = std::chrono::system_clock::now();
	auto dur = tp.time_since_epoch();
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
	mUniqueUserNodeName =  uniqueUserNamePrefix + delimiter + std::to_string(nanoseconds);
	if (auto userNode = mpSceneManager->createNode(mUniqueUserNodeName).lock())
	{
		if (auto headNode = mpSceneManager->createNode(mUniqueUserNodeName + "_HeadNode").lock())
		{
			headNode->setParentNode(userNode);
			if (auto userMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity(mUniqueUserNodeName + "_Material", Ape::Entity::MATERIAL_MANUAL).lock()))
			{
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<double> distDouble(0.0, 1.0);
				std::vector<double> randomColors;
				for (int i = 0; i < 3; i++)
					randomColors.push_back(distDouble(gen));
				userMaterial->setDiffuseColor(Ape::Color(randomColors[0], randomColors[1], randomColors[2]));
				userMaterial->setSpecularColor(Ape::Color(randomColors[0], randomColors[1], randomColors[2]));
				mUserMaterial = userMaterial;
			}
			mHeadNode = headNode;
		}
		mUserNode = userNode;
	}
	mOverlayText = Ape::TextGeometryWeakPtr();
	if (auto rayNode = mpSceneManager->createNode("rayNode" + mUniqueUserNodeName).lock())
	{
		if (auto rayGeometry = std::static_pointer_cast<Ape::IRayGeometry>(mpSceneManager->createEntity("rayQuery" + mUniqueUserNodeName, Ape::Entity::GEOMETRY_RAY).lock()))
		{
			rayGeometry->setIntersectingEnabled(true);
			rayGeometry->setParentNode(rayNode);
			mRayGeometry = rayGeometry;
		}
		rayNode->setParentNode(mUserNode);
		mRayOverlayNode = rayNode;
	}
	mOverlayBrowser = Ape::BrowserWeakPtr();
	mOverlayMouseTexture = Ape::UnitTextureWeakPtr();
	mRayGeometry = Ape::RayGeometryWeakPtr();
	mRayOverlayNode = Ape::NodeWeakPtr();
	mCursorText = Ape::TextGeometryWeakPtr();
	mKeyStringValue = std::string();
	mIsNewKeyEvent = false;
	mEnableOverlayBrowserKeyEvents = false;
	APE_LOG_FUNC_LEAVE();
}

Ape::UserInputMacro::~UserInputMacro()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::UserInputMacro::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::BROWSER_OVERLAY)
	{
		mOverlayBrowser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock());
		APE_LOG_DEBUG("overlayBrowser catched");
	}
	else if (event.type == Ape::Event::Type::TEXTURE_UNIT_CREATE)
	{
		mOverlayMouseTexture = std::static_pointer_cast<Ape::IUnitTexture>(mpSceneManager->getEntity(event.subjectName).lock());
		APE_LOG_DEBUG("overlayMouseTexture catched");
	}
	else if (event.type == Ape::Event::Type::BROWSER_FOCUS_ON_EDITABLE_FIELD)
	{
		APE_LOG_TRACE("BROWSER_FOCUS_ON_EDITABLE_FIELD");
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			if (auto focusChangedBrowser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				mEnableOverlayBrowserKeyEvents = focusChangedBrowser->isFocusOnEditableField() && overlayBrowser->getName() == focusChangedBrowser->getName();
				APE_LOG_TRACE("mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents);
				mIsNewKeyEvent = true;
			}
		}
	}
	else if (event.type == Ape::Event::Type::BROWSER_MOUSE_CLICK)
	{
		APE_LOG_DEBUG("BROWSER_MOUSE_CLICK");
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			APE_LOG_DEBUG("BROWSER_MOUSE_CLICK isFocusOnEditableField: " << overlayBrowser->isFocusOnEditableField());
		}
	}
	else if (event.type == Ape::Event::Type::GEOMETRY_RAY_CREATE)
	{
		APE_LOG_DEBUG("GEOMETRY_RAY_CREATE");
	}
	else if (event.type == Ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		APE_LOG_TRACE("GEOMETRY_RAY_INTERSECTION");
		if (auto rayGeometry = mRayGeometry.lock())
		{
			auto intersections = rayGeometry->getIntersections();
			std::list<Ape::EntityWeakPtr> intersectionList;
			std::copy(intersections.begin(), intersections.end(), std::back_inserter(intersectionList));
			APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: intersections.size: " << intersectionList.size());
			bool removeItem = false;
			std::list<Ape::EntityWeakPtr>::iterator i = intersectionList.begin();
			while (i != intersectionList.end())
			{
				removeItem = false;
				if (auto entity = i->lock())
				{
					std::string entityName = entity->getName();
					Ape::Entity::Type entityType = entity->getType();
					//APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: entityName: " << entityName << " entityType: " << entityType);
					/*if (entityName.find(mUserNode.lock()->getName()) != std::string::npos)
					removeItem = true;
					else if (entityName.find("coord") != std::string::npos)
					removeItem = true;
					else if (entityName.find("cursor") != std::string::npos)
					removeItem = true;*/
				}
				if (removeItem)
					i = intersectionList.erase(i);
				else
					i++;
			}
			APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: intersections.size after erase: " << intersectionList.size());

			bool intersectionHandled = false;
			if (intersectionList.empty())
			{
				if (mKeyStringValue == "KC_LCONTROL" || mKeyStringValue == "KC_RCONTROL")
				{
					clearNodeSelection();
					intersectionHandled = true;
				}
			}
			else
			{
				APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: -------------------------------------");
				for (auto intersection : intersectionList)
				{
					if (auto entity = intersection.lock())
					{
						std::string entityName = entity->getName();
						Ape::Entity::Type entityType = entity->getType();
						APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: entityName: " << entityName << " entityType: " << entityType);

						if (entityType >= Ape::Entity::Type::GEOMETRY_FILE && entityType <= Ape::Entity::Type::GEOMETRY_RAY)
						{
							auto geometry = std::static_pointer_cast<Ape::Geometry>(entity);
							if (auto selectedParentNode = geometry->getParentNode().lock())
							{
								APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: parentNode: " << selectedParentNode->getName());
								if (auto cursorText = mCursorText.lock())
								{
									cursorText->setCaption(entityName);
								}
								if (mKeyStringValue == "KC_LCONTROL" || mKeyStringValue == "KC_RCONTROL")
								{
									if (isNodeSelected(selectedParentNode->getName()))
										removeNodeSelection(selectedParentNode->getName());
									else
										addNodeSelection(selectedParentNode->getName());
									intersectionHandled = true;
									break;
								}
							}
						}
						else if (entityType == Ape::Entity::Type::POINT_CLOUD)
						{
							auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(entity);
							if (auto selectedParentNode = pointCloud->getParentNode().lock())
							{
								APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: parentNode: " << selectedParentNode->getName());
								if (mKeyStringValue == "KC_LCONTROL" || mKeyStringValue == "KC_RCONTROL")
								{
									if (isNodeSelected(selectedParentNode->getName()))
										removeNodeSelection(selectedParentNode->getName());
									else
										addNodeSelection(selectedParentNode->getName());
									intersectionHandled = true;
									break;
								}
							}
						}
					}
				}
			}
			APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: intersections handled: " << intersectionHandled);
		}
	}
}

void Ape::UserInputMacro::updateViewPose(ViewPose pose)
{
	if (!mEnableOverlayBrowserKeyEvents)
	{
		if (auto userNode = mUserNode.lock())
		{
			userNode->setPosition(pose.userPosition);
			userNode->setOrientation(pose.userOrientation);
		}
		if (auto headNode = mHeadNode.lock())
		{
			headNode->setPosition(pose.headPosition);
			headNode->setOrientation(pose.headOrientation);
		}
	}
}

void Ape::UserInputMacro::interpolateViewPose(ViewPose pose, unsigned int milliseconds)
{
	if (!mEnableOverlayBrowserKeyEvents)
	{
		if (auto userNode = mUserNode.lock())
		{
			auto moveInterpolator = std::make_unique<Ape::Interpolator>(false);
			moveInterpolator->addSection(
				userNode->getPosition(),
				pose.userPosition,
				milliseconds * 1000,
				[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
			);
			auto rotateInterpolator = std::make_unique<Ape::Interpolator>(false);
			rotateInterpolator->addSection(
				userNode->getOrientation(),
				pose.userOrientation,
				milliseconds * 1000,
				[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
			);
			while (!moveInterpolator->isQueueEmpty() && !rotateInterpolator->isQueueEmpty())
			{
				if (!moveInterpolator->isQueueEmpty())
					moveInterpolator->iterateTopSection();
				if (!rotateInterpolator->isQueueEmpty())
					rotateInterpolator->iterateTopSection();
			}
		}
	}
}

Ape::NodeWeakPtr Ape::UserInputMacro::getUserNode()
{
	return mUserNode;
}

Ape::NodeWeakPtr Ape::UserInputMacro::getHeadNode()
{
	return mHeadNode;
}

Ape::CameraWeakPtr Ape::UserInputMacro::createCamera(std::string name)
{
	std::string uniqueName = mUniqueUserNodeName + name;
	if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpSceneManager->createEntity(name, Ape::Entity::Type::CAMERA).lock()))
	{
		if (auto cameraNode = mpSceneManager->createNode(uniqueName + "_Node").lock())
		{
			cameraNode->setParentNode(mHeadNode);
			if (auto cameraConeNode = mpSceneManager->createNode(uniqueName + "_ConeNode").lock())
			{
				cameraConeNode->setParentNode(cameraNode);
				cameraConeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
				if (auto cameraCone = std::static_pointer_cast<Ape::IConeGeometry>(mpSceneManager->createEntity(uniqueName + "_ConeGeometry", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					cameraCone->setParameters(10.0f, 30.0f, 1.0f, Ape::Vector2(1, 1));
					cameraCone->setParentNode(cameraConeNode);
					cameraCone->setMaterial(mUserMaterial);
				}
			}
			if (auto userNameTextNode = mpSceneManager->createNode(uniqueName + "_TextNode").lock())
			{
				userNameTextNode->setParentNode(cameraNode);
				userNameTextNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
				if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity(uniqueName + "_TextGeometry", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					userNameText->setCaption(uniqueName);
					userNameText->setParentNode(userNameTextNode);
				}
			}
			camera->setParentNode(cameraNode);
		}
		mCameras[uniqueName] = camera;
		return camera;
	}
}

void Ape::UserInputMacro::updateOverLayText(std::string caption)
{
	if (auto overlayText = mOverlayText.lock())
	{
		overlayText->setCaption(caption);
	}
}

void Ape::UserInputMacro::saveViewPose()
{
	APE_LOG_FUNC_ENTER();
	if (auto userNode = mUserNode.lock())
	{
		std::ofstream userNodePoseFile;
		userNodePoseFile.open("viewPoseFile.txt", std::ios::app);
		userNodePoseFile << "[ " << userNode->getPosition().x << ", " << userNode->getPosition().y << ", " << userNode->getPosition().z << ", " <<
		userNode->getOrientation().w << ", " << userNode->getOrientation().x << ", " << userNode->getOrientation().y << ", " << userNode->getOrientation().z
		<< " ]," << std::endl;
		userNodePoseFile.close();
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::UserInputMacro::createOverLayText(std::string caption)
{
	if (mOverlayText.lock())
	{

	}
	else
	{
		if (auto userNode = mUserNode.lock())
		{
			if (auto overLayTextNode = mpSceneManager->createNode("overLayTextNode").lock())
			{
				overLayTextNode->setParentNode(mUserNode);
				overLayTextNode->setPosition(Ape::Vector3(0, 17, -50));
				if (auto overlayText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("overLayText", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					overlayText->setCaption(caption);
					overlayText->showOnTop(true);
					overlayText->setParentNode(overLayTextNode);
					mOverlayText = overlayText;
				}
			}
		}
	}
}

bool Ape::UserInputMacro::isNodeSelected(std::string nodeName)
{
	APE_LOG_TRACE("nodeName: " << nodeName);
	std::map<std::string, Ape::NodeWeakPtr>::iterator findIt;
	findIt = mSelectedNodes.find(nodeName);
	return (findIt != mSelectedNodes.end());
}

void Ape::UserInputMacro::addNodeSelection(std::string nodeName)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_TRACE("nodeName: " << nodeName);
	if (auto findNode = mpSceneManager->getNode(nodeName).lock())
	{
		Ape::NodeWeakPtrVector childNodes = findNode->getChildNodes();
		APE_LOG_DEBUG("childNodes size: " << childNodes.size());
		for (auto childNode : childNodes)
			if (auto childNodeSP = childNode.lock())
				APE_LOG_DEBUG("childNode: " << childNodeSP->getName());
		mSelectedNodes.insert(std::pair<std::string, Ape::NodeWeakPtr>(findNode->getName(), findNode));
		findNode->showBoundingBox(true);
	}
	APE_LOG_FUNC_LEAVE();
}

bool Ape::UserInputMacro::removeNodeSelection(std::string nodeName)
{
	APE_LOG_TRACE("nodeName: " << nodeName);
	std::map<std::string, Ape::NodeWeakPtr>::iterator findIt;
	findIt = mSelectedNodes.find(nodeName);
	if (findIt != mSelectedNodes.end())
	{
		if (auto findNode = findIt->second.lock())
		{
			findNode->showBoundingBox(false);
			mSelectedNodes.erase(findIt);
			return true;
		}
	}
	return false;
}

void Ape::UserInputMacro::clearNodeSelection()
{
	APE_LOG_FUNC_ENTER();
	auto nodeIt = mSelectedNodes.begin();
	while (nodeIt != mSelectedNodes.end())
	{
		if (auto selectedNodeInMap = nodeIt->second.lock())
		{
			selectedNodeInMap->showBoundingBox(false);
			nodeIt = mSelectedNodes.erase(nodeIt);
		}
		else
		{
			++nodeIt;
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::UserInputMacro::keyStringValue(std::string  keyStringValue)
{
	mKeyStringValue = keyStringValue;
}

void Ape::UserInputMacro::updateSelectedNodePose(Pose pose)
{
	for (auto nodeIt = mSelectedNodes.begin(); nodeIt != mSelectedNodes.end(); nodeIt++)
	{
		if (auto selectedNodeInMap = nodeIt->second.lock())
		{
			if (mKeyStringValue == "KC_LSHIFT" || mKeyStringValue == "KC_RSHIFT")
			{
				selectedNodeInMap->setPosition(pose.position);
			}
			if (mKeyStringValue == "KC_LCONTROL" || mKeyStringValue == "KC_RCONTROL")
			{
				selectedNodeInMap->setPosition(pose.position);
			}
			if (mKeyStringValue == "KC_LMENU" || mKeyStringValue == "KC_RMENU")
			{
				selectedNodeInMap->setPosition(pose.position);
			}
			if (mKeyStringValue == "KC_SPACE")
			{
				selectedNodeInMap->setOrientation(pose.orientation);
			}
		}
	}
}

void Ape::UserInputMacro::updateOverLayBrowserCursor(OverlayBrowserCursor overlayBrowserCursor)
{
	if (auto overlayMouseTexture = mOverlayMouseTexture.lock())
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			overlayMouseTexture->setTextureScroll(overlayBrowserCursor.cursorTexturePosition.x, overlayBrowserCursor.cursorTexturePosition.y);
			overlayBrowser->mouseMoved(overlayBrowserCursor.cursorBrowserPosition * overlayBrowser->getResoultion());
			overlayBrowser->mouseScroll(overlayBrowserCursor.cursorScrollPosition);
			overlayBrowser->mouseClick(overlayBrowserCursor.cursorClickType, overlayBrowserCursor.cursorClick);
		}
	}
}

void Ape::UserInputMacro::rayQuery(Ape::Vector3 position)
{
	if (auto rayOverlayNode = mRayOverlayNode.lock())
	{
		rayOverlayNode->setPosition(position);
		if (auto rayGeomtery = mRayGeometry.lock())
			rayGeomtery->fireIntersectionQuery();
	}
}

