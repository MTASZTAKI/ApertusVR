#include "apeHtcVivePlugin.h"

ape::apeHtcVivePlugin::apeHtcVivePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&apeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mCameraLeft = ape::CameraWeakPtr();
	mCameraRight = ape::CameraWeakPtr();
	mOpenVrRttTextureIDs[0] = nullptr;
	mOpenVrRttTextureIDs[1] = nullptr;
	mpapeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	APE_LOG_FUNC_LEAVE();
}

ape::apeHtcVivePlugin::~apeHtcVivePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

ape::Matrix4 ape::apeHtcVivePlugin::conversionFromOpenVR(vr::HmdMatrix34_t ovrMatrix34)
{
	ape::Matrix4 matrix4(
		ovrMatrix34.m[0][0], ovrMatrix34.m[0][1], ovrMatrix34.m[0][2], ovrMatrix34.m[0][3],
		ovrMatrix34.m[1][0], ovrMatrix34.m[1][1], ovrMatrix34.m[1][2], ovrMatrix34.m[1][3],
		ovrMatrix34.m[2][0], ovrMatrix34.m[2][1], ovrMatrix34.m[2][2], ovrMatrix34.m[2][3],
		0.0f, 0.0f, 0.0f, 1.0f);
	return matrix4;
}

ape::Matrix4 ape::apeHtcVivePlugin::conversionFromOpenVR(vr::HmdMatrix44_t ovrMatrix44)
{
	ape::Matrix4 matrix4(
		ovrMatrix44.m[0][0], ovrMatrix44.m[0][1], ovrMatrix44.m[0][2], ovrMatrix44.m[0][3],
		ovrMatrix44.m[1][0], ovrMatrix44.m[1][1], ovrMatrix44.m[1][2], ovrMatrix44.m[1][3],
		ovrMatrix44.m[2][0], ovrMatrix44.m[2][1], ovrMatrix44.m[2][2], ovrMatrix44.m[2][3],
		ovrMatrix44.m[3][0], ovrMatrix44.m[3][1], ovrMatrix44.m[3][2], ovrMatrix44.m[3][3]);
	return matrix4;
}

