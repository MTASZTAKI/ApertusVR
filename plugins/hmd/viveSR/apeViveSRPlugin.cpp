#include "apeViveSRPlugin.h"

ape::ViveSRPlugin::ViveSRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	//mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ViveSRPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::ViveSRPlugin::~ViveSRPlugin()
{
	APE_LOG_FUNC_ENTER();
	//mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&ViveSRPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::ViveSRPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	APE_LOG_DEBUG("waiting for main window");
	while (mpCoreConfig->getWindowConfig().handle == nullptr && mpCoreConfig->getWindowConfig().device == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("main window was found");
	mIsViveSrInit = false;
	int res = ViveSR::Error::INITIAL_FAILED;
	if (ViveSR_GetContextInfo(NULL) != ViveSR::Error::WORK) 
	{
		res = ViveSR_CreateContext("", 0);
	}
	if (res != ViveSR::Error::WORK) 
	{
		APE_LOG_DEBUG("ViveSR_Initial failed: " << res);
	}
	else
	{
		res = ViveSR_CreateModule(ViveSR::ModuleType::ENGINE_PASS_THROUGH, &mViveSrPassThroughID);
		if (res != ViveSR::Error::WORK)
		{
			APE_LOG_DEBUG("ViveSR_CreateModule(" << mViveSrPassThroughID << "): " << res << " failed");
		}
		else
		{
			APE_LOG_DEBUG("ViveSR_CreateModule(" << mViveSrPassThroughID << "): " << res << " success");
		}
		res = ViveSR_CreateModule(ViveSR::ModuleType::ENGINE_DEPTH, &mViveSrDepthID);
		if (res != ViveSR::Error::WORK)
		{
			APE_LOG_DEBUG("ViveSR_CreateModule(" << mViveSrDepthID << "): " << res << " failed");
		}
		else
		{
			APE_LOG_DEBUG("ViveSR_CreateModule(" << mViveSrDepthID << "): " << res << " success");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		ViveSR_InitialModule(mViveSrPassThroughID);
		ViveSR_SetParameterBool(mViveSrPassThroughID, ViveSR::PassThrough::Param::DISTORT_GPU_TO_CPU_ENABLE, true);
		ViveSR_InitialModule(mViveSrDepthID);
		int result = ViveSR::Error::FAILED;
		for (int i = ViveSR::PassThrough::CAMERA_Param::CAMERA_FCX_0; i < ViveSR::PassThrough::CAMERA_Param::CAMERA_PARAMS_MAX; i++)
		{
			result = ViveSR_GetParameterDouble(mViveSrPassThroughID, i, &(mViveSrCameraParameters[i]));
			if (result == ViveSR::Error::FAILED)
			{
				APE_LOG_DEBUG("ViveSR_CreateModule(" << mViveSrDepthID << "): " << res << " failed");
			}
		}
		ViveSR_SetParameterBool(mViveSrPassThroughID, ViveSR::PassThrough::Param::VR_INIT, true);
		ViveSR_SetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::Param::VR_INIT_TYPE, ViveSR::PassThrough::InitType::SCENE);
		res = ViveSR_StartModule(mViveSrPassThroughID);
		if (res != ViveSR::Error::WORK) 
		{ 
			APE_LOG_DEBUG("ViveSR_StartModule failed: " << res);
		}
		else
		{
			APE_LOG_DEBUG("ViveSR_StartModule(" << mViveSrPassThroughID << "): " << res << " success");
		}
		res = ViveSR_StartModule(mViveSrDepthID);
		if (res != ViveSR::Error::WORK)
		{ 
			APE_LOG_DEBUG("ViveSR_StartModule failed: " << res);
			
		}
		else
		{
			APE_LOG_DEBUG("ViveSR_StartModule(" << mViveSrDepthID << "): " << res << " success");
		}
		res = ViveSR_LinkModule(mViveSrPassThroughID, mViveSrDepthID, ViveSR::LinkType::ACTIVE);
		if (res != ViveSR::Error::WORK) 
		{ 
			APE_LOG_DEBUG("ViveSRs_link failed: " << res);
		}
		else
		{
			APE_LOG_DEBUG("ViveSR_LinkModule(" << mViveSrPassThroughID << " and " << mViveSrDepthID << "): " << res << " success");
			mIsViveSrInit = true;
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	if (mIsViveSrInit)
	{
		APE_LOG_DEBUG("ViveSR init success");
		//TODO opt
		const unsigned int distorted_width = 640;
		const unsigned int distorted_height = 480;
		const unsigned int distorted_channel = 4;
		unsigned char* distorted_frame_left = new unsigned char[distorted_width * distorted_height * distorted_channel];
		unsigned char* distorted_frame_right = new unsigned char[distorted_width * distorted_height * distorted_channel];
		ViveSR::MemoryElement* distorted_frame_left_data = new ViveSR::MemoryElement();
		distorted_frame_left_data->mask = 0;
		distorted_frame_left_data->ptr = distorted_frame_left;
		ViveSR::MemoryElement* distorted_frame_right_data = new ViveSR::MemoryElement();
		distorted_frame_right_data->mask = 0;
		distorted_frame_right_data->ptr = distorted_frame_right;
		while (true)
		{
			int res = ViveSR::FAILED;
			res = ViveSR_GetModuleData(mViveSrPassThroughID, distorted_frame_left_data, 1);
			if (res == ViveSR::WORK)
			{
				APE_LOG_DEBUG("distorted_frame_left_data ok");
			}
			res = ViveSR_GetModuleData(mViveSrPassThroughID, distorted_frame_right_data, 1);
			if (res == ViveSR::WORK)
			{
				APE_LOG_DEBUG("distorted_frame_right_data ok");
			}
			//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
	else
	{
		APE_LOG_DEBUG("ViveSR init failed");
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
