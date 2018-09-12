#include "ApeHtcVivePlugin.h"

Ape::ApeHtcVivePlugin::ApeHtcVivePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_MANUAL, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mCameraLeft = Ape::CameraWeakPtr();
	mCameraRight = Ape::CameraWeakPtr();
	mHeadNode = Ape::NodeWeakPtr();
	mUserMaterial = Ape::ManualMaterialWeakPtr();
	mOpenVrRttTextureIDs[0] = nullptr;
	mOpenVrRttTextureIDs[1] = nullptr;
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeHtcVivePlugin::~ApeHtcVivePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
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

void Ape::ApeHtcVivePlugin::submitTextureLeftToOpenVR()
{
	vr::EVRCompositorError error;
	error = vr::VRCompositor()->Submit(vr::Eye_Left, &mOpenVrTextures[0], &mOpenVrTextureBounds[0]);
	error = vr::VRCompositor()->Submit(vr::Eye_Right, &mOpenVrTextures[1], &mOpenVrTextureBounds[1]);
	error = vr::VRCompositor()->WaitGetPoses(mOpenVrTrackedPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
	if (!error)
	{
		vr::TrackedDevicePose_t hmdPose;
		if ((hmdPose = mOpenVrTrackedPoses[vr::k_unTrackedDeviceIndex_Hmd]).bPoseIsValid)
		{
			auto openVRPose = hmdPose.mDeviceToAbsoluteTracking;
			Ape::Vector3 apeScale;
			Ape::Quaternion apeRotation;
			Ape::Vector3 apeTranslate;
			Ape::Matrix4 apePose = conversionFromOpenVR(openVRPose);
			apePose.decomposition(apeScale, apeRotation, apeTranslate);
			apeScale = apeScale * 100;
			apeTranslate = apeTranslate * 100;
			if (auto headNode = mHeadNode.lock())
			{
				headNode->setOrientation(apeRotation);
				headNode->setPosition(apeTranslate);
				//APE_LOG_DEBUG("rotation:" << rotation.toString() << " translate:" << translate.toString());
			}
		}
	}
	else
	{
		APE_LOG_DEBUG("Error WaitGetPoses:" << error);
	}
	//APE_LOG_DEBUG("Error Submit Left:" << error);
}

void Ape::ApeHtcVivePlugin::submitTextureRightToOpenVR()
{
	vr::EVRCompositorError error;
	//error = vr::VRCompositor()->Submit(vr::Eye_Right, &mOpenVrTextures[1], &mOpenVrTextureBounds[1]);
	//APE_LOG_DEBUG("Error Submit Right:" << error);
}

Ape::CameraWeakPtr Ape::ApeHtcVivePlugin::createCamera(std::string name)
{
	if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpSceneManager->createEntity(name, Ape::Entity::Type::CAMERA).lock()))
	{
		if (auto cameraNode = mpSceneManager->createNode(name + "_Node").lock())
		{
			cameraNode->setParentNode(mHeadNode);
			if (auto cameraConeNode = mpSceneManager->createNode(name + "_ConeNode").lock())
			{
				cameraConeNode->setParentNode(cameraNode);
				cameraConeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
				if (auto cameraCone = std::static_pointer_cast<Ape::IConeGeometry>(mpSceneManager->createEntity(name + "_ConeGeometry", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					cameraCone->setParameters(10.0f, 30.0f, 1.0f, Ape::Vector2(1, 1));
					cameraCone->setParentNode(cameraConeNode);
					cameraCone->setMaterial(mUserMaterial);
				}
			}
			if (auto userNameTextNode = mpSceneManager->createNode(name + "_TextNode").lock())
			{
				userNameTextNode->setParentNode(cameraNode);
				userNameTextNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
				if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity(name + "_TextGeometry", Ape::Entity::GEOMETRY_TEXT).lock()))
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
	if (event.type == Ape::Event::Type::TEXTURE_MANUAL_GRAPHICSAPIID)
	{
		if (auto textureManual = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			if (event.subjectName == "OpenVrRenderTextureLeft")
			{
				mOpenVrRttTextureIDs[0] = textureManual->getGraphicsApiID();
				textureManual->registerFunction(std::bind(&ApeHtcVivePlugin::submitTextureLeftToOpenVR, this));
			}
			else if (event.subjectName == "OpenVrRenderTextureRight")
			{
				mOpenVrRttTextureIDs[1] = textureManual->getGraphicsApiID();
				textureManual->registerFunction(std::bind(&ApeHtcVivePlugin::submitTextureRightToOpenVR, this));
			}
		}
	}
}

void Ape::ApeHtcVivePlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	if (auto userNode = mpSceneManager->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		userNode->setFixedYaw(true);
		mUserNode = userNode;
		if (auto headNode = mpSceneManager->getNode(userNode->getName() + "_HeadNode").lock())
		{
			mHeadNode = headNode;
		}
		if (auto userMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->getEntity(userNode->getName() + "_Material").lock()))
		{
			mUserMaterial = userMaterial;
		}
	}

	APE_LOG_DEBUG("waiting for main window");
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("main window was found");

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

	if (auto manualTexture = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->createEntity("OpenVrRenderTextureLeft", Ape::Entity::TEXTURE_MANUAL).lock()))
	{
		manualTexture->setParameters(width, height, Ape::Texture::PixelFormat::R8G8B8A8, Ape::Texture::Usage::RENDERTARGET);
		mManualTextureLeftEye = manualTexture;
	}
	if (auto manualTexture = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->createEntity("OpenVrRenderTextureRight", Ape::Entity::TEXTURE_MANUAL).lock()))
	{
		manualTexture->setParameters(width, height, Ape::Texture::PixelFormat::R8G8B8A8, Ape::Texture::Usage::RENDERTARGET);
		mManualTextureRightEye = manualTexture;
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
		if (auto texture = mManualTextureLeftEye.lock())
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
		if (auto texture = mManualTextureRightEye.lock())
			texture->setSourceCamera(cameraRight);
		cameraRight->setProjection(conversionFromOpenVR(projectionRight));
	}
}

void Ape::ApeHtcVivePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
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
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtcVivePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtcVivePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtcVivePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtcVivePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
