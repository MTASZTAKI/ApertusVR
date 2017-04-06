#include <iostream>
#include "ApeLegoPlugin.h"

ApeLegoPlugin::ApeLegoPlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mInterpolators = std::map<int, std::unique_ptr<Ape::Interpolator>>();
	mKeyCodeMap = std::map<OIS::KeyCode, bool>();
	mUserNode = Ape::NodeWeakPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpKeyboard = NULL;
	mpMouse = NULL;
	mTranslateSpeedFactor = 3;
	mRotateSpeedFactor = 1;
	mAnimationToggleIndex = 0;
	mAnimationNodes = std::vector<Ape::NodeWeakPtr>();
	mMeshNames = std::vector<std::string>();
	mMeshNames.push_back("Lego-Wheel.mesh");
	mMeshNames.push_back("Lego-2x2_Pl.mesh");
	mMeshNames.push_back("Lego_Rod3_.mesh");
	mMeshNames.push_back("Lego_Pan.mesh");
	mMeshNames.push_back("Lego_3x4_Ladder.mesh");
	mMeshNames.push_back("Lego_32x32_PlRacing.mesh");
	mMeshNames.push_back("Lego_2x6_Pl.mesh");
	mMeshNames.push_back("Lego_2x4_PlWh.mesh");
	mMeshNames.push_back("Lego_2x4_Pl.mesh");
	mMeshNames.push_back("Lego_2x14_Brd.mesh");
	mMeshNames.push_back("Lego_2r2_Ti.mesh");
	mMeshNames.push_back("Lego_1x4_Ti.mesh");
	mMeshNames.push_back("Lego_1x4_Pl.mesh");
	mMeshNames.push_back("Lego_1x21_PlTi.mesh");
	mMeshNames.push_back("Lego_1x21_Bru.mesh");
	mMeshNames.push_back("Lego_1x2_Slr.mesh");
	mMeshNames.push_back("Lego_1x2_SlGr.mesh");
	mMeshNames.push_back("Lego_1x2_pl.mesh");
	mMeshNames.push_back("Lego_1x2_Gr.mesh");
	mMeshNames.push_back("Lego_1x2_Br.mesh");
	mMeshNames.push_back("Lego_1x2_1vq.mesh");
	mMeshNames.push_back("Lego_1x1_uq.mesh");
	mMeshNames.push_back("Lego_1x1_Ti.mesh");
	mMeshNames.push_back("Lego_1x1_Pl.mesh");
	mMeshNames.push_back("Lego_1x1_Brhq.mesh");
	mMeshNames.push_back("Lego_1r1-y.mesh");
	mMeshNames.push_back("Lego_1r1_w.mesh");	
}

ApeLegoPlugin::~ApeLegoPlugin()
{
	std::cout << "ApeLegoPlugin dtor" << std::endl;
	delete mpKeyboard;
	delete mpMouse;
}

void ApeLegoPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::CAMERA_CREATE)
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
			camera->setParentNode(mUserNode);
	}
}

void ApeLegoPlugin::Init()
{
	std::cout << "ApeLegoPlugin::init" << std::endl;
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

	std::string userNodeName = mpSystemConfig->getSceneSessionConfig().generatedUniqueUserName;
	mUserNode = mpScene->createNode(userNodeName);
	std::cout << "ApeLegoPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "ApeLegoPlugin main window was found" << std::endl;

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

	for (auto meshName : mMeshNames)
	{
		if (auto node = mpScene->createNode(meshName).lock())
		{
			node->setScale(Ape::Vector3(100, 100, 100));
			mAnimationNodes.push_back(node);
			if (auto geometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity(meshName, Ape::Entity::GEOMETRY_FILE).lock()))
			{
				geometry->setFileName(meshName);
				geometry->setParentNode(node);
			}
		}
	}

	for (int i = 0; i < mAnimationNodes.size(); i++)
	{
		if (auto animationNode = mAnimationNodes[i].lock())
		{
			auto moveInterpolator = std::make_unique<Ape::Interpolator>();
			moveInterpolator->addSection(
				Ape::Vector3(10, 10, 0),
				Ape::Vector3(10, 10, 100),
				10.0,
				[&](Ape::Vector3 pos) { animationNode->setPosition(pos); }
			);
			moveInterpolator->addSection(
				Ape::Vector3(10, 10, 100),
				Ape::Vector3(10, 10, 0),
				10.0,
				[&](Ape::Vector3 pos) { animationNode->setPosition(pos); }
			);
			mInterpolators[i] = std::move(moveInterpolator);
		}
	}
}

void ApeLegoPlugin::toggleAnimation()
{
	mAnimationToggleIndex++;
	mInterpolators[mAnimationToggleIndex]->iterateTopSection();
	if (mAnimationNodes.size() == mAnimationToggleIndex)
		mAnimationToggleIndex = 0;
}

void ApeLegoPlugin::moveUserNode()
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

void ApeLegoPlugin::Run()
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
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
}

void ApeLegoPlugin::Step()
{

}

void ApeLegoPlugin::Stop()
{

}

void ApeLegoPlugin::Suspend()
{

}

void ApeLegoPlugin::Restart()
{

}

bool ApeLegoPlugin::keyPressed(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = true;
	if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
		toggleAnimation();
	return true;
}

bool ApeLegoPlugin::keyReleased(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = false;
	return true;
}

bool ApeLegoPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	return true;
}

bool ApeLegoPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	return true;
}

bool ApeLegoPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	return true;
}
