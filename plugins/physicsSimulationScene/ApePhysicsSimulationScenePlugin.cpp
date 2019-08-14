#include "ApePhysicsSimulationScenePlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <random>

ape::apePhysicsSimulationScenePlugin::apePhysicsSimulationScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apePhysicsSimulationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apePhysicsSimulationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	m_groundIsWavy = false;
	APE_LOG_FUNC_LEAVE();
}

ape::apePhysicsSimulationScenePlugin::~apePhysicsSimulationScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&apePhysicsSimulationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apePhysicsSimulationScenePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhysicsSimulationScenePlugin::eventCallBack(const ape::Event& event)
{
}

void ape::apePhysicsSimulationScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apePhysicsSimulationScenePlugin.json";

	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();

	FILE* apePhysicsSceneConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apePhysicsSceneConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apePhysicsSceneConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			if (jsonDocument.HasMember("environment"))
			{
				rapidjson::Value& input = jsonDocument["environment"];
				std::string environment = input.GetString();
				m_waterEnabled = environment == "water";
				m_groundIsWavy = environment == "terrain";
			}
		}
	}

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
	//mpSceneMakerMacro->makeTerrain("terrain");
	//mpSceneMakerMacro->makeBox("a1");

	// water surface

	if (true)
	{
		if (m_waterEnabled)
		{
			makeWater("water", ape::Vector2(3000, 3000), ape::Vector3(0, 300, 0));
		}
		else if (m_groundIsWavy)
		{
			makeTerrain("terrain", ape::Vector3(1, 1, 1));
		}
		else
		{
			makeGround("ground", ape::Vector2(3000, 3000));
		}
	}


	 //makeSphere("sphere11", 25.f, {+50.,300., -300.});

	// makeBox("box111", ape::Vector3(50, 50, 50), ape::Vector3(-100.f, 400.f, 0.f));

	const int array_size = 10;
	ape::Vector3 initPos(50, 400, -200);
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
						boxMaterial->setDiffuseColor(ape::Color(float(i + 1) / float(array_size), float(j + 1) / float(array_size), float(k + 1) / float(array_size)));
						boxMaterial->setSpecularColor(ape::Color(float(i) / float(array_size - 1), float(j) / float(array_size - 1), float(k) / float(array_size - 1)));
					}


					boxNode->setPosition(initPos + ape::Vector3(float(i * 10 - array_size * 10 / 2 + 5), float(j * 10), float(k * 10 - array_size * 10 / 2 + 5)));

					// oordinateSystemArrowXTubeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
					if (auto box = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity("box" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::GEOMETRY_BOX).lock()))
					{
						box->setParameters(ape::Vector3(10, 10, 10));
						box->setParentNode(boxNode);
						box->setMaterial(boxMaterial);


						if (auto boxBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("boxbody" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::RIGIDBODY).lock()))
						{
							boxBody->setToStatic();
							boxBody->setGeometry(box);
							boxBody->setParentNode(boxNode);
							boxBody->setRestitution(0.6f);
							boxBody->setDamping(0.01, 0.01);
							boxBody->setBouyancy(m_waterEnabled);
							boxBody->setToDynamic(1.0f);

							m_bodies.push_back(boxBody);
						}
					}
				}
			}
		}
	}

	APE_LOG_FUNC_LEAVE();
}

