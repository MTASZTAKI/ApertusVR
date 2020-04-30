#include "apeOpenXRPlugin.h"


ape::OpenXRPlugin::OpenXRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::OpenXRPlugin::~OpenXRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::OpenXRPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
}

void ape::OpenXRPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("try to initialize openXR HMD");
	std::shared_ptr<Options> options = std::make_shared<Options>();
	std::shared_ptr<PlatformData> data = std::make_shared<PlatformData>();
	std::shared_ptr<IPlatformPlugin> platformPlugin = CreatePlatformPlugin(options, data);
	std::shared_ptr<IGraphicsPlugin> graphicsPlugin = CreateGraphicsPlugin(options, platformPlugin);
	std::shared_ptr<IOpenXrProgram> program = CreateOpenXrProgram(options, platformPlugin, graphicsPlugin);
	program->CreateInstance();
	program->InitializeSystem();
	program->InitializeSession();
	program->CreateSwapchains();
	APE_LOG_DEBUG("try to run openXR HMD");
	while (true)
	{
		program->PollEvents(false, false);
		if (program->IsSessionRunning()) 
		{
			program->PollActions();
			program->RenderFrame();
		}
		else 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
