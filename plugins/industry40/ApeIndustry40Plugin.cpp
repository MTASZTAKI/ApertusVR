#include "apeIndustry40Plugin.h"

ape::apeIndustry40Plugin::apeIndustry40Plugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apeIndustry40Plugin::~apeIndustry40Plugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeIndustry40Plugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeIndustry40Plugin::Init()
{
	APE_LOG_FUNC_ENTER();
	//mpSceneMakerMacro->makeLit();
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light", ape::Entity::LIGHT).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(1, -1, 0));
		light->setDiffuseColor(ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(ape::Color(0.6f, 0.6f, 0.6f));
	}
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light2", ape::Entity::LIGHT).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(0, -1, 1));
		light->setDiffuseColor(ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(ape::Color(0.6f, 0.6f, 0.6f));
	}
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeCoordinateSystem();
	mpSceneMakerMacro->makeGround();
	//mpSceneMakerMacro->makeBox("a1");

	// create box geometry
	if (auto boxNode = mpSceneManager->createNode("boxNode").lock())
	{
		// material for box
		std::shared_ptr<ape::IManualMaterial> boxMaterial;
		if (boxMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("boxMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			boxMaterial->setDiffuseColor(ape::Color(0.5f, 0.5f, 0.0f));
			boxMaterial->setSpecularColor(ape::Color(0.5f, 0.5f, 0.0f));
		}



		boxNode->setPosition(ape::Vector3(0, 2000, 0));
		// oordinateSystemArrowXTubeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
		if (auto box = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity("box", ape::Entity::GEOMETRY_BOX).lock()))
		{
			box->setParameters(ape::Vector3(10, 10, 10));
			box->setParentNode(boxNode);
			box->setMaterial(boxMaterial);
		}
	}

	// create sphere geometry
	if (auto sphereNode = mpSceneManager->createNode("sphereNode").lock())
	{
		// material for sphere 
		std::shared_ptr<ape::IManualMaterial> sphereMaterial;
		if (sphereMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("sphere1111Material", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			sphereMaterial->setDiffuseColor(ape::Color(0.0f, 0.5f, 0.5f));
			sphereMaterial->setSpecularColor(ape::Color(0.0f, 0.5f, 0.5f));
		}



		sphereNode->setPosition(ape::Vector3(10, 150, 0));
		if (auto sphere = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("sphere1111", ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			sphere->setParameters(10.0f, ape::Vector2(1, 1));
			sphere->setParentNode(sphereNode);
			sphere->setMaterial(sphereMaterial);
		}
	}

	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeIndustry40Plugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
