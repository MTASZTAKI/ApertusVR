#include <iostream>
#include "ApeOculusDK2Plugin.h"

ApeOculusDK2Plugin::ApeOculusDK2Plugin()
{
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeOculusDK2Plugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mpHMD = NULL;
}

ApeOculusDK2Plugin::~ApeOculusDK2Plugin()
{
	std::cout << "ApeOculusDK2Plugin dtor" << std::endl;
}

void ApeOculusDK2Plugin::nodeEventCallBack(const Ape::Event& event)
{
	//std::cout  << "event called on the " << event.subjectName << " node" << std::endl;
}

void ApeOculusDK2Plugin::Init()
{
	std::cout << "ApeOculusDK2Plugin::init" << std::endl;
	ovr_Initialize();
	mpHMD = ovrHmd_Create(0);
	if (!mpHMD)
	{
		mpHMD = ovrHmd_CreateDebug(ovrHmdType::ovrHmd_DK2);
		std::cout << "ApeOculusDK2Plugin: Virtual OVR device created." << std::endl;
	}
	std::cout << "ApeOculusDK2Plugin: OVR device found." << std::endl;
	std::cout << "ApeOculusDK2Plugin: Product Name: " << mpHMD->ProductName << std::endl;
	std::cout << "ApeOculusDK2Plugin: Product ID: " << mpHMD->ProductId << std::endl;
	std::cout << "ApeOculusDK2Plugin: Firmware: " << mpHMD->FirmwareMajor << "." << mpHMD->FirmwareMinor << std::endl;
	std::cout << "ApeOculusDK2Plugin: Display Resolution: " << mpHMD->Resolution.w << "x" << mpHMD->Resolution.h << std::endl;
	std::cout << "ApeOculusDK2Plugin: Monitor Index: " << mpHMD->DisplayId << std::endl;
	if (!ovrHmd_ConfigureTracking(mpHMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0))
	{
		ovrHmd_Destroy(mpHMD);
		std::cout << "ApeOculusDK2Plugin: This OVR device does not support all of the necessary features " << std::endl;
		std::terminate();
	}

	OVR::Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize(mpHMD, ovrEye_Left, mpHMD->DefaultEyeFov[0], 1.0f);
	OVR::Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize(mpHMD, ovrEye_Right, mpHMD->DefaultEyeFov[1], 1.0f);
	std::cout << "ApeOculusDK2Plugin: Rendering resolution:" << std::endl;
	std::cout << "ApeOculusDK2Plugin: Left eye: " << recommendedTex0Size.w << "x" << recommendedTex0Size.h << std::endl;
	std::cout << "ApeOculusDK2Plugin: Right eye: " << recommendedTex1Size.w << "x" << recommendedTex1Size.h << std::endl;

	Ape::ManualMaterialWeakPtr manualMaterialRightEye, manualMaterialLeftEye;
	Ape::ManualPassWeakPtr manualPassRightEye, manualPassLeftEye;
	Ape::ManualTextureWeakPtr manualTextureRightEye, manualTextureLeftEye;
	if (auto manualMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("Oculus/LeftEye_CG", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		manualMaterialLeftEye = manualMaterial;
		if (auto manualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("RiftRenderPassLeft", Ape::Entity::PASS_MANUAL).lock()))
		{
			manualPassLeftEye = manualPass;
			if (auto manualTexture = std::static_pointer_cast<Ape::IManualTexture>(mpScene->createEntity("RiftRenderTextureLeft", Ape::Entity::TEXTURE_MANUAL).lock()))
			{
				manualTexture->setParameters(recommendedTex0Size.w, recommendedTex0Size.h);
				manualPass->setTexture(manualTexture);
				manualTextureLeftEye = manualTexture;
			}
			manualMaterial->setPass(manualPass);
		}
	}
	if (auto manualMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("Oculus/RightEye_CG", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		manualMaterialRightEye = manualMaterial;
		if (auto manualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("RiftRenderPassRight", Ape::Entity::PASS_MANUAL).lock()))
		{
			manualPassRightEye = manualPass;
			if (auto manualTexture = std::static_pointer_cast<Ape::IManualTexture>(mpScene->createEntity("RiftRenderTextureRight", Ape::Entity::TEXTURE_MANUAL).lock()))
			{
				manualTexture->setParameters(recommendedTex1Size.w, recommendedTex1Size.h);
				manualPass->setTexture(manualTexture);
				manualTextureRightEye = manualTexture;
			}
			manualMaterial->setPass(manualPass);
		}
	}

	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovrHmd_GetRenderDesc(mpHMD, ovrEye_Left, mpHMD->DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovrHmd_GetRenderDesc(mpHMD, ovrEye_Right, mpHMD->DefaultEyeFov[1]);
	std::cout << "ApeOculusDK2Plugin: FOV " << eyeRenderDesc[0].Fov.DownTan << std::endl;
	std::cout << "ApeOculusDK2Plugin: eye " << eyeRenderDesc[0].Eye << std::endl;
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
	if (auto oculusDK2MeshNode = mpScene->createNode("oculusDK2MeshNode").lock())
	{
		oculusDK2MeshNode->setPosition(Ape::Vector3(0, 0, -1));
		oculusDK2MeshNode->setScale(Ape::Vector3(1, 1, -1));
		meshNode = oculusDK2MeshNode;
	}
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		ovrDistortionMesh meshData;
		ovrHmd_CreateDistortionMesh(mpHMD, eyeRenderDesc[eyeNum].Eye, eyeRenderDesc[eyeNum].Fov, 0, &meshData);
		Ape::PassGpuParameters params;
		params.push_back(Ape::PassGpuVector3Parameter("eyeToSourceUVScale", Ape::Vector3(UVScaleOffset[0].x, UVScaleOffset[0].y, 0)));
		params.push_back(Ape::PassGpuVector3Parameter("eyeToSourceUVOffset", Ape::Vector3(UVScaleOffset[1].x, UVScaleOffset[1].y, 0)));
		if (eyeNum == 0)
		{
			ovrHmd_GetRenderScaleAndOffset(eyeRenderDesc[eyeNum].Fov, recommendedTex0Size, viewports[eyeNum], UVScaleOffset);
			manualPassLeftEye.lock()->setPassGpuParameters(params);
		}
		else 
		{
			ovrHmd_GetRenderScaleAndOffset(eyeRenderDesc[eyeNum].Fov, recommendedTex1Size, viewports[eyeNum], UVScaleOffset);
			manualPassRightEye.lock()->setPassGpuParameters(params);
		}
		std::cout << "ApeOculusDK2Plugin: UVScaleOffset[0]: " << UVScaleOffset[0].x << ", " << UVScaleOffset[0].y << std::endl;
		std::cout << "ApeOculusDK2Plugin: UVScaleOffset[1]: " << UVScaleOffset[1].x << ", " << UVScaleOffset[1].y << std::endl;
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
			coordinates.push_back(-1);
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
		}
		if (eyeNum == 0)
		{
			if (auto manual = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpScene->createEntity("RiftRenderObjectLeft", Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				manual->setParameters("", coordinates, indices, Ape::GeometryNormals(), colors, textureCoordinates, manualMaterialLeftEye);
				manual->setParentNode(meshNode);
			}
		}
		else
		{
			if (auto manual = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpScene->createEntity("RiftRenderObjectRight", Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				manual->setParameters("", coordinates, indices, Ape::GeometryNormals(), colors, textureCoordinates, manualMaterialRightEye);
				manual->setParentNode(meshNode);
			}
		}
		ovrHmd_DestroyDistortionMesh(&meshData);
	}

	/*mpCameraExternal = mpSceneMgr->createCamera("OculusRiftExternalCamera");
	mpCameraExternal->setFarClipDistance(50);
	mpCameraExternal->setNearClipDistance(0.001);
	mpCameraExternal->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	mpCameraExternal->setOrthoWindow(2, 2);
	bool rotateView = false;
	if (rotateView)
	{
		mpCameraExternal->roll(Ogre::Degree(-90));
	}
	mpSceneMgr->getRootSceneNode()->attachObject(mpCameraExternal);*/

	/*mpCameraLeft = mpSceneMgr->createCamera("HmdLeftCamera");
	mpCameraRight = mpSceneMgr->createCamera("HmdRightCamera");
	mpBodyNode = mpSceneMgr->getRootSceneNode()->createChildSceneNode("BodyNode");
	mpHeadNode = mpBodyNode->createChildSceneNode("HeadNode");
	mpBodyNode->setFixedYawAxis(true);	
	mpHeadNode->attachObject(mpCameraLeft);
	mpHeadNode->attachObject(mpCameraRight);
	mLeftEyeRenderTexture->getBuffer()->getRenderTarget()->addViewport(mpCameraLeft);
	setRenderTextureProperties(mLeftEyeRenderTexture);
	mRightEyeRenderTexture->getBuffer()->getRenderTarget()->addViewport(mpCameraRight);
	setRenderTextureProperties(mRightEyeRenderTexture);
	ovrFovPort fovLeft = hmd->DefaultEyeFov[ovrEye_Left];
	ovrFovPort fovRight = hmd->DefaultEyeFov[ovrEye_Right];
	setCameraProjectionMatrix(mpCameraLeft, fovLeft);
	setCameraProjectionMatrix(mpCameraRight, fovRight);
	float combinedTanHalfFovHorizontal = std::max(fovLeft.LeftTan, fovLeft.RightTan);
	float combinedTanHalfFovVertical = std::max(fovLeft.UpTan, fovLeft.DownTan);
	float aspectRatio = combinedTanHalfFovHorizontal / combinedTanHalfFovVertical;
	mpCameraLeft->setAspectRatio(aspectRatio);
	mpCameraRight->setAspectRatio(aspectRatio);
	mIPD = ovrHmd_GetFloat(hmd, OVR_KEY_IPD, 0.064f) * 100;
	mpCameraLeft->setPosition(-mIPD / 2.0f, 0.0f, 0.0f);
	mpCameraRight->setPosition(mIPD / 2.0f, 0.0f, 0.0f);*/

	/*Ogre::NameValuePairList miscParams;
	if (mVirtualDevice)
	{
		miscParams["monitorIndex"] = "0";
		mpWindowHmd = Ogre::Root::getSingleton().createRenderWindow("Oculus Rift Debug", hmd->Resolution.w / 2, hmd->Resolution.h / 2, false, &miscParams);
	}*/

	//mpViewport = mpRenderWindow->addViewport(mpCameraExternal);
}

void ApeOculusDK2Plugin::Run()
{
	while (mpHMD)
	{
		ovrTrackingState ts = ovrHmd_GetTrackingState(mpHMD, 0);
		OVR::Posef pose = ts.HeadPose.ThePose;
		/*mpHeadNode->setOrientation(Ogre::Quaternion(pose.Rotation.w, pose.Rotation.x, pose.Rotation.y, pose.Rotation.z));
		mpHeadNode->setPosition(100 * Ogre::Vector3(pose.Translation.x, pose.Translation.y, pose.Translation.z));*/

		ovrFovPort fovLeft = mpHMD->DefaultEyeFov[ovrEye_Left];
		ovrFovPort fovRight = mpHMD->DefaultEyeFov[ovrEye_Right];
		/*setCameraProjectionMatrix(mpCameraLeft, fovLeft, *mpDisplaySystemNearClip, *mpDisplaySystemFarClip);
		setCameraProjectionMatrix(mpCameraRight, fovRight, *mpDisplaySystemNearClip, *mpDisplaySystemFarClip);*/

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeOculusDK2Plugin::nodeEventCallBack, this, std::placeholders::_1));
}

void ApeOculusDK2Plugin::Step()
{

}

void ApeOculusDK2Plugin::Stop()
{

}

void ApeOculusDK2Plugin::Suspend()
{

}

void ApeOculusDK2Plugin::Restart()
{

}
