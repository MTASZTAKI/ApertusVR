#include <iostream>
#include "apeUserInputMacro.h"

std::mutex ape::UserInputMacro::mpInstanceMutex;
ape::UserInputMacro* ape::UserInputMacro::mpInstance = 0;

ape::UserInputMacro::UserInputMacro()
{
	APE_LOG_FUNC_ENTER();
	mpInstance = this;
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::BROWSER, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mCameras = std::map<std::string, ape::CameraWeakPtr>();
	mUserNodeName = mpCoreConfig->getNetworkConfig().userName + "_" + mpCoreConfig->getNetworkGUID();
	if (auto userNode = mpSceneManager->createNode(mUserNodeName, true, mpCoreConfig->getNetworkGUID()).lock())
	{
		if (auto headNode = mpSceneManager->createNode(mUserNodeName + "_HeadNode", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			headNode->setParentNode(userNode);
			if (auto userMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(mUserNodeName + "_Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<double> distDouble(0.0, 1.0);
				std::vector<double> randomColors;
				for (int i = 0; i < 3; i++)
					randomColors.push_back(distDouble(gen));
				userMaterial->setDiffuseColor(ape::Color(randomColors[0], randomColors[1], randomColors[2]));
				userMaterial->setSpecularColor(ape::Color(randomColors[0], randomColors[1], randomColors[2]));
				mUserMaterial = userMaterial;
			}
			mHeadNode = headNode;
		}
		mUserNode = userNode;
	}
	if (auto rayOverlayNode = mpSceneManager->createNode("rayNode" + mUserNodeName, false, "").lock())
	{
		if (auto rayGeometry = std::static_pointer_cast<ape::IRayGeometry>(mpSceneManager->createEntity("rayQuery" + mUserNodeName, ape::Entity::GEOMETRY_RAY, false, "").lock()))
		{
			rayGeometry->setIntersectingEnabled(true);
			rayGeometry->setParentNode(rayOverlayNode);
			mRayGeometry = rayGeometry;
		}
		rayOverlayNode->setParentNode(mUserNode);
		mRayOverlayNode = rayOverlayNode;
	}
	mOverlayText = ape::TextGeometryWeakPtr();
	mOverlayTextNode = ape::NodeWeakPtr();
	mOverlayBrowser = ape::BrowserWeakPtr();
	mOverlayMouseTexture = ape::UnitTextureWeakPtr();
	mCursorText = ape::TextGeometryWeakPtr();
	mOverlayBrowserMaterial = ape::ManualMaterialWeakPtr();
	mIsLockHeadNodePosition = false;
	mIsLockUserNodePosition = false;
	mKeyPressedStringFunctions = std::vector<std::function<void(const std::string&)>>();
	mKeyReleasedStringFunctions = std::vector<std::function<void(const std::string&)>>();
	mMousePressedStringFunctions = std::vector<std::function<void(const std::string&)>>();
	mMouseReleasedStringFunctions = std::vector<std::function<void(const std::string&)>>();
	mMouseMovedFunctions = std::vector<std::function<void(const ape::Vector2&, const ape::Vector2&)>>();
	mMouseScrolledFunctions = std::vector<std::function<void(const int&)>>();
	mControllerTouchpadPressedValueFunctions = std::vector<std::function<void(const ape::Vector2&)>>();
	mControllerTouchpadReleasedValueFunctions = std::vector<std::function<void(const ape::Vector2&)>>();
	mControllerButtonReleasedStringFunctions = std::vector<std::function<void(const std::string&)>>();
	mControllerMovedFunctions = std::vector<std::function<void(const ape::Vector3&, const ape::Quaternion&, const ape::Vector3&)>>();
	mHmdMovedFunctions = std::vector<std::function<void(const ape::Vector3&, const ape::Quaternion&, const ape::Vector3&)>>();
	APE_LOG_FUNC_LEAVE();
}

ape::UserInputMacro::~UserInputMacro()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::UserInputMacro::eventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::BROWSER_OVERLAY)
	{
		mOverlayBrowser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock());
		APE_LOG_DEBUG("overlayBrowser catched");
	}
	else if (event.type == ape::Event::Type::MATERIAL_MANUAL_OVERLAY)
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			if (!mOverlayBrowserMaterial.lock())
			{
				if (event.subjectName == overlayBrowser->getName() + "_Material")
				{
					mOverlayBrowserMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->getEntity(event.subjectName).lock());
					APE_LOG_DEBUG("mOverlayBrowserMaterial catched");

				}
			}
		}
	}
	else if (event.type == ape::Event::Type::TEXTURE_UNIT_CREATE)
	{
		mOverlayMouseTexture = std::static_pointer_cast<ape::IUnitTexture>(mpSceneManager->getEntity(event.subjectName).lock());
		APE_LOG_DEBUG("overlayMouseTexture catched");
	}
	else if (event.type == ape::Event::Type::BROWSER_FOCUS_ON_EDITABLE_FIELD)
	{
		//APE_LOG_DEBUG("BROWSER_FOCUS_ON_EDITABLE_FIELD");
	}
	else if (event.type == ape::Event::Type::BROWSER_MOUSE_CLICK)
	{
		//APE_LOG_DEBUG("BROWSER_MOUSE_CLICK");
		if (auto overlayBrowserMaterial = mOverlayBrowserMaterial.lock())
		{
			if (overlayBrowserMaterial->isShowOnOverlay())
			{
				if (auto overlayBrowser = mOverlayBrowser.lock())
				{
					//APE_LOG_DEBUG("BROWSER_MOUSE_CLICK isFocusOnEditableField: " << overlayBrowser->isFocusOnEditableField());
				}
			}
		}
	}
	else if (event.type == ape::Event::Type::GEOMETRY_RAY_CREATE)
	{
		APE_LOG_DEBUG("GEOMETRY_RAY_CREATE");
	}
	else if (event.type == ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		if (auto overlayBrowserMaterial = mOverlayBrowserMaterial.lock())
		{
			if (!overlayBrowserMaterial->isShowOnOverlay())
			{
				APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION");
			}
		}
	}
}

