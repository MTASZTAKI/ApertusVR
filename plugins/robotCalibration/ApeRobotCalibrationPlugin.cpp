#include <fstream>
#include "ApeRobotCalibrationPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

Ape::ApeRobotCalibrationPlugin::ApeRobotCalibrationPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeRobotCalibrationPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mInterpolators = std::vector<std::unique_ptr<Ape::Interpolator>>();
	mPointCloud = Ape::PointCloudWeakPtr();
	mUserNode = Ape::NodeWeakPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeRobotCalibrationPlugin::~ApeRobotCalibrationPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeRobotCalibrationPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::parseNodeJsConfig()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "/ApeNodeJsPlugin.json";
	FILE* configFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (configFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(configFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& httpServer = jsonDocument["httpServer"];
			if (httpServer.IsObject())
			{
				rapidjson::Value& port = httpServer["port"];
				if (port.IsNumber())
				{
					mNodeJsPluginConfig.serverPort = port.GetInt();
				}
			}
		}
		fclose(configFile);
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::createOverlayBrowser()
{
	APE_LOG_FUNC_ENTER();
	parseNodeJsConfig();
	if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->createEntity("overlay_frame", Ape::Entity::BROWSER).lock()))
	{
		browser->setResoultion(1280, 720);
		std::stringstream url;
		url << "http://localhost:" << mNodeJsPluginConfig.serverPort << "/robotCalibration/public/";
		browser->setURL(url.str());
		browser->showOnOverlay(true, 0);
		if (auto mouseMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("mouseMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			mouseMaterial->setEmissiveColor(Ape::Color(1.0f, 1.0f, 1.0f));
			mouseMaterial->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
			//mouseMaterial->setLightingEnabled(false); crash in OpenGL
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

void Ape::ApeRobotCalibrationPlugin::createLights()
{
	APE_LOG_FUNC_ENTER();
	if (auto userNode = mpSceneManager->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		mUserNode = userNode;
		if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light", Ape::Entity::LIGHT).lock()))
		{
			light->setLightType(Ape::Light::Type::POINT);
			light->setLightDirection(Ape::Vector3(0, 0, 0));
			light->setDiffuseColor(Ape::Color(0.35f, 0.35f, 0.35f));
			light->setSpecularColor(Ape::Color(0.35f, 0.35f, 0.35f));
			light->setParentNode(userNode);
		}
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

void Ape::ApeRobotCalibrationPlugin::createSkyBox()
{
	APE_LOG_FUNC_ENTER();
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::createCoordinateSystem()
{
	APE_LOG_FUNC_ENTER();
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowXMaterial;
	if (coordinateSystemArrowXMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("coordinateSystemArrowXMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto coordinateSystemArrowXMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("coordinateSystemArrowXMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
		{
			coordinateSystemArrowXMaterialManualPass->setShininess(15.0f);
			coordinateSystemArrowXMaterialManualPass->setDiffuseColor(Ape::Color(1.0f, 0.0f, 0.0f));
			coordinateSystemArrowXMaterialManualPass->setSpecularColor(Ape::Color(1.0f, 0.0f, 0.0f));
			coordinateSystemArrowXMaterial->setPass(coordinateSystemArrowXMaterialManualPass);
		}
	}
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowYMaterial;
	if (coordinateSystemArrowYMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("coordinateSystemArrowYMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto coordinateSystemArrowYMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("coordinateSystemArrowYMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
		{
			coordinateSystemArrowYMaterialManualPass->setShininess(15.0f);
			coordinateSystemArrowYMaterialManualPass->setDiffuseColor(Ape::Color(0.0f, 1.0f, 0.0f));
			coordinateSystemArrowYMaterialManualPass->setSpecularColor(Ape::Color(0.0f, 1.0f, 0.0f));
			coordinateSystemArrowYMaterial->setPass(coordinateSystemArrowYMaterialManualPass);
		}
	}
	std::shared_ptr<Ape::IManualMaterial> coordinateSystemArrowZMaterial;
	if (coordinateSystemArrowZMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("coordinateSystemArrowZMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto coordinateSystemArrowZMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("coordinateSystemArrowZMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
		{
			coordinateSystemArrowZMaterialManualPass->setShininess(15.0f);
			coordinateSystemArrowZMaterialManualPass->setDiffuseColor(Ape::Color(0.0f, 0.0f, 1.0f));
			coordinateSystemArrowZMaterialManualPass->setSpecularColor(Ape::Color(0.0f, 0.0f, 1.0f));
			coordinateSystemArrowZMaterial->setPass(coordinateSystemArrowZMaterialManualPass);
		}
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
					if (auto coordinateSystemXTextNode = mpSceneManager->createNode("coordinateSystemXTextNode").lock())
					{
						coordinateSystemXTextNode->setParentNode(coordinateSystemArrowXConeNode);
						coordinateSystemXTextNode->setPosition(Ape::Vector3(0, 0, 5));
						if (auto coordinateSystemXText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemXText", Ape::Entity::GEOMETRY_TEXT).lock()))
						{
							coordinateSystemXText->setCaption("X");
							coordinateSystemXText->setParentNode(coordinateSystemXTextNode);
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
					if (auto coordinateSystemYTextNode = mpSceneManager->createNode("coordinateSystemYTextNode").lock())
					{
						coordinateSystemYTextNode->setParentNode(coordinateSystemArrowYConeNode);
						coordinateSystemYTextNode->setPosition(Ape::Vector3(0, 0, 5));
						if (auto coordinateSystemYText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemYText", Ape::Entity::GEOMETRY_TEXT).lock()))
						{
							coordinateSystemYText->setCaption("Y");
							coordinateSystemYText->setParentNode(coordinateSystemYTextNode);
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
					if (auto coordinateSystemZTextNode = mpSceneManager->createNode("coordinateSystemZTextNode").lock())
					{
						coordinateSystemZTextNode->setParentNode(coordinateSystemArrowZConeNode);
						coordinateSystemZTextNode->setPosition(Ape::Vector3(5, 0, 0));
						if (auto coordinateSystemXText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemZText", Ape::Entity::GEOMETRY_TEXT).lock()))
						{
							coordinateSystemXText->setCaption("Z");
							coordinateSystemXText->setParentNode(coordinateSystemZTextNode);
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
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeRobotCalibrationPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	createOverlayBrowser();
	createSkyBox();
	createLights();
	createCoordinateSystem();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
