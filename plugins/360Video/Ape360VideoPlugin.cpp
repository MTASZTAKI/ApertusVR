#include "Ape360VideoPlugin.h"

Ape::Ape360VideoPlugin::Ape360VideoPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::Ape360VideoPlugin::~Ape360VideoPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::Ape360VideoPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto browserNode = mpSceneManager->createNode("browserNode").lock())
	{
		//browserNode->setScale(Ape::Vector3(10, 10, 10));
		if (auto browserGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			//browserGeometry->setFileName("sphere.mesh");
			browserGeometry->setFileName("thetaSphere.mesh");
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->createEntity("browser", Ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(2048, 1024);
				browser->setURL("https://www.youtube.com/embed/ubBrznOxtQo?vq=hd1080&autoplay=1&loop=1&playlist=ubBrznOxtQo");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::Ape360VideoPlugin::Run()
{

}

void Ape::Ape360VideoPlugin::Step()
{

}

void Ape::Ape360VideoPlugin::Stop()
{

}

void Ape::Ape360VideoPlugin::Suspend()
{

}

void Ape::Ape360VideoPlugin::Restart()
{

}
