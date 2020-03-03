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
	m_ground = true;
	m_terrain = m_water1 = m_water2 = false;
	bool m_cubes = false;
	bool m_spheres = false;
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
				m_water1 = environment == "water1";
				m_water2 = environment == "water2";
				m_terrain = environment == "terrain";
				m_ground = environment == "ground";
			}
			if (jsonDocument.HasMember("cubes"))
			{
				rapidjson::Value& input = jsonDocument["cubes"];

				for (rapidjson::Value::MemberIterator it = input.MemberBegin(); it != input.MemberEnd(); it++)
				{
					if (it->name == "enable")
						m_cubes = it->value.GetBool();
					if (it->name == "arraySize")
					{
						m_cubesArraySize[0] = it->value.GetArray()[0].GetInt();
						m_cubesArraySize[1] = it->value.GetArray()[1].GetInt();
						m_cubesArraySize[2] = it->value.GetArray()[2].GetInt();
					}
					if (it->name == "position")
					{
						m_cubesInitPos.x = it->value.GetArray()[0].GetFloat();
						m_cubesInitPos.y = it->value.GetArray()[1].GetFloat();
						m_cubesInitPos.z = it->value.GetArray()[2].GetFloat();
					}
				}
			}
			if (jsonDocument.HasMember("spheres"))
			{
				rapidjson::Value& input = jsonDocument["spheres"];

				for (rapidjson::Value::MemberIterator it = input.MemberBegin(); it != input.MemberEnd(); it++)
				{
					if (it->name == "enable")
						m_spheres = it->value.GetBool();
					if (it->name == "arraySize")
					{
						m_spheresArraySize[0] = it->value.GetArray()[0].GetInt();
						m_spheresArraySize[1] = it->value.GetArray()[1].GetInt();
						m_spheresArraySize[2] = it->value.GetArray()[2].GetInt();
					}
					if (it->name == "position")
					{
						m_spheresInitPos.x = it->value.GetArray()[0].GetFloat();
						m_spheresInitPos.y = it->value.GetArray()[1].GetFloat();
						m_spheresInitPos.z = it->value.GetArray()[2].GetFloat();
					}
				}
			}
			if (jsonDocument.HasMember("assets"))
			{
				rapidjson::Value& input = jsonDocument["assets"];

				for (auto& asset : input.GetArray())
				{
					AssetConfig assetConfig;
					assetConfig.radius = 10.f;
					assetConfig.height = 10.f;


					for (rapidjson::Value::MemberIterator assetMemberIterator = asset.MemberBegin(); assetMemberIterator != asset.MemberEnd(); ++assetMemberIterator)
					{
						if (assetMemberIterator->name == "name")
							assetConfig.name = assetMemberIterator->value.GetString();
						else if (assetMemberIterator->name == "shape")
							assetConfig.shape = assetMemberIterator->value.GetString();
						else if (assetMemberIterator->name == "radius")
							assetConfig.radius = assetMemberIterator->value.GetFloat();
						else if (assetMemberIterator->name == "height")
							assetConfig.height = assetMemberIterator->value.GetFloat();
						else if (assetMemberIterator->name == "dims")
						{
							assetConfig.dims.x = assetMemberIterator->value.GetArray()[0].GetFloat();
							assetConfig.dims.y = assetMemberIterator->value.GetArray()[1].GetFloat();
							assetConfig.dims.z = assetMemberIterator->value.GetArray()[2].GetFloat();
						}
						else if (assetMemberIterator->name == "position")
						{
							assetConfig.pos.x = assetMemberIterator->value.GetArray()[0].GetFloat();
							assetConfig.pos.y = assetMemberIterator->value.GetArray()[1].GetFloat();
							assetConfig.pos.z = assetMemberIterator->value.GetArray()[2].GetFloat();
						}
						else if (assetMemberIterator->name == "orientation")
						{
							assetConfig.orient.w = assetMemberIterator->value.GetArray()[0].GetFloat();
							assetConfig.orient.x = assetMemberIterator->value.GetArray()[1].GetFloat();
							assetConfig.orient.y = assetMemberIterator->value.GetArray()[2].GetFloat();
							assetConfig.orient.z = assetMemberIterator->value.GetArray()[3].GetFloat();
						}
						else if (assetMemberIterator->name == "color")
						{
							std::string colorStr = assetMemberIterator->value.GetString();
							if (colorStr == "blue")
								assetConfig.color = ape::Color(0.01f, 0.01f, 0.7f);
							else if (colorStr == "red")
								assetConfig.color = ape::Color(0.7f, 0.01f, 0.01f);
							else if (colorStr == "green")
								assetConfig.color = ape::Color(0.01f, 0.7f, 0.01f);
							else if (colorStr == "yellow")
								assetConfig.color = ape::Color(0.7f, 0.7f, 0.01f);
						}
					}

					m_assets.push_back(assetConfig);
				}
			}


		}
	}

	//mpSceneMakerMacro->makeLit();
	if (!m_water2)
	{
		if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			light->setLightType(ape::Light::Type::DIRECTIONAL);
			light->setLightDirection(ape::Vector3(1, -1, 0));
			light->setDiffuseColor(ape::Color(0.6f, 0.6f, 0.6f));
			light->setSpecularColor(ape::Color(0.6f, 0.6f, 0.6f));
		}
		if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light2", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			light->setLightType(ape::Light::Type::DIRECTIONAL);
			light->setLightDirection(ape::Vector3(0, -1, 1));
			light->setDiffuseColor(ape::Color(0.6f, 0.6f, 0.6f));
			light->setSpecularColor(ape::Color(0.6f, 0.6f, 0.6f));
		}

		mpSceneMakerMacro->makeBackground();
	}
	mpSceneMakerMacro->makeCoordinateSystem();
	//mpSceneMakerMacro->makeTerrain("terrain");
	//mpSceneMakerMacro->makeBox("a1");

	if (true)
	{
		if (m_water1)
		{
			makeWater("water", ape::Vector3(20000, 5000, 20000), ape::Vector3(0, 30, 0));
		}
		else if (m_terrain)
		{
			makeTerrain("terrain", ape::Vector3(1, 1, 1));
		}
		else if (m_ground)
		{
			makeGround("ground", ape::Vector2(10000, 10000), 0);
		}
	}


	// makeSphere("sphere11", 25.f, {+50.,300., -300.});

	// makeBox("box111", ape::Vector3(50, 50, 50), ape::Vector3(-100.f, 400.f, 0.f));

	//makeCone("cone11", 10.f, 15.f, { 0,200,0 });

	for (auto& asset : m_assets)
	{
		if (asset.shape == "sphere")
			makeSphere(asset.name, asset.radius, asset.pos, asset.orient, asset.color);
		else if (asset.shape == "cone")
			makeCone(asset.name, asset.radius, asset.height, asset.pos, asset.orient, asset.color);
		else if (asset.shape == "cylinder")
			makeCylinder(asset.name, asset.radius, asset.height, asset.pos, asset.orient, asset.color);
		else if (asset.shape == "box")
			makeBox(asset.name, asset.dims, asset.pos, asset.orient, asset.color);
	}

	if (m_cubes)
		makeCubeArray();
	if (m_spheres)
		makeSphereArray();

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
	if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		material->setDiffuseColor(ape::Color(0.1f, 0.1f, 0.1f));
		material->setSpecularColor(ape::Color(0.3f, 0.3f, 0.2f));
		material->setCullingMode(ape::Material::CullingMode::NONE_CM);

		if (auto node = mpSceneManager->createNode(name + "Node", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			node->setPosition(ape::Vector3(0, 0, 0));
			node->setScale(scale);

			if (auto terrain = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_INDEXEDFACESET, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				int i;
				int j;

				const int NUM_VERTS_X = 50;
				const int NUM_VERTS_Y = 50;
				const int totalVerts = NUM_VERTS_X * NUM_VERTS_Y + 2 * (NUM_VERTS_X + NUM_VERTS_Y) - 4;
				const int totalTriangles = 2 * (NUM_VERTS_X - 1) * (NUM_VERTS_Y - 1) + 2 * (NUM_VERTS_X + NUM_VERTS_Y) - 4;
				const float TRIANGLE_SIZE = 30.f;

				float offset = -50;
				const float waveheight = 20.f;


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

				if (auto terrainBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY, false, "").lock()))
				{
					terrainBody->setToStatic();
					terrainBody->setParentNode(node);
					terrainBody->setGeometry(terrain);
					terrainBody->setRestitution(0.8f);
					terrainBody->setFriction(0.5, 0.3, 0.3);

				}
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeGround(std::string name, ape::Vector2 size, float height)
{
	if (auto planeNode = mpSceneManager->createNode(name + "Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{

		planeNode->setPosition(ape::Vector3(0, 0, 0));
		if (auto planeMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			planeMaterial->setDiffuseColor(ape::Color(0.1f, 0.1f, 0.1f));
			planeMaterial->setSpecularColor(ape::Color(0.3f, 0.3f, 0.2f));
			planeMaterial->setCullingMode(ape::Material::CullingMode::NONE_CM);
			if (auto plane = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_PLANE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				plane->setParameters(ape::Vector2(1, 1), size, ape::Vector2(1, 1));
				plane->setParentNode(planeNode);
				plane->setMaterial(planeMaterial);


				if (auto planeBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY, false, "").lock()))
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

void ape::apePhysicsSimulationScenePlugin::makeWater(std::string name, ape::Vector3 size, ape::Vector3 pos)
{
	if (auto boxNode = mpSceneManager->createNode(name + "Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{

		boxNode->setPosition(pos - ape::Vector3(0,size.y /2,0));
		if (auto boxMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			boxMaterial->setDiffuseColor(ape::Color(0.0f, 0.0f, 0.2f, 0.7f));
			boxMaterial->setSpecularColor(ape::Color(0.0f, 0.0f, 0.2f, 0.7f));
			boxMaterial->setCullingMode(ape::Material::CullingMode::NONE_CM);
			boxMaterial->setSceneBlending(ape::Material::SceneBlendingType::TRANSPARENT_ALPHA);


			if (auto box = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_BOX, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				box->setParameters(ape::Vector3(size.x, size.y, size.y));
				box->setParentNode(boxNode);
				box->setMaterial(boxMaterial);
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeBox(std::string name, ape::Vector3 dims, ape::Vector3 pos, ape::Quaternion orient = { 1,0,0,0 }, ape::Color color = ape::Color())
{
	// create box geometry
	if (auto boxNode = mpSceneManager->createNode(name + "Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		// material for box
		ape::ManualMaterialSharedPtr boxMaterial;
		if (boxMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			boxMaterial->setDiffuseColor(color);
			boxMaterial->setSpecularColor(color);
		}

		boxNode->setPosition(pos);
		boxNode->setOrientation(orient);

		ape::BoxGeometrySharedPtr box;
		if (box = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_BOX, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			box->setParameters(dims);
			box->setParentNode(boxNode);
			box->setMaterial(boxMaterial);

			ape::RigidBodySharedPtr boxBody;
			if (boxBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY, false, "").lock()))
			{
				boxBody->setToDynamic(1.0f);
				boxBody->setParentNode(boxNode);
				boxBody->setGeometry(box);
				boxBody->setRestitution(0.8f);
				boxBody->setFriction(0.5, 0.3, 0.3);
				boxBody->setBouyancy(m_water1 || m_water2);
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeSphere(std::string name, float radius, ape::Vector3 pos, ape::Quaternion orient = { 1,0,0,0 }, ape::Color color = ape::Color())
{
	if (auto sphereNode = mpSceneManager->createNode(name + "Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		// material for sphere 
		std::shared_ptr<ape::IManualMaterial> sphereMaterial;
		if (sphereMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMaterial->setDiffuseColor(color);
			sphereMaterial->setSpecularColor(color);
		}

		sphereNode->setPosition(pos);
		sphereNode->setOrientation(orient);

		if (auto sphere = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphere->setParameters(radius, ape::Vector2(1, 1));
			sphere->setParentNode(sphereNode);
			sphere->setMaterial(sphereMaterial);

			if (auto sphereBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY, false, "").lock()))
			{
				sphereBody->setGeometry(sphere);
				sphereBody->setParentNode(sphereNode);
				sphereBody->setToDynamic(1.0f);
				sphereBody->setRestitution(0.8f);
				sphereBody->setDamping(0.05, 0.05);
				sphereBody->setFriction(0.5, 0.3, 0.3);
				sphereBody->setBouyancy(m_water1 || m_water2);
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeCone(std::string name, float radius, float height, ape::Vector3 pos, ape::Quaternion orient = { 1,0,0,0 }, ape::Color color = ape::Color())
{
	if (auto coneNode = mpSceneManager->createNode(name + "Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		// material for cone
		std::shared_ptr<ape::IManualMaterial> coneMaterial;
		if (coneMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			coneMaterial->setDiffuseColor(color);
			coneMaterial->setSpecularColor(color);
		}

		coneNode->setPosition(pos);
		coneNode->setOrientation(orient);

		if (auto cone = std::static_pointer_cast<ape::IConeGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_CONE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			cone->setParameters(radius, height, 1, ape::Vector2(1, 1));

			cone->setParentNode(coneNode);
			cone->setMaterial(coneMaterial);


			if (auto coneBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY, false, "").lock()))
			{
				coneBody->setToDynamic(1.0f);
				coneBody->setGeometry(cone);
				coneBody->setParentNode(coneNode);
				coneBody->setRestitution(0.8f);
				coneBody->setDamping(0.05, 0.05);
				coneBody->setFriction(0.5, 0.3, 0.3);
				coneBody->setBouyancy(m_water1 || m_water2);

				m_bodies.push_back(coneBody);
			}
		}

	}
}

void ape::apePhysicsSimulationScenePlugin::makeCylinder(std::string name, float radius, float height, ape::Vector3 pos, ape::Quaternion orient = { 1,0,0,0 }, ape::Color color = ape::Color())
{
	if (auto cylinderNode = mpSceneManager->createNode(name + "Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		// material for cylinder
		std::shared_ptr<ape::IManualMaterial> cylinderMaterial;
		if (cylinderMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(name + "Material", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			cylinderMaterial->setDiffuseColor(color);
			cylinderMaterial->setSpecularColor(color);
		}


		cylinderNode->setPosition(pos);
		cylinderNode->setOrientation(ape::Quaternion(ape::Degree(90.f), ape::Vector3(1, 0, 0)));
		if (auto cylinder = std::static_pointer_cast<ape::ICylinderGeometry>(mpSceneManager->createEntity(name, ape::Entity::GEOMETRY_CYLINDER, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			cylinder->setParameters(radius, height, 1);
			cylinder->setParentNode(cylinderNode);
			cylinder->setMaterial(cylinderMaterial);


			if (auto cylinderBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity(name + "Body", ape::Entity::RIGIDBODY, false, "").lock()))
			{
				cylinderBody->setToDynamic(1.0f);
				cylinderBody->setGeometry(cylinder);
				cylinderBody->setParentNode(cylinderNode);
				cylinderBody->setRestitution(0.0f);
				cylinderBody->setDamping(0.5, 0.7);
				cylinderBody->setFriction(0.4, 0.5, 0.5);
				cylinderBody->setBouyancy(m_water1 || m_water2);

				m_bodies.push_back(cylinderBody);
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeCubeArray()
{
	for (int i = 0; i < m_cubesArraySize[0]; i++)
	{
		std::stringstream ssi;
		ssi << i;
		for (int j = 0; j < m_cubesArraySize[1]; j++)
		{
			std::stringstream ssj;
			ssj << j;
			for (int k = 0; k < m_cubesArraySize[2]; k++)
			{
				std::stringstream ssk;
				ssk << k;

				if (auto boxNode = mpSceneManager->createNode("xxboxNode" + ssi.str() + ssj.str() + ssk.str(), true, mpCoreConfig->getNetworkGUID()).lock())
				{
					// material for box
					std::shared_ptr<ape::IManualMaterial> boxMaterial;
					
					if (boxMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("xxboxMaterial" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						boxMaterial->setDiffuseColor(ape::Color(float(i + 1) / float(m_cubesArraySize[0]), float(j + 1) / float(m_cubesArraySize[1]), float(k + 1) / float(m_cubesArraySize[2])));
						boxMaterial->setSpecularColor(ape::Color(float(i) / float(m_cubesArraySize[0] - 1), float(j) / float(m_cubesArraySize[1] - 1), float(k) / float(m_cubesArraySize[2] - 1)));
						boxMaterial->setAmbientColor(ape::Color(float(i + 1) / float(m_cubesArraySize[0]), float(j + 1) / float(m_cubesArraySize[1]), float(k + 1) / float(m_cubesArraySize[2])));
					}


					boxNode->setPosition(m_cubesInitPos + ape::Vector3(float(i * 10 - m_cubesArraySize[0] * 10 / 2 + 5), float(j * 10), float(k * 10 - m_cubesArraySize[2] * 10 / 2 + 5)));


					if (auto box = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity("xxbox" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::GEOMETRY_BOX, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						box->setParameters(ape::Vector3(10, 10, 10));
						box->setParentNode(boxNode);
						box->setMaterial(boxMaterial);


						if (auto boxBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("xxboxBody" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::RIGIDBODY, false, "").lock()))
						{
							boxBody->setToStatic();
							boxBody->setGeometry(box);
							boxBody->setParentNode(boxNode);
							boxBody->setRestitution(0.6f);
							boxBody->setDamping(0.01, 0.01);
							boxBody->setBouyancy(m_water1 || m_water2);
							boxBody->setToDynamic(1.0f);

							m_bodies.push_back(boxBody);
						}
					}
				}
			}
		}
	}
}

void ape::apePhysicsSimulationScenePlugin::makeSphereArray()
{
	if (m_spheres)
	{
		for (int i = 0; i < m_spheresArraySize[0]; i++)
		{
			std::stringstream ssi;
			ssi << i;
			for (int j = 0; j < m_spheresArraySize[1]; j++)
			{
				std::stringstream ssj;
				ssj << j;
				for (int k = 0; k < m_spheresArraySize[2]; k++)
				{
					std::stringstream ssk;
					ssk << k;

					if (auto sphereNode = mpSceneManager->createNode("xxsphereNode" + ssi.str() + ssj.str() + ssk.str(), true, mpCoreConfig->getNetworkGUID()).lock())
					{
						// material for sphere
						std::shared_ptr<ape::IManualMaterial> sphereMaterial;
						if (sphereMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("xxsphereMaterial" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							ape::Color color;

							switch (rand() % 4)
							{
							case 0:
								color = ape::Color(0.01f, 0.01f, 0.7f, 0.6f);
								break;
							case 1:
								color = ape::Color(0.7f, 0.01f, 0.01f, 0.6f);
								break;
							case 2:
								color = ape::Color(0.7f, 0.7f, 0.01f, 0.6f);
								break;
							case 3:
								color = ape::Color(0.01f, 0.7f, 0.01f, 0.6f);
								break;
							}
							sphereMaterial->setDiffuseColor(color);
							sphereMaterial->setSpecularColor(color);
							sphereMaterial->setAmbientColor(color);
						}


						sphereNode->setPosition(m_spheresInitPos + ape::Vector3(float(i * 10 - m_spheresArraySize[0] * 10 / 2 + 5), float(j * 10), float(k * 10 - m_spheresArraySize[2] * 10 / 2 + 5)));

						if (auto sphere = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("xxsphere" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							sphere->setParameters(5.0f, { 1,1 });
							sphere->setParentNode(sphereNode);
							sphere->setMaterial(sphereMaterial);


							if (auto sphereBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("xxspherebody" + ssi.str() + ssj.str() + ssk.str(), ape::Entity::RIGIDBODY, false, "").lock()))
							{
								sphereBody->setToStatic();
								sphereBody->setGeometry(sphere);
								sphereBody->setParentNode(sphereNode);
								sphereBody->setRestitution(0.6f);
								sphereBody->setDamping(0.01, 0.01);
								sphereBody->setBouyancy(m_water1 || m_water2);
								sphereBody->setToDynamic(1.0f);

								m_bodies.push_back(sphereBody);
							}
						}
					}
				}
			}
		}
	}
}
