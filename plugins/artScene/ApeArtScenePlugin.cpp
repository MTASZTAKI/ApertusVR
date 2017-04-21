#include <iostream>
#include "ApeArtScenePlugin.h"

ApeArtScenePlugin::ApeArtScenePlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeArtScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeArtScenePlugin::~ApeArtScenePlugin()
{
	std::cout << "ApeArtScenePlugin dtor" << std::endl;
}

void ApeArtScenePlugin::eventCallBack(const Ape::Event& event)
{

}

void ApeArtScenePlugin::Init()
{
	std::cout << "ApeArtScenePlugin::init" << std::endl;
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto node = mpScene->createNode("tower-mid-e").lock())
	{
		node->setPosition(Ape::Vector3(417.291, -228.374, -35.9834));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-mid-e.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-mid-e.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("dome001").lock())
	{
		node->setPosition(Ape::Vector3(418.507, 182.656, -36.3186));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("dome001.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("dome001.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("dome002").lock())
	{
		node->setPosition(Ape::Vector3(-291.134, -251.842, -492.055));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("dome002.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("dome002.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-mid-b").lock())
	{
		node->setPosition(Ape::Vector3(-380.436, 866.144, -149.401));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-mid-b.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-mid-b.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-c").lock())
	{
		node->setPosition(Ape::Vector3(-173.164, -414.414, 560.734));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-c.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-c.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("big-stair-round").lock())
	{
		node->setPosition(Ape::Vector3(131.194, -478.707, -443.457));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("big-stair-round.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("big-stair-round.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("big-stair-arc").lock())
	{
		node->setPosition(Ape::Vector3(151.482, 252.561, -60.9459));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("big-stair-arc.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("big-stair-arc.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-mid-d").lock())
	{
		node->setPosition(Ape::Vector3(37.4926, 601.034, -148.419));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-mid-d.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-mid-d.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-b").lock())
	{
		node->setPosition(Ape::Vector3(39.0134, 866.939, -148.211));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-b.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-b.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-mid-g").lock())
	{
		node->setPosition(Ape::Vector3(-171.659, 257.591, 454.733));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-mid-g.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-mid-g.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("dome003").lock())
	{
		node->setPosition(Ape::Vector3(-94.8181, -774.571, -569.124));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("dome003.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("dome003.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("arc-slide").lock())
	{
		node->setPosition(Ape::Vector3(424.808, -487.586, -245.54));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("arc-slide.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("arc-slide.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-mid-a").lock())
	{
		node->setPosition(Ape::Vector3(-514.349, 36.2367, 111.655));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-mid-a.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-mid-a.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("handrail-corner-square_a").lock())
	{
		node->setPosition(Ape::Vector3(324.808, -405.31, -43.5001));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("handrail-corner-square_a.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("handrail-corner-square_a.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("bottom-01").lock())
	{
		node->setPosition(Ape::Vector3(505.969, -775.137, -569.385));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("bottom-01.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("bottom-01.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("wall-big").lock())
	{
		node->setPosition(Ape::Vector3(-283.071, -146.819, 28.3439));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("wall-big.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("wall-big.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-mid-h").lock())
	{
		node->setPosition(Ape::Vector3(-172.766, -414.561, 293.837));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-mid-h.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-mid-h.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("dome004").lock())
	{
		node->setPosition(Ape::Vector3(-169.508, 257.733, 997.123));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("dome004.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("dome004.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("stair-third").lock())
	{
		node->setPosition(Ape::Vector3(-177.674, -32.9331, 448.117));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("stair-third.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("stair-third.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("column-arc").lock())
	{
		node->setPosition(Ape::Vector3(360.893, -22.3294, -514.592));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("column-arc.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("column-arc.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("handrail-stair-filled").lock())
	{
		node->setPosition(Ape::Vector3(-491.441, 584.33, 89.6195));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("handrail-stair-filled.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("handrail-stair-filled.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("bottom-02").lock())
	{
		node->setPosition(Ape::Vector3(-393.009, 601.034, -148.223));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("bottom-02.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("bottom-02.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-mid-i").lock())
	{
		node->setPosition(Ape::Vector3(-171.031, 257.615, 724.069));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-mid-i.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-mid-i.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("dome005").lock())
	{
		node->setPosition(Ape::Vector3(-381.093, 1144.4, -149.249));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("dome005.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("dome005.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-a").lock())
	{
		node->setPosition(Ape::Vector3(-907.707, 36.0147, 111.233));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-a.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-a.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("long-stair-arc").lock())
	{
		node->setPosition(Ape::Vector3(-603.729, -69.0567, 25.5738));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("long-stair-arc.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("long-stair-arc.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("handrail-small").lock())
	{
		node->setPosition(Ape::Vector3(-171.746, 164.938, 279.138));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("handrail-small.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("handrail-small.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("handrail-bridge").lock())
	{
		node->setPosition(Ape::Vector3(-170.47, 688.295, -260.756));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("handrail-bridge.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("handrail-bridge.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("stair-filled").lock())
	{
		node->setPosition(Ape::Vector3(-491.441, 584.33, 89.61956));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("stair-filled.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("stair-filled.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("tower-mid-c").lock())
	{
		node->setPosition(Ape::Vector3(37.8619, 601.034, -148.419));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("tower-mid-c.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("tower-mid-c.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("handrail-stair-big").lock())
	{
		node->setPosition(Ape::Vector3(27.5906, -101.675, 58.7657));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("handrail-stair-big.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("handrail-stair-big.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("handrail-stair-third").lock())
	{
		node->setPosition(Ape::Vector3(-74.5619, -161.487, 440.9));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("handrail-stair-third.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("handrail-stair-third.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("handrail-stair-under").lock())
	{
		node->setPosition(Ape::Vector3(-194.794, -320.266, -361.777));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("handrail-stair-under.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("handrail-stair-under.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("handrail-stair-big-round").lock())
	{
		node->setPosition(Ape::Vector3(52.3016, -650.958, -245.969));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("handrail-stair-big-round.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("handrail-stair-big-round.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("column-line").lock())
	{
		node->setPosition(Ape::Vector3(-289.309, 146.912, -20.5767));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("column-line.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("column-line.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("big-stair").lock())
	{
		node->setPosition(Ape::Vector3(154.975, -94.3448, -42.3769));
		node->setOrientation(Ape::Quaternion(-0.499912, -0.499913, 0.500087, -0.500088));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("big-stair.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("big-stair.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("stair-down").lock())
	{
		node->setPosition(Ape::Vector3(-108.952, -443.759, -4.47637));
		node->setOrientation(Ape::Quaternion(0.499968, 0.499847, -0.500153, 0.500032));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("stair-down.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("stair-down.mesh");
			meshFile->setParentNode(node);
		}
	}
}

void ApeArtScenePlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeArtScenePlugin::eventCallBack, this, std::placeholders::_1));
}

void ApeArtScenePlugin::Step()
{

}

void ApeArtScenePlugin::Stop()
{

}

void ApeArtScenePlugin::Suspend()
{

}

void ApeArtScenePlugin::Restart()
{

}
