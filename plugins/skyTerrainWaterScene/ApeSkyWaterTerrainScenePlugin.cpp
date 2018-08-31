#include <iostream>
#include "ApeSkyWaterTerrainScenePlugin.h"

Ape::ApeSkyWaterTerrainScenePlugin::ApeSkyWaterTerrainScenePlugin()
{
	LOG_FUNC_ENTER();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_MANUAL, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mWater = Ape::WaterWeakPtr();
	mSky = Ape::SkyWeakPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mSkyLight = Ape::LightWeakPtr();
	mSunLight = Ape::LightWeakPtr();
	mSkyLightNode = Ape::NodeWeakPtr();
	mCameras = std::vector<Ape::CameraWeakPtr>();
	mExpectedCameraCount = 6;
	LOG_FUNC_LEAVE();
}

Ape::ApeSkyWaterTerrainScenePlugin::~ApeSkyWaterTerrainScenePlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA) //Oculus
	{
		if (auto texture = std::static_pointer_cast<Ape::IManualTexture>(mpScene->getEntity(event.subjectName).lock()))
		{
			if (auto camera = texture->getSourceCamera().lock())
				mCameras.push_back(camera);
		}
		if (event.subjectName == "RiftRenderTextureRight")
			mExpectedCameraCount = 2; 
	}
	else if (event.type == Ape::Event::Type::CAMERA_WINDOW && event.subjectName != "OculusRiftExternalCamera")  //Monitor
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
			mCameras.push_back(camera);
		mExpectedCameraCount = 1;
	}
}

void Ape::ApeSkyWaterTerrainScenePlugin::createSky()
{
	if (auto sky = std::static_pointer_cast<Ape::ISky>(mpScene->createEntity("sky", Ape::Entity::SKY).lock()))
	{
		sky->setSize(9500);
		sky->setTime(6.00, 4.00, 22.00);
		if (auto skyLight = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("skylight", Ape::Entity::Type::LIGHT).lock()))
		{
			sky->setSkyLight(skyLight);
			mSkyLight = skyLight;
		}
		if (auto sunLight = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("sunlight", Ape::Entity::Type::LIGHT).lock()))
		{
			sunLight->setLightType(Ape::Light::Type::DIRECTIONAL);
			sky->setSunLight(sunLight);
			mSunLight = sunLight;
		}
		mSky = sky;
	}
}

void Ape::ApeSkyWaterTerrainScenePlugin::createWater()
{
	if (auto water = std::static_pointer_cast<Ape::IWater>(mpScene->createEntity("water", Ape::Entity::WATER).lock()))
	{
		water->setCameras(mCameras);
		if (auto sky = mSky.lock())
			water->setSky(sky);
		mWater = water;
	}
}

void Ape::ApeSkyWaterTerrainScenePlugin::Init()
{
	LOG_FUNC_ENTER();
	LOG(LOG_TYPE_DEBUG, "waiting for main window");
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LOG(LOG_TYPE_DEBUG, "main window was found");
	LOG(LOG_TYPE_DEBUG, "is waiting for the cameras");
	while (mCameras.size() < mExpectedCameraCount)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LOG(LOG_TYPE_DEBUG, "expected camera count ok");
	createSky();
	createWater();
}

void Ape::ApeSkyWaterTerrainScenePlugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeSkyWaterTerrainScenePlugin::Step()
{

}

void Ape::ApeSkyWaterTerrainScenePlugin::Stop()
{

}

void Ape::ApeSkyWaterTerrainScenePlugin::Suspend()
{

}

void Ape::ApeSkyWaterTerrainScenePlugin::Restart()
{

}
