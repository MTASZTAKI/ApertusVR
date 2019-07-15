#include <fstream>
#include "apeBulletPhysicsPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::BulletPhysicsPlugin::BulletPhysicsPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::BulletPhysicsPlugin::~BulletPhysicsPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::BulletPhysicsPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for (std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
