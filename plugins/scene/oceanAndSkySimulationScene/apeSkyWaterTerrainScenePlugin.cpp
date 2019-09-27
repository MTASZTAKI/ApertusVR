#include "apeSkyWaterTerrainScenePlugin.h"

ape::apeSkyWaterTerrainScenePlugin::apeSkyWaterTerrainScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&apeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mWater = ape::WaterWeakPtr();
	mSky = ape::SkyWeakPtr();
	mSkyLight = ape::LightWeakPtr();
	mSunLight = ape::LightWeakPtr();
	mSkyLightNode = ape::NodeWeakPtr();
	mCameras = std::vector<ape::CameraWeakPtr>();
	mExpectedCameraCount = 6;
	APE_LOG_FUNC_LEAVE();
}

ape::apeSkyWaterTerrainScenePlugin::~apeSkyWaterTerrainScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::eventCallBack(const ape::Event& event)
{
	APE_LOG_FUNC_ENTER();
	if (event.type == ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA) //Oculus
	{
		if (auto texture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			if (auto camera = texture->getSourceCamera().lock())
				mCameras.push_back(camera);
		}
		if (event.subjectName == "RiftRenderTextureRight")
			mExpectedCameraCount = 2; 
	}
	else if (event.type == ape::Event::Type::CAMERA_WINDOW && event.subjectName != "OculusRiftExternalCamera")  //Monitor
	{
		if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
			mCameras.push_back(camera);
		mExpectedCameraCount = 1;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::createSky()
{
	APE_LOG_FUNC_ENTER();
	if (auto sky = std::static_pointer_cast<ape::ISky>(mpSceneManager->createEntity("sky", ape::Entity::SKY).lock()))
	{
		sky->setSize(9500);
		sky->setTime(6.00, 4.00, 22.00);
		if (auto skyLight = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("skylight", ape::Entity::Type::LIGHT).lock()))
		{
			sky->setSkyLight(skyLight);
			mSkyLight = skyLight;
		}
		if (auto sunLight = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("sunlight", ape::Entity::Type::LIGHT).lock()))
		{
			sunLight->setLightType(ape::Light::Type::DIRECTIONAL);
			sky->setSunLight(sunLight);
			mSunLight = sunLight;
		}
		mSky = sky;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::createWater()
{
	APE_LOG_FUNC_ENTER();
	if (auto water = std::static_pointer_cast<ape::IWater>(mpSceneManager->createEntity("water", ape::Entity::WATER).lock()))
	{
		water->setCameras(mCameras);
		if (auto sky = mSky.lock())
			water->setSky(sky);
		mWater = water;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("is waiting for the cameras");
	while (mCameras.size() < mExpectedCameraCount)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("expected camera count ok");
	createSky();
	createWater();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSkyWaterTerrainScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
