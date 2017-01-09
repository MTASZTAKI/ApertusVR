#include <iostream>
#include "ApeLinkageDesignerVRPlugin.h"

ApeLinkageDesignerVRPlugin::ApeLinkageDesignerVRPlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeLinkageDesignerVRPlugin::~ApeLinkageDesignerVRPlugin()
{
	std::cout << "ApeLinkageDesignerVRPlugin dtor" << std::endl;
}

void ApeLinkageDesignerVRPlugin::eventCallBack(const Ape::Event& event)
{

}

void ApeLinkageDesignerVRPlugin::Init()
{
	std::cout << "ApeLinkageDesignerVRPlugin::init" << std::endl;
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto planeNode = mpScene->createNode("planeNode").lock())
	{
		if (auto plane = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("plane", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			plane->setParameters(Ape::Vector2(1, 1), Ape::Vector2(1000, 1000), Ape::Vector2(1, 1));
			plane->setParentNode(planeNode);
			if (auto planeMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("planeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
			{
				if (auto planeMaterialPbsPass = std::static_pointer_cast<Ape::IPbsPass>(mpScene->createEntity("planeMaterialPbsPass", Ape::Entity::PASS_PBS).lock()))
				{
					planeMaterialPbsPass->setEmissiveColor(Ape::Color(0.7f, 0.5f, 0.7f));
					planeMaterial->setPass(planeMaterialPbsPass);
					plane->setMaterial(planeMaterial);
				}
			}
		}
	}
}

void ApeLinkageDesignerVRPlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
}

void ApeLinkageDesignerVRPlugin::Step()
{

}

void ApeLinkageDesignerVRPlugin::Stop()
{

}

void ApeLinkageDesignerVRPlugin::Suspend()
{

}

void ApeLinkageDesignerVRPlugin::Restart()
{

}
