#include <iostream>
#include "apeSceneMakerMacro.h"

ape::SceneMakerMacro::SceneMakerMacro()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&SceneMakerMacro::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::SceneMakerMacro::~SceneMakerMacro()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::SceneMakerMacro::makeLit(ape::Color color)
{
	APE_LOG_FUNC_ENTER();
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(1, -1, 0));
		light->setDiffuseColor(color);
		light->setSpecularColor(color);
	}

	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light2", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(0, -1, -1));
		light->setDiffuseColor(color);
		light->setSpecularColor(color);
	}
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light3", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(0, -1, 1));
		light->setDiffuseColor(color);
		light->setSpecularColor(color);
	}
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light4", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(-1, -1, 0));
		light->setDiffuseColor(color);
		light->setSpecularColor(color);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::SceneMakerMacro::makeGround()
{
	APE_LOG_FUNC_ENTER();
	if (auto planeNode = mpSceneManager->createNode("planeNode", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		planeNode->setPosition(ape::Vector3(0, -20, 0));
		if (auto planeMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("planeMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			planeMaterial->setDiffuseColor(ape::Color(0.1f, 0.1f, 0.1f));
			planeMaterial->setSpecularColor(ape::Color(0.3f, 0.3f, 0.2f));
			if (auto plane = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity("plane", ape::Entity::GEOMETRY_PLANE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				plane->setParameters(ape::Vector2(1, 1), ape::Vector2(4000, 4000), ape::Vector2(1, 1));
				plane->setParentNode(planeNode);
				plane->setMaterial(planeMaterial);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::SceneMakerMacro::makeModel(std::string fileName)
{
	if (auto node = mpSceneManager->createNode(fileName, true, mpCoreConfig->getNetworkGUID()).lock())
	{
		if (auto model = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(fileName, ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			model->setFileName(fileName);
			model->setParentNode(node);
		}
	}
}

void ape::SceneMakerMacro::makeBrowser(std::string name, std::string url, ape::Vector3 position, ape::Quaternion orientation, float width, float height, float resoultionVertical, float resolutionHorizontal)
{
	if (!mpSceneManager->getNode(name).lock())
	{
		if (auto browserNode = mpSceneManager->createNode(name, true, mpCoreConfig->getNetworkGUID()).lock())
		{
			browserNode->setPosition(position);
			browserNode->setOrientation(orientation);
			if (auto browserGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity(name + "geometry", ape::Entity::GEOMETRY_PLANE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				browserGeometry->setParameters(ape::Vector2(1, 1), ape::Vector2(width, height), ape::Vector2(1, 1));
				browserGeometry->setParentNode(browserNode);
				if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->createEntity(name + "browser", ape::Entity::BROWSER, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					browser->setResoultion(resoultionVertical, resolutionHorizontal);
					browser->setURL(url);
					browser->setGeometry(browserGeometry);
					/*if (auto browserMouseNode = mpSceneManager->createNode(name + "mouse").lock())
					{
						browserMouseNode->setParentNode(browserNode);
						browserMouseNode->setPosition(ape::Vector3(0, -1, 0));
						if (auto mouseGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity(name + "mouseGeometry", ape::Entity::GEOMETRY_PLANE).lock()))
						{
							mouseGeometry->setParameters(ape::Vector2(1, 1), ape::Vector2(width, height), ape::Vector2(1, 1));
							mouseGeometry->setParentNode(browserMouseNode);
							if (auto mouseMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "mouseMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
							{
								mouseMaterial->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
								mouseMaterial->setSceneBlending(ape::Material::SceneBlendingType::TRANSPARENT_ALPHA);
								mouseMaterial->setLightingEnabled(false);
								mouseMaterial->setCullingMode(ape::Material::CullingMode::NONE_CM);
								if (auto mouseTexture = std::static_pointer_cast<ape::IUnitTexture>(mpSceneManager->createEntity(name + "mouseTexture", ape::Entity::TEXTURE_UNIT).lock()))
								{
									mouseTexture->setParameters(mouseMaterial, "browserpointer.png");
									mouseTexture->setTextureAddressingMode(ape::Texture::AddressingMode::CLAMP);
									mouseTexture->setTextureFiltering(ape::Texture::Filtering::POINT, ape::Texture::Filtering::LINEAR, ape::Texture::Filtering::F_NONE);
								}
								std::static_pointer_cast<ape::IPlaneGeometry>(mouseGeometry)->setMaterial(mouseMaterial);
							}
						}
					}*/
				}
			}
		}
	}
}

void ape::SceneMakerMacro::makeSky()
{

}

void ape::SceneMakerMacro::makeBackground()
{
	APE_LOG_FUNC_ENTER();
	if (auto skyBoxMaterial = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->createEntity("skyBox", ape::Entity::MATERIAL_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		skyBoxMaterial->setFileName("skyBox");
		skyBoxMaterial->setAsSkyBox();
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::SceneMakerMacro::makeWater()
{
}

void ape::SceneMakerMacro::makeTerrain()
{
}

void ape::SceneMakerMacro::makeCoordinateSystem()
{
	std::shared_ptr<ape::IManualMaterial> coordinateSystemArrowXMaterial;
	if (coordinateSystemArrowXMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("SystemArrowXMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		coordinateSystemArrowXMaterial->setDiffuseColor(ape::Color(1.0f, 0.0f, 0.0f));
		coordinateSystemArrowXMaterial->setSpecularColor(ape::Color(1.0f, 0.0f, 0.0f));
	}
	std::shared_ptr<ape::IManualMaterial> coordinateSystemArrowYMaterial;
	if (coordinateSystemArrowYMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("coordinateSystemArrowYMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		coordinateSystemArrowYMaterial->setDiffuseColor(ape::Color(0.0f, 1.0f, 0.0f));
		coordinateSystemArrowYMaterial->setSpecularColor(ape::Color(0.0f, 1.0f, 0.0f));
	}
	std::shared_ptr<ape::IManualMaterial> coordinateSystemArrowZMaterial;
	if (coordinateSystemArrowZMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("coordinateSystemArrowZMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		coordinateSystemArrowZMaterial->setDiffuseColor(ape::Color(0.0f, 0.0f, 1.0f));
		coordinateSystemArrowZMaterial->setSpecularColor(ape::Color(0.0f, 0.0f, 1.0f));
	}
	if (auto coordinateSystemNode = mpSceneManager->createNode("coordinateSystemNode", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		if (auto coordinateSystemArrowXTubeNode = mpSceneManager->createNode("coordinateSystemArrowXTubeNode", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			coordinateSystemArrowXTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowXTubeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
			if (auto coordinateSystemArrowXTube = std::static_pointer_cast<ape::ITubeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowXTube", ape::Entity::GEOMETRY_TUBE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				coordinateSystemArrowXTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowXTube->setParentNode(coordinateSystemArrowXTubeNode);
				coordinateSystemArrowXTube->setMaterial(coordinateSystemArrowXMaterial);
			}
			if (auto coordinateSystemArrowXConeNode = mpSceneManager->createNode("coordinateSystemArrowXConeNode", true, mpCoreConfig->getNetworkGUID()).lock())
			{
				coordinateSystemArrowXConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowXConeNode->setPosition(ape::Vector3(100.0f, 0.0f, 0.0f));
				coordinateSystemArrowXConeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
				if (auto coordinateSystemArrowXCone = std::static_pointer_cast<ape::IConeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowXCone", ape::Entity::GEOMETRY_CONE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					coordinateSystemArrowXCone->setParameters(2.5f, 2.5f, 1.0f, ape::Vector2(1, 1));
					coordinateSystemArrowXCone->setParentNode(coordinateSystemArrowXConeNode);
					coordinateSystemArrowXCone->setMaterial(coordinateSystemArrowXMaterial);
					if (auto textNode = mpSceneManager->createNode("coordinateSystemXText_Node", true, mpCoreConfig->getNetworkGUID()).lock())
					{
						textNode->setParentNode(coordinateSystemArrowXConeNode);
						textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto text = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemXText", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							text->setCaption("X");
							text->setParentNode(textNode);
						}
					}
					if (auto coordinateSystemArrowXExtension = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity("coordinateSystemArrowXExtension", ape::Entity::GEOMETRY_INDEXEDLINESET, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						ape::GeometryIndices indices = { 0, 1, -1 };
						ape::Color color(1, 0, 0);
						coordinateSystemArrowXExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowXExtension->setParentNode(coordinateSystemArrowXConeNode);
					}
				}
			}
		}
		if (auto coordinateSystemArrowYTubeNode = mpSceneManager->createNode("coordinateSystemArrowYTubeNode", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			coordinateSystemArrowYTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowYTubeNode->rotate(ape::Degree(0.0f).toRadian(), ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::WORLD);
			if (auto coordinateSystemArrowYTube = std::static_pointer_cast<ape::ITubeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowYTube", ape::Entity::GEOMETRY_TUBE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				coordinateSystemArrowYTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowYTube->setParentNode(coordinateSystemArrowYTubeNode);
				coordinateSystemArrowYTube->setMaterial(coordinateSystemArrowYMaterial);
			}
			if (auto coordinateSystemArrowYConeNode = mpSceneManager->createNode("coordinateSystemArrowYConeNode", true, mpCoreConfig->getNetworkGUID()).lock())
			{
				coordinateSystemArrowYConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowYConeNode->setPosition(ape::Vector3(0.0f, 100.0f, 0.0f));
				if (auto coordinateSystemArrowYCone = std::static_pointer_cast<ape::IConeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowYCone", ape::Entity::GEOMETRY_CONE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					coordinateSystemArrowYCone->setParameters(2.5f, 2.5f, 1.0f, ape::Vector2(1, 1));
					coordinateSystemArrowYCone->setParentNode(coordinateSystemArrowYConeNode);
					coordinateSystemArrowYCone->setMaterial(coordinateSystemArrowYMaterial);
					if (auto textNode = mpSceneManager->createNode("coordinateSystemYText_Node", true, mpCoreConfig->getNetworkGUID()).lock())
					{
						textNode->setParentNode(coordinateSystemArrowYConeNode);
						textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto text = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemYText", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							text->setCaption("Y");
							text->setParentNode(textNode);
						}
					}
					if (auto coordinateSystemArrowYExtension = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity("coordinateSystemArrowYExtension", ape::Entity::GEOMETRY_INDEXEDLINESET, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						ape::GeometryIndices indices = { 0, 1, -1 };
						ape::Color color(0, 1, 0);
						coordinateSystemArrowYExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowYExtension->setParentNode(coordinateSystemArrowYConeNode);
					}
				}
			}
		}
		if (auto coordinateSystemArrowZTubeNode = mpSceneManager->createNode("coordinateSystemArrowZTubeNode", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			coordinateSystemArrowZTubeNode->setParentNode(coordinateSystemNode);
			coordinateSystemArrowZTubeNode->rotate(ape::Degree(90.0f).toRadian(), ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::WORLD);
			if (auto coordinateSystemArrowZTube = std::static_pointer_cast<ape::ITubeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowZTube", ape::Entity::GEOMETRY_TUBE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				coordinateSystemArrowZTube->setParameters(100.0f, 1.0f);
				coordinateSystemArrowZTube->setParentNode(coordinateSystemArrowZTubeNode);
				coordinateSystemArrowZTube->setMaterial(coordinateSystemArrowZMaterial);
			}
			if (auto coordinateSystemArrowZConeNode = mpSceneManager->createNode("coordinateSystemArrowZConeNode", true, mpCoreConfig->getNetworkGUID()).lock())
			{
				coordinateSystemArrowZConeNode->setParentNode(coordinateSystemNode);
				coordinateSystemArrowZConeNode->setPosition(ape::Vector3(0.0f, 0.0f, 100.0f));
				coordinateSystemArrowZConeNode->rotate(ape::Degree(90.0f).toRadian(), ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::WORLD);
				if (auto coordinateSystemArrowZCone = std::static_pointer_cast<ape::IConeGeometry>(mpSceneManager->createEntity("coordinateSystemArrowZCone", ape::Entity::GEOMETRY_CONE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					coordinateSystemArrowZCone->setParameters(2.5f, 2.5f, 1.0f, ape::Vector2(1, 1));
					coordinateSystemArrowZCone->setParentNode(coordinateSystemArrowZConeNode);
					coordinateSystemArrowZCone->setMaterial(coordinateSystemArrowZMaterial);
					if (auto textNode = mpSceneManager->createNode("coordinateSystemZText_Node", true, mpCoreConfig->getNetworkGUID()).lock())
					{
						textNode->setParentNode(coordinateSystemArrowZConeNode);
						textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto text = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemZText", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							text->setCaption("Z");
							text->setParentNode(textNode);
						}
					}
					if (auto coordinateSystemArrowZExtension = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity("coordinateSystemArrowZExtension", ape::Entity::GEOMETRY_INDEXEDLINESET, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						ape::GeometryCoordinates coordinates = {
							0, 0, 0,
							0, 400, 0 };
						ape::GeometryIndices indices = { 0, 1, -1 };
						ape::Color color(0, 0, 1);
						coordinateSystemArrowZExtension->setParameters(coordinates, indices, color);
						coordinateSystemArrowZExtension->setParentNode(coordinateSystemArrowZConeNode);
					}
				}
			}
		}
	}
}

void ape::SceneMakerMacro::makeBox(std::string name)
{
	if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		material->setDiffuseColor(ape::Color(1.0f, 0.0f, 0.0f));
		material->setSpecularColor(ape::Color(1.0f, 0.0f, 0.0f));
		if (auto node = mpSceneManager->createNode(name + "Node", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			if (auto box = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_INDEXEDFACESET, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				ape::GeometryCoordinates coordinates = {
					10,  10, -10,
					10, -10, -10,
					-10, -10, -10,
					-10,  10, -10,
					10,  10,  10,
					10, -10,  10,
					-10, -10,  10,
					-10,  10,  10
				};
				ape::GeometryIndices indices = {
					0, 1, 2, 3, -1,
					4, 7, 6, 5, -1,
					0, 4, 5, 1, -1,
					1, 5, 6, 2, -1,
					2, 6, 7, 3, -1,
					4, 0, 3, 7, -1 };
				box->setParameters("", coordinates, indices, ape::GeometryNormals(), true, ape::GeometryColors(), ape::GeometryTextureCoordinates(), material);
				box->setParentNode(node);
			}
		}
	}
}

void ape::SceneMakerMacro::makeOverlayBrowser(std::string url)
{
	APE_LOG_FUNC_ENTER();
	if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->createEntity("overlay_frame", ape::Entity::BROWSER, false, "").lock()))
	{
		browser->setResoultion(1280, 720);
		//TODO_SystemJson
		//url << "http://localhost:" << mNodeJsPluginConfig.serverPort << "/robotCalibration/public/";
		browser->setURL(url);
		browser->showOnOverlay(true, 0);
		if (auto mouseMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("mouseMaterial", ape::Entity::MATERIAL_MANUAL, false, "").lock()))
		{
			mouseMaterial->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
			mouseMaterial->setSceneBlending(ape::Material::SceneBlendingType::TRANSPARENT_ALPHA);
			//mouseMaterial->setLightingEnabled(false); TODO_SceneMakerMacro crash in OpenGL
			if (auto mouseTexture = std::static_pointer_cast<ape::IUnitTexture>(mpSceneManager->createEntity("mouseTexture", ape::Entity::TEXTURE_UNIT, false, "").lock()))
			{
				mouseTexture->setParameters(mouseMaterial, "browserpointer.png");
				mouseTexture->setTextureAddressingMode(ape::Texture::AddressingMode::CLAMP);
				mouseTexture->setTextureFiltering(ape::Texture::Filtering::POINT, ape::Texture::Filtering::LINEAR, ape::Texture::Filtering::F_NONE);
			}
			mouseMaterial->showOnOverlay(true, 1);
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::SceneMakerMacro::interpolate(ape::NodeWeakPtr node, ape::Vector3 position, ape::Quaternion orientation, unsigned int milliseconds)
{
	if (auto nodeSP = node.lock())
	{
#ifndef ANDROID
		auto moveInterpolator = std::make_unique<ape::Interpolator>(false);
#else
		std::unique_ptr<ape::Interpolator> moveInterpolator(new ape::Interpolator(false));
#endif
		moveInterpolator->addSection(
			nodeSP->getPosition(),
			position,
			milliseconds * 1000,
			[&](ape::Vector3 pos) { nodeSP->setPosition(pos); }
		);

#ifndef ANDROID
		auto rotateInterpolator = std::make_unique<ape::Interpolator>(false);
#else
		std::unique_ptr<ape::Interpolator> rotateInterpolator(new ape::Interpolator(false));
#endif
		rotateInterpolator->addSection(
			nodeSP->getOrientation(),
			orientation,
			milliseconds * 1000,
			[&](ape::Quaternion ori) { nodeSP->setOrientation(ori); }
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

void ape::SceneMakerMacro::eventCallBack(const ape::Event& event)
{
	
}


