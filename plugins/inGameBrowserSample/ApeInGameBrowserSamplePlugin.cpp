#include "ApeInGameBrowserSamplePlugin.h"

Ape::ApeInGameBrowserSamplePlugin::ApeInGameBrowserSamplePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeInGameBrowserSamplePlugin::~ApeInGameBrowserSamplePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeInGameBrowserSamplePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto browserNode = mpSceneManager->createNode("browserNode").lock())
	{
		browserNode->setPosition(Ape::Vector3(0, 0, -150));
		Ape::Radian angle(1.57f);
		Ape::Vector3 axis(1, 0, 0);
		Ape::Quaternion orientation;
		orientation.FromAngleAxis(angle, axis);
		Ape::Radian angle2(1.57f);
		Ape::Vector3 axis2(0, 1, 0);
		Ape::Quaternion orientation2;
		orientation2.FromAngleAxis(angle2, axis2);
		browserNode->setOrientation(orientation * orientation2);
		if (auto browserGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpSceneManager->createEntity("browserGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			browserGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(102.4, 76.8), Ape::Vector2(1, 1));
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->createEntity("browser", Ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(1024, 768);
				browser->setURL("https://www.google.com");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeInGameBrowserSamplePlugin::Run()
{

}

void Ape::ApeInGameBrowserSamplePlugin::Step()
{

}

void Ape::ApeInGameBrowserSamplePlugin::Stop()
{

}

void Ape::ApeInGameBrowserSamplePlugin::Suspend()
{

}

void Ape::ApeInGameBrowserSamplePlugin::Restart()
{

}
