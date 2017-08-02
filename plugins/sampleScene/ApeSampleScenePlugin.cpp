#include <iostream>
#include "ApeSampleScenePlugin.h"

ApeSampleScenePlugin::ApeSampleScenePlugin()
{
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeSampleScenePlugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeSampleScenePlugin::~ApeSampleScenePlugin()
{
	std::cout << "ApeSampleScenePlugin dtor" << std::endl;
}

void ApeSampleScenePlugin::nodeEventCallBack(const Ape::Event& event)
{
	//std::cout  << "event called on the " << event.subjectName << " node" << std::endl;
}

void ApeSampleScenePlugin::Init()
{
	std::cout << "ApeSampleScenePlugin::init" << std::endl;
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, 0, 0));
		light->setDiffuseColor(Ape::Color(0.8f, 0.8f, 0.8f));
		light->setSpecularColor(Ape::Color(0.8f, 0.8f, 0.8f));
	}
	if (auto universeSkyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("universe", Ape::Entity::MATERIAL_FILE).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe.material");
		universeSkyBoxMaterial->setAsSkyBox();
	}
	mPlanetNode = mpScene->createNode("planetNode");
	if (auto planetNode = mPlanetNode.lock())
	{
		planetNode->setPosition(Ape::Vector3(0, 0, -100));
		if (auto planetMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("planet.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			planetMeshFile->setFileName("planet.mesh");
			planetMeshFile->setParentNode(planetNode);
		}
		if (auto helloWorldText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("helloWorldText", Ape::Entity::GEOMETRY_TEXT).lock()))
		{
			helloWorldText->setOffset(Ape::Vector3(0.0f, 10.0f, 0.0f));
			helloWorldText->setCaption("helloWorld");
			helloWorldText->setParentNode(planetNode);
		}
	}
}

void ApeSampleScenePlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		if (auto planetNode = mPlanetNode.lock())
			planetNode->rotate(0.0017f, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::LOCAL);
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeSampleScenePlugin::nodeEventCallBack, this, std::placeholders::_1));
}

void ApeSampleScenePlugin::Step()
{

}

void ApeSampleScenePlugin::Stop()
{

}

void ApeSampleScenePlugin::Suspend()
{

}

void ApeSampleScenePlugin::Restart()
{

}
