#include <iostream>
#include "ApeSceneMakerMacro.h"

Ape::SceneMakerMacro::SceneMakerMacro()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&SceneMakerMacro::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::SceneMakerMacro::~SceneMakerMacro()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::SceneMakerMacro::makeLit()
{
	APE_LOG_FUNC_ENTER();
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.35f, 0.35f, 0.35f));
		light->setSpecularColor(Ape::Color(0.35f, 0.35f, 0.35f));
	}

	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, -1));
		light->setDiffuseColor(Ape::Color(0.35f, 0.35f, 0.35f));
		light->setSpecularColor(Ape::Color(0.35f, 0.35f, 0.35f));
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light3", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, 1));
		light->setDiffuseColor(Ape::Color(0.35f, 0.35f, 0.35f));
		light->setSpecularColor(Ape::Color(0.35f, 0.35f, 0.35f));
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light4", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(-1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.35f, 0.35f, 0.35f));
		light->setSpecularColor(Ape::Color(0.35f, 0.35f, 0.35f));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::SceneMakerMacro::makeGround()
{
	APE_LOG_FUNC_ENTER();
	if (auto planeNode = mpSceneManager->createNode("planeNode").lock())
	{
		planeNode->setPosition(Ape::Vector3(0, -20, 0));
		if (auto planeMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("planeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			planeMaterial->setDiffuseColor(Ape::Color(0.1f, 0.1f, 0.1f));
			planeMaterial->setSpecularColor(Ape::Color(0.3f, 0.3f, 0.2f));
			if (auto plane = std::static_pointer_cast<Ape::IPlaneGeometry>(mpSceneManager->createEntity("plane", Ape::Entity::GEOMETRY_PLANE).lock()))
			{
				plane->setParameters(Ape::Vector2(1, 1), Ape::Vector2(1000, 1000), Ape::Vector2(1, 1));
				plane->setParentNode(planeNode);
				plane->setMaterial(planeMaterial);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::SceneMakerMacro::makeModel(std::string fileName)
{
	if (auto node = mpSceneManager->createNode(fileName).lock())
	{
		if (auto model = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->createEntity(fileName, Ape::Entity::GEOMETRY_FILE).lock()))
		{
			model->setFileName(fileName);
			model->setParentNode(node);
		}
	}
}

void Ape::SceneMakerMacro::makeBrowser()
{
}

void Ape::SceneMakerMacro::makeSky()
{
}

void Ape::SceneMakerMacro::makeBackground()
{
	APE_LOG_FUNC_ENTER();
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::SceneMakerMacro::makeWater()
{
}

void Ape::SceneMakerMacro::makeTerrain()
{
}

void Ape::SceneMakerMacro::makeCoordinateSystem()
{
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
					if (auto coordinateSystemArrowXExtension = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity("coordinateSystemArrowXExtension", Ape::Entity::GEOMETRY_INDEXEDLINESET).lock()))
					{
						Ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						Ape::GeometryIndices indices = { 0, 1, -1 };
						Ape::Color color(1, 0, 0);
						coordinateSystemArrowXExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowXExtension->setParentNode(coordinateSystemArrowXConeNode);
					}
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
					if (auto coordinateSystemArrowYExtension = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity("coordinateSystemArrowYExtension", Ape::Entity::GEOMETRY_INDEXEDLINESET).lock()))
					{
						Ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						Ape::GeometryIndices indices = { 0, 1, -1 };
						Ape::Color color(0, 1, 0);
						coordinateSystemArrowYExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowYExtension->setParentNode(coordinateSystemArrowYConeNode);
					}
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
					if (auto coordinateSystemArrowZExtension = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity("coordinateSystemArrowZExtension", Ape::Entity::GEOMETRY_INDEXEDLINESET).lock()))
					{
						Ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						Ape::GeometryIndices indices = { 0, 1, -1 };
						Ape::Color color(0, 0, 1);
						coordinateSystemArrowZExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowZExtension->setParentNode(coordinateSystemArrowZConeNode);
					}
				}
			}
		}
	}
}

