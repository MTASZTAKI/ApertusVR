#include "ApeGyorPlugin.h"

Ape::ApeGyorPlugin::ApeGyorPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mUserNode = Ape::NodeWeakPtr();
	mStateText = Ape::EntityWeakPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeGyorPlugin::~ApeGyorPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		APE_LOG_TRACE("GEOMETRY_RAY_INTERSECTION");
		if (auto rayGeometry = std::static_pointer_cast<Ape::IRayGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			auto intersections = rayGeometry->getIntersections();
			for (auto intersection : intersections)
			{
				if (auto entity = intersection.lock())
				{
					std::string entityName = entity->getName();
					if (entityName == "stateText")
					{
						auto geometry = std::static_pointer_cast<Ape::Geometry>(entity);
						if (auto stateTextParentNode = geometry->getParentNode().lock())
						{
							// TODO
						}
						break;
					}
				}
			}
		}
	}
}

void Ape::ApeGyorPlugin::createSkyBox()
{
	APE_LOG_FUNC_ENTER();

	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
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
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.5f, 0.5f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.7f));
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, -1));
		light->setDiffuseColor(Ape::Color(0.5f, 0.5f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.7f));
	}
}

void Ape::ApeGyorPlugin::createTexts()
{
	if (auto userNode = mUserNode.lock())
	{
		if (auto stateTextNode = mpSceneManager->createNode("stateNode").lock())
		{
			stateTextNode->setParentNode(mUserNode);
			stateTextNode->setPosition(Ape::Vector3(0, 17, -50));

			mStateText = mpSceneManager->createEntity("stateText", Ape::Entity::GEOMETRY_TEXT);
			if (auto stateText = std::static_pointer_cast<Ape::ITextGeometry>(mStateText.lock()))
			{
				stateText->setCaption("stateText");
				stateText->showOnTop(true);
				stateText->setParentNode(stateTextNode);
			}
		}
	}
}

void Ape::ApeGyorPlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	if (auto userNode = mpSceneManager->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

	createSkyBox();
	createTexts();

	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		if (auto userNode = mUserNode.lock())
		{
			auto moveInterpolator = std::make_unique<Ape::Interpolator>(false);
			moveInterpolator->addSection(
				userNode->getPosition(),
				Ape::Vector3(459.301, 205.316, -75.8723),
				5.0,
				[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
			);
			auto rotateInterpolator = std::make_unique<Ape::Interpolator>(false);
			rotateInterpolator->addSection(
				userNode->getOrientation(),
				Ape::Quaternion(-0.593951, 0.0556969, -0.799066, -0.0749312),
				5.0,
				[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
			);
			while (!moveInterpolator->isQueueEmpty() && !rotateInterpolator->isQueueEmpty())
			{
				if (!moveInterpolator->isQueueEmpty())
					moveInterpolator->iterateTopSection();
				if (!rotateInterpolator->isQueueEmpty())
					rotateInterpolator->iterateTopSection();
			}
			auto moveInterpolator2 = std::make_unique<Ape::Interpolator>(false);
			moveInterpolator2->addSection(
				userNode->getPosition(),
				Ape::Vector3(-106.345, 235.75, -468.079),
				15.0,
				[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
			);
			auto rotateInterpolator2 = std::make_unique<Ape::Interpolator>(false);
			rotateInterpolator2->addSection(
				userNode->getOrientation(),
				Ape::Quaternion(0.0745079, -0.00571105, -0.994289, -0.0762119),
				15.0,
				[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
			);
			while (!moveInterpolator2->isQueueEmpty() && !rotateInterpolator2->isQueueEmpty())
			{
				if (!moveInterpolator2->isQueueEmpty())
					moveInterpolator2->iterateTopSection();
				if (!rotateInterpolator2->isQueueEmpty())
					rotateInterpolator2->iterateTopSection();
			}
			auto moveInterpolator3 = std::make_unique<Ape::Interpolator>(false);
			moveInterpolator3->addSection(
				userNode->getPosition(),
				Ape::Vector3(-533.586, 232.092, -66.402),
				15.0,
				[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
			);
			auto rotateInterpolator3 = std::make_unique<Ape::Interpolator>(false);
			rotateInterpolator3->addSection(
				userNode->getOrientation(),
				Ape::Quaternion(0.543329, -0.041646, -0.836036, -0.0640818),
				15.0,
				[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
			);
			while (!moveInterpolator3->isQueueEmpty() && !rotateInterpolator3->isQueueEmpty())
			{
				if (!moveInterpolator3->isQueueEmpty())
					moveInterpolator3->iterateTopSection();
				if (!rotateInterpolator3->isQueueEmpty())
					rotateInterpolator3->iterateTopSection();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
