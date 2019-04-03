#include "apeInGameBrowserSamplePlugin.h"

ape::apeInGameBrowserSamplePlugin::apeInGameBrowserSamplePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::apeInGameBrowserSamplePlugin::~apeInGameBrowserSamplePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeInGameBrowserSamplePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto browserNode = mpSceneManager->createNode("browserNode").lock())
	{
		browserNode->setPosition(ape::Vector3(0, 0, -150));
		ape::Radian angle(1.57f);
		ape::Vector3 axis(1, 0, 0);
		ape::Quaternion orientation;
		orientation.FromAngleAxis(angle, axis);
		ape::Radian angle2(1.57f);
		ape::Vector3 axis2(0, 1, 0);
		ape::Quaternion orientation2;
		orientation2.FromAngleAxis(angle2, axis2);
		browserNode->setOrientation(orientation * orientation2);
		if (auto browserGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity("browserGeometry", ape::Entity::GEOMETRY_PLANE).lock()))
		{
			browserGeometry->setParameters(ape::Vector2(1, 1), ape::Vector2(102.4, 76.8), ape::Vector2(1, 1));
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->createEntity("browser", ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(1024, 768);
				browser->setURL("https://www.google.com");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeInGameBrowserSamplePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeInGameBrowserSamplePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeInGameBrowserSamplePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeInGameBrowserSamplePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeInGameBrowserSamplePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
