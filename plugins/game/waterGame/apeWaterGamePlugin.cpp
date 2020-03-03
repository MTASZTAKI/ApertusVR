#include "apeWaterGamePlugin.h"

ape::apeWaterGamePlugin::apeWaterGamePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeWaterGamePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpapeUserInputMacro = nullptr;
	mScore = 0;
	APE_LOG_FUNC_LEAVE();
}

ape::apeWaterGamePlugin::~apeWaterGamePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeWaterGamePlugin::eventCallBack, this, std::placeholders::_1));
	if (mBubbleManager)
	{
		delete mBubbleManager;
		mBubbleManager = NULL;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWaterGamePlugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeWaterGamePlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	APE_LOG_DEBUG("waiting for main window");
	while (mpCoreConfig->getWindowConfig().handle == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("main window was found");

	mpapeUserInputMacro = ape::UserInputMacro::getSingletonPtr();

	if (auto headNode = mpapeUserInputMacro->getHeadNode().lock())
	{
		if (auto userBodyNode = mpSceneManager->createNode("userBodyNode", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			headNode->setParentNode(userBodyNode);
			mGameManager = new WaterGame::GameManager(mpapeUserInputMacro->getUserNode(), userBodyNode);

			if (auto userGeometry = std::static_pointer_cast<ISphereGeometry>(mpSceneManager->createEntity("userGeometrySphere", ape::Entity::Type::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				userGeometry->setParameters(20.0f, ape::Vector2(1, 1));
				if (auto userBody = std::static_pointer_cast<IRigidBody>(mpSceneManager->createEntity("userBody", ape::Entity::Type::RIGIDBODY, false, "").lock()))
				{
					userBody->setParentNode(userBodyNode);
					userBody->setToDynamic(1.0f);
					userBody->setBouyancy(true);
					userBody->setGeometry(userGeometry);
				}
			}

			mUserBodyNode = userBodyNode;
		}
	}

	mGameManager->Start();

	APE_LOG_FUNC_LEAVE();
}

void ape::apeWaterGamePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		/// debug
		if (auto headNode = mpapeUserInputMacro->getHeadNode().lock())
		{
			std::cout << "headNode pos: " << headNode->getDerivedPosition().toString() << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWaterGamePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWaterGamePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWaterGamePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWaterGamePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
