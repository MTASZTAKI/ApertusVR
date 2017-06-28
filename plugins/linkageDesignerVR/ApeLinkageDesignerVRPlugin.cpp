#include <iostream>
#include "ApeLinkageDesignerVRPlugin.h"

ApeLinkageDesignerVRPlugin::ApeLinkageDesignerVRPlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mInterpolators = std::vector<std::unique_ptr<Ape::Interpolator>>();
	mKeyCodeMap = std::map<OIS::KeyCode, bool>();
	std::string userNodeName = mpSystemConfig->getSceneSessionConfig().generatedUniqueUserName;
	mUserNode = mpScene->getNode(userNodeName);
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpKeyboard = NULL;
	mpMouse = NULL;
	mTranslateSpeedFactor = 3;
	mRotateSpeedFactor = 1;
	mSceneToggleIndex = 0;
	mScenePoses = std::vector<ScenePose>();
	mScenePoses.push_back(ScenePose(Ape::Vector3(0.0, -225, 100060.0), Ape::Quaternion(0.994803, 0, -0.101823, 0)));
	mScenePoses.push_back(ScenePose(Ape::Vector3(0.0, -285, 200060.0), Ape::Quaternion(0.994803, 0, -0.101823, 0)));
	mScenePoses.push_back(ScenePose(Ape::Vector3(-48, -258, -45), Ape::Quaternion(1, 0, 0, 0)));
	mSwitchNodeVisibilityToggleIndex = 0;
	/*mSwitchNodeVisibilityNames = std::vector<std::string>();
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@base1Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@base2Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@base3Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@base4Switch");*/
	/*mSwitchNodeVisibilityNames.push_back("WeldingFixture@p1Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@p2Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@p3Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@p4Switch");*/
	//mSwitchNodeVisibilityNames.push_back("WeldingFixture@fixture_1Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_1Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_2Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_3Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_4Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_5Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_6Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_7Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_8Switch");
	//mSwitchNodeVisibilityNames.push_back("WeldingFixture@WorkbenchSwitch");
	/*mSwitchNodeVisibilityNames.push_back("Bounding@BoxSwitch");*/
	mSwitchNodes = std::vector<Ape::NodeWeakPtr>();
}

ApeLinkageDesignerVRPlugin::~ApeLinkageDesignerVRPlugin()
{
	std::cout << "ApeLinkageDesignerVRPlugin dtor" << std::endl;
	delete mpKeyboard;
	delete mpMouse;
}

void ApeLinkageDesignerVRPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::CAMERA_CREATE)
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
			camera->setParentNode(mUserNode);
	}
	else if (event.type == Ape::Event::Type::NODE_CREATE)
	{
		if (auto node = mpScene->getNode(event.subjectName).lock())
		{
			for (auto switchNodeName : mSwitchNodeVisibilityNames)
			{
				std::size_t found = node->getName().find(switchNodeName);
				if (found != std::string::npos)
					mSwitchNodes.push_back(node);
			}
		}
	}
}

void ApeLinkageDesignerVRPlugin::Init()
{
	std::cout << "ApeLinkageDesignerVRPlugin::init" << std::endl;
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.6f));
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, 1));
		light->setDiffuseColor(Ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.6f));
	}

	std::cout << "ApeLinkageDesignerVRPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "ApeLinkageDesignerVRPlugin main window was found" << std::endl;

	std::stringstream hwndStrStream;
	hwndStrStream << mpMainWindow->getHandle();
	std::stringstream windowHndStr;
	windowHndStr << std::stoul(hwndStrStream.str(), nullptr, 16);

	OIS::ParamList pl;
	pl.insert(std::make_pair("WINDOW", windowHndStr.str()));
#ifdef WIN32
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#endif
	OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
	if (inputManager->getNumberOfDevices(OIS::OISKeyboard) > 0)
	{
		OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
		mpKeyboard = keyboard;
		mpKeyboard->setEventCallback(this);
	}
	else if (inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
	{
		OIS::Mouse*    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
		mpMouse = mouse;
		mpMouse->setEventCallback(this);
		const OIS::MouseState &ms = mouse->getMouseState();
		ms.width = mpMainWindow->getWidth();
		ms.height = mpMainWindow->getHeight();
	}
}

void ApeLinkageDesignerVRPlugin::moveUserNode()
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

void ApeLinkageDesignerVRPlugin::toggleScenePoses(Ape::NodeSharedPtr userNode)
{
	userNode->setPosition(mScenePoses[mSceneToggleIndex].position);
	userNode->setOrientation(mScenePoses[mSceneToggleIndex].orientation);
	mSceneToggleIndex++;
	if (mScenePoses.size() == mSceneToggleIndex)
		mSceneToggleIndex = 0;
}

void ApeLinkageDesignerVRPlugin::toggleSwitchNodesVisibility()
{
	if (mSwitchNodes.size() > 0)
	{
		if (auto switchNode = mSwitchNodes[mSwitchNodeVisibilityToggleIndex].lock())
		{
			if (!switchNode->getChildrenVisibility())
				switchNode->setChildrenVisibility(true);
			else
				switchNode->setChildrenVisibility(false);
		}
		mSwitchNodeVisibilityToggleIndex++;
		if (mSwitchNodes.size() == mSwitchNodeVisibilityToggleIndex)
			mSwitchNodeVisibilityToggleIndex = 0;
	}
}

void ApeLinkageDesignerVRPlugin::saveUserNodePose(Ape::NodeSharedPtr userNode)
{
	std::ofstream userNodePoseFile;
	userNodePoseFile.open("userNodePoseFile.txt", std::ios::app);
	userNodePoseFile << userNode->getPosition().x << "," << userNode->getPosition().y << "," << userNode->getPosition().z << " : " <<
		userNode->getOrientation().w << "," << userNode->getOrientation().x << "," << userNode->getOrientation().y << "," << userNode->getOrientation().z << std::endl;
	userNodePoseFile.close();
}


void ApeLinkageDesignerVRPlugin::Run()
{
	while (true)
	{
		if (mpKeyboard)
			mpKeyboard->capture();
		else if (mpMouse)
			mpMouse->capture();
		moveUserNode();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
}

void ApeLinkageDesignerVRPlugin::Step()
{

}

void ApeLinkageDesignerVRPlugin::Stop()
{

}

void ApeLinkageDesignerVRPlugin::Suspend()
{

}

void ApeLinkageDesignerVRPlugin::Restart()
{

}

bool ApeLinkageDesignerVRPlugin::keyPressed(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = true;
	auto userNode = mUserNode.lock();
	if (userNode)
	{
		if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
			toggleScenePoses(userNode);
		else if (mKeyCodeMap[OIS::KeyCode::KC_C])
			saveUserNodePose(userNode);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_V])
		toggleSwitchNodesVisibility();
	return true;
}

bool ApeLinkageDesignerVRPlugin::keyReleased(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = false;
	return true;
}

bool ApeLinkageDesignerVRPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	return true;
}

bool ApeLinkageDesignerVRPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	return true;
}

bool ApeLinkageDesignerVRPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	return true;
}
