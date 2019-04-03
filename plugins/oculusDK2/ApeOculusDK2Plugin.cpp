#include "apeOculusDK2Plugin.h"

ape::apeOculusDK2Plugin::apeOculusDK2Plugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeOculusDK2Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeOculusDK2Plugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpHMD = NULL;
	mCameraLeft = ape::CameraWeakPtr();
	mCameraRight = ape::CameraWeakPtr();
	mpapeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	APE_LOG_FUNC_LEAVE();
}

ape::apeOculusDK2Plugin::~apeOculusDK2Plugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeOculusDK2Plugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

ape::Matrix4 ape::apeOculusDK2Plugin::conversionFromOVR(ovrMatrix4f ovrMatrix4)
{
	ape::Matrix4 matrix4(
		ovrMatrix4.M[0][0], ovrMatrix4.M[0][1], ovrMatrix4.M[0][2], ovrMatrix4.M[0][3],
		ovrMatrix4.M[1][0], ovrMatrix4.M[1][1], ovrMatrix4.M[1][2], ovrMatrix4.M[1][3],
		ovrMatrix4.M[2][0], ovrMatrix4.M[2][1], ovrMatrix4.M[2][2], ovrMatrix4.M[2][3],
		ovrMatrix4.M[3][0], ovrMatrix4.M[3][1], ovrMatrix4.M[3][2], ovrMatrix4.M[3][3]);
	return matrix4;
}


