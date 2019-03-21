#include "ApeLegoPlugin.h"

ape::ApeLegoPlugin::ApeLegoPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mModelFileNames = std::vector<std::string>();
	mInterpolatePositions = std::vector<ape::Vector3>();
	mModelFileNames.push_back("Lego-Wheel.mesh");
	mInterpolatePositions.push_back(ape::Vector3(123.527, 49.114, 16.422));
	mModelFileNames.push_back("Lego-Wheel_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-79.592, 49.114, 16.422));
	mModelFileNames.push_back("Lego_1x21_Bru.mesh");
	mInterpolatePositions.push_back(ape::Vector3(159.087, 74.514, 16.423));
	mModelFileNames.push_back("Lego_1x2_pl.mesh");
	mInterpolatePositions.push_back(ape::Vector3(98.308, 64.354, 16.427));
	mModelFileNames.push_back("Lego_1x2_1vq.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-122.891, 64.354, 16.427));
	mModelFileNames.push_back("Lego_Rod3_.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-155.874, 64.354, 16.425));
	mModelFileNames.push_back("Lego_2x6_Pl.mesh");
	mInterpolatePositions.push_back(ape::Vector3(21.926, 84.674, 16.425));
	mModelFileNames.push_back("Lego-2x2_Pl.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-79.674, 104.994, 16.425));
	mModelFileNames.push_back("Lego-2x2_Pl_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(124.317, 104.994, 16.425));
	mModelFileNames.push_back("Lego_1x4_Pl.mesh");
	mInterpolatePositions.push_back(ape::Vector3(47.326, 104.994, 16.424));
	mModelFileNames.push_back("Lego_1x4_Pl_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-3.976, 104.994, 16.424));
	mModelFileNames.push_back("Lego_2x4_PlWh.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-79.674, 135.474, 16.425));
	mModelFileNames.push_back("Lego_2x4_PlWh_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(123.67, 135.474, 16.425));
	mModelFileNames.push_back("Lego_2x4_Pl.mesh");
	mInterpolatePositions.push_back(ape::Vector3(21.926, 125.314, 16.424));
	mModelFileNames.push_back("Lego_2x14_Brd.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-117.774, 130.394, 16.426));
	mModelFileNames.push_back("Lego_2x14_Brd_2.mesh");
		//node->rotate(ape::Degree(180).toRadian()); mInterpolatePositions.push_back(ape::Vector3(0, 1, 0)); ape::Node::TransformationSpace::WORLD);
	mInterpolatePositions.push_back(ape::Vector3(159.161, 130.394, 16.426));
	mModelFileNames.push_back("Lego-2x2_Pl_3.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-29.022, 145.539, 16.425));
	mModelFileNames.push_back("Lego-2x2_Pl_4.mesh");
	mInterpolatePositions.push_back(ape::Vector3(72.754, 145.539, 16.425));
	mModelFileNames.push_back("Lego_1x2_pl_2.mesh");
		//node->rotate(ape::Degree(90).toRadian()); mInterpolatePositions.push_back(ape::Vector3(0, 1, 0)); ape::Node::TransformationSpace::WORLD);
	mInterpolatePositions.push_back(ape::Vector3(22.017, 145.885, -59.864));
	mModelFileNames.push_back("Lego_1x2_pl_3.mesh");
		//node->rotate(ape::Degree(90).toRadian()); mInterpolatePositions.push_back(ape::Vector3(0, 1, 0)); ape::Node::TransformationSpace::WORLD);
	mInterpolatePositions.push_back(ape::Vector3(22.017, 145.88, 92.717));
	mModelFileNames.push_back("Lego_1r1_w.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-158.393, 125.323, -59.771));
	mModelFileNames.push_back("Lego_1r1_w_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-158.393, 125.323, 92.778));
	mModelFileNames.push_back("Lego_1x2_Gr.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-153.318, 125.327, 16.429));
	mModelFileNames.push_back("Lego_1x2_Br.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-5.536, 187.21, 67.143));
	mModelFileNames.push_back("Lego_1x2_Br_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-5.536, 187.21, -33.981));
	mModelFileNames.push_back("Lego_1x2_SlGr.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-82.214, 170.374, -8.975));
	mModelFileNames.push_back("Lego_1x2_SlGr_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-82.214, 170.374, 41.735));
	mModelFileNames.push_back("Lego_1x21_Bru.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-13.65, 236.982, -34.525));
	mModelFileNames.push_back("Lego_1x21_Bru_2.mesh");
		//node->rotate(ape::Degree(180).toRadian()); mInterpolatePositions.push_back(ape::Vector3(0, 1, 0)); ape::Node::TransformationSpace::WORLD);
	mInterpolatePositions.push_back(ape::Vector3(-13.65, 236.982, 67.043));
	mModelFileNames.push_back("Lego_1r1-y.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-49.21, 237.053, -59.851));
	mModelFileNames.push_back("Lego_1r1-y_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-49.21, 237.092, -9.125));
	mModelFileNames.push_back("Lego_1r1-y_3.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-49.21, 237.053, 41.717));
	mModelFileNames.push_back("Lego_1r1-y_4.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-49.21, 237.092, 92.442));
	mModelFileNames.push_back("Lego_1x21_PlTi.mesh");
	mInterpolatePositions.push_back(ape::Vector3(98.126, 181.194, 16.425));
	mModelFileNames.push_back("Lego_1x21_PlTi_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(47.491, 181.194, 16.426));
	mModelFileNames.push_back("Lego_1x1_Pl.mesh");
	mInterpolatePositions.push_back(ape::Vector3(148.679, 165.954, 92.619));
	mModelFileNames.push_back("Lego_1x1_Pl_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(148.679, 165.954, -59.855));
	mModelFileNames.push_back("Lego_1x21_Bru_3.mesh");
	mInterpolatePositions.push_back(ape::Vector3(159.087, 176.227, 16.423));
	mModelFileNames.push_back("Lego_1x1_Ti.mesh");
	mInterpolatePositions.push_back(ape::Vector3(194.664, 125.313, 92.626));
	mModelFileNames.push_back("Lego_1x1_Ti_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(194.664, 125.313, -59.675));
	mModelFileNames.push_back("Lego_1x2_pl_4.mesh");
	mInterpolatePositions.push_back(ape::Vector3(199.621, 74.788, 16.427));
	mModelFileNames.push_back("Lego_2r2_Ti.mesh");
	mInterpolatePositions.push_back(ape::Vector3(204.898, 150.71, 16.373));
	mModelFileNames.push_back("Lego_1x2_Slr.mesh");
	mInterpolatePositions.push_back(ape::Vector3(204.806, 74.514, 64.686));
	mModelFileNames.push_back("Lego_1x2_Slr_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(204.806, 74.514, -37.408));
	mModelFileNames.push_back("Lego_1x1_Brhq.mesh");
	mInterpolatePositions.push_back(ape::Vector3(166.484, 206.594, -59.781));
	mModelFileNames.push_back("Lego_1x1_Brhq_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(166.484, 206.594, 92.449));
	mModelFileNames.push_back("Lego_1x2_Br_3.mesh");
	mInterpolatePositions.push_back(ape::Vector3(148.673, 206.594, 16.419));
    mModelFileNames.push_back("Lego_1x1_uq.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-3.478, 254.854, -8.975));
	mModelFileNames.push_back("Lego_1x1_uq_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(-3.478, 254.854, 41.749));
    mModelFileNames.push_back("Lego_1x2_Gr_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(148.639, 242.17, 16.429));
	mModelFileNames.push_back("Lego_1x4_Ti.mesh");
	mInterpolatePositions.push_back(ape::Vector3(72.734, 242.154, 92.627));
	mModelFileNames.push_back("Lego_1x4_Ti_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(72.734, 242.154, -59.817));
	mModelFileNames.push_back("Lego_3x4_Ladder.mesh");
	mInterpolatePositions.push_back(ape::Vector3(72.722, 265.015, 18.959));
	mModelFileNames.push_back("Lego_Rod3_2.mesh");
	mInterpolatePositions.push_back(ape::Vector3(199.41, 231.368, 92.418));
	mModelFileNames.push_back("Lego_Pan.mesh");
	mInterpolatePositions.push_back(ape::Vector3(206.884, 238.341, -62.3259));
	APE_LOG_FUNC_LEAVE();
}

ape::ApeLegoPlugin::~ApeLegoPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&ApeLegoPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeLegoPlugin::eventCallBack(const ape::Event& event)
{

}

void ape::ApeLegoPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	for (auto& modelFileName : mModelFileNames)
	{
		mpSceneMakerMacro->makeModel(modelFileName);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeLegoPlugin::blowModel()
{
	APE_LOG_FUNC_ENTER();
	for (auto& modelFileName : mModelFileNames)
	{
		if (auto node = mpSceneManager->getNode(modelFileName).lock())
		{
			node->setPosition(ape::Vector3(1000, 0, 0));
			node->setOrientation(ape::Quaternion(1, 0, 0, 0));
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeLegoPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		for (auto& interpolatePosition : mInterpolatePositions)
		{
			if (auto node = mpSceneManager->getNode(mModelFileNames[&interpolatePosition - &mInterpolatePositions[0]]).lock())
			{
				mpSceneMakerMacro->interpolate(node, interpolatePosition, ape::Quaternion(1, 0, 0, 0), 5000);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeLegoPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeLegoPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeLegoPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeLegoPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
