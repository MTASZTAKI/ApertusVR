#include <iostream>
#include "ApeOculusDK2Plugin.h"

Ape::ApeOculusDK2Plugin::ApeOculusDK2Plugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeOculusDK2Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeOculusDK2Plugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpHMD = NULL;
	mCameraLeft = Ape::CameraWeakPtr();
	mCameraRight = Ape::CameraWeakPtr();
	mHeadNode = Ape::NodeWeakPtr();
	mUserMaterial = Ape::ManualMaterialWeakPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeOculusDK2Plugin::~ApeOculusDK2Plugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

Ape::Matrix4 Ape::ApeOculusDK2Plugin::conversionFromOVR(ovrMatrix4f ovrMatrix4)
{
	Ape::Matrix4 matrix4(
		ovrMatrix4.M[0][0], ovrMatrix4.M[0][1], ovrMatrix4.M[0][2], ovrMatrix4.M[0][3],
		ovrMatrix4.M[1][0], ovrMatrix4.M[1][1], ovrMatrix4.M[1][2], ovrMatrix4.M[1][3],
		ovrMatrix4.M[2][0], ovrMatrix4.M[2][1], ovrMatrix4.M[2][2], ovrMatrix4.M[2][3],
		ovrMatrix4.M[3][0], ovrMatrix4.M[3][1], ovrMatrix4.M[3][2], ovrMatrix4.M[3][3]);
	return matrix4;
}

Ape::CameraWeakPtr Ape::ApeOculusDK2Plugin::createCamera(std::string name)
{
	if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpSceneManager->createEntity(name, Ape::Entity::Type::CAMERA).lock()))
	{
		if (auto cameraNode = mpSceneManager->createNode(name + "_Node").lock())
		{
			cameraNode->setParentNode(mHeadNode);
			/*if (auto cameraConeNode = mpSceneManager->createNode(name + "_ConeNode").lock())
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
			}*/
			camera->setParentNode(cameraNode);
		}
		return camera;
	}
}


