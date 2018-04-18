#include <iostream>
#include "ApeLegoPlugin.h"

ApeLegoPlugin::ApeLegoPlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mKeyCodeMap = std::map<OIS::KeyCode, bool>();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpKeyboard = NULL;
	mpMouse = NULL;
	mTranslateSpeedFactor = 3;
	mRotateSpeedFactor = 1;
	mInterpolatorsToggleIndex = 0;
	mInterpolatorCount = 55;
	mAnimationNodes = std::vector<Ape::NodeWeakPtr>();
	mMeshNames = std::vector<std::string>();
	mMeshNames.push_back("Lego_1r1_w.mesh");
	mMeshNames.push_back("Lego_1r1_w_2.mesh");
	mMeshNames.push_back("Lego_1r1-y.mesh");
	mMeshNames.push_back("Lego_1r1-y_2.mesh");
	mMeshNames.push_back("Lego_1r1-y_3.mesh");
	mMeshNames.push_back("Lego_1r1-y_4.mesh");
	mMeshNames.push_back("Lego_1x1_Brhq.mesh");
	mMeshNames.push_back("Lego_1x1_Brhq_2.mesh");
	mMeshNames.push_back("Lego_1x1_Pl.mesh");
	mMeshNames.push_back("Lego_1x1_Pl_2.mesh");
	mMeshNames.push_back("Lego_1x1_Ti.mesh");
	mMeshNames.push_back("Lego_1x1_Ti_2.mesh");
	mMeshNames.push_back("Lego_1x1_uq.mesh");
	mMeshNames.push_back("Lego_1x1_uq_2.mesh");
	mMeshNames.push_back("Lego_1x2_1vq.mesh");
	mMeshNames.push_back("Lego_1x2_Br.mesh");
	mMeshNames.push_back("Lego_1x2_Br_2.mesh");
	mMeshNames.push_back("Lego_1x2_Br_3.mesh");
	mMeshNames.push_back("Lego_1x2_Gr.mesh");
	mMeshNames.push_back("Lego_1x2_Gr_2.mesh");
	mMeshNames.push_back("Lego_1x2_pl.mesh");
	mMeshNames.push_back("Lego_1x2_pl_2.mesh");
	mMeshNames.push_back("Lego_1x2_pl_3.mesh");
	mMeshNames.push_back("Lego_1x2_pl_4.mesh");
	mMeshNames.push_back("Lego_1x2_pl_5.mesh");
	mMeshNames.push_back("Lego_1x2_SlGr.mesh");
	mMeshNames.push_back("Lego_1x2_SlGr_2.mesh");
	mMeshNames.push_back("Lego_1x2_Slr.mesh");
	mMeshNames.push_back("Lego_1x2_Slr_2.mesh");
	mMeshNames.push_back("Lego_1x21_Bru.mesh");
	mMeshNames.push_back("Lego_1x21_Bru_2.mesh");
	mMeshNames.push_back("Lego_1x21_Bru_3.mesh");
	mMeshNames.push_back("Lego_1x21_PlTi.mesh");
	mMeshNames.push_back("Lego_1x21_PlTi_2.mesh");
	mMeshNames.push_back("Lego_1x4_Pl.mesh");
	mMeshNames.push_back("Lego_1x4_Pl_2.mesh");
	mMeshNames.push_back("Lego_1x4_Ti.mesh");
	mMeshNames.push_back("Lego_1x4_Ti_2.mesh");
	mMeshNames.push_back("Lego_2r2_Ti.mesh");
	mMeshNames.push_back("Lego_2x14_Brd.mesh");
	mMeshNames.push_back("Lego_2x14_Brd_2.mesh");
	mMeshNames.push_back("Lego_2x4_Pl.mesh");
	mMeshNames.push_back("Lego_2x4_PlWh.mesh");
	mMeshNames.push_back("Lego_2x4_PlWh_2.mesh");
	mMeshNames.push_back("Lego_2x6_Pl.mesh");
	mMeshNames.push_back("Lego_32x32_PlRacing.mesh");
	mMeshNames.push_back("Lego_3x4_Ladder.mesh");
	mMeshNames.push_back("Lego_Pan.mesh");
	mMeshNames.push_back("Lego_Rod3_.mesh");
	mMeshNames.push_back("Lego_Rod3_2.mesh");
	mMeshNames.push_back("Lego-2x2_Pl.mesh");
	mMeshNames.push_back("Lego-2x2_Pl_2.mesh");
	mMeshNames.push_back("Lego-2x2_Pl_3.mesh");
	mMeshNames.push_back("Lego-2x2_Pl_4.mesh");
	mMeshNames.push_back("Lego-Wheel.mesh");
	mMeshNames.push_back("Lego-Wheel_2.mesh");

	
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

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

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
			if (node->getName() != "Lego_32x32_PlRacing.mesh")
			{
				node->setPosition(Ape::Vector3(1000, 0, 0));
				/*if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity(meshName + "text", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					text->setCaption(meshName);
					text->setParentNode(node);
				}*/
			}
			mAnimationNodes.push_back(node);
			if (auto geometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity(meshName, Ape::Entity::GEOMETRY_FILE).lock()))
			{
				geometry->setFileName(meshName);
				geometry->setParentNode(node);
			}
		}
	}
}