void ape::apeHtcVivePlugin::submitTextureLeftToOpenVR()
{
	vr::VRCompositor()->Submit(vr::Eye_Left, &mOpenVrTextures[0], &mOpenVrTextureBounds[0]);
	vr::VRCompositor()->Submit(vr::Eye_Right, &mOpenVrTextures[1], &mOpenVrTextureBounds[1]);
	vr::EVRCompositorError error;
	error = vr::VRCompositor()->WaitGetPoses(mOpenVrTrackedPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
	if (!error)
	{
		vr::TrackedDevicePose_t controllerPose;
		unsigned int controllerID = 3;
		if ((controllerPose = mOpenVrTrackedPoses[controllerID]).bPoseIsValid)
		{
			auto openVRPose = controllerPose.mDeviceToAbsoluteTracking;
			ape::Vector3 controllerTrackerScale;
			ape::Quaternion controllerTrackerOrientation;
			ape::Vector3 controllerTrackerPosition;
			ape::Matrix4 controllerTrackerPose = conversionFromOpenVR(openVRPose);
			controllerTrackerPose.decomposition(controllerTrackerScale, controllerTrackerOrientation, controllerTrackerPosition);
			controllerTrackerScale = 100;
			controllerTrackerPosition = controllerTrackerPosition * controllerTrackerScale;
			vr::VRControllerState_t openVRControllerState;
			mpOpenVrSystem->GetControllerState(controllerID, &openVRControllerState, sizeof openVRControllerState);
			mUserInputMacroPose.userPosition += (controllerTrackerOrientation * ape::Vector3(0, 0, 5 * -openVRControllerState.rAxis[0].y));
		}
		vr::TrackedDevicePose_t hmdPose;
		if ((hmdPose = mOpenVrTrackedPoses[vr::k_unTrackedDeviceIndex_Hmd]).bPoseIsValid)
		{
			auto openVRPose = hmdPose.mDeviceToAbsoluteTracking;
			ape::Vector3 hmdTrackerScale;
			ape::Quaternion hmdTrackerOrientation;
			ape::Vector3 hmdTrackerPosition;
			ape::Matrix4 hmdTrackerPose = conversionFromOpenVR(openVRPose);
			hmdTrackerPose.decomposition(hmdTrackerScale, hmdTrackerOrientation, hmdTrackerPosition);
			hmdTrackerScale = 100;
			hmdTrackerPosition = hmdTrackerPosition * hmdTrackerScale;
			mUserInputMacroPose.headPosition = hmdTrackerPosition;
			mUserInputMacroPose.headOrientation = hmdTrackerOrientation;
		}
		mpapeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	else
	{
		APE_LOG_DEBUG("Error WaitGetPoses:" << error);
	}
}

void ape::apeHtcVivePlugin::submitTextureRightToOpenVR()
{
	//for openVR the only chanche to submit and render the compositor if you doing everything
	//in only one CB function from the render thread which currently is ape::apeHtcVivePlugin::submitTextureLeftToOpenVR() function
}

void ape::apeHtcVivePlugin::eventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::TEXTURE_MANUAL_GRAPHICSAPIID)
	{
		if (auto textureManual = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			if (event.subjectName == "OpenVrRenderTextureLeft")
			{
				mOpenVrRttTextureIDs[0] = textureManual->getGraphicsApiID();
				textureManual->registerFunction(std::bind(&apeHtcVivePlugin::submitTextureLeftToOpenVR, this));
			}
			else if (event.subjectName == "OpenVrRenderTextureRight")
			{
				mOpenVrRttTextureIDs[1] = textureManual->getGraphicsApiID();
				textureManual->registerFunction(std::bind(&apeHtcVivePlugin::submitTextureRightToOpenVR, this));
			}
		}
	}
}