void ape::apeOculusDK2Plugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeOculusDK2Plugin::Init()
{
	APE_LOG_FUNC_ENTER();
	ovr_Initialize();
	mpHMD = ovrHmd_Create(0);
	if (!mpHMD)
	{
		mpHMD = ovrHmd_CreateDebug(ovrHmdType::ovrHmd_DK2);
		APE_LOG_DEBUG("Virtual OVR device created.");
	}
	APE_LOG_DEBUG("OVR device found.");
	APE_LOG_DEBUG("Product Name: " << mpHMD->ProductName);
	APE_LOG_DEBUG("Product ID: " << mpHMD->ProductId);
	APE_LOG_DEBUG("Firmware: " << mpHMD->FirmwareMajor << "." << mpHMD->FirmwareMinor);
	APE_LOG_DEBUG("Display Resolution: " << mpHMD->Resolution.w << "x" << mpHMD->Resolution.h);
	APE_LOG_DEBUG("Monitor Index: " << mpHMD->DisplayId);
	if (!ovrHmd_ConfigureTracking(mpHMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0))
	{
		ovrHmd_Destroy(mpHMD);
		APE_LOG_ERROR("This OVR device does not support all of the necessary features");
		std::terminate();
	}

	OVR::Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize(mpHMD, ovrEye_Left, mpHMD->DefaultEyeFov[0], 1.0f);
	OVR::Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize(mpHMD, ovrEye_Right, mpHMD->DefaultEyeFov[1], 1.0f);
	APE_LOG_DEBUG("Rendering resolution:");
	APE_LOG_DEBUG("Left eye: " << recommendedTex0Size.w << "x" << recommendedTex0Size.h);
	APE_LOG_DEBUG("Right eye: " << recommendedTex1Size.w << "x" << recommendedTex1Size.h);

	ape::FileMaterialWeakPtr fileMaterialRightEye, fileMaterialLeftEye;
	ape::ManualTextureWeakPtr manualTextureRightEye, manualTextureLeftEye;
	if (auto fileMaterial = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->createEntity("Oculus/LeftEye_CG", ape::Entity::MATERIAL_FILE).lock()))
	{
		fileMaterialLeftEye = fileMaterial;
		if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity("RiftRenderTextureLeft", ape::Entity::TEXTURE_MANUAL).lock()))
		{
			manualTexture->setParameters(recommendedTex0Size.w, recommendedTex0Size.h, ape::Texture::PixelFormat::R8G8B8, ape::Texture::Usage::RENDERTARGET);
			fileMaterial->setPassTexture(manualTexture);
			manualTextureLeftEye = manualTexture;
		}
	}
	if (auto fileMaterial = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->createEntity("Oculus/RightEye_CG", ape::Entity::MATERIAL_FILE).lock()))
	{
		fileMaterialRightEye = fileMaterial;
		if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity("RiftRenderTextureRight", ape::Entity::TEXTURE_MANUAL).lock()))
		{
			manualTexture->setParameters(recommendedTex1Size.w, recommendedTex1Size.h, ape::Texture::PixelFormat::R8G8B8, ape::Texture::Usage::RENDERTARGET);
			fileMaterial->setPassTexture(manualTexture);
			manualTextureRightEye = manualTexture;
		}
	}

	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovrHmd_GetRenderDesc(mpHMD, ovrEye_Left, mpHMD->DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovrHmd_GetRenderDesc(mpHMD, ovrEye_Right, mpHMD->DefaultEyeFov[1]);
	APE_LOG_DEBUG("FOV " << eyeRenderDesc[0].Fov.DownTan);
	APE_LOG_DEBUG("eye " << eyeRenderDesc[0].Eye);
	ovrVector2f UVScaleOffset[2];
	ovrRecti viewports[2];
	viewports[0].Pos.x = 0;
	viewports[0].Pos.y = 0;
	viewports[0].Size.w = recommendedTex0Size.w;
	viewports[0].Size.h = recommendedTex0Size.h;
	viewports[1].Pos.x = 0;
	viewports[1].Pos.y = 0;
	viewports[1].Size.w = recommendedTex1Size.w;
	viewports[1].Size.h = recommendedTex1Size.h;
	ape::NodeWeakPtr meshNode;
	if (auto oculusDK2MeshNode = mpSceneManager->createNode("oculusDK2MeshNode").lock())
	{
		oculusDK2MeshNode->setPosition(ape::Vector3(0, 0, -1));
		oculusDK2MeshNode->setScale(ape::Vector3(1, 1, -1));
		meshNode = oculusDK2MeshNode;
	}
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		ovrDistortionMesh meshData;
		ovrHmd_CreateDistortionMesh(mpHMD, eyeRenderDesc[eyeNum].Eye, eyeRenderDesc[eyeNum].Fov, 0, &meshData);
		if (eyeNum == 0)
		{
			ovrHmd_GetRenderScaleAndOffset(eyeRenderDesc[eyeNum].Fov, recommendedTex0Size, viewports[eyeNum], UVScaleOffset);
			ape::PassGpuParameters params;
			params.push_back(ape::PassGpuVector3Parameter("eyeToSourceUVScale", ape::Vector3(UVScaleOffset[0].x, UVScaleOffset[0].y, 0)));
			params.push_back(ape::PassGpuVector3Parameter("eyeToSourceUVOffset", ape::Vector3(UVScaleOffset[1].x, UVScaleOffset[1].y, 0)));
			if (auto fileMaterial = fileMaterialLeftEye.lock())
				fileMaterial->setPassGpuParameters(params);
		}
		else
		{
			ovrHmd_GetRenderScaleAndOffset(eyeRenderDesc[eyeNum].Fov, recommendedTex1Size, viewports[eyeNum], UVScaleOffset);
			ape::PassGpuParameters params;
			params.push_back(ape::PassGpuVector3Parameter("eyeToSourceUVScale", ape::Vector3(UVScaleOffset[0].x, UVScaleOffset[0].y, 0)));
			params.push_back(ape::PassGpuVector3Parameter("eyeToSourceUVOffset", ape::Vector3(UVScaleOffset[1].x, UVScaleOffset[1].y, 0)));
			if (auto fileMaterial = fileMaterialRightEye.lock())
				fileMaterial->setPassGpuParameters(params);
		}
		APE_LOG_DEBUG("UVScaleOffset[0]: " << UVScaleOffset[0].x << ", " << UVScaleOffset[0].y);
		APE_LOG_DEBUG("UVScaleOffset[1]: " << UVScaleOffset[1].x << ", " << UVScaleOffset[1].y);
		ape::GeometryCoordinates coordinates;
		ape::GeometryIndices indices;
		ape::GeometryColors colors;
		ape::GeometryTextureCoordinates textureCoordinates;
		for (unsigned int i = 0; i < meshData.VertexCount; i++)
		{
			ovrDistortionVertex v = meshData.pVertexData[i];
			coordinates.push_back(v.ScreenPosNDC.x);
			coordinates.push_back(v.ScreenPosNDC.y);
			coordinates.push_back(0);
			textureCoordinates.push_back(v.TanEyeAnglesR.x);
			textureCoordinates.push_back(v.TanEyeAnglesR.y);
			textureCoordinates.push_back(v.TanEyeAnglesG.x);
			textureCoordinates.push_back(v.TanEyeAnglesG.y);
			textureCoordinates.push_back(v.TanEyeAnglesB.x);
			textureCoordinates.push_back(v.TanEyeAnglesB.y);
			float vig = std::max(v.VignetteFactor, (float)0.0);
			colors.push_back(vig);
			colors.push_back(vig);
			colors.push_back(vig);
			colors.push_back(vig);
		}
		for (unsigned int i = 0; i < meshData.IndexCount; i++)
		{
			indices.push_back(meshData.pIndexData[i]);
			if ((i + 1) % 3 == 0)
				indices.push_back(-1);
		}
		if (eyeNum == 0)
		{
			if (auto manual = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity("RiftRenderObjectLeft", ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				manual->setParameters("", coordinates, indices, ape::GeometryNormals(), false, colors, textureCoordinates, fileMaterialLeftEye);
				manual->setParentNode(meshNode);
			}
		}
		else
		{
			if (auto manual = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity("RiftRenderObjectRight", ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				manual->setParameters("", coordinates, indices, ape::GeometryNormals(), false, colors, textureCoordinates, fileMaterialRightEye);
				manual->setParentNode(meshNode);
			}
		}
		ovrHmd_DestroyDistortionMesh(&meshData);
	}
	mCameraLeft = mpapeUserInputMacro->createCamera("HmdLeftCamera");
	mCameraRight = mpapeUserInputMacro->createCamera("HmdRightCamera");
	ovrFovPort fovLeft = mpHMD->DefaultEyeFov[ovrEye_Left];
	ovrFovPort fovRight = mpHMD->DefaultEyeFov[ovrEye_Right];
	float combinedTanHalfFovHorizontal = std::max(fovLeft.LeftTan, fovLeft.RightTan);
	float combinedTanHalfFovVertical = std::max(fovLeft.UpTan, fovLeft.DownTan);
	float aspectRatio = combinedTanHalfFovHorizontal / combinedTanHalfFovVertical;
	float ipd = ovrHmd_GetFloat(mpHMD, OVR_KEY_IPD, 0.064f) * 100;
	if (auto cameraLeft = mCameraLeft.lock())
	{
		cameraLeft->setProjection(conversionFromOVR(ovrMatrix4f_Projection(fovLeft, 1, 10000, true)));
		cameraLeft->setAspectRatio(aspectRatio);
		if (auto cameraNode = cameraLeft->getParentNode().lock())
		{
			cameraNode->setPosition(ape::Vector3(-ipd / 2.0f, 0.0f, 0.0f));
		}

		if (auto texture = manualTextureLeftEye.lock())
			texture->setSourceCamera(cameraLeft);
	}
	if (auto cameraRight = mCameraRight.lock())
	{
		cameraRight->setProjection(conversionFromOVR(ovrMatrix4f_Projection(fovRight, 1, 10000, true)));
		cameraRight->setAspectRatio(aspectRatio);
		if (auto cameraNode = cameraRight->getParentNode().lock())
		{
			cameraNode->setPosition(ape::Vector3(ipd / 2.0f, 0.0f, 0.0f));
		}

		if (auto texture = manualTextureRightEye.lock())
			texture->setSourceCamera(cameraRight);
	}

	if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->createEntity("OculusRiftExternalCamera", ape::Entity::Type::CAMERA).lock()))
	{
		camera->setWindow(mpCoreConfig->getWindowConfig().name);
		camera->setFarClipDistance(50);
		camera->setNearClipDistance(0.001);
		camera->setProjectionType(ape::Camera::ORTHOGRAPHIC);
		camera->setOrthoWindowSize(2, 2);
	}
}

void ape::apeOculusDK2Plugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (mpHMD)
	{
		ovrTrackingState ts = ovrHmd_GetTrackingState(mpHMD, 0);
		OVR::Posef pose = ts.HeadPose.ThePose;
		ovrFovPort fovLeft = mpHMD->DefaultEyeFov[ovrEye_Left];
		ovrFovPort fovRight = mpHMD->DefaultEyeFov[ovrEye_Right];
		mUserInputMacroPose.headPosition = ape::Vector3(pose.Translation.x * 100, pose.Translation.y * 100, pose.Translation.z * 100);
		mUserInputMacroPose.headOrientation = ape::Quaternion(pose.Rotation.w, pose.Rotation.x, pose.Rotation.y, pose.Rotation.z);
		mpapeUserInputMacro->updateViewPose(mUserInputMacroPose);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeOculusDK2Plugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeOculusDK2Plugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeOculusDK2Plugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeOculusDK2Plugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
