#include "apeIndustry40Plugin.h"

ape::apeIndustry40Plugin::apeIndustry40Plugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apeIndustry40Plugin::~apeIndustry40Plugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeIndustry40Plugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeGround();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Run()
{
	APE_LOG_FUNC_ENTER();
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
			ape::PointCloudPoints points = {
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
			ape::PointCloudColors colors = {
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
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
