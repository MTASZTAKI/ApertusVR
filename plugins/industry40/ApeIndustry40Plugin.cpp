#include "apeIndustry40Plugin.h"

ape::apeIndustry40Plugin::apeIndustry40Plugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apeIndustry40Plugin::~apeIndustry40Plugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeIndustry40Plugin::Init()
{
	APE_LOG_FUNC_ENTER();
	//mpSceneMakerMacro->makeLit();
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light", ape::Entity::LIGHT).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(1, -1, 0));
		light->setDiffuseColor(ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(ape::Color(0.6f, 0.6f, 0.6f));
	}
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light2", ape::Entity::LIGHT).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(0, -1, 1));
		light->setDiffuseColor(ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(ape::Color(0.6f, 0.6f, 0.6f));
	}
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeGround();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
