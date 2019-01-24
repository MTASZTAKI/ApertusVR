#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "ApeOgreRenderPlugin.h"
#include "ApeOgreUtilities.h"

Ape::OgreRenderPlugin::OgreRenderPlugin( )
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mEventDoubleQueue = Ape::DoubleQueue<Event>();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::LIGHT, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TEXT, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_PLANE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_BOX, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_CONE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TUBE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_SPHERE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TORUS, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::MATERIAL_FILE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::MATERIAL_MANUAL, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::PASS_PBS, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::PASS_MANUAL, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_MANUAL, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_FILE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_UNIT, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::SKY, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::WATER, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpRoot = nullptr;
	mpOgreSceneManager = nullptr;
	mRenderWindows = std::map<std::string, Ogre::RenderWindow*>();
	mpOverlaySys = nullptr;
	mpOgreMovableTextFactory = nullptr;
	mpOverlayMgr = nullptr;
	mpHlmsPbsManager = nullptr;
	mpShaderGenerator = nullptr;
	mpShaderGeneratorResolver = nullptr;
	mpMeshLodGenerator = nullptr;
	mCurrentlyLoadingMeshEntityLodConfig = Ogre::LodConfig();
	mpCurrentlyLoadingMeshEntity = nullptr;
	mOgreRenderPluginConfig = Ape::OgreRenderPluginConfig();
	mOgreCameras = std::vector<Ogre::Camera*>();
	mPbsMaterials = std::map<std::string, Ogre::PbsMaterial*>();
	mpHydrax = nullptr;
	mpSkyx = nullptr;
	mpSkyxSunlight = nullptr;
	mpSkyxSkylight = nullptr;
	mpSkyxBasicController = nullptr;
	mOgrePointCloudMeshes = std::map<std::string, Ape::OgrePointCloud*>();
	mCameraCountFromConfig = 0;
	mRttList = std::vector<Ape::ManualTextureWeakPtr>();
	APE_LOG_FUNC_LEAVE();
}

Ape::OgreRenderPlugin::~OgreRenderPlugin()
{
	APE_LOG_FUNC_ENTER();
	delete mpRoot;
	APE_LOG_FUNC_LEAVE();
}

void Ape::OgreRenderPlugin::eventCallBack(const Ape::Event& event)
{
	mEventDoubleQueue.push(event);
}

