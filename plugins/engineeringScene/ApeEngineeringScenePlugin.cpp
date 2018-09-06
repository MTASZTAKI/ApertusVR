#include <iostream>
#include "ApeEngineeringScenePlugin.h"

Ape::ApeEngineeringScenePlugin::ApeEngineeringScenePlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeEngineeringScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mInterpolators = std::vector<std::unique_ptr<Ape::Interpolator>>();
	LOG_FUNC_LEAVE();
}

Ape::ApeEngineeringScenePlugin::~ApeEngineeringScenePlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeEngineeringScenePlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeEngineeringScenePlugin::Init()
{
	LOG_FUNC_ENTER();
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.3f, 0.3f, 0.3f));
		light->setSpecularColor(Ape::Color(0.3f, 0.3f, 0.3f));
	}
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowXMaterial;
	if (coordinateSystemArrowXMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("SystemArrowXMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		coordinateSystemArrowXMaterial->setDiffuseColor(Ape::Color(1.0f, 0.0f, 0.0f));
		coordinateSystemArrowXMaterial->setSpecularColor(Ape::Color(1.0f, 0.0f, 0.0f));
	}
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowYMaterial;
	if (coordinateSystemArrowYMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("coordinateSystemArrowYMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		coordinateSystemArrowYMaterial->setDiffuseColor(Ape::Color(0.0f, 1.0f, 0.0f));
		coordinateSystemArrowYMaterial->setSpecularColor(Ape::Color(0.0f, 1.0f, 0.0f));
	}
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowZMaterial;
	if (coordinateSystemArrowZMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("coordinateSystemArrowZMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		coordinateSystemArrowZMaterial->setDiffuseColor(Ape::Color(0.0f, 0.0f, 1.0f));
		coordinateSystemArrowZMaterial->setSpecularColor(Ape::Color(0.0f, 0.0f, 1.0f));
	}
	if (auto coordinateSystemNode = mpSceneManager->createNode("coordinateSystemNode").lock())
	{
		if (auto coordinateSystemArrowXTubeNode = mpSceneManager->createNode("coordinateSystemArrowXTubeNode").lock())
		{
			coordinateSystemArrowXTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowXTubeNode->rotate(Ape::Degree(-90.0f).toRadian(), Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
			if (auto coordinateSystemArrowXTube = std::static_pointer_cast<Ape::ITubeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowXTube", Ape::Entity::GEOMETRY_TUBE).lock()))
			{
				coordinateSystemArrowXTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowXTube->setParentNode(coordinateSystemArrowXTubeNode);
				coordinateSystemArrowXTube->setMaterial(coordinateSystemArrowXMaterial);
			}
			if (auto coordinateSystemArrowXConeNode = mpSceneManager->createNode("coordinateSystemArrowXConeNode").lock())
			{
				coordinateSystemArrowXConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowXConeNode->setPosition(Ape::Vector3(100.0f, 0.0f, 0.0f));
				coordinateSystemArrowXConeNode->rotate(Ape::Degree(-90.0f).toRadian(), Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
				if (auto coordinateSystemArrowXCone = std::static_pointer_cast<Ape::IConeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowXCone", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					coordinateSystemArrowXCone->setParameters(2.5f, 2.5f, 1.0f, Ape::Vector2(1, 1));
					coordinateSystemArrowXCone->setParentNode(coordinateSystemArrowXConeNode);
					coordinateSystemArrowXCone->setMaterial(coordinateSystemArrowXMaterial);
					if (auto textNode = mpSceneManager->createNode("coordinateSystemXText_Node").lock())
					{
						textNode->setParentNode(coordinateSystemArrowXConeNode);
						textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemXText", Ape::Entity::GEOMETRY_TEXT).lock()))
						{
							text->setCaption("X");
							text->setParentNode(textNode);
						}
					}
					/*if (auto coordinateSystemArrowXExtension = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpScene->createEntity("coordinateSystemArrowXExtension", Ape::Entity::GEOMETRY_INDEXEDLINESET).lock()))
					{
						Ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						Ape::GeometryIndices indices = { 0, 1, -1 };
						Ape::Color color(1, 0, 0);
						coordinateSystemArrowXExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowXExtension->setParentNode(coordinateSystemArrowXConeNode);
					}*/
				}
			}
		}
		if (auto coordinateSystemArrowYTubeNode = mpSceneManager->createNode("coordinateSystemArrowYTubeNode").lock())
		{
			coordinateSystemArrowYTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowYTubeNode->rotate(Ape::Degree(0.0f).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			if (auto coordinateSystemArrowYTube = std::static_pointer_cast<Ape::ITubeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowYTube", Ape::Entity::GEOMETRY_TUBE).lock()))
			{
				coordinateSystemArrowYTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowYTube->setParentNode(coordinateSystemArrowYTubeNode);
				coordinateSystemArrowYTube->setMaterial(coordinateSystemArrowYMaterial);
			}
			if (auto coordinateSystemArrowYConeNode = mpSceneManager->createNode("coordinateSystemArrowYConeNode").lock())
			{
				coordinateSystemArrowYConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowYConeNode->setPosition(Ape::Vector3(0.0f, 100.0f, 0.0f));
				if (auto coordinateSystemArrowYCone = std::static_pointer_cast<Ape::IConeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowYCone", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					coordinateSystemArrowYCone->setParameters(2.5f, 2.5f, 1.0f, Ape::Vector2(1, 1));
					coordinateSystemArrowYCone->setParentNode(coordinateSystemArrowYConeNode);
					coordinateSystemArrowYCone->setMaterial(coordinateSystemArrowYMaterial);
					if (auto textNode = mpSceneManager->createNode("coordinateSystemYText_Node").lock())
					{
						textNode->setParentNode(coordinateSystemArrowYConeNode);
						textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemYText", Ape::Entity::GEOMETRY_TEXT).lock()))
						{
							text->setCaption("Y");
							text->setParentNode(textNode);
						}
					}
					/*if (auto coordinateSystemArrowYExtension = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpScene->createEntity("coordinateSystemArrowYExtension", Ape::Entity::GEOMETRY_INDEXEDLINESET).lock()))
					{
						Ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						Ape::GeometryIndices indices = { 0, 1, -1 };
						Ape::Color color(0, 1, 0);
						coordinateSystemArrowYExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowYExtension->setParentNode(coordinateSystemArrowYConeNode);
					}*/
				}
			}
		}
		if (auto coordinateSystemArrowZTubeNode = mpSceneManager->createNode("coordinateSystemArrowZTubeNode").lock())
		{
			coordinateSystemArrowZTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowZTubeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
			if (auto coordinateSystemArrowZTube = std::static_pointer_cast<Ape::ITubeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowZTube", Ape::Entity::GEOMETRY_TUBE).lock()))
			{
				coordinateSystemArrowZTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowZTube->setParentNode(coordinateSystemArrowZTubeNode);
				coordinateSystemArrowZTube->setMaterial(coordinateSystemArrowZMaterial);
			}
			if (auto coordinateSystemArrowZConeNode = mpSceneManager->createNode("coordinateSystemArrowZConeNode").lock())
			{
				coordinateSystemArrowZConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowZConeNode->setPosition(Ape::Vector3(0.0f, 0.0f, 100.0f));
				coordinateSystemArrowZConeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
				if (auto coordinateSystemArrowZCone = std::static_pointer_cast<Ape::IConeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowZCone", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					coordinateSystemArrowZCone->setParameters(2.5f, 2.5f, 1.0f, Ape::Vector2(1, 1));
					coordinateSystemArrowZCone->setParentNode(coordinateSystemArrowZConeNode);
					coordinateSystemArrowZCone->setMaterial(coordinateSystemArrowZMaterial);
					if (auto textNode = mpSceneManager->createNode("coordinateSystemZText_Node").lock())
					{
						textNode->setParentNode(coordinateSystemArrowZConeNode);
						textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemZText", Ape::Entity::GEOMETRY_TEXT).lock()))
						{
							text->setCaption("Z");
							text->setParentNode(textNode);
						}
					}
					/*if (auto coordinateSystemArrowZExtension = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpScene->createEntity("coordinateSystemArrowZExtension", Ape::Entity::GEOMETRY_INDEXEDLINESET).lock()))
					{
						Ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						Ape::GeometryIndices indices = { 0, 1, -1 };
						Ape::Color color(0, 0, 1);
						coordinateSystemArrowZExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowZExtension->setParentNode(coordinateSystemArrowZConeNode);
					}*/
				}
			}
		}
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeEngineeringScenePlugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeEngineeringScenePlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeEngineeringScenePlugin::Step()
{

}

void Ape::ApeEngineeringScenePlugin::Stop()
{

}

void Ape::ApeEngineeringScenePlugin::Suspend()
{

}

void Ape::ApeEngineeringScenePlugin::Restart()
{

}
