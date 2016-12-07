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
	mKeyCode = OIS::KeyCode::KC_UNASSIGNED;
	mIsPressed = false;
	mSpeedFactor = 2;
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mCameraNode = Ape::NodeWeakPtr();
}

Ape::OISUserInputPlugin::~OISUserInputPlugin()
{
	std::cout << "OISUserInputPlugin dtor" << std::endl;

	delete mpKeyboard; 
	delete mpMouse;
}

void Ape::OISUserInputPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.group == Ape::Event::Group::NODE && event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == mpSystemConfig->getSceneSessionConfig().generatedUniqueUserName)
	{
		if (auto node = (mpScene->getNode(event.subjectName).lock()))
			mCameraNode = node;
	}
}

void Ape::OISUserInputPlugin::Init()
{

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


	
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

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
	if (e.key == OIS::KeyCode::KC_PGUP ||
		e.key == OIS::KeyCode::KC_PGDOWN ||
		e.key == OIS::KeyCode::KC_D ||
		e.key == OIS::KeyCode::KC_A ||
		e.key == OIS::KeyCode::KC_W ||
		e.key == OIS::KeyCode::KC_S ||
		e.key == OIS::KeyCode::KC_LEFT ||
		e.key == OIS::KeyCode::KC_RIGHT ||
		e.key == OIS::KeyCode::KC_UP ||
		e.key == OIS::KeyCode::KC_DOWN ||
		e.key == OIS::KeyCode::KC_N ||
		e.key == OIS::KeyCode::KC_RETURN)
	{
		mKeyCode = e.key;
		mIsPressed = true;
	}
	return true;
}

bool Ape::OISUserInputPlugin::keyReleased(const OIS::KeyEvent& e)
{
	if (e.key == OIS::KeyCode::KC_PGUP ||
		e.key == OIS::KeyCode::KC_PGDOWN ||
		e.key == OIS::KeyCode::KC_D ||
		e.key == OIS::KeyCode::KC_A ||
		e.key == OIS::KeyCode::KC_W ||
		e.key == OIS::KeyCode::KC_S ||
		e.key == OIS::KeyCode::KC_LEFT ||
		e.key == OIS::KeyCode::KC_RIGHT ||
		e.key == OIS::KeyCode::KC_UP ||
		e.key == OIS::KeyCode::KC_DOWN ||
		e.key == OIS::KeyCode::KC_N ||
		e.key == OIS::KeyCode::KC_RETURN)
	{
		mKeyCode = OIS::KeyCode::KC_UNASSIGNED;
		mIsPressed = false;
	}
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


void Ape::OISUserInputPlugin::moveUserNode()
{
	auto cameraNode = mCameraNode.lock();
	if (cameraNode && mIsPressed)
	{
		if (mKeyCode == OIS::KeyCode::KC_PGUP)
			cameraNode->translate(Ape::Vector3(0, 1 * mSpeedFactor, 0), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_PGDOWN)
			cameraNode->translate(Ape::Vector3(0, -1 * mSpeedFactor, 0), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_D)
			cameraNode->translate(Ape::Vector3(1 * mSpeedFactor, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_A)
			cameraNode->translate(Ape::Vector3(-1 * mSpeedFactor, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_W)
			cameraNode->translate(Ape::Vector3(0, 0, -1 * mSpeedFactor), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_S)
			cameraNode->translate(Ape::Vector3(0, 0, 1 * mSpeedFactor), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_LEFT)
			cameraNode->rotate(0.017f * mSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_RIGHT)
			cameraNode->rotate(-0.017f * mSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_UP)
			cameraNode->rotate(0.017f * mSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		else if (mKeyCode == OIS::KeyCode::KC_DOWN)
			cameraNode->rotate(-0.017f * mSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
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