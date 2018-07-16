#include <iostream>
#include "ApeHtcVivePlugin.h"

Ape::ApeHtcVivePlugin::ApeHtcVivePlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mCameraLeft = Ape::CameraWeakPtr();
	mCameraRight = Ape::CameraWeakPtr();
	mHeadNode = Ape::NodeWeakPtr();
	mUserMaterial = Ape::ManualMaterialWeakPtr();
	mpOpenVrSystem = nullptr;
	mOpenVrHmdError = vr::HmdError();
	mOpenVrTrackedPoses[vr::k_unMaxTrackedDeviceCount] = vr::TrackedDevicePose_t();
	LOG_FUNC_LEAVE();
}

Ape::ApeHtcVivePlugin::~ApeHtcVivePlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

Ape::Matrix4 Ape::ApeHtcVivePlugin::conversionFromOpenVR(vr::HmdMatrix34_t ovrMatrix34)
{
	Ape::Matrix4 matrix4(
		ovrMatrix34.m[0][0], ovrMatrix34.m[0][1], ovrMatrix34.m[0][2], ovrMatrix34.m[0][3],
		ovrMatrix34.m[1][0], ovrMatrix34.m[1][1], ovrMatrix34.m[1][2], ovrMatrix34.m[1][3],
		ovrMatrix34.m[2][0], ovrMatrix34.m[2][1], ovrMatrix34.m[2][2], ovrMatrix34.m[2][3],
		0.0f, 0.0f, 0.0f, 1.0f);
	return matrix4;
}

Ape::Matrix4 Ape::ApeHtcVivePlugin::conversionFromOpenVR(vr::HmdMatrix44_t ovrMatrix44)
{
	Ape::Matrix4 matrix4(
		ovrMatrix44.m[0][0], ovrMatrix44.m[0][1], ovrMatrix44.m[0][2], ovrMatrix44.m[0][3],
		ovrMatrix44.m[1][0], ovrMatrix44.m[1][1], ovrMatrix44.m[1][2], ovrMatrix44.m[1][3],
		ovrMatrix44.m[2][0], ovrMatrix44.m[2][1], ovrMatrix44.m[2][2], ovrMatrix44.m[2][3],
		ovrMatrix44.m[3][0], ovrMatrix44.m[3][1], ovrMatrix44.m[3][2], ovrMatrix44.m[3][3]);
	return matrix4;
}

