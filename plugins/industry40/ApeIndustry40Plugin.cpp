#include <iostream>
#include "ApeIndustry40Plugin.h"

ApeIndustry40Plugin::ApeIndustry40Plugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mInterpolators = std::vector<std::unique_ptr<Ape::Interpolator>>();	
	mPointCloud = Ape::PointCloudWeakPtr();
}

ApeIndustry40Plugin::~ApeIndustry40Plugin()
{
	std::cout << "ApeIndustry40Plugin dtor" << std::endl;
}

void ApeIndustry40Plugin::eventCallBack(const Ape::Event& event)
{
	
}

void ApeIndustry40Plugin::Init()
{
	std::cout << "ApeIndustry40Plugin::init" << std::endl;

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.6f));
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, 1));
		light->setDiffuseColor(Ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.6f));
	}
	if (auto pointCloudNode = mpScene->createNode("pointCloudNode").lock())
	{
		pointCloudNode->setPosition(Ape::Vector3(0, 50, -300));
		if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud", Ape::Entity::POINT_CLOUD).lock()))
		{
			Ape::PointCloudPoints points = {
				-5, 0, 0,
				-4, 0, 0,
				-3, 0, 0,
				-2, 0, 0,
				-1, 0, 0,
				 0, 0, 0,
				 1, 0, 0,
				 2, 0, 0,
				 3, 0, 0
			};
			Ape::PointCloudColors colors = {
				(float)0.9, 0, 0,
				(float)0.9, 0, 0,
				(float)0.9, 0, 0,
				(float)0.9, 0, 0,
				(float)0.9, 0, 0,
				(float)0.9, 0, 0,
				(float)0.9, 0, 0,
				(float)0.9, 0, 0,
				(float)0.9, 0, 0
			};
			pointCloud->setParameters(points, colors, 10.0f);
			pointCloud->setParentNode(pointCloudNode);
			mPointCloud = pointCloud;
		}
	}
}
void ApeIndustry40Plugin::Run()
{
	while (true)
	{
		if (auto pointCloud = mPointCloud.lock())
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distInt(-5, 3);
			std::vector<double> randomPoints;
			for (int i = 0; i < 9; i++)
				randomPoints.push_back(distInt(gen));
			Ape::PointCloudPoints points = {
				(float)randomPoints[0], 0, 0,
				(float)randomPoints[1], 0, 0,
				(float)randomPoints[2], 0, 0,
				(float)randomPoints[3], 0, 0,
				(float)randomPoints[4], 0, 0,
				(float)randomPoints[5], 0, 0,
				(float)randomPoints[6], 0, 0,
				(float)randomPoints[7], 0, 0,
				(float)randomPoints[8], 0, 0
				};
			std::uniform_real_distribution<double> distDouble(0.0, 1.0);
			std::vector<double> randomRedColors;
			for (int i = 0; i < 9; i++)
				randomRedColors.push_back(distDouble(gen));
			Ape::PointCloudColors colors = {
				(float)randomRedColors[0], 0, 0,
				(float)randomRedColors[1], 0, 0,
				(float)randomRedColors[2], 0, 0,
				(float)randomRedColors[3], 0, 0,
				(float)randomRedColors[4], 0, 0,
				(float)randomRedColors[5], 0, 0,
				(float)randomRedColors[6], 0, 0,
				(float)randomRedColors[7], 0, 0,
				(float)randomRedColors[8], 0, 0
				};
			pointCloud->updatePoints(points);
			pointCloud->updateColors(colors);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
}

void ApeIndustry40Plugin::Step()
{

}

void ApeIndustry40Plugin::Stop()
{

}

void ApeIndustry40Plugin::Suspend()
{

}

void ApeIndustry40Plugin::Restart()
{

}
