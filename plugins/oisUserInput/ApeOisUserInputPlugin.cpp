/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "ApeOisUserInputPlugin.h"


Ape::OISUserInputPlugin::OISUserInputPlugin()
{
	mpKeyboard = NULL;
	mpMouse = NULL;
	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mKeyCodeMap = std::map<OIS::KeyCode, bool>();
	mTranslateSpeedFactor = 3;
	mRotateSpeedFactor = 1;
	mOverlayBrowser = Ape::BrowserWeakPtr();
	mOverlayMouseTexture = Ape::UnitTextureWeakPtr();
	mIsNewKeyEvent = false;
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::BROWSER, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_UNIT, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mUserNodePoses = std::vector<UserNodePose>();

	/*gallery*/
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-941.765, 47.9583, 631.489), Ape::Quaternion(0.977507, -0.210903, -4.55392e-07, 2.41213e-07)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-895.301, -22.3373, -44.3792), Ape::Quaternion(0.9987, -0.0509765, -4.37447e-07, 3.08579e-07)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, 0), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, 0), Ape::Quaternion(0, 0, 1, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, 0), Ape::Quaternion(1, 0, -0.05, 0)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, -600), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, -600), Ape::Quaternion(0, 0, 1, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, -600), Ape::Quaternion(1, 0, -0.05, 0)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, -600, -500), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, -600, -500), Ape::Quaternion(0, 0, 1, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, -600, -500), Ape::Quaternion(1, 0, -0.05, 0)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, 0, -500), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, 0, -500), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, 0, -500), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, -600, 100), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, 0, 100), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, 0, 100), Ape::Quaternion(1, 0, 0, 0)));

	
	/*mUserNodePoses.push_back(UserNodePose(Ape::Vector3(100.079, -583, -478.537), Ape::Quaternion(0.250597, 0, 0.968092, 0)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-50.2942, -267, -39.543), Ape::Quaternion(0.108578, 0.000922807, -0.994053, 0.00844969)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(109.65, -254.404, 683.883), Ape::Quaternion(-0.994235, -0.00845123, -0.106884, 0.000908416)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(182.939, -582, 322.612), Ape::Quaternion(0.299624, 3.09414e-10, 0.954057, -8.78543e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-33.8135, -582, 524.087), Ape::Quaternion(-0.0356937, -1.2817e-12, 0.999363, -9.31436e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(136.762, -582, 607.147), Ape::Quaternion(-0.938136, -8.84368e-10, -0.346269, 2.9235e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-636.561, -267, 251.884), Ape::Quaternion(0.809606, 7.34888e-10, -0.586974, 5.72286e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(47.2687, -579, -656.354), Ape::Quaternion(-0.152424, -1.73481e-10, -0.988316, 9.15137e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(95.8991, -579, -640.988), Ape::Quaternion(-0.152424, -1.73481e-10, -0.988316, 9.15137e-10)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(332.168, -562, -343.67), Ape::Quaternion(-0.919263, 0, 0.393645, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(419.488, -510.18, -28.347), Ape::Quaternion(-0.728918, 0.0309977, 0.683282, 0.029057)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-269.323, -469.674, 275.482), Ape::Quaternion(0.506584, -0.0475041, 0.857121, 0.0803752)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-374.755, -506.984, 53.2619), Ape::Quaternion(0.689286, -0.0528335, 0.720448, 0.0552219)));*/

	mUserNodePosesToggleIndex = 0;
	mIsKeyPressed = false;
}

Ape::OISUserInputPlugin::~OISUserInputPlugin()
{
	std::cout << "OISUserInputPlugin dtor" << std::endl;

	delete mpKeyboard; 
	delete mpMouse;
}

void Ape::OISUserInputPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName)
		mUserNode = mpScene->getNode(event.subjectName);
	else if (event.type == Ape::Event::Type::BROWSER_OVERLAY)
	{
		mOverlayBrowser = std::static_pointer_cast<Ape::IBrowser>(mpScene->getEntity(event.subjectName).lock());
		std::cout << "overlayBrowser catched" << std::endl;
	}
	else if (event.type == Ape::Event::Type::TEXTURE_UNIT_CREATE)
	{
		mOverlayMouseTexture = std::static_pointer_cast<Ape::IUnitTexture>(mpScene->getEntity(event.subjectName).lock());
		std::cout << "overlayMouseTexture catched" << std::endl;
	}
	else if (event.type == Ape::Event::Type::BROWSER_FOCUS_ON_EDITABLE_FIELD)
	{
		std::cout << "Ape::OISUserInputPlugin::eventCallBack(): BROWSER_FOCUS_ON_EDITABLE_FIELD: " << std::endl;
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			if (auto focusChangedBrowser = std::static_pointer_cast<Ape::IBrowser>(mpScene->getEntity(event.subjectName).lock()))
			{
				mEnableOverlayBrowserKeyEvents = focusChangedBrowser->isFocusOnEditableField() && overlayBrowser->getName() == focusChangedBrowser->getName();
				std::cout << "Ape::OISUserInputPlugin::eventCallBack(): mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents << std::endl;
				mIsNewKeyEvent = true;
			}
		}
	}
}

