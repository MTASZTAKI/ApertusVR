#include <iostream>
#include "ApeInGameBrowserSamplePlugin.h"

ApeInGameBrowserSamplePlugin::ApeInGameBrowserSamplePlugin()
{
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeInGameBrowserSamplePlugin::~ApeInGameBrowserSamplePlugin()
{
	std::cout << "ApeInGameBrowserSamplePlugin dtor" << std::endl;
}

void ApeInGameBrowserSamplePlugin::Init()
{
	std::cout << "ApeInGameBrowserSamplePlugin::init" << std::endl;
	if (auto browserNode = mpScene->createNode("browserNode").lock())
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
		if (auto browserGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("browserGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			browserGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(102.4, 76.8), Ape::Vector2(1, 1));
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("browser", Ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(1024, 768);
				browser->setURL("https://www.google.com");
				browser->setGeometry(browserGeometry);
			}
		}
	}

}

void ApeInGameBrowserSamplePlugin::Run()
{

}

void ApeInGameBrowserSamplePlugin::Step()
{

}

void ApeInGameBrowserSamplePlugin::Stop()
{

}

void ApeInGameBrowserSamplePlugin::Suspend()
{

}

void ApeInGameBrowserSamplePlugin::Restart()
{

}
