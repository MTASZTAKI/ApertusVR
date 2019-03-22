#include "ape360VideoPlugin.h"

ape::ape360VideoPlugin::ape360VideoPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::ape360VideoPlugin::~ape360VideoPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360VideoPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto browserNode = mpSceneManager->createNode("browserNode").lock())
	{
		//browserNode->setScale(ape::Vector3(10, 10, 10));
		if (auto browserGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			//browserGeometry->setFileName("sphere.mesh");
			browserGeometry->setFileName("thetaSphere.mesh");
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->createEntity("browser", ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(2048, 1024);
				browser->setURL("https://www.youtube.com/embed/ubBrznOxtQo?vq=hd1080&autoplay=1&loop=1&playlist=ubBrznOxtQo");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360VideoPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360VideoPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360VideoPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360VideoPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360VideoPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