void ape::UserInputMacro::interpolateViewPose(ViewPose pose, unsigned int milliseconds)
{
	if (auto userNode = mUserNode.lock())
	{
#ifndef ANDROID
		auto moveInterpolator = std::make_unique<ape::Interpolator>(false);
#else
		std::unique_ptr<ape::Interpolator> moveInterpolator(new ape::Interpolator(false));
#endif
		moveInterpolator->addSection(
			userNode->getPosition(),
			pose.userPosition,
			milliseconds / 1000,
			[&](ape::Vector3 pos) { userNode->setPosition(pos); }
		);

#ifndef ANDROID
		auto rotateInterpolator = std::make_unique<ape::Interpolator>(false);
#else
		std::unique_ptr<ape::Interpolator> rotateInterpolator(new ape::Interpolator(false));
#endif
		rotateInterpolator->addSection(
			userNode->getOrientation(),
			pose.userOrientation,
			milliseconds / 1000,
			[&](ape::Quaternion ori) { userNode->setOrientation(ori); }
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

ape::NodeWeakPtr ape::UserInputMacro::getUserNode()
{
	return mUserNode;
}

ape::NodeWeakPtr ape::UserInputMacro::getHeadNode()
{
	return mHeadNode;
}

ape::CameraWeakPtr ape::UserInputMacro::createCamera(std::string name)
{
	if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->createEntity(name, ape::Entity::Type::CAMERA, false, "").lock()))
	{
		APE_LOG_DEBUG("uniqueID: " << mpCoreConfig->getNetworkGUID());
		if (auto cameraNode = mpSceneManager->createNode(name + "_" + mpCoreConfig->getNetworkGUID() + "_Node", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			cameraNode->setParentNode(mHeadNode);
			if (auto cameraConeNode = mpSceneManager->createNode(name + "_" + mpCoreConfig->getNetworkGUID() + "_ConeNode", true, mpCoreConfig->getNetworkGUID()).lock())
			{
				cameraConeNode->setParentNode(cameraNode);
				cameraConeNode->rotate(ape::Degree(90.0f).toRadian(), ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::WORLD);
				if (auto cameraCone = std::static_pointer_cast<ape::IConeGeometry>(mpSceneManager->createEntity(name + "_" + mpCoreConfig->getNetworkGUID() + "_ConeGeometry", ape::Entity::GEOMETRY_CONE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					cameraCone->setParameters(10.0f, 30.0f, 1.0f, ape::Vector2(1, 1));
					cameraCone->setParentNode(cameraConeNode);
					cameraCone->setMaterial(mUserMaterial);
				}
			}
			if (auto userNameTextNode = mpSceneManager->createNode(name + "_" + mpCoreConfig->getNetworkGUID() + "_TextNode", true, mpCoreConfig->getNetworkGUID()).lock())
			{
				userNameTextNode->setParentNode(cameraNode);
				userNameTextNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
				if (auto userNameText = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity(name + "_" + mpCoreConfig->getNetworkGUID() + "_TextGeometry", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					userNameText->setCaption(name + "_" + mpCoreConfig->getNetworkGUID());
					userNameText->setParentNode(userNameTextNode);
				}
			}
			camera->setParentNode(cameraNode);
		}
		mCameras[name] = camera;
		return camera;
	}
}

void ape::UserInputMacro::updateOverLayText(std::string caption)
{
	if (auto overlayText = mOverlayText.lock())
	{
		overlayText->setCaption(caption);
	}
}

void ape::UserInputMacro::updateOverLayTextPose(ape::UserInputMacro::Pose pose)
{
	if (auto overlayTextNode = mOverlayTextNode.lock())
	{
		overlayTextNode->setPosition(pose.position);
		overlayTextNode->setOrientation(pose.orientation);
		overlayTextNode->setScale(pose.scale);
	}
}

void ape::UserInputMacro::saveViewPose()
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

void ape::UserInputMacro::createOverLayText(std::string caption)
{
	if (mOverlayText.lock())
	{
		;
	}
	else
	{
		if (auto userNode = mUserNode.lock())
		{
			if (auto overLayTextNode = mpSceneManager->createNode("overLayTextNode", false, "").lock())
			{
				overLayTextNode->setParentNode(mUserNode);
				if (auto overlayText = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("overLayText", ape::Entity::GEOMETRY_TEXT, false, "").lock()))
				{
					overlayText->setCaption(caption);
					overlayText->showOnTop(true);
					overlayText->setParentNode(overLayTextNode);
					mOverlayText = overlayText;
					mOverlayTextNode = overLayTextNode;
				}
			}
		}
	}
}

void ape::UserInputMacro::mouseMovedValue(ape::Vector2 mouseMovedValueRel, ape::Vector2 mouseMovedValueAbs)
{
	auto functionList = mMouseMovedFunctions;
	for (auto it : functionList)
	{
		it(mouseMovedValueRel, mouseMovedValueAbs);
	}
}

void ape::UserInputMacro::mouseScrolledValue(int mouseScrolledValue)
{
	auto functionList = mMouseScrolledFunctions;
	for (auto it : functionList)
	{
		it(mouseScrolledValue);
	}
}

void ape::UserInputMacro::controllerTouchpadPressedValue(ape::Vector2 axis)
{
	auto functionList = mControllerTouchpadPressedValueFunctions;
	for (auto it : functionList)
	{
		it(axis);
	}
}

void ape::UserInputMacro::controllerTouchpadReleasedValue(ape::Vector2 axis)
{
	auto functionList = mControllerTouchpadReleasedValueFunctions;
	for (auto it : functionList)
	{
		it(axis);
	}
}

void ape::UserInputMacro::controllerButtonPressedStringValue(std::string controllerButtonPressedStringValue)
{
	auto functionList = mControllerButtonReleasedStringFunctions;
	for (auto it : functionList)
	{
		it(controllerButtonPressedStringValue);
	}
}

void ape::UserInputMacro::controllerMovedValue(ape::Vector3 controllerMovedValuePos, ape::Quaternion controllerMovedValueOri, ape::Vector3 controllerMovedValueScl)
{
	auto functionList = mControllerMovedFunctions;
	for (auto it : functionList)
	{
		it(controllerMovedValuePos, controllerMovedValueOri, controllerMovedValueScl);
	}
}

void ape::UserInputMacro::hmdMovedValue(ape::Vector3 hmdMovedValuePos, ape::Quaternion hmdMovedValueOri, ape::Vector3 hmdMovedValueScl)
{
	auto functionList = mHmdMovedFunctions;
	for (auto it : functionList)
	{
		it(hmdMovedValuePos, hmdMovedValueOri, hmdMovedValueScl);
	}
}

void ape::UserInputMacro::mousePressedStringValue(std::string mousePressedStringValue)
{
	auto functionList = mMousePressedStringFunctions;
	for (auto it : functionList)
	{
		it(mousePressedStringValue);
	}
}

void ape::UserInputMacro::mouseReleasedStringValue(std::string mouseReleasedStringValue)
{
	auto functionList = mMouseReleasedStringFunctions;
	for (auto it : functionList)
	{
		it(mouseReleasedStringValue);
	}
}

void ape::UserInputMacro::keyReleasedStringValue(std::string keyReleasedStringValue)
{
	auto functionList = mKeyReleasedStringFunctions;
	for (auto it : functionList)
	{
		it(keyReleasedStringValue);
	}
}

void ape::UserInputMacro::keyPressedStringValue(std::string keyPressedStringValue)
{
	//APE_LOG_DEBUG("keyPressedStringValue: " << keyPressedStringValue);
	auto functionList = mKeyPressedStringFunctions;
	for (auto it : functionList)
	{
		auto type = &it.target_type();
		//APE_LOG_DEBUG("keyPressedStringValue: cb called: " << type->name());
		it(keyPressedStringValue);
	}
}

void ape::UserInputMacro::registerCallbackForKeyPressedStringValue(std::function<void(const std::string&)> callback)
{
	auto type = &callback.target_type();
	APE_LOG_DEBUG("registerCallbackForKeyPressedStringValue: " << type->name());
	mKeyPressedStringFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForKeyReleasedStringValue(std::function<void(const std::string&)> callback)
{
	mKeyReleasedStringFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForMouseReleasedStringValue(std::function<void(const std::string&)> callback)
{
	mMouseReleasedStringFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForMousePressedStringValue(std::function<void(const std::string&)> callback)
{
	mMousePressedStringFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForMouseMovedValue(std::function<void(const ape::Vector2&, const ape::Vector2&)> callback)
{
	mMouseMovedFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForMouseScrolledValue(std::function<void(const int&)> callback)
{
	mMouseScrolledFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForControllerTouchpadPressedValue(std::function<void(const ape::Vector2&)> callback)
{
	mControllerTouchpadPressedValueFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForControllerTouchpadReleasedValue(std::function<void(const ape::Vector2&)> callback)
{
	mControllerTouchpadReleasedValueFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForControllerButtonPressedStringValue(std::function<void(const std::string&)> callback)
{
	mControllerButtonReleasedStringFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForControllerMovedValue(std::function<void(const ape::Vector3&, const ape::Quaternion&, const ape::Vector3&)> callback)
{
	mControllerMovedFunctions.push_back(callback);
}

void ape::UserInputMacro::registerCallbackForHmdMovedValue(std::function<void(const ape::Vector3&, const ape::Quaternion&, const ape::Vector3&)> callback)
{
	mHmdMovedFunctions.push_back(callback);
}

void ape::UserInputMacro::updateOverLayBrowserCursor(OverlayBrowserCursor overlayBrowserCursor)
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

void ape::UserInputMacro::setOverLayBrowserClickedElement(std::string clickedElementName)
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		overlayBrowser->setClickedElementName(clickedElementName);
	}
}

void ape::UserInputMacro::fireOverLayBrowserHoverIn()
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		overlayBrowser->fireHoverIn();
	}
}

void ape::UserInputMacro::fireOverLayBrowserHoverOut()
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		overlayBrowser->fireHoverOut();
	}
}

void ape::UserInputMacro::sendOverlayBrowserMessage(std::string message)
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		overlayBrowser->sendMessage(message);
	}
}

std::string ape::UserInputMacro::getOverlayBrowserLastMessage()
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		return overlayBrowser->getLastMessage();
	}
}

bool ape::UserInputMacro::isOverlayBrowserShowed()
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		if (auto browserMaterial = mOverlayBrowserMaterial.lock())
		{
			return browserMaterial->isShowOnOverlay();
		}
	}
}

void ape::UserInputMacro::showOverlayBrowser(bool show)
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		while (!mOverlayBrowserMaterial.lock())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		if (auto browserMaterial = mOverlayBrowserMaterial.lock())
		{
			APE_LOG_DEBUG("showOverlayBrowser: " << show);
			browserMaterial->showOnOverlay(show, 0);
		}
	}
}

