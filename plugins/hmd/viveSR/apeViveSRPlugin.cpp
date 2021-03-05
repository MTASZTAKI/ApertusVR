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
		int distorted_width, distorted_height, distorted_channel;
		ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_DISTORTED_WIDTH, &distorted_width);
		ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_DISTORTED_HEIGHT, &distorted_height);
		ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_DISTORTED_CHANNEL, &distorted_channel);
		int undistorted_width, undistorted_height, undistorted_channel;
		ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_UNDISTORTED_WIDTH, &undistorted_width);
		ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_UNDISTORTED_HEIGHT, &undistorted_height);
		ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_UNDISTORTED_CHANNEL, &undistorted_channel);
		std::unique_ptr<char[]> distorted_frame_left = std::make_unique<char[]>(distorted_width * distorted_height * distorted_channel);
		std::unique_ptr<char[]> distorted_frame_right = std::make_unique<char[]>(distorted_width * distorted_height * distorted_channel);
		std::unique_ptr<char[]> undistorted_frame_left = std::make_unique<char[]>(undistorted_width * undistorted_height * undistorted_channel);
		std::unique_ptr<char[]> undistorted_frame_right = std::make_unique<char[]>(undistorted_width * undistorted_height * undistorted_channel);
		float pose_left[16];
		float pose_right[16];
		std::unique_ptr<char[]> camera_params_see_through = std::make_unique<char[]>(1032);
		unsigned int frameSeq;
		unsigned int timeStp;
		int lux_left;
		int lux_right;
		int color_temperature_left;
		int color_temperature_right;
		int exposure_time_left;
		int exposure_time_right;
		int analog_gain_left;
		int analog_gain_right;
		int digital_gain_left;
		int digital_gain_right;
		const int see_throught_length = ViveSR::PassThrough::OutputMask::MAX;
		static std::vector<ViveSR::MemoryElement> see_through_element(see_throught_length);
		void* see_through_ptrs[ViveSR::PassThrough::OutputMask::MAX]{
			distorted_frame_left.get(),
			distorted_frame_right.get(),
			undistorted_frame_left.get(),
			undistorted_frame_right.get(),
			&frameSeq,
			&timeStp,
			pose_left,
			pose_right,
			&lux_left,
			&lux_right,
			&color_temperature_left,
			&color_temperature_right,
			&exposure_time_left,
			&exposure_time_right,
			&analog_gain_left,
			&analog_gain_right,
			&digital_gain_left,
			&digital_gain_right,
			camera_params_see_through.get()
		};
		int see_through_count = 0;
		for (int i = 0; i < see_throught_length; ++i)
		{
			if (see_through_ptrs[i])
			{
				see_through_element[see_through_count].mask = i;
				see_through_element[see_through_count].ptr = see_through_ptrs[i];
				see_through_count++;
			}
		}
		const static int DEPTH_ELEM_LENGTH = ViveSR::Depth::OutputMask::OUTPUT_MASK_MAX;
		int depth_img_width, depth_img_height, depth_img_channel, depth_color_img_channel;
		ViveSR_GetParameterInt(mViveSrDepthID, ViveSR::Depth::OUTPUT_WIDTH, &depth_img_width);
		ViveSR_GetParameterInt(mViveSrDepthID, ViveSR::Depth::OUTPUT_HEIGHT, &depth_img_height);
		ViveSR_GetParameterInt(mViveSrDepthID, ViveSR::Depth::OUTPUT_CHAANEL_1, &depth_img_channel);
		ViveSR_GetParameterInt(mViveSrDepthID, ViveSR::Depth::OUTPUT_CHAANEL_0, &depth_color_img_channel);
		static std::vector<ViveSR::MemoryElement> depth_element(DEPTH_ELEM_LENGTH);
		auto left_frame = std::make_unique<char[]>(depth_img_height * depth_img_width * depth_color_img_channel);
		auto depth_map = std::make_unique<float[]>(depth_img_height * depth_img_width * depth_img_channel);
		unsigned int frame_seq;
		unsigned int time_stp;
		float pose[16];
		auto camera_params = std::make_unique<char[]>(1032);
		void* depth_ptrs[ViveSR::Depth::OutputMask::OUTPUT_MASK_MAX]{
			left_frame.get(),
			depth_map.get(),
			&frame_seq,
			&time_stp,
			pose,
			&lux_left,
			&color_temperature_left,
			&exposure_time_left,
			&analog_gain_left,
			&digital_gain_left,
			camera_params.get(),
		};
		int depth_count = 0;
		for (int i = 0; i < DEPTH_ELEM_LENGTH; ++i)
		{
			if (depth_ptrs[i])
			{
				depth_element[depth_count].mask = i;
				depth_element[depth_count].ptr = depth_ptrs[i];
				depth_count++;
			}
		}
		while (true)
		{
			APE_LOG_DEBUG("passthrough data require started, count: " << see_through_count);
			int res = ViveSR::FAILED;
			res = ViveSR_GetModuleData(mViveSrPassThroughID, see_through_element.data(), see_through_count);
			if (res == ViveSR::WORK)
			{
				//auto leftImage = distorted_frame_left.get();
				//auto rightImage = distorted_frame_right.get();
				APE_LOG_DEBUG("passthrough data ok");
			}
			else
			{
				APE_LOG_DEBUG("no passthrough data");
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