void ape::apeHtcVivePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("try to initialize openVR HMD");
	mpOpenVrSystem = vr::VR_Init(&mOpenVrHmdError, vr::EVRApplicationType::VRApplication_Scene);
	if (mOpenVrHmdError != vr::VRInitError_None)
	{
		switch (mOpenVrHmdError)
		{
		default:
			APE_LOG_DEBUG("Error: failed OpenVR VR_Init, Undescribed error when initalizing the OpenVR Render object");
		case vr::VRInitError_Init_HmdNotFound:
		case vr::VRInitError_Init_HmdNotFoundPresenceFailed:
			APE_LOG_DEBUG("Error: cannot find HMD, OpenVR cannot find HMD, Please install SteamVR and launch it, and verrify HMD USB and HDMI connection");
		}
	}
	if (!vr::VRCompositor())
	{
		APE_LOG_DEBUG("Error: failed to init OpenVR VRCompositor, Failed to initialize the VR Compositor");
	}
	char buf[128];
	vr::TrackedPropertyError err;
	mpOpenVrSystem->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, buf, sizeof(buf), &err);
	if (err != vr::TrackedProp_Success)
	{
		APE_LOG_DEBUG("Error Getting String: " << mpOpenVrSystem->GetPropErrorNameFromEnum(err));
	}
	else
	{
		APE_LOG_DEBUG("OpenVR device system driver name: " << buf);
	}
	unsigned int width, height;
	mpOpenVrSystem->GetRecommendedRenderTargetSize(&width, &height);
	APE_LOG_DEBUG("Recomended Render Target Size : " << width << "x" << height);

	if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity("OpenVrRenderTextureLeft", ape::Entity::TEXTURE_MANUAL).lock()))
	{
		manualTexture->setParameters(width, height, ape::Texture::PixelFormat::R8G8B8A8, ape::Texture::Usage::RENDERTARGET);
		mManualTextureLeftEye = manualTexture;
	}
	if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity("OpenVrRenderTextureRight", ape::Entity::TEXTURE_MANUAL).lock()))
	{
		manualTexture->setParameters(width, height, ape::Texture::PixelFormat::R8G8B8A8, ape::Texture::Usage::RENDERTARGET);
		mManualTextureRightEye = manualTexture;
	}
	mCameraLeft = mpapeUserInputMacro->createCamera("OpenVRHmdLeftCamera");
	mCameraRight = mpapeUserInputMacro->createCamera("OpenVRHmdRightCamera");
	vr::HmdMatrix44_t projectionLeft = mpOpenVrSystem->GetProjectionMatrix(vr::Eye_Left, 1, 10000);
	vr::HmdMatrix44_t projectionRight = mpOpenVrSystem->GetProjectionMatrix(vr::Eye_Right, 1, 10000);
	if (auto cameraLeft = mCameraLeft.lock())
	{
		cameraLeft->setAutoAspectRatio(true);
		if (auto cameraNode = cameraLeft->getParentNode().lock())
		{
			ape::Vector3 scale;
			ape::Quaternion rotation;
			ape::Vector3 translate;
			conversionFromOpenVR(mpOpenVrSystem->GetEyeToHeadTransform(vr::Eye_Left)).makeTransform(scale, rotation, translate);
			cameraNode->setPosition(translate);
		}
		if (auto texture = mManualTextureLeftEye.lock())
			texture->setSourceCamera(cameraLeft);
		cameraLeft->setProjection(conversionFromOpenVR(projectionLeft));
	}
	if (auto cameraRight = mCameraRight.lock())
	{
	    cameraRight->setAutoAspectRatio(true);
		if (auto cameraNode = cameraRight->getParentNode().lock())
		{
			ape::Vector3 scale;
			ape::Quaternion rotation;
			ape::Vector3 translate;
			conversionFromOpenVR(mpOpenVrSystem->GetEyeToHeadTransform(vr::Eye_Right)).makeTransform(scale, rotation, translate);
			cameraNode->setPosition(translate);
		}
		if (auto texture = mManualTextureRightEye.lock())
			texture->setSourceCamera(cameraRight);
		cameraRight->setProjection(conversionFromOpenVR(projectionRight));
	}
	APE_LOG_DEBUG("Wait while RTT textures are created...");
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		if (mOpenVrRttTextureIDs[0] != nullptr && mOpenVrRttTextureIDs[1] != nullptr)
		{
			break;
		}
	}
	APE_LOG_DEBUG("RTT textures are successfully created...");
	mOpenVrTextures[0].handle = mOpenVrRttTextureIDs[0];
	mOpenVrTextures[0].eType = vr::ETextureType::TextureType_OpenGL;
	mOpenVrTextures[0].eColorSpace = vr::ColorSpace_Gamma;
	mOpenVrTextures[1].handle = mOpenVrRttTextureIDs[1];
	mOpenVrTextures[1].eType = vr::ETextureType::TextureType_OpenGL;
	mOpenVrTextures[1].eColorSpace = vr::ColorSpace_Gamma;
	mOpenVrTextureBounds[0].uMin = 0;
	mOpenVrTextureBounds[0].uMax = 1;
	mOpenVrTextureBounds[0].vMin = 1;
	mOpenVrTextureBounds[0].vMax = 0;
	mOpenVrTextureBounds[1].uMin = 0;
	mOpenVrTextureBounds[1].uMax = 1;
	mOpenVrTextureBounds[1].vMin = 1;
	mOpenVrTextureBounds[1].vMax = 0;
	APE_LOG_DEBUG("mOpenVrTextures[0]:" << mOpenVrTextures[0].handle << " mOpenVrTextures[1]" << mOpenVrTextures[1].handle);
}

void ape::apeHtcVivePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		//nothing to do there becuse it is the apeHtcVivePlugin thread. 
		//compositor and pose update is done by the ape::apeHtcVivePlugin::submitTextureLeftToOpenVR() function called from the rendering thread
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtcVivePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtcVivePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtcVivePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtcVivePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