void Ape::SceneMakerMacro::makeBox(std::string name)
{
	if (auto material = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		material->setDiffuseColor(Ape::Color(1.0f, 0.0f, 0.0f));
		material->setSpecularColor(Ape::Color(1.0f, 0.0f, 0.0f));
		if (auto node = mpSceneManager->createNode(name + "Node").lock())
		{
			if (auto box = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity(name, Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				Ape::GeometryCoordinates coordinates = {
					10,  10, -10,
					10, -10, -10,
					-10, -10, -10,
					-10,  10, -10,
					10,  10,  10,
					10, -10,  10,
					-10, -10,  10,
					-10,  10,  10
				};
				Ape::GeometryIndices indices = {
					0, 1, 2, 3, -1,
					4, 7, 6, 5, -1,
					0, 4, 5, 1, -1,
					1, 5, 6, 2, -1,
					2, 6, 7, 3, -1,
					4, 0, 3, 7, -1 };
				box->setParameters("", coordinates, indices, Ape::GeometryNormals(), true, Ape::GeometryColors(), Ape::GeometryTextureCoordinates(), material);
				box->setParentNode(node);
			}
		}
	}
}

void Ape::SceneMakerMacro::makeOverlayBrowser()
{
	if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->createEntity("overlay_frame", Ape::Entity::BROWSER).lock()))
	{
		browser->setResoultion(1280, 720);
		std::stringstream url;
		APE_LOG_FUNC_ENTER();
		//TODO_SystemJson
		//url << "http://localhost:" << mNodeJsPluginConfig.serverPort << "/robotCalibration/public/";
		browser->setURL(url.str());
		browser->showOnOverlay(true, 0);
		if (auto mouseMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("mouseMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			mouseMaterial->setEmissiveColor(Ape::Color(1.0f, 1.0f, 1.0f));
			mouseMaterial->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
			//mouseMaterial->setLightingEnabled(false); TODO_SceneMakerMacro crash in OpenGL
			if (auto mouseTexture = std::static_pointer_cast<Ape::IUnitTexture>(mpSceneManager->createEntity("mouseTexture", Ape::Entity::TEXTURE_UNIT).lock()))
			{
				mouseTexture->setParameters(mouseMaterial, "browserpointer.png");
				mouseTexture->setTextureAddressingMode(Ape::Texture::AddressingMode::CLAMP);
				mouseTexture->setTextureFiltering(Ape::Texture::Filtering::POINT, Ape::Texture::Filtering::LINEAR, Ape::Texture::Filtering::F_NONE);
			}
			mouseMaterial->showOnOverlay(true, 1);
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::SceneMakerMacro::interpolate(Ape::NodeWeakPtr node, Ape::Vector3 position, Ape::Quaternion orientation, unsigned int milliseconds)
{
	if (auto nodeSP = node.lock())
	{
		auto moveInterpolator = std::make_unique<Ape::Interpolator>(false);
		moveInterpolator->addSection(
			nodeSP->getPosition(),
			position,
			milliseconds * 1000,
			[&](Ape::Vector3 pos) { nodeSP->setPosition(pos); }
		);
		auto rotateInterpolator = std::make_unique<Ape::Interpolator>(false);
		rotateInterpolator->addSection(
			nodeSP->getOrientation(),
			orientation,
			milliseconds * 1000,
			[&](Ape::Quaternion ori) { nodeSP->setOrientation(ori); }
		);
		while (!moveInterpolator->isQueueEmpty() && !rotateInterpolator->isQueueEmpty())
		{
			if (!moveInterpolator->isQueueEmpty())
				moveInterpolator->iterateTopSection();
			if (!rotateInterpolator->isQueueEmpty())
				rotateInterpolator->iterateTopSection();
		}
	}
}

void Ape::SceneMakerMacro::eventCallBack(const Ape::Event& event)
{
	
}


