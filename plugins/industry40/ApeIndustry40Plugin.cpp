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
	//mpSceneMakerMacro->makeGround("plane",ape::Vector2(3000,3000));

	

	//mpSceneMakerMacro->makeBox("a1");

	 // create box geometry
	if (auto boxNode = mpSceneManager->createNode("boxNode").lock())
	{
		// material for box
		std::shared_ptr<ape::IManualMaterial> boxMaterial;
		if (boxMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("boxMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			boxMaterial->setDiffuseColor(ape::Color(0.1f, 0.1f, 0.1f));
			boxMaterial->setSpecularColor(ape::Color(0.5f, 0.5f, 0.5f));
		}



		boxNode->setPosition(ape::Vector3(0, -2500.f, 0));

		// oordinateSystemArrowXTubeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
		ape::BoxGeometrySharedPtr box;
		if (box = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity("box", ape::Entity::GEOMETRY_BOX).lock()))
		{
			box->setParameters(ape::Vector3(5000, 5000,5000));
			box->setParentNode(boxNode);
			box->setMaterial(boxMaterial);
		}

		if (auto boxBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("boxbody", ape::Entity::RIGIDBODY).lock()))
		{
			boxBody->setToStatic();
			boxBody->setParentNode(boxNode);
			boxBody->setGeometry(box);
			boxBody->setRestitution(0.6f);
		}
		
	}

	// create sphere geometry
	/*if (auto sphereNode = mpSceneManager->createNode("sphereNode").lock())
	{
		// material for sphere 
		std::shared_ptr<ape::IManualMaterial> sphereMaterial;
		if (sphereMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("sphere1111Material", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			sphereMaterial->setDiffuseColor(ape::Color(0.0f, 0.5f, 0.5f));
			sphereMaterial->setSpecularColor(ape::Color(0.0f, 0.5f, 0.5f));
		}



		sphereNode->setPosition(ape::Vector3(0.f, 200.f, 0.f));
		if (auto sphere = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("sphere1111", ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			sphere->setParameters(15.f, ape::Vector2(1, 1));
			sphere->setParentNode(sphereNode);
			sphere->setMaterial(sphereMaterial);

			if (auto sphereBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("sphere1111Body", ape::Entity::RIGIDBODY).lock()))
			{
				sphereBody->setGeometry(sphere);
				sphereBody->setParentNode(sphereNode);
				sphereBody->setType(ape::RigidBodyType::DYNAMIC);
				sphereBody->setMass(1.0f);
				sphereBody->setRestitution(0.8f);
			}
		}
	}*/

	
	
	std::vector< RigidBodyWeakPtr> bodies;
	const int array_size = 6;
	for (int i = 0; i < array_size; i++)
	{
		std::stringstream ssi;
		ssi << i;
		for (int j = 0; j < 1; j++)
		{
			std::stringstream ssj;
			ssj << j;
			for (int k = 0; k < array_size; k++)
			{
				std::stringstream ssk;
				ssk << k;

				if (auto boxNode = mpSceneManager->createNode("boxNode" + ssi.str() + ssj.str() + ssk.str()).lock())
				{
					// material for box
					std::shared_ptr<ape::IManualMaterial> boxMaterial;
					if (boxMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("boxMaterial" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::MATERIAL_MANUAL).lock()))
					{
						boxMaterial->setDiffuseColor(ape::Color(float(i+1) / float(array_size), float(j+1) / float(array_size), float(k+1) / float(array_size)));
						boxMaterial->setSpecularColor(ape::Color(float(i) / float(array_size-1), float(j) / float(array_size-1), float(k) / float(array_size-1)));
					}


					boxNode->setPosition(ape::Vector3(float(i * 10 - array_size*10/2 +5), float(j * 10 )+ 1000, float(k * 10 - array_size * 10 / 2 + 5)));

					// oordinateSystemArrowXTubeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
					if (auto box = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity("box" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::GEOMETRY_BOX).lock()))
					{
						box->setParameters(ape::Vector3(10,10,10));
						box->setParentNode(boxNode);
						box->setMaterial(boxMaterial);


						if (auto boxBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("boxbody" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::RIGIDBODY).lock()))
						{	
							boxBody->setToStatic();
							boxBody->setGeometry(box);
							boxBody->setParentNode(boxNode);
							boxBody->setRestitution(0.8f);
							boxBody->setDamping(0.2, 0.1);
							
							bodies.push_back(boxBody);
						}
					}

				}
			}
		}
	}
	
	// cone
	//if (auto coneNode = mpSceneManager->createNode("coneNode").lock())
	//{
	//	// material for cone
	//	std::shared_ptr<ape::IManualMaterial> coneMaterial;
	//	if (coneMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("coneMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
	//	{
	//		coneMaterial->setDiffuseColor(ape::Color(.5, .5, .5));
	//		coneMaterial->setSpecularColor(ape::Color(.2, .5, .2));
	//	}


	//	coneNode->setPosition(ape::Vector3(0,400, 0));

	//	// oordinateSystemArrowXTubeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
	//	if (auto cone = std::static_pointer_cast<ape::IConeGeometry>(mpSceneManager->createEntity("cone", ape::Entity::GEOMETRY_CONE).lock()))
	//	{
	//		cone->setParameters(35,30,1,ape::Vector2(1,1));
	//		
	//		cone->setParentNode(coneNode);
	//		cone->setMaterial(coneMaterial);


	//		if (auto coneBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("conebody", ape::Entity::RIGIDBODY).lock()))
	//		{
	//			coneBody->setToStatic();
	//			coneBody->setGeometry(cone);
	//			coneBody->setParentNode(coneNode);
	//			coneBody->setRestitution(0.8f);
	//			coneBody->setDamping(0.1, 0.1);
	//			//coneBody->setFriction(0.5, 0.3, 0.3);

	//			bodies.push_back(coneBody);
	//		}
	//	}

	//}

	//cylinder
	//if (auto cylinderNode = mpSceneManager->createNode("cylinderNode").lock())
	//{
	//	// material for cylinder
	//	std::shared_ptr<ape::IManualMaterial> cylinderMaterial;
	//	if (cylinderMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("cylinderMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
	//	{
	//		cylinderMaterial->setDiffuseColor(ape::Color(.5, .5, .5));
	//		cylinderMaterial->setSpecularColor(ape::Color(.2, .5, .2));
	//	}


	//	cylinderNode->setPosition(ape::Vector3(0, 200, 0));
	//	cylinderNode->setOrientation(ape::Quaternion(ape::Degree(90.f), ape::Vector3(1, 0, 0)));
	//	// coordinateSystemArrowXTubeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
	//	if (auto cylinder = std::static_pointer_cast<ape::ICylinderGeometry>(mpSceneManager->createEntity("cylinder", ape::Entity::GEOMETRY_CYLINDER).lock()))
	//	{
	//		
	//		cylinder->setParameters(10.f,20.f, 1);

	//		cylinder->setParentNode(cylinderNode);
	//		cylinder->setMaterial(cylinderMaterial);


	//		if (auto cylinderBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("cylinderbody", ape::Entity::RIGIDBODY).lock()))
	//		{
	//			cylinderBody->setToStatic();
	//			cylinderBody->setGeometry(cylinder);
	//			cylinderBody->setParentNode(cylinderNode);
	//			cylinderBody->setRestitution(0.0f);
	//			cylinderBody->setDamping(0.5, 0.7);
	//			cylinderBody->setFriction(0.4, 0.5, 0.5);

	//			bodies.push_back(cylinderBody);
	//		}
	//	}

	//}

	for (size_t i = 0; i < bodies.size(); i++)
	{
		if (auto boxBody = bodies[i].lock())
		{
			boxBody->setToDynamic(0.3f);
		}
	}

	// bodies[bodies.size() - 1].lock()->setToDynamic(1.2f);

	
	
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