void Ape::OgreRenderPlugin::createAutoGeneratedLodLevelsIfNeeded(std::string geometryName)
{
	std::string fileName = geometryName.substr(0, geometryName.find_last_of("."));
	fileName.append(".mesh");
	mCurrentlyLoadingMeshEntityLodConfig.mesh = Ogre::MeshManager::getSingleton().load(fileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mpCurrentlyLoadingMeshEntity = mpOgreSceneManager->createEntity(geometryName, fileName);

	if (mCurrentlyLoadingMeshEntityLodConfig.mesh->getNumLodLevels() > 1)
	{
		mpCurrentlyLoadingMeshEntity->setMeshLodBias(mOgreRenderPluginConfig.ogreLodLevelsConfig.bias);
	}
	else if (mOgreRenderPluginConfig.ogreLodLevelsConfig.autoGenerateAndSave)
	{
		mCurrentlyLoadingMeshEntityLodConfig.advanced = Ogre::LodConfig::Advanced();
		mCurrentlyLoadingMeshEntityLodConfig.strategy = Ogre::PixelCountLodStrategy::getSingletonPtr();
		mCurrentlyLoadingMeshEntityLodConfig.levels.clear();
		mCurrentlyLoadingMeshEntityLodConfig.advanced.profile.clear();
		Ogre::LodWorkQueueWorker::getSingleton().clearPendingLodRequests();
		Ogre::LodConfig lodConfig;
		mpMeshLodGenerator->getAutoconfig(mCurrentlyLoadingMeshEntityLodConfig.mesh, lodConfig);
		lodConfig.advanced.useBackgroundQueue = true;
		lodConfig.advanced.profile = mCurrentlyLoadingMeshEntityLodConfig.advanced.profile;
		lodConfig.advanced.useVertexNormals = mCurrentlyLoadingMeshEntityLodConfig.advanced.useVertexNormals;
		mpMeshLodGenerator->generateLodLevels(lodConfig);
	}	
}

void Ape::OgreRenderPlugin::processEventDoubleQueue()
{
	mEventDoubleQueue.swap();
	while (!mEventDoubleQueue.emptyPop())
	{
		Ape::Event event = mEventDoubleQueue.front();
		if (event.group == Ape::Event::Group::NODE)
		{
			if (auto node = mpSceneManager->getNode(event.subjectName).lock())
			{
				std::string nodeName = node->getName();
				if (event.type == Ape::Event::Type::NODE_CREATE)
					mpOgreSceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
				else 
				{
					Ogre::SceneNode* ogreNode = nullptr;
					if (mpOgreSceneManager->hasSceneNode(nodeName))
						ogreNode = mpOgreSceneManager->getSceneNode(nodeName);
					if (ogreNode)
					{
						switch (event.type)
						{
						case Ape::Event::Type::NODE_PARENTNODE:
						{
							if (auto parentNode = node->getParentNode().lock())
							{
								auto ogreOldParentNode = ogreNode->getParentSceneNode();
								if (ogreOldParentNode)
									ogreOldParentNode->removeChild(ogreNode);
								if (mpOgreSceneManager->hasSceneNode(parentNode->getName()))
								{
									auto ogreNewParentNode = mpOgreSceneManager->getSceneNode(parentNode->getName());
									ogreNewParentNode->addChild(ogreNode);
								}
							}
						}
							break;
						case Ape::Event::Type::NODE_DELETE:
							;
							break;
						case Ape::Event::Type::NODE_POSITION:
							ogreNode->setPosition(Ape::ConversionToOgre(node->getPosition()));
							break;
						case Ape::Event::Type::NODE_ORIENTATION:
							ogreNode->setOrientation(Ape::ConversionToOgre(node->getOrientation()));
							break;
						case Ape::Event::Type::NODE_SCALE:
							ogreNode->setScale(Ape::ConversionToOgre(node->getScale()));
							break;
						case Ape::Event::Type::NODE_CHILDVISIBILITY:
							ogreNode->setVisible(node->getChildrenVisibility());
							break;
						case Ape::Event::Type::NODE_FIXEDYAW:
							ogreNode->setFixedYawAxis(node->isFixedYaw());
							break;
						case Ape::Event::Type::NODE_INHERITORIENTATION:
							ogreNode->setInheritOrientation(node->isInheritOrientation());
							break;
						case Ape::Event::Type::NODE_INITIALSTATE:
							ogreNode->setInitialState();
							break;
						case Ape::Event::Type::NODE_SHOWBOUNDINGBOX:
							ogreNode->showBoundingBox(true);
							break;
						case Ape::Event::Type::NODE_HIDEBOUNDINGBOX:
							ogreNode->showBoundingBox(false);
							break;
						}
					}
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_FILE)
		{
			if (auto geometryFile = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = geometryFile->getName();
				std::string fileName = geometryFile->getFileName();
				std::string parentNodeName = "";
				if (auto parentNode = geometryFile->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_FILE_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_FILE_PARENTNODE:
				{
					if (mpOgreSceneManager->hasEntity(geometryName))
					{
						if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
						{
							if (mpOgreSceneManager->hasSceneNode(parentNodeName))
							{
								if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
								{
									ogreParentNode->attachObject(ogreEntity);
								}
							}
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_FILE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_FILE_FILENAME:
				{
					if (fileName.find_first_of(".") != std::string::npos)
					{
						std::string fileExtension = fileName.substr(fileName.find_last_of("."));
						if (fileExtension == ".mesh")
						{
							createAutoGeneratedLodLevelsIfNeeded(geometryName);
						}
					}
					else if (mpOgreSceneManager->hasManualObject(fileName))
					{
						auto ogreManual = mpOgreSceneManager->getManualObject(fileName);
						std::stringstream meshName;
						meshName << fileName << ".mesh";
						if (Ogre::MeshManager::getSingleton().getByName(meshName.str()).isNull())
						{
							ogreManual->convertToMesh(meshName.str());
						}
						if (!mpOgreSceneManager->hasEntity(geometryName))
						{
							mpOgreSceneManager->createEntity(geometryName, meshName.str());
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES:
				{
					if (geometryFile->isMergeSubMeshes())
					{
						if (mpOgreSceneManager->hasEntity(geometryName))
						{
							if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
							{
								auto ogreStaticGeometry = mpOgreSceneManager->createStaticGeometry(geometryName + "SG");
								ogreStaticGeometry->addEntity(ogreEntity, Ogre::Vector3::ZERO);
								ogreStaticGeometry->build();
							}
						}
					}
				}
				break;
				case Ape::Event::Type::GEOMETRY_FILE_EXPORT:
				{
					if (geometryFile->isExportMesh())
					{
						if (fileName.find_first_of(".") != std::string::npos)
						{
							std::string fileExtension = fileName.substr(fileName.find_last_of("."));
							if (fileExtension == ".mesh")
							{
								auto mesh = Ogre::MeshManager::getSingleton().getByName(fileName);
								if (!mesh.isNull())
									mMeshSerializer.exportMesh(mesh.getPointer(), fileName);
							}
						}
						else if (mpOgreSceneManager->hasManualObject(geometryName))
						{
							auto ogreManual = mpOgreSceneManager->getManualObject(geometryName);
							std::stringstream meshName;
							meshName << geometryName << ".mesh";
							if (Ogre::MeshManager::getSingleton().getByName(meshName.str()).isNull())
							{
								auto mesh = ogreManual->convertToMesh(meshName.str());
								if (!mesh.isNull())
								{
									mMeshSerializer.exportMesh(mesh.getPointer(), meshName.str());
									Ogre::Mesh::SubMeshIterator subMeshIterator = mesh->getSubMeshIterator();
									Ogre::SubMesh* subMesh = nullptr;
									while (subMeshIterator.hasMoreElements())
									{
										subMesh = subMeshIterator.getNext();
										std::string materialName = subMesh->getMaterialName();
										auto ogreMaterial = Ogre::MaterialManager::getSingletonPtr()->getByName(materialName);
										if (!ogreMaterial.isNull())
										{
											std::string filePath = ogreManual->getName();
											std::size_t found = filePath.find_last_of("/\\");
											filePath = filePath.substr(0, found + 1);
											std::string materialFileName = materialName;
											//TODO automatic filesystem check for filenames and coding conversion
											materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), '<'), materialFileName.end());
											materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), '>'), materialFileName.end());
											materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), '/'), materialFileName.end());
											materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), '/\\'), materialFileName.end());
											materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), ':'), materialFileName.end());
											materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), ','), materialFileName.end());
											/*std::wstring wMaterialFileName = utf8_decode(materialFileName.c_str());
											std::wcout << wMaterialFileName << std::endl;*/
											std::size_t materialFileNameHash = std::hash<std::string>{}(materialFileName);
											std::stringstream materialFilePath;
											materialFilePath << filePath << materialFileNameHash << ".material";
											std::ifstream materialFile(materialFilePath.str());
											if (!materialFile)
												mMaterialSerializer.exportMaterial(ogreMaterial, materialFilePath.str());
										}
									}
									mpOgreSceneManager->destroyManualObject(ogreManual);
									auto ogreEntity = mpOgreSceneManager->createEntity(geometryName, meshName.str());
								}
							}
						}
					}
				}
				break;
				case Ape::Event::Type::GEOMETRY_FILE_MATERIAL:
				{
					if (mpOgreSceneManager->hasEntity(geometryName))
					{
						if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
						{
							if (auto material = geometryFile->getMaterial().lock())
							{
								auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
								if (!ogreMaterial.isNull())
									ogreEntity->setMaterial(ogreMaterial);
							}
						}
					}
				}
				break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_PLANE)
		{
			if (auto primitive = std::static_pointer_cast<Ape::IPlaneGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_PLANE_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_PLANE_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_PLANE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_PLANE_MATERIAL:
				{
					if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_PLANE_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						Ape::GeometryPlaneParameters parameters = primitive->getParameters();
						Procedural::PlaneGenerator().setNumSegX(parameters.numSeg.x).setNumSegY(parameters.numSeg.y).setSizeX(parameters.size.x).setSizeY(parameters.size.y)
							.setUTile(parameters.tile.x).setVTile(parameters.tile.y).realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);
				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_BOX)
		{
			if (auto primitive = std::static_pointer_cast<Ape::IBoxGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_BOX_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_BOX_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_BOX_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_BOX_MATERIAL:
				{
					if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_BOX_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						Ape::GeometryBoxParameters parameters = primitive->getParameters();
						Procedural::BoxGenerator().setSizeX(parameters.dimensions.x).setSizeY(parameters.dimensions.x).setSizeZ(parameters.dimensions.x)
							.realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);
				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_SPHERE)
		{
			if (auto primitive = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_SPHERE_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_SPHERE_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_SPHERE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_SPHERE_MATERIAL:
				{
					if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_SPHERE_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						Ape::GeometrySphereParameters parameters = primitive->getParameters();
						Procedural::SphereGenerator().setRadius(parameters.radius)
							.setUTile(parameters.tile.x).setVTile(parameters.tile.y)
							.realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);

				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_CYLINDER)
		{
			if (auto primitive = std::static_pointer_cast<Ape::ICylinderGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_CYLINDER_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_CYLINDER_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_CYLINDER_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_CYLINDER_MATERIAL:
				{
					if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_CYLINDER_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						Ape::GeometryCylinderParameters parameters = primitive->getParameters();
						Procedural::CylinderGenerator().setHeight(parameters.height)
							.setRadius(parameters.radius)
							.setUTile(parameters.tile)
							.realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);

				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_TORUS)
		{
			if (auto primitive = std::static_pointer_cast<Ape::ITorusGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_TORUS_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TORUS_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TORUS_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TORUS_MATERIAL:
				{
					if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TORUS_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						Ape::GeometryTorusParameters parameters = primitive->getParameters();
						Procedural::TorusGenerator().setRadius(parameters.radius)
							.setSectionRadius(parameters.sectionRadius)
							.setUTile(parameters.tile.x).setVTile(parameters.tile.y)
							.realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);

				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_CONE)
		{
			if (auto primitive = std::static_pointer_cast<Ape::IConeGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_CONE_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_CONE_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_CONE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_CONE_MATERIAL:
				{
					if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_CONE_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						Ape::GeometryConeParameters parameters = primitive->getParameters();
						Procedural::ConeGenerator().setRadius(parameters.radius)
							.setHeight(parameters.height)
							//.setNumSegBase(parameters.numSeg.x).setNumSegHeight(parameters.numSeg.y)
							//.setUTile(parameters.tile)
							.realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);

				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_TUBE)
		{
			if (auto primitive = std::static_pointer_cast<Ape::ITubeGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_TUBE_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TUBE_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TUBE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TUBE_MATERIAL:
				{
					if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
						}
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TUBE_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						Ape::GeometryTubeParameters parameters = primitive->getParameters();
						Procedural::TubeGenerator().setHeight(parameters.height)
							.setUTile(parameters.tile)
							.realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);

				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_INDEXEDFACESET)
		{
			if (auto manual = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				Ape::GeometryIndexedFaceSetParameters parameters = manual->getParameters();
				std::string geometryName = manual->getName();
				if (parameters.groupName.size())
					geometryName = parameters.groupName;
				std::string parentNodeName = "";
				if (auto parentNode = manual->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
					case Ape::Event::Type::GEOMETRY_INDEXEDFACESET_CREATE:
					{

					}
					break;
					case Ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARENTNODE:
					{
						if (mpOgreSceneManager->hasManualObject(geometryName))
						{
							if (auto ogreManual = mpOgreSceneManager->getManualObject(geometryName))
							{
								if (mpOgreSceneManager->hasSceneNode(parentNodeName))
								{
									if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
									{
										ogreParentNode->attachObject(ogreManual);
									}
								}
							}
						}
						else if (mpOgreSceneManager->hasEntity(geometryName))
						{
							if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
							{
								if (mpOgreSceneManager->hasSceneNode(parentNodeName))
								{
									if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
										ogreParentNode->attachObject(ogreEntity);
								}
							}
						}
					}
					break;
					case Ape::Event::Type::GEOMETRY_INDEXEDFACESET_DELETE:
						;
						break;
					case Ape::Event::Type::GEOMETRY_INDEXEDFACESET_MATERIAL:
					{
						if (auto ogreEntity = mpOgreSceneManager->hasEntity(geometryName))
						{
							if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
							{
								if (auto material = manual->getMaterial().lock())
								{
									auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
									ogreEntity->setMaterial(ogreMaterial);
								}
							}
						}
					}
					break;
					case Ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS:
					{
						//Convert when command is fired somehow, for example GeometryRef
						//std::stringstream meshFileName;
						//meshFileName << geometryName << ".mesh";
						//if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
						//{
						if (!mpOgreSceneManager->hasManualObject(geometryName))
						{
							auto ogreManual = mpOgreSceneManager->createManualObject(geometryName);
							ogreManual->setDynamic(true);
						}
						if (mpOgreSceneManager->hasManualObject(geometryName))
						{
							if (auto ogreManual = mpOgreSceneManager->getManualObject(geometryName))
							{
								std::vector<Ogre::Vector3> normals = std::vector<Ogre::Vector3>();
								if (parameters.generateNormals)
								{
									/*for (int i = 0; i < parameters.coordinates.size(); i = i + 9)
									{
										Ogre::Vector3 coordinate0(parameters.coordinates[i], parameters.coordinates[i + 1], parameters.coordinates[i + 2]);
										Ogre::Vector3 coordinate1(parameters.coordinates[i + 3], parameters.coordinates[i + 4], parameters.coordinates[i + 5]);
										Ogre::Vector3 coordinate2(parameters.coordinates[i + 6], parameters.coordinates[i + 7], parameters.coordinates[i + 8]);
										Ogre::Vector3 v1;
										v1.x = coordinate2.x - coordinate0.x;
										v1.y = coordinate2.y - coordinate0.y;
										v1.z = coordinate2.z - coordinate0.z;
										Ogre::Vector3 v2;
										v2.x = coordinate1.x - coordinate0.x;
										v2.y = coordinate1.y - coordinate0.y;
										v2.z = coordinate1.z - coordinate0.z;
										Ogre::Vector3 coordinateNormal((v1).crossProduct(v2));
										normals.push_back(coordinateNormal);
									}*/
									normals.resize(parameters.coordinates.size() / 3);
									for (int normalIndex = 0; normalIndex < normals.size(); normalIndex++)
										normals[normalIndex] = Ogre::Vector3::ZERO;
									int indexIndex = 0;
									while (indexIndex < parameters.indices.size())
									{
										int indexCount = 0;
										while (indexIndex + indexCount < parameters.indices.size() && parameters.indices[indexIndex + indexCount] != -1)
											indexCount++;
										if (indexCount == 4)
										{
											int coordinate0Index = parameters.indices[indexIndex] * 3;
											Ogre::Vector3 coordinate0(parameters.coordinates[coordinate0Index], parameters.coordinates[coordinate0Index + 1], parameters.coordinates[coordinate0Index + 2]);

											int coordinate1Index = parameters.indices[(indexIndex + 1)] * 3;
											Ogre::Vector3 coordinate1(parameters.coordinates[coordinate1Index], parameters.coordinates[coordinate1Index + 1], parameters.coordinates[coordinate1Index + 2]);

											int coordinate2Index = parameters.indices[(indexIndex + 2)] * 3;
											Ogre::Vector3 coordinate2(parameters.coordinates[coordinate2Index], parameters.coordinates[coordinate2Index + 1], parameters.coordinates[coordinate2Index + 2]);

											int coordinate3Index = parameters.indices[(indexIndex + 3)] * 3;
											Ogre::Vector3 coordinate3(parameters.coordinates[coordinate3Index], parameters.coordinates[coordinate3Index + 1], parameters.coordinates[coordinate3Index + 2]);

											Ogre::Vector3 v1;
											v1.x = coordinate2.x - coordinate0.x;
											v1.y = coordinate2.y - coordinate0.y;
											v1.z = coordinate2.z - coordinate0.z;
											Ogre::Vector3 v2;
											v2.x = coordinate1.x - coordinate0.x;
											v2.y = coordinate1.y - coordinate0.y;
											v2.z = coordinate1.z - coordinate0.z;
											Ogre::Vector3 coordinateNormal((v2).crossProduct(v1));

											//TODO maybe create new vertices because of trinagle list, instead of not accumulating the normals?
											normals[parameters.indices[indexIndex]] += coordinateNormal;
											normals[parameters.indices[indexIndex + 1]] += coordinateNormal;
											normals[parameters.indices[indexIndex + 2]] += coordinateNormal;
											normals[parameters.indices[indexIndex + 3]] += coordinateNormal;

											indexIndex = indexIndex + 5;
										}
										else if (indexCount == 3)
										{
											int coordinate0Index = parameters.indices[indexIndex] * 3;
											Ogre::Vector3 coordinate0(parameters.coordinates[coordinate0Index], parameters.coordinates[coordinate0Index + 1], parameters.coordinates[coordinate0Index + 2]);

											int coordinate1Index = parameters.indices[(indexIndex + 1)] * 3;
											Ogre::Vector3 coordinate1(parameters.coordinates[coordinate1Index], parameters.coordinates[coordinate1Index + 1], parameters.coordinates[coordinate1Index + 2]);

											int coordinate2Index = parameters.indices[(indexIndex + 2)] * 3;
											Ogre::Vector3 coordinate2(parameters.coordinates[coordinate2Index], parameters.coordinates[coordinate2Index + 1], parameters.coordinates[coordinate2Index + 2]);

											Ogre::Vector3 v1;
											v1.x = coordinate2.x - coordinate0.x;
											v1.y = coordinate2.y - coordinate0.y;
											v1.z = coordinate2.z - coordinate0.z;
											Ogre::Vector3 v2;
											v2.x = coordinate1.x - coordinate0.x;
											v2.y = coordinate1.y - coordinate0.y;
											v2.z = coordinate1.z - coordinate0.z;
											Ogre::Vector3 coordinateNormal((v2).crossProduct(v1));

											//TODO maybe create new vertices because of trinagle list, instead of not accumulating the normals?
											normals[parameters.indices[indexIndex]] += coordinateNormal;
											normals[parameters.indices[indexIndex + 1]] += coordinateNormal;
											normals[parameters.indices[indexIndex + 2]] += coordinateNormal;

											indexIndex = indexIndex + 4;
										}
										else
										{
											// TODO
											indexIndex = indexIndex + indexCount + 1;
										}
									}
								}
								Ogre::MaterialPtr ogreMaterial = Ogre::MaterialPtr();
								if (auto material = parameters.material.lock())
								{
									ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
									ogreManual->begin(material->getName(), Ogre::RenderOperation::OperationType::OT_TRIANGLE_LIST);
								}
								else
								{
									ogreManual->begin("FlatVertexColorLighting", Ogre::RenderOperation::OperationType::OT_TRIANGLE_LIST);
								}
								for (int i = 0; i < parameters.coordinates.size(); i = i + 3)
								{
									ogreManual->position(parameters.coordinates[i], parameters.coordinates[i + 1], parameters.coordinates[i + 2]);
									if (parameters.generateNormals)
									{
										normals[i / 3].normalise();
										ogreManual->normal(normals[i / 3]);
									}
									else if (parameters.normals.size() > i && parameters.normals.size() > 0)
									{
										ogreManual->normal(Ogre::Vector3(parameters.normals[i], parameters.normals[i + 1], parameters.normals[i + 2]));
									}
									if (parameters.textureCoordinates.size() > 0)
									{
										int textCoordIndex = (i / 3) * 2;
										ogreManual->textureCoord(parameters.textureCoordinates[textCoordIndex], parameters.textureCoordinates[textCoordIndex + 1]);
									}
									if (parameters.colors.size() > 0)
									{
										int colorIndex = (i / 3) * 4;
										Ogre::ColourValue color(parameters.colors[colorIndex], parameters.colors[colorIndex + 1], parameters.colors[colorIndex + 2], parameters.colors[colorIndex + 3]);
										ogreManual->colour(color);
									}
								}
								int indexIndex = 0;
								while (indexIndex < parameters.indices.size())
								{
									int indexCount = 0;
									while (indexIndex + indexCount < parameters.indices.size() && parameters.indices[indexIndex + indexCount] != -1)
										indexCount++;

									if (indexCount == 4)
									{
										ogreManual->quad(parameters.indices[indexIndex], parameters.indices[indexIndex + 1], parameters.indices[indexIndex + 2], parameters.indices[indexIndex + 3]);
										indexIndex = indexIndex + 5;
									}
									else if (indexCount == 3)
									{
										ogreManual->triangle(parameters.indices[indexIndex], parameters.indices[indexIndex + 1], parameters.indices[indexIndex + 2]);
										indexIndex = indexIndex + 4;
									}
									else
									{
										for (int i = 0; i < indexCount; i++)
											ogreManual->index(parameters.indices[indexIndex + i]);

										indexIndex = indexIndex + indexCount + 1;
									}
								}
								ogreManual->end();
							}
						}
					}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_INDEXEDLINESET)
		{
			if (auto manual = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = manual->getName();
				std::string parentNodeName = "";
				if (auto parentNode = manual->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_INDEXEDLINESET_CREATE:
					mpOgreSceneManager->createManualObject(geometryName);
					break;
				case Ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_INDEXEDLINESET_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARAMETERS:
				{
					Ape::GeometryIndexedLineSetParameters parameters = manual->getParameters();
					if (auto ogreManual = mpOgreSceneManager->getManualObject(geometryName))
					{
						ogreManual->begin("FlatVertexColorNoLighting", Ogre::RenderOperation::OperationType::OT_LINE_LIST);
						for (int coordinateIndex = 0; coordinateIndex < parameters.coordinates.size(); coordinateIndex = coordinateIndex + 3)
						{
							ogreManual->position(parameters.coordinates[coordinateIndex], parameters.coordinates[coordinateIndex + 1], parameters.coordinates[coordinateIndex + 2]);
							ogreManual->colour(Ape::ConversionToOgre(parameters.color));
						}
						int indexIndex = 0;
						while (indexIndex < parameters.indices.size())
						{
							while (indexIndex < parameters.indices.size() && parameters.indices[indexIndex] != -1)
							{
								ogreManual->index(parameters.indices[indexIndex]);
								indexIndex++;
							}
							indexIndex++;
						}
						ogreManual->end();
						ogreManual->convertToMesh(geometryName);
						mpOgreSceneManager->createEntity(geometryName, geometryName);
						mpOgreSceneManager->destroyManualObject(geometryName);
					}

				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_TEXT)
		{
			if (auto geometryText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = geometryText->getName();
				std::string parentNodeName = "";
				if (auto parentNode = geometryText->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_TEXT_CREATE:
				{
					if (auto ogreText = (Ape::OgreMovableText*)mpOgreSceneManager->createMovableObject(geometryName, "MovableText"))
					{
						ogreText->setTextAlignment(Ape::OgreMovableText::H_CENTER, Ape::OgreMovableText::V_ABOVE);
						ogreText->showOnTop(false);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_SHOWONTOP:
				{
					if (auto ogreText = (Ape::OgreMovableText*)mpOgreSceneManager->getMovableObject(geometryName, "MovableText"))
					{
						if (auto textGeometry = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->getEntity(geometryName).lock()))
							ogreText->showOnTop(textGeometry->isShownOnTop());
					}
				}
				break;
				case Ape::Event::Type::GEOMETRY_TEXT_PARENTNODE:
				{
					if (auto ogreTextGeometry = (Ape::OgreMovableText*)mpOgreSceneManager->getMovableObject(geometryName, "MovableText"))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreTextGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_CAPTION:
				{
					if (auto ogreText = (Ape::OgreMovableText*)mpOgreSceneManager->getMovableObject(geometryName, "MovableText"))
					{
						if (auto textGeometry = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->getEntity(geometryName).lock()))
							ogreText->setCaption(textGeometry->getCaption());
					}
				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::MATERIAL_FILE)
		{
			if (auto materialFile = std::static_pointer_cast<Ape::IFileMaterial>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string materialName = materialFile->getName();
				Ogre::MaterialPtr ogreMaterial;
				if (Ogre::MaterialManager::getSingleton().resourceExists(materialName))
				{
					ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				}
				switch (event.type)
				{
				case Ape::Event::Type::MATERIAL_FILE_CREATE:
					;
					break;
				case Ape::Event::Type::MATERIAL_FILE_DELETE:
					;
					break;
				case Ape::Event::Type::MATERIAL_FILE_FILENAME:
					;
					break;
				case Ape::Event::Type::MATERIAL_FILE_SETASSKYBOX:
				{
					if (Ogre::MaterialManager::getSingleton().resourceExists(materialName))
						mpOgreSceneManager->setSkyBox(true, materialName);
				}
					break;
				case Ape::Event::Type::MATERIAL_FILE_TEXTURE:
				{
					if (auto texture = materialFile->getPassTexture().lock())
					{
						auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(texture->getName());
						if (!ogreTexture.isNull() && !ogreMaterial.isNull())
							ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(ogreTexture);
					}
				}
				break;
				case Ape::Event::Type::MATERIAL_FILE_GPUPARAMETERS:
				{
					if (!ogreMaterial.isNull())
					{
						Ogre::GpuProgramParametersSharedPtr ogreGpuParameters = ogreMaterial->getTechnique(0)->getPass(0)->getVertexProgramParameters();
						if (!ogreGpuParameters.isNull())
						{
							for (auto passGpuParameter : materialFile->getPassGpuParameters())
								ogreGpuParameters->setNamedConstant(passGpuParameter.name, ConversionToOgre(passGpuParameter.value));
						}
					}
				}
				break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::MATERIAL_MANUAL)
		{
			if (auto materialManual = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string materialName = materialManual->getName();
				auto result = Ogre::MaterialManager::getSingleton().createOrRetrieve(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				Ogre::MaterialPtr ogreMaterial = result.first.staticCast<Ogre::Material>();
				switch (event.type)
				{
				case Ape::Event::Type::MATERIAL_MANUAL_CREATE:
					;
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_DELETE:
					;
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_DIFFUSE:
					ogreMaterial->setDiffuse(ConversionToOgre(materialManual->getDiffuseColor()));
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_SPECULAR:
					ogreMaterial->setSpecular(ConversionToOgre(materialManual->getSpecularColor()));
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_AMBIENT:
					ogreMaterial->setAmbient(ConversionToOgre(materialManual->getAmbientColor()));
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_EMISSIVE:
					ogreMaterial->setSelfIllumination(ConversionToOgre(materialManual->getEmissiveColor()));
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_PASS:
					{
						if (auto pass = materialManual->getPass().lock())
						{
							Ogre::MaterialManager::getSingleton().remove(materialName);
							auto ogrePassMaterial = Ogre::MaterialManager::getSingleton().getByName(pass->getName());
							if (!ogrePassMaterial.isNull())
								ogrePassMaterial->clone(materialName);
						}
					}
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_TEXTURE:
					{
						if (auto texture = materialManual->getPassTexture().lock())
						{
							auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(texture->getName());
							if (!ogreTexture.isNull() && !ogreMaterial.isNull())
							{
								if (!ogreMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates())
									ogreMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
								ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(ogreTexture);
							}
						}
					}
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_CULLINGMODE:
					{
						 ogreMaterial->setCullingMode(Ape::ConversionToOgre(materialManual->getCullingMode()));
					}
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_DEPTHBIAS:
					{
						ogreMaterial->setDepthBias(materialManual->getDepthBias().x, materialManual->getDepthBias().x);
					}
				break;
				case Ape::Event::Type::MATERIAL_MANUAL_LIGHTING:
					{
						ogreMaterial->setLightingEnabled(materialManual->getLightingEnabled());
						if (mpShaderGenerator)
							mpShaderGenerator->removeAllShaderBasedTechniques(ogreMaterial->getName());
						mpShaderGeneratorResolver->appendIgnoreList(ogreMaterial->getName());
					}
				break;
				case Ape::Event::Type::MATERIAL_MANUAL_SCENEBLENDING:
					{
						ogreMaterial->setCullingMode(Ape::ConversionToOgre(materialManual->getCullingMode()));
						ogreMaterial->setSceneBlending(Ape::ConversionToOgre(materialManual->getSceneBlendingType()));
						if (materialManual->getSceneBlendingType() == Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA)
							ogreMaterial->setDepthWriteEnabled(false);
					}
					break;
				case Ape::Event::Type::MATERIAL_MANUAL_OVERLAY:
					{
						auto overlay = Ogre::OverlayManager::getSingleton().getByName(materialName);
						if (materialManual->isShowOnOverlay())
						{
							if (!overlay)
							{
								auto overlayPanelElement = static_cast<Ogre::PanelOverlayElement*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", materialName));
								overlayPanelElement->setMetricsMode(Ogre::GMM_RELATIVE);
								overlayPanelElement->setMaterialName(materialName);
								overlayPanelElement->setDimensions(1, 1);
								overlayPanelElement->setPosition(0, 0);
								overlay = Ogre::OverlayManager::getSingleton().create(materialName);
								overlay->add2D(overlayPanelElement);
								overlay->setZOrder(materialManual->getZOrder());
							}
							overlay->show();
						}
						else if(overlay)
							overlay->hide();
					}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::PASS_PBS)
		{
			if (auto passPbs = std::static_pointer_cast<Ape::IPbsPass>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string passPbsName = passPbs->getName();
				auto result = Ogre::MaterialManager::getSingleton().createOrRetrieve(passPbsName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				Ogre::MaterialPtr ogrePbsPassMaterial = result.first.staticCast<Ogre::Material>();
				if (!ogrePbsPassMaterial.isNull())
				{
					switch (event.type)
					{
					case Ape::Event::Type::PASS_PBS_CREATE:
					{
						ogrePbsPassMaterial->createTechnique()->createPass();
						Ogre::PbsMaterial* ogrePbsMaterial = new Ogre::PbsMaterial();
						mPbsMaterials[passPbsName] = ogrePbsMaterial;
					}
						break;
					case Ape::Event::Type::PASS_PBS_AMBIENT:
						ogrePbsPassMaterial->setAmbient(ConversionToOgre(passPbs->getAmbientColor()));
						break;
					case Ape::Event::Type::PASS_PBS_DIFFUSE:
						ogrePbsPassMaterial->setDiffuse(ConversionToOgre(passPbs->getDiffuseColor()));
						break;
					case Ape::Event::Type::PASS_PBS_EMISSIVE:
						ogrePbsPassMaterial->setSelfIllumination(ConversionToOgre(passPbs->getEmissiveColor()));
						break;
					case Ape::Event::Type::PASS_PBS_SPECULAR:
						ogrePbsPassMaterial->setSpecular(ConversionToOgre(passPbs->getSpecularColor()));
						break;
					case Ape::Event::Type::PASS_PBS_SHININESS:
						ogrePbsPassMaterial->setShininess(passPbs->getShininess());
						break;
					case Ape::Event::Type::PASS_PBS_ALBEDO:
						mPbsMaterials[passPbsName]->setAlbedo(ConversionToOgre(passPbs->getAlbedo()));
						break;
					case Ape::Event::Type::PASS_PBS_F0:
						mPbsMaterials[passPbsName]->setF0(ConversionToOgre(passPbs->getF0()));
						break;
					case Ape::Event::Type::PASS_PBS_ROUGHNESS:
						mPbsMaterials[passPbsName]->setRoughness(passPbs->getRoughness());
						break;
					case Ape::Event::Type::PASS_PBS_LIGHTROUGHNESSOFFSET:
						mPbsMaterials[passPbsName]->setLightRoughnessOffset(passPbs->getLightRoughnessOffset());
						break;
					case Ape::Event::Type::PASS_PBS_DELETE:
						;
						break;
					}
				}
			}
		}
		else if (event.group == Ape::Event::Group::PASS_MANUAL)
		{
			if (auto passManual = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string passManualName = passManual->getName();
				auto result = Ogre::MaterialManager::getSingleton().createOrRetrieve(passManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				Ogre::MaterialPtr ogreManualPassMaterial = result.first.staticCast<Ogre::Material>();
				if (!ogreManualPassMaterial.isNull())
				{
					switch (event.type)
					{
					case Ape::Event::Type::PASS_MANUAL_CREATE:
						ogreManualPassMaterial->createTechnique()->createPass();
						break;
					case Ape::Event::Type::PASS_MANUAL_AMBIENT:
						ogreManualPassMaterial->setAmbient(ConversionToOgre(passManual->getAmbientColor()));
						break;
					case Ape::Event::Type::PASS_MANUAL_DIFFUSE:
						ogreManualPassMaterial->setDiffuse(ConversionToOgre(passManual->getDiffuseColor()));
						break;
					case Ape::Event::Type::PASS_MANUAL_EMISSIVE:
						ogreManualPassMaterial->setSelfIllumination(ConversionToOgre(passManual->getEmissiveColor()));
						break;
					case Ape::Event::Type::PASS_MANUAL_SPECULAR:
						ogreManualPassMaterial->setSpecular(ConversionToOgre(passManual->getSpecularColor()));
						break;
					case Ape::Event::Type::PASS_MANUAL_SHININESS:
						ogreManualPassMaterial->setShininess(passManual->getShininess());
						break;
					case Ape::Event::Type::PASS_MANUAL_SCENEBLENDING:
					{
						ogreManualPassMaterial->setSceneBlending(ConversionToOgre(passManual->getSceneBlendingType()));
						if (passManual->getSceneBlendingType() == Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA)
							ogreManualPassMaterial->setDepthWriteEnabled(false);
					}
						break;
					case Ape::Event::Type::PASS_MANUAL_TEXTURE:
					{
						if (auto texture = passManual->getTexture().lock())
						{
							auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(texture->getName());
							if (!ogreTexture.isNull())
							{
								if (!ogreManualPassMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates())
									ogreManualPassMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
								ogreManualPassMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(ogreTexture);
							}
						}
					}
						break;
					case Ape::Event::Type::PASS_MANUAL_GPUPARAMETERS:
					{
						Ogre::GpuProgramParametersSharedPtr ogreGpuParameters = ogreManualPassMaterial->getTechnique(0)->getPass(0)->getVertexProgramParameters();
						if (!ogreGpuParameters.isNull())
						{
							for (auto passGpuParameter : passManual->getPassGpuParameters())
								ogreGpuParameters->setNamedConstant(passGpuParameter.name, ConversionToOgre(passGpuParameter.value));
						}
					}
						break;
					case Ape::Event::Type::PASS_MANUAL_DELETE:
						;
						break;
					}
				}
			}
		}
		else if (event.group == Ape::Event::Group::TEXTURE_FILE)
		{
			if (auto textureManual = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string textureManualName = textureManual->getName();
				switch (event.type)
				{
				case Ape::Event::Type::TEXTURE_FILE_CREATE:
					break;
				case Ape::Event::Type::TEXTURE_FILE_FILENAME:
				{
					auto ogreTexture = Ogre::TextureManager::getSingleton().createOrRetrieve(textureManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				}
				break;
				case Ape::Event::Type::TEXTURE_FILE_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::TEXTURE_MANUAL)
		{
			if (auto textureManual = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string textureManualName = textureManual->getName();
				switch (event.type)
				{
				case Ape::Event::Type::TEXTURE_MANUAL_CREATE:
					break;
				case Ape::Event::Type::TEXTURE_MANUAL_PARAMETERS:
					{
						Ape::ManualTextureParameters parameters = textureManual->getParameters();
						auto ogreTexture = Ogre::TextureManager::getSingleton().createManual(textureManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
							Ogre::TEX_TYPE_2D, Ogre::uint(parameters.width), Ogre::uint(parameters.height), 0, Ape::ConversionToOgre(parameters.pixelFormat),
							Ape::ConversionToOgre(parameters.usage), nullptr, true, mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].fsaa, mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].fsaaHint);
						if (mOgreRenderPluginConfig.renderSystem == "OGL")
						{
							/*GLuint glid;
							ogreTexture->getCustomAttribute("GLID", &glid);
							textureManual->setGraphicsApiID((void*)glid);*/
							textureManual->setGraphicsApiID((void*)static_cast<Ogre::GLTexture*>(Ogre::TextureManager::getSingleton().getByName(textureManualName).getPointer())->getGLID());
						}
					}
					break;
				case Ape::Event::Type::TEXTURE_MANUAL_BUFFER:
				{
					auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(textureManualName);
					if (!ogreTexture.isNull())
					{
						//APE_LOG_DEBUG("TEXTURE_MANUAL_BUFFER write begin");
						Ogre::HardwarePixelBufferSharedPtr texBuf = ogreTexture->getBuffer();
						texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
						memcpy(texBuf->getCurrentLock().data, textureManual->getBuffer(), textureManual->getParameters().width * textureManual->getParameters().height * 4);
						texBuf->unlock();
						/*static int s = 1;
						std::wostringstream oss;
						oss << std::setw(4) << std::setfill(L'0') << s++ << L".bmp";
						Ape::SaveVoidBufferToImage(oss.str(), textureManual->getBuffer(), textureManual->getParameters().width, textureManual->getParameters().height);*/
						//APE_LOG_DEBUG("TEXTURE_MANUAL_BUFFER write end");
					}
				}
				break;
				case Ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA:
					{
						auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(textureManualName);
						if (!ogreTexture.isNull())
						{
							if (auto camera = textureManual->getSourceCamera().lock())
							{
								if (auto ogreCamera = mpOgreSceneManager->getCamera(camera->getName()))
								{
									if (auto ogreRenderTexture = ogreTexture->getBuffer()->getRenderTarget())
									{
										ogreRenderTexture->setAutoUpdated(true);
										if (auto ogreViewport = ogreRenderTexture->addViewport(ogreCamera))
										{
											//ogreViewport->setClearEveryFrame(true);
											//ogreViewport->setAutoUpdated(true);
											if (mOgreRenderPluginConfig.shading == "perPixel" || mOgreRenderPluginConfig.shading == "")
												ogreViewport->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
											mRttList.push_back(textureManual);
										}
									}
								}
							}
						}
					}
					break;
				case Ape::Event::Type::TEXTURE_MANUAL_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::TEXTURE_UNIT)
		{
			if (auto textureUnit = std::static_pointer_cast<Ape::IUnitTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string textureUnitName = textureUnit->getName();
				Ape::IUnitTexture::Parameters parameters = textureUnit->getParameters();
				Ogre::MaterialPtr ogreMaterial;
				if (auto material = parameters.material.lock())
					ogreMaterial = Ogre::MaterialManager::getSingletonPtr()->getByName(material->getName());
				switch (event.type)
				{
				case Ape::Event::Type::TEXTURE_UNIT_CREATE:
					break;
				case Ape::Event::Type::TEXTURE_UNIT_PARAMETERS:
					{
						if (!ogreMaterial.isNull())
							ogreMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(parameters.fileName);
					}
				break;
				case Ape::Event::Type::TEXTURE_UNIT_SCROLL:
					{
						if (!ogreMaterial.isNull())
						{
							auto ogreTextureUnit = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
							if (ogreTextureUnit)
								ogreTextureUnit->setTextureScroll(textureUnit->getTextureScroll().x, textureUnit->getTextureScroll().y);
						}
					}
				break;
				case Ape::Event::Type::TEXTURE_UNIT_ADDRESSING:
					{
						if (!ogreMaterial.isNull())
						{
							auto ogreTextureUnit = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
							if (ogreTextureUnit)
								ogreTextureUnit->setTextureAddressingMode(Ape::ConversionToOgre(textureUnit->getTextureAddressingMode()));
						}
					}
				break;
				case Ape::Event::Type::TEXTURE_UNIT_FILTERING:
					{
						if (!ogreMaterial.isNull())
						{
							auto ogreTextureUnit = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
							if (ogreTextureUnit)
								ogreTextureUnit->setTextureFiltering(Ape::ConversionToOgre(textureUnit->getTextureFiltering().minFilter), Ape::ConversionToOgre(textureUnit->getTextureFiltering().magFilter), Ape::ConversionToOgre(textureUnit->getTextureFiltering().mipFilter));
						}
					}
				break;
				case Ape::Event::Type::TEXTURE_MANUAL_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_RAY)
		{
			if (auto geometryRay = std::static_pointer_cast<Ape::IRayGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_RAY_CREATE:
					break;
				case Ape::Event::Type::GEOMETRY_RAY_INTERSECTIONQUERY:
					{
						if (auto rayOverlayNode = geometryRay->getParentNode().lock())
						{
							if (auto raySpaceNode = rayOverlayNode->getParentNode().lock())
							{
								Ogre::Ray ray = mOgreCameras[0]->getCameraToViewportRay(rayOverlayNode->getPosition().x / mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].width,
									rayOverlayNode->getPosition().y / mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].height); // TODO: check enabled window in ogreRenderWindowConfigList
								Ogre::RaySceneQuery *raySceneQuery = mpOgreSceneManager->createRayQuery(ray, Ogre::SceneManager::ENTITY_TYPE_MASK);
								if (raySceneQuery != NULL)
								{
									raySceneQuery->setSortByDistance(true);
									raySceneQuery->execute();
									Ogre::RaySceneQueryResult query_result = raySceneQuery->getLastResults();
									std::vector<Ape::EntityWeakPtr> intersections;
									for (size_t i = 0, size = query_result.size(); i < size; ++i)
									{
										if (auto entiy = mpSceneManager->getEntity(query_result[i].movable->getName()).lock())
										{
											intersections.push_back(entiy);
										}
									}
									geometryRay->setIntersections(intersections);
								}
							}
						}
					}
					break;
				case Ape::Event::Type::GEOMETRY_RAY_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::LIGHT)
		{
			if (auto light = std::static_pointer_cast<Ape::ILight>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				Ogre::Light* ogreLight = nullptr;
				if (mpOgreSceneManager->hasLight(light->getName()))
					ogreLight = mpOgreSceneManager->getLight(light->getName());
				switch (event.type)
				{
				case Ape::Event::Type::LIGHT_CREATE:
					mpOgreSceneManager->createLight(light->getName());
					break;
				case Ape::Event::Type::LIGHT_ATTENUATION:
					ogreLight->setAttenuation(light->getLightAttenuation().range, light->getLightAttenuation().constant, light->getLightAttenuation().linear, light->getLightAttenuation().quadratic);
					break;
				case Ape::Event::Type::LIGHT_DIFFUSE:
					ogreLight->setDiffuseColour(Ape::ConversionToOgre(light->getDiffuseColor()));
					break;
				case Ape::Event::Type::LIGHT_DIRECTION:
					ogreLight->setDirection(Ape::ConversionToOgre(light->getLightDirection()));
					break;
				case Ape::Event::Type::LIGHT_SPECULAR:
					ogreLight->setSpecularColour(Ape::ConversionToOgre(light->getSpecularColor()));
					break;
				case Ape::Event::Type::LIGHT_SPOTRANGE:
					ogreLight->setSpotlightRange(Ogre::Radian(light->getLightSpotRange().innerAngle.toRadian()), Ogre::Radian(light->getLightSpotRange().outerAngle.toRadian()), light->getLightSpotRange().falloff);
					break;
				case Ape::Event::Type::LIGHT_TYPE:
					ogreLight->setType(Ape::ConversionToOgre(light->getLightType()));
					break;
				case Ape::Event::Type::LIGHT_PARENTNODE:
					{
						if (auto parentNode = light->getParentNode().lock())
						{
							if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNode->getName()))
								ogreParentNode->attachObject(ogreLight);
						}
					}
					break;
				case Ape::Event::Type::LIGHT_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::SKY)
		{
			if (auto sky = std::static_pointer_cast<Ape::ISky>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case Ape::Event::Type::SKY_CREATE:
					{

					}
				break;
				case Ape::Event::Type::SKY_SIZE:
					{
						mpSkyxBasicController = new SkyX::BasicController();
						mpSkyx = new SkyX::SkyX(mpOgreSceneManager, mpSkyxBasicController);
						mpOgreSceneManager->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
						SkyX::CfgFileManager *skyxCFG = new SkyX::CfgFileManager(mpSkyx, mpSkyxBasicController, sky->getSize());
						skyxCFG->load("SkyXDefault.skx");
						mpSkyx->create();
						mpSkyx->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(1, -1));
						mpRoot->addFrameListener(mpSkyx);
						mRenderWindows[mpMainWindow->getName()]->addListener(mpSkyx);
						mpSkyx->getGPUManager()->addGroundPass(static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Terrain"))->getTechnique(0)->createPass(), 250, Ogre::SBT_TRANSPARENT_COLOUR);
						//APE_LOG_DEBUG("skyDomeRadius:" << mpSkyx->getMeshManager()->getSkydomeRadius(ogreCamera));
						static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Terrain"))->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uLightY", mpSkyxBasicController->getSunDirection().y);
					}
				break;
				case Ape::Event::Type::SKY_TIME:
					{
						Ape::ISky::Time time = sky->getTime();
						mpSkyxBasicController->setTime(Ogre::Vector3(time.currentTime, time.sunRiseTime, time.sunSetTime));
					}
				break;
				case Ape::Event::Type::SKY_SKYLIGHT:
					{
						;
					}
				break;
				case Ape::Event::Type::SKY_SUNLIGHT:
					{
						;
					}
				break;
				case Ape::Event::Type::SKY_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::WATER)
		{
			if (auto water = std::static_pointer_cast<Ape::IWater>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case Ape::Event::Type::WATER_CREATE:
					{
						;
					}
					break;
				case Ape::Event::Type::WATER_CAMERAS:
					{
						if (auto cameraMain = water->getCameras()[0].lock())
						{
							if (mpOgreSceneManager->hasCamera(cameraMain->getName()))
							{
								if (auto ogreCamera = mpOgreSceneManager->getCamera(cameraMain->getName()))
								{
									if (auto ogreViewport = ogreCamera->getViewport())
									{
										#ifdef HYDRAX_NEW
											mpHydrax = new Hydrax::Hydrax(mpOgreSceneManager, ogreViewport);
										#else
											mpHydrax = new Hydrax::Hydrax(mpOgreSceneManager, ogreCamera, nullptr, ogreViewport, nullptr);
										#endif
									}
								}
							}
						}
						#ifdef HYDRAX_NEW
							for (int i = 1; i < water->getCameras().size(); i++)
							{
								if (auto camera = water->getCameras()[i].lock())
								{
									if (mpOgreSceneManager->hasCamera(camera->getName()))
									{
										if (auto ogreCamera = mpOgreSceneManager->getCamera(camera->getName()))
										{
											if (auto ogreViewport = ogreCamera->getViewport())
												mpHydrax->registerViewport(ogreViewport);
										}
									}
								}
							}
						#endif
						#ifdef HYDRAX_NEW
							mpHydrax->setPosition(Ogre::Vector3(0, 100, 0));//just a hotfix in order to avoid displaying the "waves" in the oculus background
							mpHydrax->setModule(new Hydrax::Module::ProjectedGrid(mpHydrax, new Hydrax::Noise::Perlin(), Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)),
								Hydrax::MaterialManager::NM_VERTEX, Hydrax::Module::ProjectedGrid::Options()), true);
							mpHydrax->setComponents(static_cast<Hydrax::HydraxComponent> (0x0014));
							mpHydrax->create();
						#else
							Hydrax::Module::ProjectedGrid *module = new Hydrax::Module::ProjectedGrid(mpHydrax, new Hydrax::Noise::Perlin(), Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)),
								Hydrax::MaterialManager::NM_VERTEX, Hydrax::Module::ProjectedGrid::Options());
							mpHydrax->setModule(static_cast<Hydrax::Module::Module*>(module));
							mpHydrax->loadCfg("HydraxDemo.hdx");
							mpHydrax->create();
						#endif
					}
					break;
				case Ape::Event::Type::WATER_SKY:
					{
						if (auto sky = water->getSky().lock())
						{
							if (auto skyLight = sky->getSkyLight().lock())
							{
								mpSkyxSkylight = mpOgreSceneManager->getLight(skyLight->getName());
								mpHydrax->setSunPosition(mpSkyxSkylight->getPosition());
							}
							if (auto sunLight = sky->getSunLight().lock())
							{
								Ape::Color sunColor = sunLight->getSpecularColor();
								mpHydrax->setSunColor(Ogre::Vector3(Ogre::Real(sunColor.r), Ogre::Real(sunColor.g), Ogre::Real(sunColor.b)));
							}
							/*Ape::Color waterColor;
							mpHydrax->setWaterColor(Ogre::Vector3(Ogre::Real(waterColor.r), Ogre::Real(waterColor.g), Ogre::Real(waterColor.b)));*/
						}
					}
				break;
				case Ape::Event::Type::WATER_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::POINT_CLOUD)
		{
			if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string pointCloudName = pointCloud->getName();
				Ape::PointCloudSetParameters pointCloudParameters = pointCloud->getParameters();
				switch (event.type)
				{
				case Ape::Event::Type::POINT_CLOUD_CREATE:
				{
					;
				}
				break;
				case Ape::Event::Type::POINT_CLOUD_PARAMETERS:
				{
					int size = pointCloudParameters.points.size() / 3;
					float* points = &pointCloudParameters.points[0];
					float* colors = &pointCloudParameters.colors[0];
					if (auto ogrePointCloudMesh = new Ape::OgrePointCloud(pointCloudName + "Mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, size, points, colors, pointCloudParameters.boundigSphereRadius))
					{
						if (auto ogreEntity = mpOgreSceneManager->createEntity(pointCloudName, pointCloudName + "Mesh"))
						{
							ogreEntity->setMaterialName("Pointcloud");
							mOgrePointCloudMeshes[pointCloudName + "Mesh"] = ogrePointCloudMesh;
						}
					}
				}
				break;
				case Ape::Event::Type::POINT_CLOUD_POINTS:
				{
					Ape::PointCloudPoints points = pointCloud->getCurrentPoints();
					int size = points.size() / 3;
					float* pPoints = &points[0];
					mOgrePointCloudMeshes[pointCloudName + "Mesh"]->updateVertexPositions(size, pPoints);
				}
				break;
				case Ape::Event::Type::POINT_CLOUD_COLORS:
				{
					Ape::PointCloudColors colors = pointCloud->getCurrentColors();
					int size = colors.size() / 3;
					float* pColors = &colors[0];
					mOgrePointCloudMeshes[pointCloudName + "Mesh"]->updateVertexColours(size, pColors);
				}
				break;
				case Ape::Event::Type::POINT_CLOUD_PARENTNODE:
				{
					if (mpOgreSceneManager->hasEntity(pointCloudName))
					{
						if (auto ogreEntity = mpOgreSceneManager->getEntity(pointCloudName))
						{
							if (auto parentNode = pointCloud->getParentNode().lock())
							{
								std::string parentNodeName = parentNode->getName();
								if (mpOgreSceneManager->hasSceneNode(parentNodeName))
								{
									if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
									{
										if (ogreEntity->getParentSceneNode())
											ogreEntity->detachFromParent();
										ogreParentNode->attachObject(ogreEntity);
									}
								}
							}
						}
					}
				}
				break;
				case Ape::Event::Type::POINT_CLOUD_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::CAMERA)
		{
			if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case Ape::Event::Type::CAMERA_CREATE:
					{
						mOgreCameras.push_back(mpOgreSceneManager->createCamera(event.subjectName));
					}
					break;
				case Ape::Event::Type::CAMERA_WINDOW:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
					{
						if (auto ogreCamera = mpOgreSceneManager->getCamera(event.subjectName))
						{

							//begin: cave mosaic case :), for sure, waiting for ApeViewport class :)
							//int zorder = (mOgreCameras.size() - 1);
							/*float width = 1;
							float height = 1;
							float left = 0;
							float top = 0;
							APE_LOG_DEBUG("camera: " << ogreCamera->getName() << " left: " << left << " width: " << width << " top: " << top << " zorder: " << zorder);*/
							//if (mCameraCountFromConfig > 2) 
							//{
							//	width = 1.0f / (float)mCameraCountFromConfig;
							//	left = zorder * width;
							//	APE_LOG_DEBUG("camera: " << ogreCamera->getName() << " left: " << left << " width: " << width << " top: " << top << " zorder: " << zorder);
							//}
							//if (auto viewPort = mRenderWindows[mpMainWindow->getName()]->addViewport(ogreCamera, zorder, left, top, width, height))
							//end: cave mosaic case :), for sure, waiting for ApeViewport class :)

							if (auto viewPort = mRenderWindows[camera->getWindow()]->addViewport(ogreCamera))
							{
								//TODO why it is working instead of in the init phase?
								APE_LOG_DEBUG("ogreCamera->setAspectRatio: width: " << viewPort->getActualWidth() << " height: " << viewPort->getActualHeight() << " left: " << viewPort->getActualLeft());
								ogreCamera->setAspectRatio(Ogre::Real(viewPort->getActualWidth()) / Ogre::Real(viewPort->getActualHeight()));
								if (mOgreRenderPluginConfig.shading == "perPixel" || mOgreRenderPluginConfig.shading == "")
								{
									if (mOgreCameras.size() == 1) //because Ogre::RTShader init needed only once and this is the right time to do this :)
									{
										if (Ogre::RTShader::ShaderGenerator::initialize())
										{
											mpShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
											mpShaderGenerator->addSceneManager(mpOgreSceneManager);
											mpShaderGeneratorResolver = new Ape::ShaderGeneratorResolver(mpShaderGenerator);
											mpShaderGeneratorResolver->appendIgnoreList("FlatVertexColorLighting");
											Ogre::MaterialManager::getSingleton().addListener(mpShaderGeneratorResolver);
											Ogre::RTShader::RenderState* pMainRenderState = mpShaderGenerator->createOrRetrieveRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME).first;
											pMainRenderState->reset();
											pMainRenderState->addTemplateSubRenderState(mpShaderGenerator->createSubRenderState(Ogre::RTShader::PerPixelLighting::Type));
											mpShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
										}
										else
											APE_LOG_DEBUG("Problem in the RTSS init");
									}
									viewPort->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
								}
							}
						}
					}
				}
				break;
				case Ape::Event::Type::CAMERA_PARENTNODE:
				{
					if (auto ogreCamera = mpOgreSceneManager->getCamera(camera->getName()))
					{
						if (auto parentNode = camera->getParentNode().lock())
						{
							if (mpOgreSceneManager->hasSceneNode(parentNode->getName()))
							{
								if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNode->getName()))
								{
									if (ogreCamera->getParentNode())
										ogreCamera->detachFromParent();
									ogreParentNode->attachObject(ogreCamera);
								}
							}
						}
					}
				}
					break;
				case Ape::Event::Type::CAMERA_DELETE:
					;
					break;
				case Ape::Event::Type::CAMERA_FOCALLENGTH:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setFocalLength(camera->getFocalLength());
				}
					break;
				case Ape::Event::Type::CAMERA_ASPECTRATIO:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setAspectRatio(camera->getAspectRatio());
				}
					break;
				case Ape::Event::Type::CAMERA_AUTOASPECTRATIO:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setAutoAspectRatio(camera->isAutoAspectRatio());
				}
					break;
				case Ape::Event::Type::CAMERA_FOVY:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setFOVy(ConversionToOgre(camera->getFOVy()));
				}
					break;
				case Ape::Event::Type::CAMERA_FRUSTUMOFFSET:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setFrustumOffset(Ape::ConversionToOgre(camera->getFrustumOffset()));
				}
					break;
				case Ape::Event::Type::CAMERA_FARCLIP:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setFarClipDistance(camera->getFarClipDistance());
				}
					break;
				case Ape::Event::Type::CAMERA_NEARCLIP:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setNearClipDistance(camera->getNearClipDistance());
				}
					break;
				case Ape::Event::Type::CAMERA_PROJECTION:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setCustomProjectionMatrix(true, Ape::ConversionToOgre(camera->getProjection()));
				}
					break;
				case Ape::Event::Type::CAMERA_PROJECTIONTYPE:
					{
						if (mpOgreSceneManager->hasCamera(event.subjectName))
							mpOgreSceneManager->getCamera(event.subjectName)->setProjectionType(ConversionToOgre(camera->getProjectionType()));
					}
					break;
				case Ape::Event::Type::CAMERA_ORTHOWINDOWSIZE:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setOrthoWindow(camera->getOrthoWindowSize().x, camera->getOrthoWindowSize().y);
				}
				break;
				}
			}
		}
		mEventDoubleQueue.pop();
	}
}

bool Ape::OgreRenderPlugin::shouldInject(Ogre::LodWorkQueueRequest* request)
{
	return true;
}

void Ape::OgreRenderPlugin::injectionCompleted(Ogre::LodWorkQueueRequest* request)
{
	std::string meshEntityName = mpCurrentlyLoadingMeshEntity->getName();
	mpOgreSceneManager->destroyEntity(mpCurrentlyLoadingMeshEntity);
	mpCurrentlyLoadingMeshEntity = mpOgreSceneManager->createEntity(meshEntityName, mCurrentlyLoadingMeshEntityLodConfig.mesh);
	mpCurrentlyLoadingMeshEntity->setMeshLodBias(mOgreRenderPluginConfig.ogreLodLevelsConfig.bias);
	std::stringstream filePath;
	filePath << mpSystemConfig->getSceneSessionConfig().sessionResourceLocation[0] << "/" << mCurrentlyLoadingMeshEntityLodConfig.mesh->getName();
	mMeshSerializer.exportMesh(mCurrentlyLoadingMeshEntityLodConfig.mesh.getPointer(), filePath.str());
}

bool Ape::OgreRenderPlugin::frameStarted( const Ogre::FrameEvent& evt )
{
	return Ogre::FrameListener::frameStarted( evt );
}

bool Ape::OgreRenderPlugin::frameRenderingQueued( const Ogre::FrameEvent& evt )
{
	//if (mRenderWindows.size() > 0)
	//{
	//	APE_LOG_DEBUG("FPS: " << mRenderWindows.begin()->second->getLastFPS() << " triangles: " << mRenderWindows.begin()->second->getTriangleCount() << " batches: " << mRenderWindows.begin()->second->getBatchCount());
	//}
	processEventDoubleQueue();
	if (mpHydrax && mpSkyxSkylight)
	{
		mpHydrax->setSunPosition(mpSkyxSkylight->getPosition());
		mpHydrax->update(evt.timeSinceLastFrame);
	}
	return Ogre::FrameListener::frameRenderingQueued( evt );
}

bool Ape::OgreRenderPlugin::frameEnded( const Ogre::FrameEvent& evt )
{
	if (mRttList.size())
	{
		for (auto it = mRttList.begin(); it != mRttList.end(); ++it)
		{
			if (auto textureManual = (*it).lock())
			{
				auto functionList = textureManual->getFunctionList();
				for (auto it : functionList)
				{
					it();
				}
			}
		}
	}
	return Ogre::FrameListener::frameEnded( evt );
}

void Ape::OgreRenderPlugin::Stop()
{
	
}

void Ape::OgreRenderPlugin::Suspend()
{
	
}

void Ape::OgreRenderPlugin::Restart()
{
	
}

void Ape::OgreRenderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	try
	{
        mpRoot->startRendering();
	}
	catch (const Ogre::RenderingAPIException& ex)
	{
		APE_LOG_ERROR(ex.getFullDescription());
	}
	catch (const Ogre::Exception& ex)
	{
		APE_LOG_ERROR(ex.getFullDescription());
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::OgreRenderPlugin::Step()
{
	try
	{
		mpRoot->renderOneFrame();
#ifndef __APPLE__
		Ogre::WindowEventUtilities::messagePump();
#endif
	}
	catch (const Ogre::RenderingAPIException& ex)
	{
		APE_LOG_ERROR(ex.getFullDescription());
	}
	catch (const Ogre::Exception& ex)
	{
		APE_LOG_ERROR(ex.getFullDescription());
	}
}

void Ape::OgreRenderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	if (auto userNode = mpSceneManager->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		mUserNode = userNode;
		if (auto headNode = mpSceneManager->getNode(userNode->getName() + "_HeadNode").lock())
		{
			mHeadNode = headNode;
		}
	}

	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "\\ApeOgreRenderPlugin.json";
	FILE* apeOgreRenderPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeOgreRenderPluginConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeOgreRenderPluginConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& renderSystem = jsonDocument["renderSystem"];
			mOgreRenderPluginConfig.renderSystem = renderSystem.GetString();
			rapidjson::Value& lodLevels = jsonDocument["lodLevels"];
			for (rapidjson::Value::MemberIterator lodLevelsMemberIterator =
				lodLevels.MemberBegin(); lodLevelsMemberIterator != lodLevels.MemberEnd(); ++lodLevelsMemberIterator)
			{
				if (lodLevelsMemberIterator->name == "autoGenerateAndSave")
					mOgreRenderPluginConfig.ogreLodLevelsConfig.autoGenerateAndSave = lodLevelsMemberIterator->value.GetBool();
				else if (lodLevelsMemberIterator->name == "bias")
					mOgreRenderPluginConfig.ogreLodLevelsConfig.bias = lodLevelsMemberIterator->value.GetFloat();
			}
			if (jsonDocument.HasMember("shading"))
			{
				rapidjson::Value& shading = jsonDocument["shading"];
				mOgreRenderPluginConfig.shading = shading.GetString();
			}
			rapidjson::Value& renderWindows = jsonDocument["renderWindows"];
			for (auto& renderWindow : renderWindows.GetArray())
			{
				Ape::OgreRenderWindowConfig ogreRenderWindowConfig;
				for (rapidjson::Value::MemberIterator renderWindowMemberIterator = 
					renderWindow.MemberBegin(); renderWindowMemberIterator != renderWindow.MemberEnd(); ++renderWindowMemberIterator)
				{
					if (renderWindowMemberIterator->name == "enable")
						ogreRenderWindowConfig.enable = renderWindowMemberIterator->value.GetBool();
					else if (renderWindowMemberIterator->name == "name")
						ogreRenderWindowConfig.name = renderWindowMemberIterator->value.GetString();
					else if (renderWindowMemberIterator->name == "monitorIndex")
						ogreRenderWindowConfig.monitorIndex = renderWindowMemberIterator->value.GetInt();
					else if (renderWindowMemberIterator->name == "resolution")
					{
						for (rapidjson::Value::MemberIterator resolutionMemberIterator = 
							renderWindow[renderWindowMemberIterator->name].MemberBegin();
							resolutionMemberIterator != renderWindow[renderWindowMemberIterator->name].MemberEnd(); ++resolutionMemberIterator)
						{
							if (resolutionMemberIterator->name == "width")
								ogreRenderWindowConfig.width = resolutionMemberIterator->value.GetInt();
							else if (resolutionMemberIterator->name == "height")
								ogreRenderWindowConfig.height = resolutionMemberIterator->value.GetInt();
							else if (resolutionMemberIterator->name == "fullScreen")
								ogreRenderWindowConfig.fullScreen = resolutionMemberIterator->value.GetBool();
						}
					}
					else if (renderWindowMemberIterator->name == "miscParams")
					{
						for (rapidjson::Value::MemberIterator miscParamsMemberIterator =
							renderWindow[renderWindowMemberIterator->name].MemberBegin();
							miscParamsMemberIterator != renderWindow[renderWindowMemberIterator->name].MemberEnd(); ++miscParamsMemberIterator)
						{
							if (miscParamsMemberIterator->name == "vSync")
								ogreRenderWindowConfig.vSync = miscParamsMemberIterator->value.GetBool();
							else if (miscParamsMemberIterator->name == "vSyncInterval")
								ogreRenderWindowConfig.vSyncInterval = miscParamsMemberIterator->value.GetInt();
							else if (miscParamsMemberIterator->name == "colorDepth")
								ogreRenderWindowConfig.colorDepth = miscParamsMemberIterator->value.GetInt();
							else if (miscParamsMemberIterator->name == "FSAA")
								ogreRenderWindowConfig.fsaa = miscParamsMemberIterator->value.GetInt();
							else if (miscParamsMemberIterator->name == "FSAAHint")
								ogreRenderWindowConfig.fsaaHint = miscParamsMemberIterator->value.GetString();
						}
					}
					else if (renderWindowMemberIterator->name == "viewports")
					{
						rapidjson::Value& viewports = renderWindow[renderWindowMemberIterator->name];
						for (auto& viewport : viewports.GetArray())
						{
							Ape::OgreViewPortConfig ogreViewPortConfig;
							for (rapidjson::Value::MemberIterator viewportMemberIterator =
								viewport.MemberBegin();
								viewportMemberIterator != viewport.MemberEnd(); ++viewportMemberIterator)
							{
								if (viewportMemberIterator->name == "zOrder")
									ogreViewPortConfig.zOrder = viewportMemberIterator->value.GetInt();
								else if (viewportMemberIterator->name == "left")
									ogreViewPortConfig.left = viewportMemberIterator->value.GetInt();
								else if (viewportMemberIterator->name == "top")
									ogreViewPortConfig.top = viewportMemberIterator->value.GetInt();
								else if (viewportMemberIterator->name == "width")
									ogreViewPortConfig.width = viewportMemberIterator->value.GetInt();
								else if (viewportMemberIterator->name == "height")
									ogreViewPortConfig.height = viewportMemberIterator->value.GetInt();
								else if (viewportMemberIterator->name == "camera")
								{
									for (rapidjson::Value::MemberIterator cameraMemberIterator =
										viewport[viewportMemberIterator->name].MemberBegin();
										cameraMemberIterator != viewport[viewportMemberIterator->name].MemberEnd(); ++cameraMemberIterator)
									{
										if (cameraMemberIterator->name == "name")
											ogreViewPortConfig.camera.name = cameraMemberIterator->value.GetString();
										else if (cameraMemberIterator->name == "nearClip")
											ogreViewPortConfig.camera.nearClip = cameraMemberIterator->value.GetFloat();
										else if (cameraMemberIterator->name == "farClip")
											ogreViewPortConfig.camera.farClip = cameraMemberIterator->value.GetFloat();
										else if (cameraMemberIterator->name == "fovY")
											ogreViewPortConfig.camera.fovY = cameraMemberIterator->value.GetFloat();
										else if (cameraMemberIterator->name == "positionOffset")
										{
											for (rapidjson::Value::MemberIterator elementMemberIterator =
												viewport[viewportMemberIterator->name][cameraMemberIterator->name].MemberBegin();
												elementMemberIterator != viewport[viewportMemberIterator->name][cameraMemberIterator->name].MemberEnd(); ++elementMemberIterator)
											{
												if (elementMemberIterator->name == "x")
													ogreViewPortConfig.camera.positionOffset.x = elementMemberIterator->value.GetFloat();
												else if (elementMemberIterator->name == "y")
													ogreViewPortConfig.camera.positionOffset.y = elementMemberIterator->value.GetFloat();
												else if (elementMemberIterator->name == "z")
													ogreViewPortConfig.camera.positionOffset.z = elementMemberIterator->value.GetFloat();
											}
										}
										else if (cameraMemberIterator->name == "orientationOffset")
										{
											Ogre::Quaternion orientationOffset;
											Ogre::Degree angle;
											Ogre::Vector3 axis;
											for (rapidjson::Value::MemberIterator elementMemberIterator =
												viewport[viewportMemberIterator->name][cameraMemberIterator->name].MemberBegin();
												elementMemberIterator != viewport[viewportMemberIterator->name][cameraMemberIterator->name].MemberEnd(); ++elementMemberIterator)
											{
												if (elementMemberIterator->name == "angle")
													angle = elementMemberIterator->value.GetFloat();
												else if (elementMemberIterator->name == "x")
													axis.x = elementMemberIterator->value.GetFloat();
												else if (elementMemberIterator->name == "y")
													axis.y = elementMemberIterator->value.GetFloat();
												else if (elementMemberIterator->name == "z")
													axis.z = elementMemberIterator->value.GetFloat();
											}
											orientationOffset.FromAngleAxis(angle, axis);
											ogreViewPortConfig.camera.orientationOffset = Ape::ConversionFromOgre(orientationOffset);
										}
										else if (cameraMemberIterator->name == "parentNodeName")
										{
											ogreViewPortConfig.camera.parentNodeName = cameraMemberIterator->value.GetString();
										}
									}
									mCameraCountFromConfig++;
								}
							}
							ogreRenderWindowConfig.viewportList.push_back(ogreViewPortConfig);
						}
					}
				}
				mOgreRenderPluginConfig.ogreRenderWindowConfigList.push_back(ogreRenderWindowConfig);
			}
		}
		fclose(apeOgreRenderPluginConfigFile);
	}	
	
	Ogre::LogManager* lm = new Ogre::LogManager();
	lm->createLog("ApeOgreRenderPlugin.log", true, false, false);
	mpRoot = new Ogre::Root("", "", "");
	//Ogre::LogManager::getSingleton().createLog("ApeOgreRenderPlugin.log", true, false, false);

	#if defined (_DEBUG)
		Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
		if (mOgreRenderPluginConfig.renderSystem == "DX11")
			mpRoot->loadPlugin( "RenderSystem_Direct3D11_d" );
		else if (mOgreRenderPluginConfig.renderSystem == "DX9")
			mpRoot->loadPlugin("RenderSystem_Direct3D9_d");
		else 
			mpRoot->loadPlugin( "RenderSystem_GL_d" );
		mpRoot->loadPlugin("Plugin_CgProgramManager_d");
	#else
		Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);
		if (mOgreRenderPluginConfig.renderSystem == "DX11")
			mpRoot->loadPlugin("RenderSystem_Direct3D11");
		else if (mOgreRenderPluginConfig.renderSystem == "DX9")
			mpRoot->loadPlugin("RenderSystem_Direct3D9");
		else
			mpRoot->loadPlugin("RenderSystem_GL");
		mpRoot->loadPlugin("Plugin_CgProgramManager");
	#endif
    
	Ogre::RenderSystem* renderSystem = nullptr;
	if (mOgreRenderPluginConfig.renderSystem == "DX11")
		renderSystem = mpRoot->getRenderSystemByName("Direct3D11 Rendering Subsystem");
	else if (mOgreRenderPluginConfig.renderSystem == "DX9")
		renderSystem = mpRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
	else
		renderSystem = mpRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	
    

	std::stringstream mediaFolder;
	mediaFolder << APE_SOURCE_DIR << "/plugins/ogreRender/media";

	mpRoot->setRenderSystem(renderSystem);
	if (mOgreRenderPluginConfig.renderSystem == "DX9")
	{
		renderSystem->setConfigOption("Resource Creation Policy", "Create on all devices");
		renderSystem->setConfigOption("Multi device memory hint", "Auto hardware buffers management");
	}

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/fonts", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/materials", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/pbs", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/Cg", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/GLSL", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/GLSL150", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/GLSLES", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/HLSL", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/materials", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/hydrax", "FileSystem", "Hydrax");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/skyx", "FileSystem", "Skyx");
	for (auto resourceLocation : mpSystemConfig->getSceneSessionConfig().sessionResourceLocation)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourceLocation, "FileSystem");
	
	mpRoot->initialise(false, "Ape");
	mpOgreSceneManager = mpRoot->createSceneManager(Ogre::ST_GENERIC);

	mpRoot->addFrameListener(this);

	Ogre::RenderWindowList renderWindowList;
	Ogre::RenderWindowDescriptionList winDescList;
	void* mainWindowHnd = 0;
	for (int i = 0; i < mOgreRenderPluginConfig.ogreRenderWindowConfigList.size(); i++)
	{
		if (mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].enable)
		{
			Ogre::RenderWindowDescription winDesc;
			std::stringstream ss;
			ss << mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].name;
			winDesc.name = ss.str();
			winDesc.height = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].height;
			winDesc.width = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].width;
			winDesc.useFullScreen = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].fullScreen;
			std::stringstream colourDepthSS;
			colourDepthSS << mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].colorDepth;
			winDesc.miscParams["colourDepth"] = colourDepthSS.str().c_str();
			winDesc.miscParams["vsync"] = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].vSync ? "Yes" : "No";
			std::stringstream vsyncIntervalSS;
			vsyncIntervalSS << mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].vSyncInterval;
			winDesc.miscParams["vsyncInterval"] = vsyncIntervalSS.str().c_str();
			std::stringstream fsaaSS;
			fsaaSS << mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].fsaa;
			winDesc.miscParams["FSAA"] = fsaaSS.str().c_str();
			winDesc.miscParams["FSAAHint"] = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].fsaaHint;
			std::stringstream monitorIndexSS;
			monitorIndexSS << mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].monitorIndex;
			winDesc.miscParams["monitorIndex"] = monitorIndexSS.str().c_str();
			winDescList.push_back(winDesc);
			APE_LOG_DEBUG("winDesc:" << " name=" << winDesc.name << " width=" << winDesc.width << " height=" << winDesc.height << " fullScreen=" << winDesc.useFullScreen);
			mRenderWindows[winDesc.name] = mpRoot->createRenderWindow(winDesc.name, winDesc.width, winDesc.height, winDesc.useFullScreen, &winDesc.miscParams);
			mRenderWindows[winDesc.name]->setDeactivateOnFocusChange(false);
			for (int j = 0; j < mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList.size(); j++)
			{
				OgreCameraConfig cameraSetting = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].camera;
				if (auto userNode = mUserNode.lock())
				{
					std::string cameraName = userNode->getName() + cameraSetting.name;
					if (auto cameraNode = mpSceneManager->createNode(cameraName + "_Node").lock())
					{
						cameraNode->setParentNode(mHeadNode);
						auto camera = std::static_pointer_cast<Ape::ICamera>(mpSceneManager->createEntity(cameraName, Ape::Entity::Type::CAMERA).lock());
						if (camera)
						{
							//TODO why it is not ok
							//camera->setAspectRatio((float)mOgreRenderWindowConfigList[i].width / (float)mOgreRenderWindowConfigList[i].height);
							camera->setWindow(winDesc.name);
							camera->setFocalLength(1.0f);
							camera->setNearClipDistance(cameraSetting.nearClip);
							camera->setFarClipDistance(cameraSetting.farClip);
							camera->setFOVy(cameraSetting.fovY.toRadian());
							if (cameraSetting.parentNodeName == "" || cameraSetting.parentNodeName == "cameraNode")
							{
								camera->setParentNode(cameraNode);
							}
							else if (cameraSetting.parentNodeName == "userNode")
							{
								camera->setParentNode(userNode);
							}
							else
							{
								if (auto parentNode = mpSceneManager->getNode(cameraSetting.parentNodeName).lock())
								{
									camera->setParentNode(parentNode);
								}
							}
							if (auto userMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->getEntity(userNode->getName() + "_Material").lock()))
							{
								if (auto cameraConeNode = mpSceneManager->createNode(cameraName + "_ConeNode").lock())
								{
									cameraConeNode->setParentNode(cameraNode);
									cameraConeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
									if (auto cameraCone = std::static_pointer_cast<Ape::IConeGeometry>(mpSceneManager->createEntity(cameraName + "_ConeGeometry", Ape::Entity::GEOMETRY_CONE).lock()))
									{
										cameraCone->setParameters(10.0f, 30.0f, 1.0f, Ape::Vector2(1, 1));
										cameraCone->setParentNode(cameraConeNode);
										cameraCone->setMaterial(userMaterial);
									}
								}
								if (auto userNameTextNode = mpSceneManager->createNode(cameraName + "_TextNode").lock())
								{
									userNameTextNode->setParentNode(cameraNode);
									userNameTextNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
									if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity(cameraName + "_TextGeometry", Ape::Entity::GEOMETRY_TEXT).lock()))
									{
										userNameText->setCaption(cameraName);
										userNameText->setParentNode(userNameTextNode);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	mpOverlaySys = new Ogre::OverlaySystem();
	mpOgreSceneManager->addRenderQueueListener(mpOverlaySys);
	mpHlmsPbsManager = new Ogre::HlmsManager(mpOgreSceneManager);
	mpOgreMovableTextFactory = new Ape::OgreMovableTextFactory();
	mpRoot->addMovableObjectFactory(mpOgreMovableTextFactory);
	mpMeshLodGenerator = new  Ogre::MeshLodGenerator();
	mpMeshLodGenerator->_initWorkQueue();
	Ogre::LodWorkQueueInjector::getSingleton().setInjectorListener(this);
	if (mpSystemConfig->getMainWindowConfig().creator == THIS_PLUGINNAME)
	{
		int mainWindowID = 0; //first window will be the main window
		Ogre::RenderWindowDescription mainWindowDesc = winDescList[mainWindowID];
		mRenderWindows[mainWindowDesc.name]->getCustomAttribute("WINDOW", &mainWindowHnd);
		std::ostringstream windowHndStr;
		windowHndStr << mainWindowHnd;
		mOgreRenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].windowHandler = windowHndStr.str();
		mpMainWindow->setName(mainWindowDesc.name);
		mpMainWindow->setWidth(mOgreRenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].width);
		mpMainWindow->setHeight(mOgreRenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].height);
		mpMainWindow->setHandle(mainWindowHnd);
	}
	APE_LOG_FUNC_LEAVE();
}
