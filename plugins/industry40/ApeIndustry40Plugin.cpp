#include <iostream>
#include "ApeIndustry40Plugin.h"

Ape::ApeIndustry40Plugin::ApeIndustry40Plugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mInterpolators = std::vector<std::unique_ptr<Ape::Interpolator>>();	
	mPointCloud = Ape::PointCloudWeakPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeIndustry40Plugin::~ApeIndustry40Plugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeIndustry40Plugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeIndustry40Plugin::Init()
{
	LOG_FUNC_ENTER();

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto planeNode = mpScene->createNode("planeNode").lock())
	{
		if (auto planeMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("planeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			planeMaterial->setDiffuseColor(Ape::Color(0.1f, 0.1f, 0.1f));
			planeMaterial->setSpecularColor(Ape::Color(0.3f, 0.3f, 0.2f));
			if (auto plane = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("plane", Ape::Entity::GEOMETRY_PLANE).lock()))
			{
				plane->setParameters(Ape::Vector2(1, 1), Ape::Vector2(1000, 1000), Ape::Vector2(1, 1));
				plane->setParentNode(planeNode);
				plane->setMaterial(planeMaterial);
			}
		}
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
	/*if (auto pointCloudNode = mpScene->createNode("pointCloudNode").lock())
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
	}*/
	LOG_FUNC_LEAVE();
}

void Ape::ApeIndustry40Plugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		/*if (auto pointCloud = mPointCloud.lock())
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
		}*/
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeIndustry40Plugin::Step()
{

}

void Ape::ApeIndustry40Plugin::Stop()
{

}

void Ape::ApeIndustry40Plugin::Suspend()
{

}

void Ape::ApeIndustry40Plugin::Restart()
{

}