void Ape::ApeOculusDK2Plugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeOculusDK2Plugin::Init()
{
	LOG_FUNC_ENTER();

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

	LOG(LOG_TYPE_DEBUG, "waiting for main window");
	while (Ape::IMainWindow::getSingletonPtr()->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LOG(LOG_TYPE_DEBUG, "main window was found");
	ovr_Initialize();
	mpHMD = ovrHmd_Create(0);
	if (!mpHMD)
	{
		mpHMD = ovrHmd_CreateDebug(ovrHmdType::ovrHmd_DK2);
		LOG(LOG_TYPE_DEBUG, "Virtual OVR device created.");
	}
	LOG(LOG_TYPE_DEBUG, "OVR device found.");
	LOG(LOG_TYPE_DEBUG, "Product Name: " << mpHMD->ProductName);
	LOG(LOG_TYPE_DEBUG, "Product ID: " << mpHMD->ProductId);
	LOG(LOG_TYPE_DEBUG, "Firmware: " << mpHMD->FirmwareMajor << "." << mpHMD->FirmwareMinor);
	LOG(LOG_TYPE_DEBUG, "Display Resolution: " << mpHMD->Resolution.w << "x" << mpHMD->Resolution.h);
	LOG(LOG_TYPE_DEBUG, "Monitor Index: " << mpHMD->DisplayId);
	if (!ovrHmd_ConfigureTracking(mpHMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0))
	{
		ovrHmd_Destroy(mpHMD);
		LOG(LOG_TYPE_ERROR, "This OVR device does not support all of the necessary features");
		std::terminate();
	}

	OVR::Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize(mpHMD, ovrEye_Left, mpHMD->DefaultEyeFov[0], 1.0f);
	OVR::Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize(mpHMD, ovrEye_Right, mpHMD->DefaultEyeFov[1], 1.0f);
	LOG(LOG_TYPE_DEBUG, "Rendering resolution:");
	LOG(LOG_TYPE_DEBUG, "Left eye: " << recommendedTex0Size.w << "x" << recommendedTex0Size.h);
	LOG(LOG_TYPE_DEBUG, "Right eye: " << recommendedTex1Size.w << "x" << recommendedTex1Size.h);

	Ape::FileMaterialWeakPtr fileMaterialRightEye, fileMaterialLeftEye;
	Ape::ManualTextureWeakPtr manualTextureRightEye, manualTextureLeftEye;
	if (auto fileMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->createEntity("Oculus/LeftEye_CG", Ape::Entity::MATERIAL_FILE).lock()))
	{
		fileMaterialLeftEye = fileMaterial;
		if (auto manualTexture = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->createEntity("RiftRenderTextureLeft", Ape::Entity::TEXTURE_MANUAL).lock()))
		{
			manualTexture->setParameters(recommendedTex0Size.w, recommendedTex0Size.h, Ape::Texture::PixelFormat::R8G8B8, Ape::Texture::Usage::RENDERTARGET);
			fileMaterial->setPassTexture(manualTexture);
			manualTextureLeftEye = manualTexture;
		}
	}
	if (auto fileMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->createEntity("Oculus/RightEye_CG", Ape::Entity::MATERIAL_FILE).lock()))
	{
		fileMaterialRightEye = fileMaterial;
		if (auto manualTexture = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->createEntity("RiftRenderTextureRight", Ape::Entity::TEXTURE_MANUAL).lock()))
		{
			manualTexture->setParameters(recommendedTex1Size.w, recommendedTex1Size.h, Ape::Texture::PixelFormat::R8G8B8, Ape::Texture::Usage::RENDERTARGET);
			fileMaterial->setPassTexture(manualTexture);
			manualTextureRightEye = manualTexture;
		}
	}

	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovrHmd_GetRenderDesc(mpHMD, ovrEye_Left, mpHMD->DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovrHmd_GetRenderDesc(mpHMD, ovrEye_Right, mpHMD->DefaultEyeFov[1]);
	LOG(LOG_TYPE_DEBUG, "FOV " << eyeRenderDesc[0].Fov.DownTan);
	LOG(LOG_TYPE_DEBUG, "eye " << eyeRenderDesc[0].Eye);
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
	Ape::NodeWeakPtr meshNode;
	if (auto oculusDK2MeshNode = mpSceneManager->createNode("oculusDK2MeshNode").lock())
	{
		oculusDK2MeshNode->setPosition(Ape::Vector3(0, 0, -1));
		oculusDK2MeshNode->setScale(Ape::Vector3(1, 1, -1));
		meshNode = oculusDK2MeshNode;
	}
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		ovrDistortionMesh meshData;
		ovrHmd_CreateDistortionMesh(mpHMD, eyeRenderDesc[eyeNum].Eye, eyeRenderDesc[eyeNum].Fov, 0, &meshData);
		if (eyeNum == 0)
		{
			ovrHmd_GetRenderScaleAndOffset(eyeRenderDesc[eyeNum].Fov, recommendedTex0Size, viewports[eyeNum], UVScaleOffset);
			Ape::PassGpuParameters params;
			params.push_back(Ape::PassGpuVector3Parameter("eyeToSourceUVScale", Ape::Vector3(UVScaleOffset[0].x, UVScaleOffset[0].y, 0)));
			params.push_back(Ape::PassGpuVector3Parameter("eyeToSourceUVOffset", Ape::Vector3(UVScaleOffset[1].x, UVScaleOffset[1].y, 0)));
			if (auto fileMaterial = fileMaterialLeftEye.lock())
				fileMaterial->setPassGpuParameters(params);
		}
		else
		{
			ovrHmd_GetRenderScaleAndOffset(eyeRenderDesc[eyeNum].Fov, recommendedTex1Size, viewports[eyeNum], UVScaleOffset);
			Ape::PassGpuParameters params;
			params.push_back(Ape::PassGpuVector3Parameter("eyeToSourceUVScale", Ape::Vector3(UVScaleOffset[0].x, UVScaleOffset[0].y, 0)));
			params.push_back(Ape::PassGpuVector3Parameter("eyeToSourceUVOffset", Ape::Vector3(UVScaleOffset[1].x, UVScaleOffset[1].y, 0)));
			if (auto fileMaterial = fileMaterialRightEye.lock())
				fileMaterial->setPassGpuParameters(params);
		}
		LOG(LOG_TYPE_DEBUG, "UVScaleOffset[0]: " << UVScaleOffset[0].x << ", " << UVScaleOffset[0].y);
		LOG(LOG_TYPE_DEBUG, "UVScaleOffset[1]: " << UVScaleOffset[1].x << ", " << UVScaleOffset[1].y);
		Ape::GeometryCoordinates coordinates;
		Ape::GeometryIndices indices;
		Ape::GeometryColors colors;
		Ape::GeometryTextureCoordinates textureCoordinates;
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
			if (auto manual = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity("RiftRenderObjectLeft", Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				manual->setParameters("", coordinates, indices, Ape::GeometryNormals(), false, colors, textureCoordinates, fileMaterialLeftEye);
				manual->setParentNode(meshNode);
			}
		}
		else
		{
			if (auto manual = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity("RiftRenderObjectRight", Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				manual->setParameters("", coordinates, indices, Ape::GeometryNormals(), false, colors, textureCoordinates, fileMaterialRightEye);
				manual->setParentNode(meshNode);
			}
		}
		ovrHmd_DestroyDistortionMesh(&meshData);
	}
	if (auto userNode = mUserNode.lock())
	{
		mCameraLeft = createCamera(userNode->getName() + "HmdLeftCamera");
		mCameraRight = createCamera(userNode->getName() + "HmdRightCamera");
	}

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
			cameraNode->setPosition(Ape::Vector3(-ipd / 2.0f, 0.0f, 0.0f));
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
			cameraNode->setPosition(Ape::Vector3(ipd / 2.0f, 0.0f, 0.0f));
		}

		if (auto texture = manualTextureRightEye.lock())
			texture->setSourceCamera(cameraRight);
	}

	if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpSceneManager->createEntity("OculusRiftExternalCamera", Ape::Entity::Type::CAMERA).lock()))
	{
		camera->setWindow(Ape::IMainWindow::getSingletonPtr()->getName());
		camera->setFarClipDistance(50);
		camera->setNearClipDistance(0.001);
		camera->setProjectionType(Ape::Camera::ORTHOGRAPHIC);
		camera->setOrthoWindowSize(2, 2);
	}
}

