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
		browserNode->setPosition(Ape::Vector3(0, 0, -100));
		Ape::Radian angle(1.57f);
		Ape::Vector3 axis(1, 0, 0);
		Ape::Quaternion orientation;
		orientation.FromAngleAxis(angle, axis);
		browserNode->setOrientation(orientation);
		if (auto browserGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("browserGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			browserGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(100, 70), Ape::Vector2(1, 1));
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("browser", Ape::Entity::BROWSER).lock()))
				browser->setGeometry(browserGeometry);
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
