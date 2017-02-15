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
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mUserNode = Ape::NodeWeakPtr();
	mUserNodePoses = std::vector<UserNodePose>();
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(102.032, -582, -582), Ape::Quaternion(0.250597, 0, 0.968092, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(103.374, -252, -252), Ape::Quaternion(0.993901, 0, 0.110273, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(5.87041, -252, -252), Ape::Quaternion(-0.0187007, 0, 0.999825, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(427.51, -582, -582), Ape::Quaternion(0.250598, 0, 0.968091, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(151.171, -582, -582), Ape::Quaternion(0.984112, 0, 0.177551, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-32.9196, -582, -582), Ape::Quaternion(-0.0187003, 1.45519e-11, 0.999825, -9.31323e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(121.314, -582, -582), Ape::Quaternion(-0.286629, -2.97443e-10, -0.958042, 8.82668e-10)));
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
	if (event.type == Ape::Event::Type::CAMERA_CREATE)
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
			camera->setParentNode(mUserNode);
	}
}

void Ape::OISUserInputPlugin::Init()
{
	std::cout << "OISUserInputPlugin::Init" << std::endl;

	std::string userNodeName = mpSystemConfig->getSceneSessionConfig().generatedUniqueUserName;
	mUserNode = mpScene->createNode(userNodeName);
	if (mpSystemConfig->getSceneSessionConfig().participantType == SceneSession::ParticipantType::HOST || mpSystemConfig->getSceneSessionConfig().participantType == SceneSession::ParticipantType::GUEST)
	{
		if (mUserNode.lock())
		{
			if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity(userNodeName, Ape::Entity::GEOMETRY_TEXT).lock()))
			{
				userNameText->setCaption(userNodeName);
				userNameText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
				userNameText->setParentNode(mUserNode);
			}
		}
	}

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
	mKeyCodeMap[e.key] = true;
	auto userNode = mUserNode.lock();
	if (userNode)
	{
		if (mKeyCodeMap[OIS::KeyCode::KC_C])
			saveUserNodePose(userNode);
		if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
			toggleUserNodePoses(userNode);
	}
	return true;
}

bool Ape::OISUserInputPlugin::keyReleased(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = false;
	return true;
}

bool Ape::OISUserInputPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	return true;
}

bool Ape::OISUserInputPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	return true;
}

bool Ape::OISUserInputPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	return true;
}

void Ape::OISUserInputPlugin::saveUserNodePose(Ape::NodeSharedPtr userNode)
{
	std::ofstream userNodePoseFile;
	userNodePoseFile.open("userNodePoseFile.txt", std::ios::app);
	userNodePoseFile << userNode->getPosition().x << "," << userNode->getPosition().y << "," << userNode->getPosition().y << " : " <<
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
		else if (mpMouse)
			mpMouse->capture();
		moveUserNode();
		std::this_thread::sleep_for (std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
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