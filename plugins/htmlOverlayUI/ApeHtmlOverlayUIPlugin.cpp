#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "ApeHtmlOverlayUIPlugin.h"

Ape::ApeHtmlOverlayUIPlugin::ApeHtmlOverlayUIPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtmlOverlayUIPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mPointCloud = Ape::PointCloudWeakPtr();
	mUserNode = Ape::NodeWeakPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeHtmlOverlayUIPlugin::~ApeHtmlOverlayUIPlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::eventCallBack(const Ape::Event& event)
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::parseNodeJsConfig()
{
	LOG_FUNC_ENTER();
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
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::createOverlayBrowser()
{
	LOG_FUNC_ENTER();
	parseNodeJsConfig();
	if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("overlay_frame", Ape::Entity::BROWSER).lock()))
	{
		browser->setResoultion(1280, 720);
		std::stringstream url;
		url << "http://localhost:" << mNodeJsPluginConfig.serverPort << "/htmlOverlayUI/public/";
		browser->setURL(url.str());
		browser->showOnOverlay(true, 0);

		if (auto mouseMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("mouseMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			mouseMaterial->setEmissiveColor(Ape::Color(1.0f, 1.0f, 1.0f));
			mouseMaterial->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
			//mouseMaterial->setLightingEnabled(false); crash in OpenGL
			if (auto mouseTexture = std::static_pointer_cast<Ape::IUnitTexture>(mpScene->createEntity("mouseTexture", Ape::Entity::TEXTURE_UNIT).lock()))
			{
				mouseTexture->setParameters(mouseMaterial, "browserpointer.png");
				mouseTexture->setTextureAddressingMode(Ape::Texture::AddressingMode::CLAMP);
				mouseTexture->setTextureFiltering(Ape::Texture::Filtering::POINT, Ape::Texture::Filtering::LINEAR, Ape::Texture::Filtering::F_NONE);
			}
			mouseMaterial->showOnOverlay(true, 1);
		}
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Init()
{
	LOG_FUNC_ENTER();
	createOverlayBrowser();
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Run()
{
	LOG_FUNC_ENTER();
	double duration = 0;
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtmlOverlayUIPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Step()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Stop()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Suspend()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Restart()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}
