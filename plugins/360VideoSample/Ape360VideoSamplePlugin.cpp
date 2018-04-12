#include <iostream>
#include "Ape360VideoSamplePlugin.h"

Ape360VideoSamplePlugin::Ape360VideoSamplePlugin()
{
	LOG_FUNC_ENTER();
	mpScene = Ape::IScene::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

Ape360VideoSamplePlugin::~Ape360VideoSamplePlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape360VideoSamplePlugin::Init()
{
	LOG_FUNC_ENTER();
	if (auto browserNode = mpScene->createNode("browserNode").lock())
	{
		//browserNode->setScale(Ape::Vector3(10, 10, 10));
		if (auto browserGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			//browserGeometry->setFileName("sphere.mesh");
			browserGeometry->setFileName("thetaSphere.mesh");
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("browser", Ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(2048, 1024);
				browser->setURL("https://www.youtube.com/embed/ubBrznOxtQo?vq=hd1080&autoplay=1&loop=1&playlist=ubBrznOxtQo");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	LOG_FUNC_LEAVE();
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