void ape::apePhysicsSimulationScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhysicsSimulationScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhysicsSimulationScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhysicsSimulationScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhysicsSimulationScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhysicsSimulationScenePlugin::makeTerrain(std::string name, ape::Vector3 scale)
{
	if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL).lock()))
	{
		material->setDiffuseColor(ape::Color(0.5f, 0.5f, 0.5f));
		material->setSpecularColor(ape::Color(0.5f, 0.5f, 0.5f));
		//material->setCullingMode(ape::Material::CullingMode::NONE_CM);

		if (auto node = mpSceneManager->createNode(name + "Node").lock())
		{
			node->setPosition(ape::Vector3(0, 0, 0));
			node->setScale(scale);

			if (auto terrain = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				int i;
				int j;

				const int NUM_VERTS_X = 50;
				const int NUM_VERTS_Y = 50;
				const int totalVerts = NUM_VERTS_X * NUM_VERTS_Y + 2 * (NUM_VERTS_X + NUM_VERTS_Y) - 4;
				const int totalTriangles = 2 * (NUM_VERTS_X - 1) * (NUM_VERTS_Y - 1) + 2 * (NUM_VERTS_X + NUM_VERTS_Y) - 4;
				const float TRIANGLE_SIZE = 30.f;

				float offset = -50;
				const float waveheight = 30.f;


				ape::GeometryCoordinates coordinates;
				ape::GeometryIndices indices;

				coordinates.resize(totalVerts * 3);
				indices.resize(totalTriangles * 4);

				int index = 0;
				for (j = 0; j < NUM_VERTS_Y; j++)
				{
					for (i = 0; i < NUM_VERTS_X; i++)
					{
						float randOffset;
						coordinates[index++] = (i - NUM_VERTS_X * 0.5f) * TRIANGLE_SIZE;
						coordinates[index++] = waveheight * sinf((float)i) * cosf((float)j);
						coordinates[index++] = (j - NUM_VERTS_Y * 0.5f) * TRIANGLE_SIZE;
					}
				}

				index = 0;
				for (j = 0; j < NUM_VERTS_Y - 1; j++)
				{
					for (i = 0; i < NUM_VERTS_X - 1; i++)
					{
						indices[index++] = i + j * NUM_VERTS_Y;
						indices[index++] = i + (j + 1) * NUM_VERTS_Y;
						indices[index++] = (i + 1) + j * NUM_VERTS_Y;
						indices[index++] = -1;

						indices[index++] = (i + 1) + j * NUM_VERTS_Y;
						indices[index++] = i + (j + 1) * NUM_VERTS_Y;
						indices[index++] = (i + 1) + (j + 1) * NUM_VERTS_Y;
						indices[index++] = -1;
					}
				}


				terrain->setParameters("", coordinates, indices, ape::GeometryNormals(), true, ape::GeometryColors(), ape::GeometryTextureCoordinates(), material);
				terrain->setParentNode(node);

				if (auto terrainBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY).lock()))
				{
					terrainBody->setToStatic();
					terrainBody->setParentNode(node);
					terrainBody->setGeometry(terrain);
					terrainBody->setRestitution(0.6f);
					terrainBody->setFriction(0.5, 0.3, 0.3);

				}
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeGround(std::string name, ape::Vector2 size)
{
	if (auto planeNode = mpSceneManager->createNode(name + "Node").lock())
	{

		planeNode->setPosition(ape::Vector3(0, -30, 0));
		if (auto planeMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			planeMaterial->setDiffuseColor(ape::Color(0.1f, 0.1f, 0.1f));
			planeMaterial->setSpecularColor(ape::Color(0.3f, 0.3f, 0.2f));
			planeMaterial->setCullingMode(ape::Material::CullingMode::NONE_CM);
			if (auto plane = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_PLANE).lock()))
			{
				plane->setParameters(ape::Vector2(1, 1), size, ape::Vector2(1, 1));
				plane->setParentNode(planeNode);
				plane->setMaterial(planeMaterial);


				if (auto planeBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY).lock()))
				{
					planeBody->setGeometry(plane);
					planeBody->setParentNode(planeNode);
					planeBody->setToStatic();
					planeBody->setRestitution(1.0f);
				}
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeWater(std::string name, ape::Vector2 size, ape::Vector3 pos)
{
	if (auto planeNode = mpSceneManager->createNode(name + "Node").lock())
	{

		planeNode->setPosition(pos);
		if (auto planeMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			planeMaterial->setDiffuseColor(ape::Color(0.0f, 0.0f, 0.1f, 0.7f));
			planeMaterial->setSpecularColor(ape::Color(0.0f, 0.0f, 0.1f, 0.7f));
			planeMaterial->setCullingMode(ape::Material::CullingMode::NONE_CM);

			if (auto plane = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_PLANE).lock()))
			{
				plane->setParameters(ape::Vector2(1, 1), ape::Vector2(3000, 3000), ape::Vector2(1, 1));
				plane->setParentNode(planeNode);
				plane->setMaterial(planeMaterial);
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeBox(std::string name, ape::Vector3 dims, ape::Vector3 pos)
{
	// create box geometry
	if (auto boxNode = mpSceneManager->createNode("boxNode").lock())
	{
		// material for box
		std::shared_ptr<ape::IManualMaterial> boxMaterial;
		if (boxMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("boxMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			boxMaterial->setDiffuseColor(ape::Color(0.0f, 0.5f, 0.5f));
			boxMaterial->setSpecularColor(ape::Color(0.0f, 0.5f, 0.5f));
		}

		boxNode->setPosition(pos);

		ape::BoxGeometrySharedPtr box;
		if (box = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity("box", ape::Entity::GEOMETRY_BOX).lock()))
		{
			box->setParameters(dims);
			box->setParentNode(boxNode);
			box->setMaterial(boxMaterial);
		}

		if (auto boxBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("boxbody", ape::Entity::RIGIDBODY).lock()))
		{
			boxBody->setToDynamic(1.0f);
			boxBody->setParentNode(boxNode);
			boxBody->setGeometry(box);
			boxBody->setRestitution(0.8f);
			boxBody->setFriction(0.5, 0.3, 0.3);
			boxBody->setBouyancy(m_waterEnabled);
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeSphere(std::string name, float radius, ape::Vector3 pos)
{
	if (auto sphereNode = mpSceneManager->createNode(name + "Node").lock())
	{
		// material for sphere 
		std::shared_ptr<ape::IManualMaterial> sphereMaterial;
		if (sphereMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			sphereMaterial->setDiffuseColor(ape::Color(0.0f, 0.5f, 0.5f));
			sphereMaterial->setSpecularColor(ape::Color(0.0f, 0.5f, 0.5f));
		}

		sphereNode->setPosition(pos);
		if (auto sphere = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			sphere->setParameters(radius, ape::Vector2(1, 1));
			sphere->setParentNode(sphereNode);
			sphere->setMaterial(sphereMaterial);

			if (auto sphereBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY).lock()))
			{
				sphereBody->setGeometry(sphere);
				sphereBody->setParentNode(sphereNode);
				sphereBody->setToDynamic(1.0f);
				sphereBody->setRestitution(1.2f);
				sphereBody->setFriction(0.5, 0.3, 0.3);
				sphereBody->setBouyancy(m_waterEnabled);
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeCone(std::string name, float radius, float height, ape::Vector3 pos)
{
	if (auto coneNode = mpSceneManager->createNode("coneNode").lock())
	{
		// material for cone
		std::shared_ptr<ape::IManualMaterial> coneMaterial;
		if (coneMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("coneMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			coneMaterial->setDiffuseColor(ape::Color(.5, .5, .5));
			coneMaterial->setSpecularColor(ape::Color(.2, .5, .2));
		}

		coneNode->setPosition(pos);

		if (auto cone = std::static_pointer_cast<ape::IConeGeometry>(mpSceneManager->createEntity("cone", ape::Entity::GEOMETRY_CONE).lock()))
		{
			cone->setParameters(radius,height,1,ape::Vector2(1,1));
				
			cone->setParentNode(coneNode);
			cone->setMaterial(coneMaterial);


			if (auto coneBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("conebody", ape::Entity::RIGIDBODY).lock()))
			{
				coneBody->setToDynamic(1.0f);
				coneBody->setGeometry(cone);
				coneBody->setParentNode(coneNode);
				coneBody->setRestitution(0.8f);
				coneBody->setDamping(0.1, 0.1);
				//coneBody->setFriction(0.5, 0.3, 0.3);
				coneBody->setBouyancy(m_waterEnabled);

				m_bodies.push_back(coneBody);
			}
		}

	}
}

void ape::apePhysicsSimulationScenePlugin::makeCylinder(std::string name, float radius, float height, ape::Vector3 pos)
{
	if (auto cylinderNode = mpSceneManager->createNode("cylinderNode").lock())
	{
		// material for cylinder
		std::shared_ptr<ape::IManualMaterial> cylinderMaterial;
		if (cylinderMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("cylinderMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
		{
			cylinderMaterial->setDiffuseColor(ape::Color(.5, .5, .5));
			cylinderMaterial->setSpecularColor(ape::Color(.2, .5, .2));
		}


		cylinderNode->setPosition(pos);
		cylinderNode->setOrientation(ape::Quaternion(ape::Degree(90.f), ape::Vector3(1, 0, 0)));
		if (auto cylinder = std::static_pointer_cast<ape::ICylinderGeometry>(mpSceneManager->createEntity("cylinder", ape::Entity::GEOMETRY_CYLINDER).lock()))
		{
			cylinder->setParameters(radius, height, 1);
			cylinder->setParentNode(cylinderNode);
			cylinder->setMaterial(cylinderMaterial);


			if (auto cylinderBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("cylinderbody", ape::Entity::RIGIDBODY).lock()))
			{
				cylinderBody->setToDynamic(1.0f);
				cylinderBody->setGeometry(cylinder);
				cylinderBody->setParentNode(cylinderNode);
				cylinderBody->setRestitution(0.0f);
				cylinderBody->setDamping(0.5, 0.7);
				cylinderBody->setFriction(0.4, 0.5, 0.5);
				cylinderBody->setBouyancy(m_waterEnabled);

				m_bodies.push_back(cylinderBody);
			}
		}
	}
}