void Ape::OISUserInputPlugin::Init()
{
	std::cout << "OISUserInputPlugin::Init" << std::endl;

	Ape::OisWindowConfig oisWindowConfig;
	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "\\ApeOisUserInputPlugin.json";
	FILE* apeOisUserInputConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeOisUserInputConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeOisUserInputConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& inputs = jsonDocument["inputs"];
			for (auto& input : inputs.GetArray())
			{
				oisWindowConfig.inputs.push_back(input.GetString());
			}
		}
		fclose(apeOisUserInputConfigFile);
	}


	std::cout << "OISUserInputPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "OisUserInputPlugin main window was found" << std::endl;

	std::stringstream hwndStrStream;
	hwndStrStream << mpMainWindow->getHandle();
	std::stringstream windowHndStr;
	windowHndStr << std::stoul(hwndStrStream.str(), nullptr, 16);
	oisWindowConfig.handler = windowHndStr.str();
	oisWindowConfig.width = mpMainWindow->getWidth();
	oisWindowConfig.height = mpMainWindow->getHeight();



	OIS::ParamList pl;
	pl.insert(std::make_pair("WINDOW", oisWindowConfig.handler));
#ifdef WIN32
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#endif
	OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
	for (auto it = oisWindowConfig.inputs.begin(); it != oisWindowConfig.inputs.end(); it++)
	{
		if ((*it) == "keyboard" && inputManager->getNumberOfDevices(OIS::OISKeyboard) > 0)
		{
			OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
			mpKeyboard = keyboard;
			mpKeyboard->setEventCallback(this);
		}
		else if ((*it) == "mouse" && inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
		{
			OIS::Mouse*    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
			mpMouse = mouse;
			mpMouse->setEventCallback(this);
			const OIS::MouseState &ms = mouse->getMouseState();
			ms.width = oisWindowConfig.width;
			ms.height = oisWindowConfig.height;
		}
	}
}

bool Ape::OISUserInputPlugin::keyPressed(const OIS::KeyEvent& e)
{
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << "Ape::OISUserInputPlugin::keyPressed(): mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents << std::endl;
	mKeyCodeMap[e.key] = true;

	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		std::string keyAsString = mpKeyboard->getAsString(e.key);
		std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);
		//std::cout << "ApePresentationScenePlugin::keyPressed " << "keyAsString:" << keyAsString << std::endl;
		std::wstring keyAsWString(keyAsString.begin(), keyAsString.end());
		if (e.key == OIS::KeyCode::KC_BACK)
			keyAsWString = 8;
		else if (e.key == OIS::KeyCode::KC_TAB)
			keyAsWString = 9;
		else if (e.key == OIS::KeyCode::KC_RETURN)
			keyAsWString = 13;
		else if (e.key == OIS::KeyCode::KC_LSHIFT)
			keyAsWString = 14;
		mIsNewKeyEvent = false;
		overlayBrowser->keyASCIIValue(keyAsWString[0]);
	}

	std::cout << "Before waiting: mIsNewKeyEvent: " << mIsNewKeyEvent << std::endl;
	while (!mIsNewKeyEvent)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mIsNewKeyEvent = false;
	std::cout << "After waiting mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents << std::endl;
	return true;
}

bool Ape::OISUserInputPlugin::keyReleased(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = false;
	return true;
}

