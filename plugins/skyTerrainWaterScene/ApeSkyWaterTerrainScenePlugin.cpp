#include "ApeSkyWaterTerrainScenePlugin.h"

Ape::ApeSkyWaterTerrainScenePlugin::ApeSkyWaterTerrainScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_MANUAL, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mWater = Ape::WaterWeakPtr();
	mSky = Ape::SkyWeakPtr();
	mSkyLight = Ape::LightWeakPtr();
	mSunLight = Ape::LightWeakPtr();
	mSkyLightNode = Ape::NodeWeakPtr();
	mCameras = std::vector<Ape::CameraWeakPtr>();
	mExpectedCameraCount = 6;
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeSkyWaterTerrainScenePlugin::~ApeSkyWaterTerrainScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::eventCallBack(const Ape::Event& event)
{
	APE_LOG_FUNC_ENTER();
	if (event.type == Ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA) //Oculus
	{
		if (auto texture = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			if (auto camera = texture->getSourceCamera().lock())
				mCameras.push_back(camera);
		}
		if (event.subjectName == "RiftRenderTextureRight")
			mExpectedCameraCount = 2; 
	}
	else if (event.type == Ape::Event::Type::CAMERA_WINDOW && event.subjectName != "OculusRiftExternalCamera")  //Monitor
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
			mCameras.push_back(camera);
		mExpectedCameraCount = 1;
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::createSky()
{
	APE_LOG_FUNC_ENTER();
	if (auto sky = std::static_pointer_cast<Ape::ISky>(mpSceneManager->createEntity("sky", Ape::Entity::SKY).lock()))
	{
		sky->setSize(9500);
		sky->setTime(6.00, 4.00, 22.00);
		if (auto skyLight = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("skylight", Ape::Entity::Type::LIGHT).lock()))
		{
			sky->setSkyLight(skyLight);
			mSkyLight = skyLight;
		}
		if (auto sunLight = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("sunlight", Ape::Entity::Type::LIGHT).lock()))
		{
			sunLight->setLightType(Ape::Light::Type::DIRECTIONAL);
			sky->setSunLight(sunLight);
			mSunLight = sunLight;
		}
		mSky = sky;
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::createWater()
{
	APE_LOG_FUNC_ENTER();
	if (auto water = std::static_pointer_cast<Ape::IWater>(mpSceneManager->createEntity("water", Ape::Entity::WATER).lock()))
	{
		water->setCameras(mCameras);
		if (auto sky = mSky.lock())
			water->setSky(sky);
		mWater = water;
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::Init()
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

void Ape::ApeSkyWaterTerrainScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
