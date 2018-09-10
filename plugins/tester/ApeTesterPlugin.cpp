#include "ApeTesterPlugin.h"

Ape::ApeTesterPlugin::ApeTesterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeTesterPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mInterpolators = std::vector<std::unique_ptr<Ape::Interpolator>>();
	mDemoObjectNode = Ape::NodeWeakPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeTesterPlugin::~ApeTesterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeTesterPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeTesterPlugin::createSkyBox()
{
	APE_LOG_FUNC_ENTER();
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::createLight()
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

void Ape::ApeTesterPlugin::createPlane()
{
	APE_LOG_FUNC_ENTER();
	if (auto planeNode = mpSceneManager->createNode("planeNode").lock())
	{
		planeNode->setPosition(Ape::Vector3(0, -10, 0));
		if (auto plane = std::static_pointer_cast<Ape::IPlaneGeometry>(mpSceneManager->createEntity("plane", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			plane->setParameters(Ape::Vector2(1, 1), Ape::Vector2(1000, 1000), Ape::Vector2(1, 1));
			plane->setParentNode(planeNode);
			if (auto planeMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("planeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
			{
				if (auto planeMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("planeMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
				{
					planeMaterialManualPass->setShininess(15.0f);
					planeMaterialManualPass->setDiffuseColor(Ape::Color(0.29f, 0.266f, 0.29f));
					planeMaterialManualPass->setSpecularColor(Ape::Color(0.29f, 0.266f, 0.29f));
					planeMaterial->setPass(planeMaterialManualPass);
					plane->setMaterial(planeMaterial);
				}
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::createDemoObj()
{
	APE_LOG_FUNC_ENTER();
	std::shared_ptr<Ape::IManualMaterial> demoObjectMaterial;
	if (demoObjectMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("demoObjectMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto demoObjectMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("demoObjectMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
		{
			demoObjectMaterialManualPass->setShininess(15.0f);
			demoObjectMaterialManualPass->setDiffuseColor(Ape::Color(1.0f, 0.0f, 0.0f));
			demoObjectMaterialManualPass->setSpecularColor(Ape::Color(1.0f, 0.0f, 0.0f));
			demoObjectMaterial->setPass(demoObjectMaterialManualPass);
		}
	}
	mDemoObjectNode = mpSceneManager->createNode("mDemoObjectNode");
	if (auto demoObjectNode = mDemoObjectNode.lock())
	{
		demoObjectNode->setPosition(Ape::Vector3(10, 10, 10));
		if (auto demoBox = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity("demoBox", Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
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
			demoBox->setParameters("", coordinates, indices, Ape::GeometryNormals(), true, Ape::GeometryColors(), Ape::GeometryTextureCoordinates(), demoObjectMaterial);
			demoBox->setParentNode(mDemoObjectNode);
		}
		if (auto demoPyramid = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity("demoPyramid", Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
		{
			Ape::GeometryCoordinates coordinates = {
				-10, 20, 10,
				10, 20,  10,
				10, 20, -10,
				-10, 20, -10,
				0, 40, 0
			};
			Ape::GeometryIndices indices = {
				0, 3, 2, 1, -1,
				0, 1, 4, -1,
				1, 2, 4, -1,
				2, 3, 4, -1,
				3, 0, 4, -1
			};
			demoPyramid->setParameters("", coordinates, indices, Ape::GeometryNormals(), true, Ape::GeometryColors(), Ape::GeometryTextureCoordinates(), demoObjectMaterial);
			demoPyramid->setParentNode(mDemoObjectNode);
		}
		if (auto demoLine = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity("demoLine", Ape::Entity::GEOMETRY_INDEXEDLINESET).lock()))
		{
			Ape::GeometryCoordinates coordinates = {
				10, 20, 10,
				10, 10, 10,
				10, 10, -10,
				10, 20, -10,
				-10, 20, 10,
				-10, 10, 10,
				-10, 10, -10,
				-10, 20, -10
			};
			Ape::GeometryIndices indices = {
				0, 1, -1,
				1, 2, -1,
				2, 3, -1,
				3, 0, -1,
				0, 4, -1,
				4, 5, -1,
				5, 1, -1,
				5, 6, -1,
				6, 7, -1,
				7, 4, -1,
				3, 7, -1,
				2, 6, -1
			};
			Ape::Color color(1, 0, 0);
			demoLine->setParameters(coordinates, indices, color);
			demoLine->setParentNode(mDemoObjectNode);
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::createDemoObjAnim()
{
	APE_LOG_FUNC_ENTER();
	auto moveInterpolator = std::make_unique<Ape::Interpolator>(true);
	moveInterpolator->addSection(
		Ape::Vector3(10, 10, 0),
		Ape::Vector3(10, 10, 100),
		10.0,
		[&](Ape::Vector3 pos) {
			if (auto demoObjectNode = mDemoObjectNode.lock())
			{
				demoObjectNode->setPosition(pos);
			}
		}
	);
	moveInterpolator->addSection(
		Ape::Vector3(10, 10, 100),
		Ape::Vector3(10, 10, 0),
		10.0,
		[&](Ape::Vector3 pos) {
			if (auto demoObjectNode = mDemoObjectNode.lock())
			{
				demoObjectNode->setPosition(pos);
			}
		}
	);
	mInterpolators.push_back(std::move(moveInterpolator));

	auto rotateInterpolator = std::make_unique<Ape::Interpolator>(true);
	rotateInterpolator->addSection(
		Ape::Quaternion(1, 0, 0, 0),
		Ape::Quaternion(0.7071, 0, 0.7071, 0),
		10.0,
		[&](Ape::Quaternion ori) {
			if (auto demoObjectNode = mDemoObjectNode.lock())
			{
				demoObjectNode->setOrientation(ori);
			}
		}
	);
	rotateInterpolator->addSection(
		Ape::Quaternion(0.7071, 0, 0.7071, 0),
		Ape::Quaternion(1, 0, 0, 0),
		10.0,
		[&](Ape::Quaternion ori) {
			if (auto demoObjectNode = mDemoObjectNode.lock())
			{
				demoObjectNode->setOrientation(ori);
			}
		}
	);
	mInterpolators.push_back(std::move(rotateInterpolator));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::createCoordinateSystem()
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
					if (auto coordinateSystemXText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemXText", Ape::Entity::GEOMETRY_TEXT).lock()))
					{
						coordinateSystemXText->setCaption("X");
						coordinateSystemXText->setParentNode(coordinateSystemArrowXConeNode);
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
					if (auto coordinateSystemYText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemYText", Ape::Entity::GEOMETRY_TEXT).lock()))
					{
						coordinateSystemYText->setCaption("Y");
						coordinateSystemYText->setParentNode(coordinateSystemArrowYConeNode);
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
					if (auto coordinateSystemZText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("coordinateSystemZText", Ape::Entity::GEOMETRY_TEXT).lock()))
					{
						coordinateSystemZText->setCaption("Z");
						coordinateSystemZText->setParentNode(coordinateSystemArrowZConeNode);
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

void Ape::ApeTesterPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	createSkyBox();
	createLight();
	createPlane();
	createDemoObj();
	createDemoObjAnim();
	createCoordinateSystem();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{	
		if (!mInterpolators.empty())
		{
			for (std::vector<std::unique_ptr<Ape::Interpolator>>::iterator it = mInterpolators.begin(); it != mInterpolators.end();)
			{
				(*it)->iterateTopSection();
				if ((*it)->isQueueEmpty())
					it = mInterpolators.erase(it);
				else 
					it++;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