bool Ape::OISUserInputPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	if (auto overlayMouseTexture = mOverlayMouseTexture.lock())
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			Ape::Vector2 cursorTexturePosition;
			cursorTexturePosition.x = (float)-e.state.X.abs / (float)mpMainWindow->getWidth();
			cursorTexturePosition.y = (float)-e.state.Y.abs / (float)mpMainWindow->getHeight();
			overlayMouseTexture->setTextureScroll(cursorTexturePosition.x, cursorTexturePosition.y);
			Ape::Vector2 cursorBrowserPosition;
			cursorBrowserPosition.x = ((float)e.state.X.abs / (float)mpMainWindow->getWidth()) * overlayBrowser->getResoultion().x;
			cursorBrowserPosition.y = ((float)e.state.Y.abs / (float)mpMainWindow->getHeight()) * overlayBrowser->getResoultion().y;
			overlayBrowser->mouseMoved(cursorBrowserPosition);
			overlayBrowser->mouseScroll(Ape::Vector2(0, e.state.Z.rel));
			/*std::cout << "ApePresentationScenePlugin::mouseMoved " << "cursorTexturePosition:" << cursorTexturePosition.x << ";" << cursorTexturePosition.y << std::endl;
			std::cout << "ApePresentationScenePlugin::mouseMoved " << "cursorBrowserPosition:" << cursorBrowserPosition.x << ";" << cursorBrowserPosition.y << std::endl;*/
		}
	}
	return true;
}

bool Ape::OISUserInputPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			overlayBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, true);
			std::cout << "Ape::OISUserInputPlugin::mousePressed overlayBrowser->mouseClick" << std::endl;
		}
	}
	return true;
}

bool Ape::OISUserInputPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			overlayBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, false);
			std::cout << "Ape::OISUserInputPlugin::mouseReleased overlayBrowser->mouseClick" << std::endl;
		}
	}
	return true;
}

void Ape::OISUserInputPlugin::saveUserNodePose(Ape::NodeSharedPtr userNode)
{
	std::ofstream userNodePoseFile;
	userNodePoseFile.open("userNodePoseFile.txt", std::ios::app);
	userNodePoseFile << userNode->getPosition().x << "," << userNode->getPosition().y << "," << userNode->getPosition().z << " : " <<
		userNode->getOrientation().w << "," << userNode->getOrientation().x << "," << userNode->getOrientation().y << "," << userNode->getOrientation().z << std::endl;
	userNodePoseFile.close();
}

void Ape::OISUserInputPlugin::toggleUserNodePoses(Ape::NodeSharedPtr userNode)
{
	userNode->setPosition(mUserNodePoses[mUserNodePosesToggleIndex].position);
	userNode->setOrientation(mUserNodePoses[mUserNodePosesToggleIndex].orientation);
	mUserNodePosesToggleIndex++;
	if (mUserNodePoses.size() == mUserNodePosesToggleIndex)
		mUserNodePosesToggleIndex = 0;
}


void Ape::OISUserInputPlugin::moveUserNode()
{
	auto userNode = mUserNode.lock();
	if (userNode)
	{
		if (mKeyCodeMap[OIS::KeyCode::KC_PGUP])
			userNode->translate(Ape::Vector3(0, 1 * mTranslateSpeedFactor, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_PGDOWN])
			userNode->translate(Ape::Vector3(0, -1 * mTranslateSpeedFactor, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_D])
			userNode->translate(Ape::Vector3(1 * mTranslateSpeedFactor, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_A])
			userNode->translate(Ape::Vector3(-1 * mTranslateSpeedFactor, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_W])
			userNode->translate(Ape::Vector3(0, 0, -1 * mTranslateSpeedFactor), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_S])
			userNode->translate(Ape::Vector3(0, 0, 1 * mTranslateSpeedFactor), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_LEFT])
			userNode->rotate(0.017f * mRotateSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
		if (mKeyCodeMap[OIS::KeyCode::KC_RIGHT])
			userNode->rotate(-0.017f * mRotateSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
		if (mKeyCodeMap[OIS::KeyCode::KC_UP])
			userNode->rotate(0.017f * mRotateSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_DOWN])
			userNode->rotate(-0.017f * mRotateSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_NUMPAD4])
			userNode->rotate(0.017f * mRotateSpeedFactor, Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
		if (mKeyCodeMap[OIS::KeyCode::KC_NUMPAD6])
			userNode->rotate(-0.017f * mRotateSpeedFactor, Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
	}
}

void Ape::OISUserInputPlugin::Run()
{
	while (true)
	{
		if (mpKeyboard)
			mpKeyboard->capture();
		if (mpMouse)
			mpMouse->capture();
		if (!mEnableOverlayBrowserKeyEvents)
			moveUserNode();
		std::this_thread::sleep_for (std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
}

void Ape::OISUserInputPlugin::Step()
{
	
}

void Ape::OISUserInputPlugin::Stop()
{
	
}

void Ape::OISUserInputPlugin::Suspend()
{
	
}

void Ape::OISUserInputPlugin::Restart()
{
	
}