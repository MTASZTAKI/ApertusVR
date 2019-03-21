#include "ApeArtScenePlugin.h"

Ape::ApeArtScenePlugin::ApeArtScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeArtScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new Ape::SceneMakerMacro();
	mModelFileNames = std::vector<std::string>();
	mModelFileNames.push_back("tower-mid-e.mesh");
	mModelFileNames.push_back("dome001.mesh");
	mModelFileNames.push_back("dome002.mesh");
	mModelFileNames.push_back("tower-mid-b.mesh");
	mModelFileNames.push_back("tower-c.mesh");
	mModelFileNames.push_back("big-stair-round.mesh");
	mModelFileNames.push_back("big-stair-arc.mesh");
	mModelFileNames.push_back("tower-mid-d.mesh");
	mModelFileNames.push_back("tower-b.mesh");
	mModelFileNames.push_back("tower-mid-g.mesh");
	mModelFileNames.push_back("dome003.mesh");
	mModelFileNames.push_back("arc-slide.mesh");
	mModelFileNames.push_back("tower-mid-a.mesh");
	mModelFileNames.push_back("handrail-corner-square_a.mesh");
	mModelFileNames.push_back("bottom-01.mesh");
	mModelFileNames.push_back("wall-big.mesh");
	mModelFileNames.push_back("tower-mid-h.mesh");
	mModelFileNames.push_back("dome004.mesh");
	mModelFileNames.push_back("stair-third.mesh");
	mModelFileNames.push_back("column-arc.mesh");
	mModelFileNames.push_back("handrail-stair-filled.mesh");
	mModelFileNames.push_back("bottom-02.mesh");
	mModelFileNames.push_back("tower-mid-i.mesh");
	mModelFileNames.push_back("dome005.mesh");
	mModelFileNames.push_back("tower-a.mesh");
	mModelFileNames.push_back("long-stair-arc.mesh");
	mModelFileNames.push_back("handrail-small.mesh");
	mModelFileNames.push_back("handrail-bridge.mesh");
	mModelFileNames.push_back("stair-filled.mesh");
	mModelFileNames.push_back("tower-mid-c.mesh");
	mModelFileNames.push_back("handrail-stair-big.mesh");
	mModelFileNames.push_back("handrail-stair-third.mesh");
	mModelFileNames.push_back("handrail-stair-under.mesh");
	mModelFileNames.push_back("handrail-stair-big-round.mesh");
	mModelFileNames.push_back("column-line.mesh");
	mModelFileNames.push_back("big-stair.mesh");
	mModelFileNames.push_back("stair-down.mesh");
	mModelPositions = std::vector<Ape::Vector3>();
	mModelPositions.push_back(Ape::Vector3(417.291, -228.374, -35.9834));
	mModelPositions.push_back(Ape::Vector3(418.507, 182.656, -36.3186));
	mModelPositions.push_back(Ape::Vector3(-291.134, -251.842, -492.055));
	mModelPositions.push_back(Ape::Vector3(-380.436, 866.144, -149.401));
	mModelPositions.push_back(Ape::Vector3(-173.164, -414.414, 560.734));
	mModelPositions.push_back(Ape::Vector3(131.194, -478.707, -443.457));
	mModelPositions.push_back(Ape::Vector3(151.482, 252.561, -60.9459));
	mModelPositions.push_back(Ape::Vector3(37.4926, 601.034, -148.419));
	mModelPositions.push_back(Ape::Vector3(39.0134, 866.939, -148.211));
	mModelPositions.push_back(Ape::Vector3(-171.659, 257.591, 454.733));
	mModelPositions.push_back(Ape::Vector3(-94.8181, -774.571, -569.124));
	mModelPositions.push_back(Ape::Vector3(424.808, -487.586, -245.54));
	mModelPositions.push_back(Ape::Vector3(-514.349, 36.2367, 111.655));
	mModelPositions.push_back(Ape::Vector3(324.808, -405.31, -43.5001));
	mModelPositions.push_back(Ape::Vector3(505.969, -775.137, -569.385));
	mModelPositions.push_back(Ape::Vector3(-283.071, -146.819, 28.3439));
	mModelPositions.push_back(Ape::Vector3(-172.766, -414.561, 293.837));
	mModelPositions.push_back(Ape::Vector3(-169.508, 257.733, 997.123));
	mModelPositions.push_back(Ape::Vector3(-177.674, -32.9331, 448.117));
	mModelPositions.push_back(Ape::Vector3(360.893, -22.3294, -514.592));
	mModelPositions.push_back(Ape::Vector3(-491.441, 584.33, 89.6195));
	mModelPositions.push_back(Ape::Vector3(-393.009, 601.034, -148.223));
	mModelPositions.push_back(Ape::Vector3(-171.031, 257.615, 724.069));
	mModelPositions.push_back(Ape::Vector3(-381.093, 1144.4, -149.249));
	mModelPositions.push_back(Ape::Vector3(-907.707, 36.0147, 111.233));
	mModelPositions.push_back(Ape::Vector3(-603.729, -69.0567, 25.5738));
	mModelPositions.push_back(Ape::Vector3(-171.746, 164.938, 279.138));
	mModelPositions.push_back(Ape::Vector3(-170.47, 688.295, -260.756));
	mModelPositions.push_back(Ape::Vector3(-491.441, 584.33, 89.61956));
	mModelPositions.push_back(Ape::Vector3(37.8619, 601.034, -148.419));
	mModelPositions.push_back(Ape::Vector3(27.5906, -101.675, 58.7657));
	mModelPositions.push_back(Ape::Vector3(-74.5619, -161.487, 440.9));
	mModelPositions.push_back(Ape::Vector3(-194.794, -320.266, -361.777));
	mModelPositions.push_back(Ape::Vector3(52.3016, -650.958, -245.969));
	mModelPositions.push_back(Ape::Vector3(-289.309, 146.912, -20.5767));
	mModelPositions.push_back(Ape::Vector3(154.975, -94.3448, -42.3769));
	mModelPositions.push_back(Ape::Vector3(-108.952, -443.759, -4.47637));
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeArtScenePlugin::~ApeArtScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeArtScenePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeArtScenePlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeArtScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	for (auto& modelFileName : mModelFileNames)
	{
		mpSceneMakerMacro->makeModel(modelFileName);
	}
	for (auto& modelPosition : mModelPositions)
	{
		if (auto node = mpSceneManager->getNode(mModelFileNames[&modelPosition - &mModelPositions[0]]).lock())
		{
			node->setPosition(modelPosition);
		}
	}
	//node->setOrientation(Ape::Quaternion(-0.499912, -0.499913, 0.500087, -0.500088));
	//node->setOrientation(Ape::Quaternion(0.499968, 0.499847, -0.500153, 0.500032));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeArtScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeArtScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeArtScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeArtScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeArtScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