void Ape::ApeOculusDK2Plugin::Run()
{
	LOG_FUNC_ENTER();
	while (mpHMD)
	{
		ovrTrackingState ts = ovrHmd_GetTrackingState(mpHMD, 0);
		OVR::Posef pose = ts.HeadPose.ThePose;
		ovrFovPort fovLeft = mpHMD->DefaultEyeFov[ovrEye_Left];
		ovrFovPort fovRight = mpHMD->DefaultEyeFov[ovrEye_Right];
		if (auto headNode = mHeadNode.lock())
		{
			headNode->setOrientation(Ape::Quaternion(pose.Rotation.w, pose.Rotation.x, pose.Rotation.y, pose.Rotation.z));
			headNode->setPosition(Ape::Vector3(pose.Translation.x * 100, pose.Translation.y * 100, pose.Translation.z * 100));
		}
		//TODO is it needed? or it is enough to uopdate the projection for the cameras only once?
		/*if (auto cameraLeft = mCameraLeft.lock())
			cameraLeft->setProjection(conversionFromOVR(ovrMatrix4f_Projection(fovLeft, 1, 10000, true)));
		if (auto cameraRight = mCameraRight.lock())
			cameraRight->setProjection(conversionFromOVR(ovrMatrix4f_Projection(fovRight, 1, 10000, true)));*/
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeOculusDK2Plugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeOculusDK2Plugin::Step()
{

}

void Ape::ApeOculusDK2Plugin::Stop()
{

}

void Ape::ApeOculusDK2Plugin::Suspend()
{

}

void Ape::ApeOculusDK2Plugin::Restart()
{

}