Ape::CameraWeakPtr Ape::ApeHtcVivePlugin::createCamera(std::string name)
{
	if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->createEntity(name, Ape::Entity::Type::CAMERA).lock()))
	{
		if (auto cameraNode = mpScene->createNode(name + "_Node").lock())
		{
			cameraNode->setParentNode(mHeadNode);
			if (auto cameraConeNode = mpScene->createNode(name + "_ConeNode").lock())
			{
				cameraConeNode->setParentNode(cameraNode);
				cameraConeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
				if (auto cameraCone = std::static_pointer_cast<Ape::IConeGeometry>(mpScene->createEntity(name + "_ConeGeometry", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					cameraCone->setParameters(10.0f, 30.0f, 1.0f, Ape::Vector2(1, 1));
					cameraCone->setParentNode(cameraConeNode);
					cameraCone->setMaterial(mUserMaterial);
				}
			}
			if (auto userNameTextNode = mpScene->createNode(name + "_TextNode").lock())
			{
				userNameTextNode->setParentNode(cameraNode);
				userNameTextNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
				if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity(name + "_TextGeometry", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					userNameText->setCaption(name);
					userNameText->setParentNode(userNameTextNode);
				}
			}
			camera->setParentNode(cameraNode);
		}
		return camera;
	}
}


void Ape::ApeHtcVivePlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeHtcVivePlugin::Init()
{
	LOG_FUNC_ENTER();

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		userNode->setFixedYaw(true);
		mUserNode = userNode;
		if (auto headNode = mpScene->getNode(userNode->getName() + "_HeadNode").lock())
		{
			mHeadNode = headNode;
		}
		if (auto userMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->getEntity(userNode->getName() + "_Material").lock()))
		{
			mUserMaterial = userMaterial;
		}
	}

	LOG(LOG_TYPE_DEBUG, "waiting for main window");
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LOG(LOG_TYPE_DEBUG, "main window was found");

	LOG(LOG_TYPE_DEBUG, "try to initialize openVR HMD");
	mpOpenVrSystem = vr::VR_Init(&mOpenVrHmdError, vr::EVRApplicationType::VRApplication_Scene);
	if (mOpenVrHmdError != vr::VRInitError_None)
	{
		switch (mOpenVrHmdError)
		{
		default:
			LOG(LOG_TYPE_DEBUG, "Error: failed OpenVR VR_Init, Undescribed error when initalizing the OpenVR Render object");
		case vr::VRInitError_Init_HmdNotFound:
		case vr::VRInitError_Init_HmdNotFoundPresenceFailed:
			LOG(LOG_TYPE_DEBUG, "Error: cannot find HMD, OpenVR cannot find HMD, Please install SteamVR and launch it, and verrify HMD USB and HDMI connection");
		}
	}
	if (!vr::VRCompositor())
	{
		LOG(LOG_TYPE_DEBUG, "Error: failed to init OpenVR VRCompositor, Failed to initialize the VR Compositor");
	}
	char buf[128];
	vr::TrackedPropertyError err;
	mpOpenVrSystem->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, buf, sizeof(buf), &err);
	if (err != vr::TrackedProp_Success)
	{
		LOG(LOG_TYPE_DEBUG, "Error Getting String: " << mpOpenVrSystem->GetPropErrorNameFromEnum(err));
	}
	else
	{
		LOG(LOG_TYPE_DEBUG, "OpenVR device system driver name: " << buf);
	}
	unsigned int widht, height;
	mpOpenVrSystem->GetRecommendedRenderTargetSize(&widht, &height);
	LOG(LOG_TYPE_DEBUG, "Recomended Render Target Size : " << widht << "x" << height);

	Ape::ManualTextureWeakPtr manualTextureRightEye, manualTextureLeftEye;
	if (auto manualTexture = std::static_pointer_cast<Ape::IManualTexture>(mpScene->createEntity("OpenVrRenderTextureLeft", Ape::Entity::TEXTURE_MANUAL).lock()))
	{
		manualTexture->setParameters(widht, height, Ape::Texture::PixelFormat::R8G8B8, Ape::Texture::Usage::RENDERTARGET);
		manualTextureLeftEye = manualTexture;
	}
	if (auto manualTexture = std::static_pointer_cast<Ape::IManualTexture>(mpScene->createEntity("OpenVrRenderTextureRight", Ape::Entity::TEXTURE_MANUAL).lock()))
	{
		manualTexture->setParameters(widht, height, Ape::Texture::PixelFormat::R8G8B8, Ape::Texture::Usage::RENDERTARGET);
		manualTextureRightEye = manualTexture;
	}
	if (auto userNode = mUserNode.lock())
	{
		mCameraLeft = createCamera(userNode->getName() + "OpenVRHmdLeftCamera");
		mCameraRight = createCamera(userNode->getName() + "OpenVRHmdRightCamera");
	}
	vr::HmdMatrix44_t projectionLeft = mpOpenVrSystem->GetProjectionMatrix(vr::Eye_Left, 1, 10000);
	vr::HmdMatrix44_t projectionRight = mpOpenVrSystem->GetProjectionMatrix(vr::Eye_Right, 1, 10000);
	if (auto cameraLeft = mCameraLeft.lock())
	{
		cameraLeft->setAutoAspectRatio(true);
		if (auto cameraNode = cameraLeft->getParentNode().lock())
		{
			Ape::Vector3 scale;
			Ape::Quaternion rotation;
			Ape::Vector3 translate;
			conversionFromOpenVR(mpOpenVrSystem->GetEyeToHeadTransform(vr::Eye_Left)).makeTransform(scale, rotation, translate);
			cameraNode->setPosition(translate);
		}
		if (auto texture = manualTextureLeftEye.lock())
			texture->setSourceCamera(cameraLeft);
		cameraLeft->setProjection(conversionFromOpenVR(projectionLeft));
	}
	if (auto cameraRight = mCameraRight.lock())
	{
	    cameraRight->setAutoAspectRatio(true);
		if (auto cameraNode = cameraRight->getParentNode().lock())
		{
			Ape::Vector3 scale;
			Ape::Quaternion rotation;
			Ape::Vector3 translate;
			conversionFromOpenVR(mpOpenVrSystem->GetEyeToHeadTransform(vr::Eye_Right)).makeTransform(scale, rotation, translate);
			cameraNode->setPosition(translate);
		}
		if (auto texture = manualTextureRightEye.lock())
			texture->setSourceCamera(cameraRight);
		cameraRight->setProjection(conversionFromOpenVR(projectionRight));
	}
}

void Ape::ApeHtcVivePlugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		vr::VRCompositor()->WaitGetPoses(mOpenVrTrackedPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
		vr::TrackedDevicePose_t hmdPose;
		if ((hmdPose = mOpenVrTrackedPoses[vr::k_unTrackedDeviceIndex_Hmd]).bPoseIsValid)
		{
			auto pose = hmdPose.mDeviceToAbsoluteTracking;
			Ape::Vector3 scale;
			Ape::Quaternion rotation;
			Ape::Vector3 translate;
			conversionFromOpenVR(pose).makeTransform(scale, rotation, translate);
			if (auto headNode = mHeadNode.lock())
			{
				headNode->setOrientation(rotation);
				headNode->setPosition(translate);
			}
		}
		//TODO is it needed? or it is enough to uopdate the projection for the cameras only once?
		vr::HmdMatrix44_t projectionLeft = mpOpenVrSystem->GetProjectionMatrix(vr::Eye_Left, 1, 10000);
		vr::HmdMatrix44_t projectionRight = mpOpenVrSystem->GetProjectionMatrix(vr::Eye_Right, 1, 10000);
		if (auto cameraLeft = mCameraLeft.lock())
			cameraLeft->setProjection(conversionFromOpenVR(projectionLeft));
		if (auto cameraRight = mCameraRight.lock())
			cameraRight->setProjection(conversionFromOpenVR(projectionRight));
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtcVivePlugin::Step()
{

}

void Ape::ApeHtcVivePlugin::Stop()
{

}

void Ape::ApeHtcVivePlugin::Suspend()
{

}

void Ape::ApeHtcVivePlugin::Restart()
{

}
