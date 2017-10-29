#include <iostream>
#include "ApeSkyWaterTerrainScenePlugin.h"

ApeSkyWaterTerrainScenePlugin::ApeSkyWaterTerrainScenePlugin()
{
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
	mCamera = Ape::CameraWeakPtr();
}

ApeSkyWaterTerrainScenePlugin::~ApeSkyWaterTerrainScenePlugin()
{
	std::cout << "ApeSkyWaterTerrainScenePlugin dtor" << std::endl;
}

void ApeSkyWaterTerrainScenePlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA && event.subjectName == "RiftRenderTextureRight") //when oculusDK2 camera and its texture is ready to use
	{
		if (auto texture = std::static_pointer_cast<Ape::IManualTexture>(mpScene->getEntity(event.subjectName).lock()))
		{
			if (auto camera = texture->getSourceCamera().lock())
			{
				mCamera = camera;
				createSky();
				createWater();
			}
		}
	}
	else if (event.type == Ape::Event::Type::CAMERA_WINDOW && event.subjectName != "OculusRiftExternalCamera") //when simple camera is ready to use
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
		{
			mCamera = camera;
			createSky();
			createWater();
		}
	}
}

void ApeSkyWaterTerrainScenePlugin::createSky()
{
	if (auto sky = std::static_pointer_cast<Ape::ISky>(mpScene->createEntity("sky", Ape::Entity::SKY).lock()))
	{
		sky->setCamera(mCamera);
		sky->setTime(16.00);
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

void ApeSkyWaterTerrainScenePlugin::createWater()
{
	if (auto water = std::static_pointer_cast<Ape::IWater>(mpScene->createEntity("water", Ape::Entity::WATER).lock()))
	{
		water->setCamera(mCamera);
		if (auto sky = mSky.lock())
			water->setSky(sky);
		mWater = water;
	}
}

void ApeSkyWaterTerrainScenePlugin::Init()
{
	std::cout << "ApeSkyWaterTerrainScenePlugin::init" << std::endl;
	std::cout << "ApeSkyWaterTerrainScenePlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "ApeSkyWaterTerrainScenePlugin main window was found" << std::endl;
}

void ApeSkyWaterTerrainScenePlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeSkyWaterTerrainScenePlugin::eventCallBack, this, std::placeholders::_1));
}

void ApeSkyWaterTerrainScenePlugin::Step()
{

}

void ApeSkyWaterTerrainScenePlugin::Stop()
{

}

void ApeSkyWaterTerrainScenePlugin::Suspend()
{

}

void ApeSkyWaterTerrainScenePlugin::Restart()
{

}
