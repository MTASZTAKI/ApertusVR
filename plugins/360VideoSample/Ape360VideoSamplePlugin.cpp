#include <iostream>
#include "Ape360VideoSamplePlugin.h"

Ape360VideoSamplePlugin::Ape360VideoSamplePlugin()
{
	mpScene = Ape::IScene::getSingletonPtr();
}

Ape360VideoSamplePlugin::~Ape360VideoSamplePlugin()
{
	std::cout << "Ape360VideoSamplePlugin dtor" << std::endl;
}

void Ape360VideoSamplePlugin::Init()
{
	if (auto browserNode = mpScene->createNode("browserNode").lock())
	{
		if (auto browserGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			browserGeometry->setFileName("sphere.mesh");
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("browser", Ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(2048, 1024);
				//browser->setURL("https://www.youtube.com/embed/9b4VCYLOL8Q?vq=hd1080&autoplay=1&loop=1");
				browser->setURL("https://www.youtube.com/embed/WRW84FRYPkk?vq=hd1080&autoplay=1&loop=1");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	std::cout << "Ape360VideoSamplePlugin::init" << std::endl;
}

void Ape360VideoSamplePlugin::Run()
{

}

void Ape360VideoSamplePlugin::Step()
{

}

void Ape360VideoSamplePlugin::Stop()
{

}

void Ape360VideoSamplePlugin::Suspend()
{

}

void Ape360VideoSamplePlugin::Restart()
{

}
