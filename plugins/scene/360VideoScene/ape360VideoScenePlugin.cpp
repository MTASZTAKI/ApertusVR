#include "ape360VideoScenePlugin.h"

ape::Video360ScenePlugin::Video360ScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mLastBrowserReload = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	APE_LOG_FUNC_LEAVE();
}

ape::Video360ScenePlugin::~Video360ScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Video360ScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Video360ScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	//TODO we have to wait because of the possible GPU race
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	if (auto browserNode = mpSceneManager->createNode("browserNode", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		browserNode->setPosition(ape::Vector3(-100, 170, 0));
		if (auto browserGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			browserGeometry->setFileName("thetaSphere.mesh");
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->createEntity("browser", ape::Entity::BROWSER, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				browser->setResoultion(2048, 1024);
				browser->setURL("https://www.youtube.com/embed/aRB13POSo80?start=400&vq=hd1080&autoplay=1&playlist=aRB13POSo80");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		auto timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		long timeToReloadBrowserInMilliseconds = timeStamp.count() - mLastBrowserReload.count();
		if (timeToReloadBrowserInMilliseconds > 900000)
		{
			if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity("browser").lock()))
			{
				browser->reload();
				mLastBrowserReload = timeStamp;
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::Video360ScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Video360ScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Video360ScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Video360ScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
