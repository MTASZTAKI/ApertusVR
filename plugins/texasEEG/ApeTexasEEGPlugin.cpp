#include "apeTexasEEGPlugin.h"

ape::apeTexasEEGPlugin::apeTexasEEGPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeTexasEEGPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mScore = 0;
	APE_LOG_FUNC_LEAVE();
}

ape::apeTexasEEGPlugin::~apeTexasEEGPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeTexasEEGPlugin::eventCallBack, this, std::placeholders::_1));
	if (mBubbleManager)
	{
		delete mBubbleManager;
		mBubbleManager = NULL;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTexasEEGPlugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeTexasEEGPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpapeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();



	mGameManager = new TexasEEG::GameManager(mpapeUserInputMacro->getUserNode());

	if (auto userNode = mpapeUserInputMacro->getUserNode().lock())
	{
		if (auto userBodyNode = mpSceneManager->createNode("userBodyNode").lock())
		{
			userNode->setParentNode(userBodyNode);

			if (auto userGeometry = std::static_pointer_cast<ISphereGeometry>(mpSceneManager->createEntity("userGeometrySphere", ape::Entity::Type::GEOMETRY_SPHERE).lock()))
			{
				userGeometry->setParameters(35.0f, ape::Vector2(1, 1));
				if (auto userBody = std::static_pointer_cast<IRigidBody>(mpSceneManager->createEntity("userBody", ape::Entity::Type::RIGIDBODY).lock()))
				{
					userBody->setParentNode(userBodyNode);
					userBody->setToDynamic(1.0f);
					userBody->setBouyancy(true, 0.0f, 1.0f);
					userBody->setGeometry(userGeometry);
				}
			}
		}
	}

	mGameManager->Start();

	APE_LOG_FUNC_LEAVE();
}

void ape::apeTexasEEGPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTexasEEGPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTexasEEGPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTexasEEGPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTexasEEGPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
