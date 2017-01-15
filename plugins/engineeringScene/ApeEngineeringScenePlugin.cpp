#include <iostream>
#include "ApeEngineeringScenePlugin.h"

ApeEngineeringScenePlugin::ApeEngineeringScenePlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeEngineeringScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeEngineeringScenePlugin::~ApeEngineeringScenePlugin()
{
	std::cout << "ApeEngineeringScenePlugin dtor" << std::endl;
}

void ApeEngineeringScenePlugin::eventCallBack(const Ape::Event& event)
{

}

void ApeEngineeringScenePlugin::Init()
{
	std::cout << "ApeEngineeringScenePlugin::init" << std::endl;
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.3f, 0.3f, 0.3f));
		light->setSpecularColor(Ape::Color(0.3f, 0.3f, 0.3f));
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
					planeMaterialPbsPass->setShininess(15.0f);
					planeMaterialPbsPass->setDiffuseColor(Ape::Color(1.0f, 0.9f, 0.8f));
					planeMaterialPbsPass->setSpecularColor(Ape::Color(1.0f, 0.9f, 0.8f));
					planeMaterial->setPass(planeMaterialPbsPass);
					plane->setMaterial(planeMaterial);
				}
			}
		}
	}
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowXMaterial;
	if (coordinateSystemArrowXMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("coordinateSystemArrowXMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto coordinateSystemArrowXMaterialPbsPass = std::static_pointer_cast<Ape::IPbsPass>(mpScene->createEntity("coordinateSystemArrowXMaterialPbsPass", Ape::Entity::PASS_PBS).lock()))
		{
			coordinateSystemArrowXMaterialPbsPass->setShininess(15.0f);
			coordinateSystemArrowXMaterialPbsPass->setDiffuseColor(Ape::Color(1.0f, 0.0f, 0.0f));
			coordinateSystemArrowXMaterialPbsPass->setSpecularColor(Ape::Color(1.0f, 0.0f, 0.0f));
			coordinateSystemArrowXMaterial->setPass(coordinateSystemArrowXMaterialPbsPass);
		}
	}
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowYMaterial;
	if (coordinateSystemArrowYMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("coordinateSystemArrowYMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto coordinateSystemArrowYMaterialPbsPass = std::static_pointer_cast<Ape::IPbsPass>(mpScene->createEntity("coordinateSystemArrowYMaterialPbsPass", Ape::Entity::PASS_PBS).lock()))
		{
			coordinateSystemArrowYMaterialPbsPass->setShininess(15.0f);
			coordinateSystemArrowYMaterialPbsPass->setDiffuseColor(Ape::Color(0.0f, 1.0f, 0.0f));
			coordinateSystemArrowYMaterialPbsPass->setSpecularColor(Ape::Color(0.0f, 1.0f, 0.0f));
			coordinateSystemArrowYMaterial->setPass(coordinateSystemArrowYMaterialPbsPass);
		}
	}
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowZMaterial;
	if (coordinateSystemArrowZMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("coordinateSystemArrowZMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto coordinateSystemArrowZMaterialPbsPass = std::static_pointer_cast<Ape::IPbsPass>(mpScene->createEntity("coordinateSystemArrowZMaterialPbsPass", Ape::Entity::PASS_PBS).lock()))
		{
			coordinateSystemArrowZMaterialPbsPass->setShininess(15.0f);
			coordinateSystemArrowZMaterialPbsPass->setDiffuseColor(Ape::Color(0.0f, 0.0f, 1.0f));
			coordinateSystemArrowZMaterialPbsPass->setSpecularColor(Ape::Color(0.0f, 0.0f, 1.0f));
			coordinateSystemArrowZMaterial->setPass(coordinateSystemArrowZMaterialPbsPass);
		}
	}
	if (auto coordinateSystemNode = mpScene->createNode("coordinateSystemNode").lock())
	{
		if (auto coordinateSystemArrowXTubeNode = mpScene->createNode("coordinateSystemArrowXTubeNode").lock())
		{
			coordinateSystemArrowXTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowXTubeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::LOCAL);
			if (auto coordinateSystemArrowXTube = std::static_pointer_cast<Ape::ITubeGeometry>(mpScene->createEntity("coordinateSystemArrowXTube", Ape::Entity::GEOMETRY_TUBE).lock()))
			{
				coordinateSystemArrowXTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowXTube->setParentNode(coordinateSystemArrowXTubeNode);
				coordinateSystemArrowXTube->setMaterial(coordinateSystemArrowXMaterial);
			}
			if (auto coordinateSystemArrowXConeNode = mpScene->createNode("coordinateSystemArrowXConeNode").lock())
			{
				coordinateSystemArrowXConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowXConeNode->setPosition(Ape::Vector3(0.0f, 100.0f, 0.0f));
				if (auto coordinateSystemArrowXCone = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("coordinateSystemArrowXCone", Ape::Entity::GEOMETRY_SPHERE).lock()))
				{
					coordinateSystemArrowXCone->setParameters(2.5f, Ape::Vector2(1, 1));
					coordinateSystemArrowXCone->setParentNode(coordinateSystemArrowXConeNode);
					coordinateSystemArrowXCone->setMaterial(coordinateSystemArrowXMaterial);
				}
			}
		}
		if (auto coordinateSystemArrowYTubeNode = mpScene->createNode("coordinateSystemArrowYTubeNode").lock())
		{
			coordinateSystemArrowYTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowYTubeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
			if (auto coordinateSystemArrowYTube = std::static_pointer_cast<Ape::ITubeGeometry>(mpScene->createEntity("coordinateSystemArrowYTube", Ape::Entity::GEOMETRY_TUBE).lock()))
			{
				coordinateSystemArrowYTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowYTube->setParentNode(coordinateSystemArrowYTubeNode);
				coordinateSystemArrowYTube->setMaterial(coordinateSystemArrowYMaterial);
			}
			if (auto coordinateSystemArrowYConeNode = mpScene->createNode("coordinateSystemArrowYConeNode").lock())
			{
				coordinateSystemArrowYConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowYConeNode->setPosition(Ape::Vector3(0.0f, 0.0f, 100.0f));
				if (auto coordinateSystemArrowYCone = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("coordinateSystemArrowYCone", Ape::Entity::GEOMETRY_SPHERE).lock()))
				{
					coordinateSystemArrowYCone->setParameters(2.5f, Ape::Vector2(1, 1));
					coordinateSystemArrowYCone->setParentNode(coordinateSystemArrowYConeNode);
					coordinateSystemArrowYCone->setMaterial(coordinateSystemArrowYMaterial);
				}
			}
		}
		if (auto coordinateSystemArrowZTubeNode = mpScene->createNode("coordinateSystemArrowZTubeNode").lock())
		{
			coordinateSystemArrowZTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowZTubeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::LOCAL);
			if (auto coordinateSystemArrowZTube = std::static_pointer_cast<Ape::ITubeGeometry>(mpScene->createEntity("coordinateSystemArrowZTube", Ape::Entity::GEOMETRY_TUBE).lock()))
			{
				coordinateSystemArrowZTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowZTube->setParentNode(coordinateSystemArrowZTubeNode);
				coordinateSystemArrowZTube->setMaterial(coordinateSystemArrowZMaterial);
			}
			if (auto coordinateSystemArrowZConeNode = mpScene->createNode("coordinateSystemArrowZConeNode").lock())
			{
				coordinateSystemArrowZConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowZConeNode->setPosition(Ape::Vector3(-100.0f, 0.0f, 0.0f));
				if (auto coordinateSystemArrowZCone = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("coordinateSystemArrowZCone", Ape::Entity::GEOMETRY_SPHERE).lock()))
				{
					coordinateSystemArrowZCone->setParameters(2.5f, Ape::Vector2(1, 1));
					coordinateSystemArrowZCone->setParentNode(coordinateSystemArrowZConeNode);
					coordinateSystemArrowZCone->setMaterial(coordinateSystemArrowZMaterial);
				}
			}
		}
	}
}

void ApeEngineeringScenePlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeEngineeringScenePlugin::eventCallBack, this, std::placeholders::_1));
}

void ApeEngineeringScenePlugin::Step()
{

}

void ApeEngineeringScenePlugin::Stop()
{

}

void ApeEngineeringScenePlugin::Suspend()
{

}

void ApeEngineeringScenePlugin::Restart()
{

}
