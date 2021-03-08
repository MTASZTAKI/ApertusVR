#include "apeViveSRPlugin.h"

ape::ViveSRPlugin::ViveSRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&ViveSRPlugin::EventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::ViveSRPlugin::~ViveSRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&ViveSRPlugin::EventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ViveSRPlugin::CreateSphere(std::string cameraName, std::string sphereNodeName, std::string meshName, unsigned int visibility)
{
	if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(cameraName).lock()))
	{
		if (auto sphereNode = mpSceneManager->createNode(sphereNodeName, true, mpCoreConfig->getNetworkGUID()).lock())
		{
			sphereNode->setParentNode(mpApeUserInputMacro->getHeadNode());
			if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(meshName, ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				sphereMeshFile->setFileName(meshName);
				sphereMeshFile->setParentNode(sphereNode);
				sphereMeshFile->setVisibilityFlag(visibility);
				camera->setVisibilityMask(visibility);
				if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(meshName + "_Material", ape::Entity::MATERIAL_MANUAL, false, "").lock()))
				{
					material->setAmbientColor(ape::Color(1.0f, 1.0f, 1.0f));
					material->setDiffuseColor(ape::Color(1.0f, 1.0f, 1.0f));
					material->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
					if (auto texture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity(meshName + "_Texture", ape::Entity::TEXTURE_MANUAL, false, "").lock()))
					{
						//TODO somehow wait the init for mDistortedWidth & mDistortedHeight & PixelFormat
						texture->setParameters(640, 480, ape::Texture::PixelFormat::A8R8G8B8, ape::Texture::Usage::DYNAMIC_WRITE_ONLY, false, false, false);
						material->setTexture(texture);
						material->setSceneBlending(ape::Material::SceneBlendingType::TRANSPARENT_ALPHA);
						sphereMeshFile->setMaterial(material);
						material->setCullingMode(ape::Material::CullingMode::CLOCKWISE);
						if (sphereNodeName == "sphereNodeLeft")
						{
							mApeManualTextureLeft = texture;
						}
						else if (sphereNodeName == "sphereNodeRight")
						{
							mApeManualTextureRight = texture;
						}
					}
				}
			}
		}
	}
}

void ape::ViveSRPlugin::EventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::CAMERA_CREATE)
	{
		if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			std::string cameraName = camera->getName();
			std::size_t found = cameraName.find("Left");
			if (found != std::string::npos)
			{
				CreateSphere(cameraName, "sphereNodeLeft", "sphere_left.mesh", 1);
			}
			found = cameraName.find("Right");
			if (found != std::string::npos)
			{
				CreateSphere(cameraName, "sphereNodeRight", "sphere_right.mesh", 2);
			}
		}
	}
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
			ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_DISTORTED_WIDTH, &mDistortedWidth);
			ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_DISTORTED_HEIGHT, &mDistortedHeight);
			ViveSR_GetParameterInt(mViveSrPassThroughID, ViveSR::PassThrough::OUTPUT_DISTORTED_CHANNEL, &mDistortedChannel);
			APE_LOG_DEBUG("ViveSR passthrough size: " << mDistortedWidth << "x" << mDistortedHeight << "x" << mDistortedChannel);
			mDistortedFrameLeft = new unsigned char[mDistortedWidth * mDistortedHeight * mDistortedChannel];
			mDistortedFrameRight = new unsigned char[mDistortedWidth * mDistortedHeight * mDistortedChannel];
			mDistortedFrameLeftData = new ViveSR::MemoryElement();
			mDistortedFrameLeftData->mask = 0;
			mDistortedFrameLeftData->ptr = mDistortedFrameLeft;
			mDistortedFrameRightData = new ViveSR::MemoryElement();
			mDistortedFrameRightData->mask = 0;
			mDistortedFrameRightData->ptr = mDistortedFrameRight;
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
		while (true)
		{
			int res = ViveSR::FAILED;
			res = ViveSR_GetModuleData(mViveSrPassThroughID, mDistortedFrameLeftData, 1);
			if (res == ViveSR::WORK)
			{
				//APE_LOG_DEBUG("mDistortedFrameLeftData ok");
				if (auto apeManualTextureLeft = mApeManualTextureLeft.lock())
				{
					apeManualTextureLeft->setBuffer(mDistortedFrameLeft);
				}
			}
			res = ViveSR_GetModuleData(mViveSrPassThroughID, mDistortedFrameRightData, 1);
			if (res == ViveSR::WORK)
			{
				//APE_LOG_DEBUG("mDistortedFrameRightData ok");
				if (auto apeManualTextureRight = mApeManualTextureRight.lock())
				{
					apeManualTextureRight->setBuffer(mDistortedFrameRight);
				}
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