void ApeLegoPlugin::blowModel()
{
	for (auto meshName : mMeshNames)
	{
		if (auto node = mpScene->getNode(meshName).lock())
		{
			if (node->getName() != "Lego_32x32_PlRacing.mesh")
			{
				node->setPosition(Ape::Vector3(1000, 0, 0));
				node->setOrientation(Ape::Quaternion(1, 0, 0, 0));
			}
		}
	}
	mInterpolatorsToggleIndex = 0;
}

void ApeLegoPlugin::toggleInterpolators()
{
	auto animationThread = std::thread(&ApeLegoPlugin::interpolate, this, mInterpolatorsToggleIndex);
	animationThread.detach();
	mInterpolatorsToggleIndex++;
	if (mInterpolatorCount == mInterpolatorsToggleIndex)
	{
		mInterpolatorsToggleIndex = 0;
	}
}

void ApeLegoPlugin::interpolate(int interpolatorIndex)
{
	if (interpolatorIndex == 0)
	{
		if (auto node = mpScene->getNode("Lego-Wheel.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(123.527, 49.114, 16.422),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 1)
	{
		if (auto node = mpScene->getNode("Lego-Wheel_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-79.592, 49.114, 16.422),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 2)
	{
		if (auto node = mpScene->getNode("Lego_1x21_Bru.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(159.087, 74.514, 16.423),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 3)
	{
		if (auto node = mpScene->getNode("Lego_1x2_pl.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(98.308, 64.354, 16.427),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 4)
	{
		if (auto node = mpScene->getNode("Lego_1x2_1vq.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-122.891, 64.354, 16.427),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 5)
	{
		if (auto node = mpScene->getNode("Lego_Rod3_.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-155.874, 64.354, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 6)
	{
		if (auto node = mpScene->getNode("Lego_2x6_Pl.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(21.926, 84.674, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 7)
	{
		if (auto node = mpScene->getNode("Lego-2x2_Pl.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-79.674, 104.994, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 8)
	{
		if (auto node = mpScene->getNode("Lego-2x2_Pl_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(124.317, 104.994, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 9)
	{
		if (auto node = mpScene->getNode("Lego_1x4_Pl.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(47.326, 104.994, 16.424),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 10)
	{
		if (auto node = mpScene->getNode("Lego_1x4_Pl_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-3.976, 104.994, 16.424),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 11)
	{
		if (auto node = mpScene->getNode("Lego_2x4_PlWh.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-79.674, 135.474, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 12)
	{
		if (auto node = mpScene->getNode("Lego_2x4_PlWh_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(123.67, 135.474, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 13)
	{
		if (auto node = mpScene->getNode("Lego_2x4_Pl.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(21.926, 125.314, 16.424),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 14)
	{
		if (auto node = mpScene->getNode("Lego_2x14_Brd.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-117.774, 130.394, 16.426),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 15)
	{
		if (auto node = mpScene->getNode("Lego_2x14_Brd_2.mesh").lock())
		{
			node->rotate(Ape::Degree(180).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(159.161, 130.394, 16.426),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 16)
	{
		if (auto node = mpScene->getNode("Lego-2x2_Pl_3.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-29.022, 145.539, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 17)
	{
		if (auto node = mpScene->getNode("Lego-2x2_Pl_4.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(72.754, 145.539, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 18)
	{
		if (auto node = mpScene->getNode("Lego_1x2_pl_2.mesh").lock())
		{
			node->rotate(Ape::Degree(90).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(22.017, 145.885, -59.864),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 19)
	{
		if (auto node = mpScene->getNode("Lego_1x2_pl_3.mesh").lock())
		{
			node->rotate(Ape::Degree(90).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(22.017, 145.88, 92.717),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 20)
	{
		if (auto node = mpScene->getNode("Lego_1r1_w.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-158.393, 125.323, -59.771),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 21)
	{
		if (auto node = mpScene->getNode("Lego_1r1_w_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-158.393, 125.323, 92.778),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 22)
	{
		if (auto node = mpScene->getNode("Lego_1x2_Gr.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-153.318, 125.327, 16.429),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 23)
	{
		if (auto node = mpScene->getNode("Lego_1x2_Br.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-5.536, 187.21, 67.143),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 24)
	{
		if (auto node = mpScene->getNode("Lego_1x2_Br_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-5.536, 187.21, -33.981),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 25)
	{
		if (auto node = mpScene->getNode("Lego_1x2_SlGr.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-82.214, 170.374, -8.975),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 26)
	{
		if (auto node = mpScene->getNode("Lego_1x2_SlGr_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-82.214, 170.374, 41.735),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 27)
	{
		if (auto node = mpScene->getNode("Lego_1x21_Bru.mesh").lock())
		{
			node->rotate(Ape::Degree(180).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-13.65, 236.982, -34.525),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 28)
	{
		if (auto node = mpScene->getNode("Lego_1x21_Bru_2.mesh").lock())
		{
			node->rotate(Ape::Degree(180).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-13.65, 236.982, 67.043),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 29)
	{
		if (auto node = mpScene->getNode("Lego_1r1-y.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-49.21, 237.053, -59.851),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 30)
	{
		if (auto node = mpScene->getNode("Lego_1r1-y_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-49.21, 237.092, -9.125),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 31)
	{
		if (auto node = mpScene->getNode("Lego_1r1-y_3.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-49.21, 237.053, 41.717),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 32)
	{
		if (auto node = mpScene->getNode("Lego_1r1-y_4.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-49.21, 237.092, 92.442),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 33)
	{
		if (auto node = mpScene->getNode("Lego_1x21_PlTi.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(98.126, 181.194, 16.425),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 34)
	{
		if (auto node = mpScene->getNode("Lego_1x21_PlTi_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(47.491, 181.194, 16.426),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 35)
	{
		if (auto node = mpScene->getNode("Lego_1x1_Pl.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(148.679, 165.954, 92.619),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 36)
	{
		if (auto node = mpScene->getNode("Lego_1x1_Pl_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(148.679, 165.954, -59.855),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 37)
	{
		if (auto node = mpScene->getNode("Lego_1x21_Bru_3.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(159.087, 176.227, 16.423),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 38)
	{
		if (auto node = mpScene->getNode("Lego_1x1_Ti.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(194.664, 125.313, 92.626),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 39)
	{
		if (auto node = mpScene->getNode("Lego_1x1_Ti_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(194.664, 125.313, -59.675),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 40)
	{
		if (auto node = mpScene->getNode("Lego_1x2_pl_4.mesh").lock())
		{
			node->rotate(Ape::Degree(270).toRadian(), Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(199.621, 74.788, 16.427),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 41)
	{
		if (auto node = mpScene->getNode("Lego_2r2_Ti.mesh").lock())
		{
			node->rotate(Ape::Degree(90).toRadian(), Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(204.898, 150.71, 16.373),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 42)
	{
		if (auto node = mpScene->getNode("Lego_1x2_Slr.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(204.806, 74.514, 64.686),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 43)
	{
		if (auto node = mpScene->getNode("Lego_1x2_Slr_2.mesh").lock())
		{
			node->rotate(Ape::Degree(180).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(204.806, 74.514, -37.408),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 44)
	{
		if (auto node = mpScene->getNode("Lego_1x1_Brhq.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(166.484, 206.594, -59.781),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 45)
	{
		if (auto node = mpScene->getNode("Lego_1x1_Brhq_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(166.484, 206.594, 92.449),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 46)
	{
		if (auto node = mpScene->getNode("Lego_1x2_Br_3.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(148.673, 206.594, 16.419),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 47)
	{
		if (auto node = mpScene->getNode("Lego_1x1_uq.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-3.478, 254.854, -8.975),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 48)
	{
		if (auto node = mpScene->getNode("Lego_1x1_uq_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(-3.478, 254.854, 41.749),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 49)
	{
		if (auto node = mpScene->getNode("Lego_1x2_Gr_2.mesh").lock())
		{
			node->rotate(Ape::Degree(270).toRadian(), Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(148.639, 242.17, 16.429),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 50)
	{
		if (auto node = mpScene->getNode("Lego_1x4_Ti.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(72.734, 242.154, 92.627),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 51)
	{
		if (auto node = mpScene->getNode("Lego_1x4_Ti_2.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(72.734, 242.154, -59.817),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 52)
	{
		if (auto node = mpScene->getNode("Lego_3x4_Ladder.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(72.722, 265.015, 18.959),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 53)
	{
		if (auto node = mpScene->getNode("Lego_Rod3_2.mesh").lock())
		{
			node->rotate(Ape::Degree(90).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(199.41, 231.368, 92.418),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
	else if (interpolatorIndex == 54)
	{
		if (auto node = mpScene->getNode("Lego_Pan.mesh").lock())
		{
			auto interpolator = std::make_unique<Ape::Interpolator>();
			interpolator->addSection(
				node->getPosition(),
				Ape::Vector3(206.884, 238.341, -62.3259),
				1.2,
				[&](Ape::Vector3 pos) { node->setPosition(pos); }
			);
			while (!interpolator->isQueueEmpty())
			{
				interpolator->iterateTopSection();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
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
		toggleInterpolators();
	if (mKeyCodeMap[OIS::KeyCode::KC_V])
		blowModel();
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