void ape::UserInputMacro::setOverlayBrowserURL(std::string url)
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		overlayBrowser->setURL(url);
	}
}

std::string ape::UserInputMacro::getOverlayBrowserURL()
{
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		return overlayBrowser->getURL();
	}
}

void ape::UserInputMacro::setOverlayBrowserKeyValue(std::string keyValue)
{
	if (keyValue == "shift" || keyValue == "right shift" || keyValue == "alt" || keyValue == "right alt" || keyValue == "ctrl")
		return;
	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		//APE_LOG_DEBUG("setOverlayBrowserKeyValue: " << keyValue);
		std::wstring keyAsWString(keyValue.begin(), keyValue.end());
		if (keyValue== "backspace")
			keyAsWString = 8;
		else if (keyValue== "tab")
			keyAsWString = 9;
		else if (keyValue== "enter")
			keyAsWString = 13;
		else if (keyValue== "space")
			keyAsWString = 32;
		else if (keyValue== "end")
			keyAsWString = 35;
		else if (keyValue== "home")
			keyAsWString = 36;
		else if (keyValue== "left")
			keyAsWString = 37;
		else if (keyValue== "up")
			keyAsWString = 38;
		else if (keyValue== "right")
			keyAsWString = 39;
		else if (keyValue== "down")
			keyAsWString = 40;
		else if (keyValue== "delete")
			keyAsWString = 46;
		else if (keyValue== "esc")
			keyAsWString = 1046;
		overlayBrowser->keyASCIIValue(keyAsWString[0]);
	}
}

void ape::UserInputMacro::rayQuery(ape::Vector3 position)
{
	if (auto rayOverlayNode = mRayOverlayNode.lock())
	{
		rayOverlayNode->setPosition(position);
		if (auto rayGeomtery = mRayGeometry.lock())
			rayGeomtery->fireIntersectionQuery();
	}
}

void ape::UserInputMacro::changeUserNode(ape::NodeWeakPtr newUserNode)
{
	if (auto newUserNodeSP = newUserNode.lock())
	{
		if (auto headNode = mHeadNode.lock())
		{
			headNode->setParentNode(newUserNodeSP);
			mUserNodeName = newUserNodeSP->getName();
			if (auto rayOverlayNode = mRayOverlayNode.lock())
			{
				rayOverlayNode->setParentNode(newUserNodeSP);
			}
			mUserNode = newUserNodeSP;
		}
	}
}

