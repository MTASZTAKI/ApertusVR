#include "ApePcdImporterPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

Ape::PcdImporterPlugin::PcdImporterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	std::srand(std::time(0));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&PcdImporterPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

Ape::PcdImporterPlugin::~PcdImporterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&PcdImporterPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::PcdImporterPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpAssimpImporter = new Assimp::Importer();
	APE_LOG_FUNC_LEAVE();
}

void Ape::PcdImporterPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	loadConfig();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::PcdImporterPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::PcdImporterPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::PcdImporterPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::PcdImporterPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::PcdImporterPlugin::eventCallBack(const Ape::Event & event)
{
	if (event.type == Ape::Event::Type::GEOMETRY_FILE_FILENAME)
	{
	}
}
