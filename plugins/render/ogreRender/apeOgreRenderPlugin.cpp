#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "apeOgreRenderPlugin.h"
#include "apeOgreUtilities.h"

ape::OgreRenderPlugin::OgreRenderPlugin( )
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mEventDoubleQueue = ape::DoubleQueue<Event>();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CLONE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
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
	mOgreRenderPluginConfig = ape::OgreRenderPluginConfig();
	mOgreCameras = std::vector<Ogre::Camera*>();
	mPbsMaterials = std::map<std::string, Ogre::PbsMaterial*>();
	mpHydrax = nullptr;
	mpSkyx = nullptr;
	mpSkyxSunlight = nullptr;
	mpSkyxSkylight = nullptr;
	mpSkyxBasicController = nullptr;
	mOgrePointCloudMeshes = std::map<std::string, ape::OgrePointCloud*>();
	mRttList = std::vector<ape::ManualTextureWeakPtr>();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::OgreRenderPlugin::~OgreRenderPlugin()
{
	APE_LOG_FUNC_ENTER();
	delete mpRoot;
	APE_LOG_FUNC_LEAVE();
}

void ape::OgreRenderPlugin::eventCallBack(const ape::Event& event)
{
	mEventDoubleQueue.push(event);
}

void ape::OgreRenderPlugin::createAutoGeneratedLodLevelsIfNeeded(std::string geometryName)
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

void ape::OgreRenderPlugin::processEventDoubleQueue()
{
	try
	{
	mEventDoubleQueue.swap();
	while (!mEventDoubleQueue.emptyPop())
	{
		ape::Event event = mEventDoubleQueue.front();
		if (event.group == ape::Event::Group::NODE)
		{
			if (auto node = mpSceneManager->getNode(event.subjectName).lock())
			{
				std::string nodeName = node->getName();
				if (event.type == ape::Event::Type::NODE_CREATE)
				{
					mpOgreSceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
				}
				else 
				{
					Ogre::SceneNode* ogreNode = nullptr;
					if (mpOgreSceneManager->hasSceneNode(nodeName))
						ogreNode = mpOgreSceneManager->getSceneNode(nodeName);
					if (ogreNode)
					{
						switch (event.type)
						{
						case ape::Event::Type::NODE_PARENTNODE:
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
						case ape::Event::Type::NODE_POSITION:
							ogreNode->setPosition(ape::ConversionToOgre(node->getPosition()));
							break;
						case ape::Event::Type::NODE_ORIENTATION:
							ogreNode->setOrientation(ape::ConversionToOgre(node->getOrientation()));
							break;
						case ape::Event::Type::NODE_SCALE:
							ogreNode->setScale(ape::ConversionToOgre(node->getScale()));
							break;
						case ape::Event::Type::NODE_CHILDVISIBILITY:
							ogreNode->setVisible(node->getChildrenVisibility());
							break;
						case ape::Event::Type::NODE_FIXEDYAW:
							ogreNode->setFixedYawAxis(node->isFixedYaw());
							break;
						case ape::Event::Type::NODE_INHERITORIENTATION:
							ogreNode->setInheritOrientation(node->isInheritOrientation());
							break;
						case ape::Event::Type::NODE_INITIALSTATE:
							ogreNode->setInitialState();
							break;
						case ape::Event::Type::NODE_SHOWBOUNDINGBOX:
							ogreNode->showBoundingBox(true);
							break;
						case ape::Event::Type::NODE_HIDEBOUNDINGBOX:
							ogreNode->showBoundingBox(false);
							break;
						}
					}
				}
			}
			else if (event.type == ape::Event::Type::NODE_DELETE)
			{
				Ogre::SceneNode* ogreNode = nullptr;
				if (mpOgreSceneManager->hasSceneNode(event.subjectName))
					ogreNode = mpOgreSceneManager->getSceneNode(event.subjectName);
				if (ogreNode)
					ogreNode->removeAndDestroyAllChildren();
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_FILE)
		{
			if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = geometryFile->getName();
				std::string fileName = geometryFile->getFileName();
				std::string parentNodeName = "";
				if (auto parentNode = geometryFile->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_FILE_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_FILE_PARENTNODE:
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
				case ape::Event::Type::GEOMETRY_FILE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_FILE_FILENAME:
				{
					if (fileName.find_first_of(".") != std::string::npos)
					{
						std::string fileExtension = fileName.substr(fileName.find_last_of("."));
						if (fileExtension == ".mesh")
						{
							createAutoGeneratedLodLevelsIfNeeded(geometryName);
						}
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES:
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
				case ape::Event::Type::GEOMETRY_FILE_EXPORT:
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
											//TODO_apeOgreRenderPlugin automatic filesystem check for filenames and coding conversion
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
				case ape::Event::Type::GEOMETRY_FILE_MATERIAL:
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
				case ape::Event::Type::GEOMETRY_FILE_VISIBILITY:
					{
						if (mpOgreSceneManager->hasEntity(geometryName))
						{
							if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
							{
								ogreEntity->setVisibilityFlags(geometryFile->getVisibilityFlag());
							}
						}
					}
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_PLANE)
		{
			if (auto primitive = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_PLANE_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_PLANE_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_PLANE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_PLANE_MATERIAL:
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
				case ape::Event::Type::GEOMETRY_PLANE_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						ape::GeometryPlaneParameters parameters = primitive->getParameters();
						Procedural::PlaneGenerator().setNumSegX(parameters.numSeg.x).setNumSegY(parameters.numSeg.y).setSizeX(parameters.size.x).setSizeY(parameters.size.y)
							.setUTile(parameters.tile.x).setVTile(parameters.tile.y).realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);
				}
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_BOX)
		{
			if (auto primitive = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_BOX_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_BOX_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_BOX_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_BOX_MATERIAL:
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
				case ape::Event::Type::GEOMETRY_BOX_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						ape::GeometryBoxParameters parameters = primitive->getParameters();
						Procedural::BoxGenerator().setSizeX(parameters.dimensions.x).setSizeY(parameters.dimensions.y).setSizeZ(parameters.dimensions.z)
							.realizeMesh(meshFileName.str());
					}
					createAutoGeneratedLodLevelsIfNeeded(geometryName);
				}
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_SPHERE)
		{
			if (auto primitive = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_SPHERE_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_SPHERE_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_SPHERE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_SPHERE_MATERIAL:
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
				case ape::Event::Type::GEOMETRY_SPHERE_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						ape::GeometrySphereParameters parameters = primitive->getParameters();
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
		else if (event.group == ape::Event::Group::GEOMETRY_CYLINDER)
		{
			if (auto primitive = std::static_pointer_cast<ape::ICylinderGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_CYLINDER_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_CYLINDER_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_CYLINDER_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_CYLINDER_MATERIAL:
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
				case ape::Event::Type::GEOMETRY_CYLINDER_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						ape::GeometryCylinderParameters parameters = primitive->getParameters();
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
		else if (event.group == ape::Event::Group::GEOMETRY_TORUS)
		{
			if (auto primitive = std::static_pointer_cast<ape::ITorusGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_TORUS_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_TORUS_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_TORUS_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_TORUS_MATERIAL:
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
				case ape::Event::Type::GEOMETRY_TORUS_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						ape::GeometryTorusParameters parameters = primitive->getParameters();
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
		else if (event.group == ape::Event::Group::GEOMETRY_CONE)
		{
			if (auto primitive = std::static_pointer_cast<ape::IConeGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_CONE_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_CONE_PARENTNODE:
				{
					if (mpOgreSceneManager->hasEntity(geometryName))
					{
						if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
						{
							if (mpOgreSceneManager->hasSceneNode(parentNodeName))
							{
								if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
									ogreParentNode->attachObject(ogreGeometry);
							}
						}
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_CONE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_CONE_MATERIAL:
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
				case ape::Event::Type::GEOMETRY_CONE_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						ape::GeometryConeParameters parameters = primitive->getParameters();
						Procedural::ConeGenerator().setRadius(parameters.radius)
							.setHeight(parameters.height)
							//.setNumSegBase(parameters.numSeg.x).setNumSegHeight(parameters.numSeg.y)
							//.setUTile(parameters.tile)
							.realizeMesh(meshFileName.str());
						createAutoGeneratedLodLevelsIfNeeded(geometryName);
					}
				}
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_TUBE)
		{
			if (auto primitive = std::static_pointer_cast<ape::ITubeGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = primitive->getName();
				std::string parentNodeName = "";
				if (auto parentNode = primitive->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_TUBE_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_TUBE_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_TUBE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_TUBE_MATERIAL:
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
				case ape::Event::Type::GEOMETRY_TUBE_PARAMETERS:
				{
					std::stringstream meshFileName;
					meshFileName << geometryName << ".mesh";
					if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, meshFileName.str()))
					{
						ape::GeometryTubeParameters parameters = primitive->getParameters();
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
		else if (event.group == ape::Event::Group::GEOMETRY_INDEXEDFACESET)
		{
			if (auto manual = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				ape::GeometryIndexedFaceSetParameters parameters = manual->getParameters();
				std::string geometryName = manual->getName();
				if (parameters.groupName.size())
					geometryName = parameters.groupName;
				std::string parentNodeName = "";
				if (auto parentNode = manual->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
					case ape::Event::Type::GEOMETRY_INDEXEDFACESET_CREATE:
					{

					}
					break;
					case ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARENTNODE:
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
					case ape::Event::Type::GEOMETRY_INDEXEDFACESET_DELETE:
						;
						break;
					case ape::Event::Type::GEOMETRY_INDEXEDFACESET_MATERIAL:
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
					case ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS:
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

											//TODO_apeOgreRenderPlugin maybe create new vertices because of trinagle list, instead of not accumulating the normals?
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

											//TODO_apeOgreRenderPlugin  maybe create new vertices because of trinagle list, instead of not accumulating the normals?
											normals[parameters.indices[indexIndex]] += coordinateNormal;
											normals[parameters.indices[indexIndex + 1]] += coordinateNormal;
											normals[parameters.indices[indexIndex + 2]] += coordinateNormal;

											indexIndex = indexIndex + 4;
										}
										else
										{
											//TODO_apeOgreRenderPlugin 
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
		else if (event.group == ape::Event::Group::GEOMETRY_INDEXEDLINESET)
		{
			if (auto manual = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = manual->getName();
				std::string parentNodeName = "";
				if (auto parentNode = manual->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_INDEXEDLINESET_CREATE:
					mpOgreSceneManager->createManualObject(geometryName);
					break;
				case ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARENTNODE:
				{
					if (auto ogreGeometry = mpOgreSceneManager->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_INDEXEDLINESET_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARAMETERS:
				{
					ape::GeometryIndexedLineSetParameters parameters = manual->getParameters();
					if (auto ogreManual = mpOgreSceneManager->getManualObject(geometryName))
					{
						ogreManual->begin("FlatVertexColorNoLighting", Ogre::RenderOperation::OperationType::OT_LINE_LIST);
						for (int coordinateIndex = 0; coordinateIndex < parameters.coordinates.size(); coordinateIndex = coordinateIndex + 3)
						{
							ogreManual->position(parameters.coordinates[coordinateIndex], parameters.coordinates[coordinateIndex + 1], parameters.coordinates[coordinateIndex + 2]);
							ogreManual->colour(ape::ConversionToOgre(parameters.color));
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
		else if (event.group == ape::Event::Group::GEOMETRY_TEXT)
		{
			if (auto geometryText = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = geometryText->getName();
				std::string parentNodeName = "";
				if (auto parentNode = geometryText->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_TEXT_CREATE:
				{
					if (auto ogreText = (ape::OgreMovableText*)mpOgreSceneManager->createMovableObject(geometryName, "MovableText"))
					{
						ogreText->setTextAlignment(ape::OgreMovableText::H_CENTER, ape::OgreMovableText::V_ABOVE);
						ogreText->showOnTop(false);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_TEXT_SHOWONTOP:
				{
					if (auto ogreText = (ape::OgreMovableText*)mpOgreSceneManager->getMovableObject(geometryName, "MovableText"))
					{
						if (auto textGeometry = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->getEntity(geometryName).lock()))
							ogreText->showOnTop(textGeometry->isShownOnTop());
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_TEXT_PARENTNODE:
				{
					if (auto ogreTextGeometry = (ape::OgreMovableText*)mpOgreSceneManager->getMovableObject(geometryName, "MovableText"))
					{
						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreTextGeometry);
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_TEXT_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_TEXT_CAPTION:
				{
					if (auto ogreText = (ape::OgreMovableText*)mpOgreSceneManager->getMovableObject(geometryName, "MovableText"))
					{
						if (auto textGeometry = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->getEntity(geometryName).lock()))
							ogreText->setCaption(textGeometry->getCaption());
					}
				}
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_CLONE)
		{
			if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = geometryClone->getName();
				std::string parentNodeName = "";
				std::string sourceGeometryName = "";
				if (auto parentNode = geometryClone->getParentNode().lock())
					parentNodeName = parentNode->getName();
				if (auto sourceGeometry = geometryClone->getSourceGeometry().lock())
					sourceGeometryName = sourceGeometry->getName();

				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_CLONE_CREATE:
				{
					
				}
					break;
				case ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRY:
				{
					if (mpOgreSceneManager->hasManualObject(sourceGeometryName))
					{
						auto ogreManual = mpOgreSceneManager->getManualObject(sourceGeometryName);
						std::stringstream meshName;
						meshName << sourceGeometryName << ".mesh";
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
				case ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME:
				{
					std::string geometryGroupName = geometryClone->getSourceGeometryGroupName();
					if (mpOgreSceneManager->hasManualObject(geometryGroupName))
					{
						auto ogreManual = mpOgreSceneManager->getManualObject(geometryGroupName);
						std::stringstream meshName;
						meshName << geometryGroupName << ".mesh";
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
				case ape::Event::Type::GEOMETRY_CLONE_PARENTNODE:
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
				case ape::Event::Type::GEOMETRY_CLONE_DELETE:
				{
				}
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::MATERIAL_FILE)
		{
			if (auto materialFile = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string materialFileName = materialFile->getfFileName();
				Ogre::MaterialPtr ogreMaterial;
				if (Ogre::MaterialManager::getSingleton().resourceExists(materialFileName))
				{
					ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(materialFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				}
				switch (event.type)
				{
				case ape::Event::Type::MATERIAL_FILE_CREATE:
					;
					break;
				case ape::Event::Type::MATERIAL_FILE_DELETE:
					;
					break;
				case ape::Event::Type::MATERIAL_FILE_FILENAME:
					;
					break;
				case ape::Event::Type::MATERIAL_FILE_SETASSKYBOX:
				{
					if (Ogre::MaterialManager::getSingleton().resourceExists(materialFileName))
						mpOgreSceneManager->setSkyBox(true, materialFileName);
				}
					break;
				case ape::Event::Type::MATERIAL_FILE_TEXTURE:
				{
					if (auto texture = materialFile->getTexture().lock())
					{
						auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(texture->getName());
						if (!ogreTexture.isNull() && !ogreMaterial.isNull())
							ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(ogreTexture);
					}
				}
				break;
				case ape::Event::Type::MATERIAL_FILE_GPUPARAMETERS:
				{
					if (!ogreMaterial.isNull())
					{
						Ogre::GpuProgramParametersSharedPtr ogreGpuParameters = ogreMaterial->getTechnique(0)->getPass(0)->getVertexProgramParameters();
						if (!ogreGpuParameters.isNull())
						{
							/*for (auto passGpuParameter : materialFile->getPassGpuParameters())
								ogreGpuParameters->setNamedConstant(passGpuParameter.name, ConversionToOgre(passGpuParameter.value));*/
						}
					}
				}
				break;
				}
			}
		}
		else if (event.group == ape::Event::Group::MATERIAL_MANUAL)
		{
			if (auto materialManual = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string materialName = materialManual->getName();
				auto result = Ogre::MaterialManager::getSingleton().createOrRetrieve(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				Ogre::MaterialPtr ogreMaterial = result.first.staticCast<Ogre::Material>();
				switch (event.type)
				{
				case ape::Event::Type::MATERIAL_MANUAL_CREATE:
					;
					break;
				case ape::Event::Type::MATERIAL_MANUAL_DELETE:
					;
					break;
				case ape::Event::Type::MATERIAL_MANUAL_DIFFUSE:
					ogreMaterial->setDiffuse(ConversionToOgre(materialManual->getDiffuseColor()));
					break;
				case ape::Event::Type::MATERIAL_MANUAL_SPECULAR:
					ogreMaterial->setSpecular(ConversionToOgre(materialManual->getSpecularColor()));
					break;
				case ape::Event::Type::MATERIAL_MANUAL_AMBIENT:
					ogreMaterial->setAmbient(ConversionToOgre(materialManual->getAmbientColor()));
					break;
				case ape::Event::Type::MATERIAL_MANUAL_EMISSIVE:
					ogreMaterial->setSelfIllumination(ConversionToOgre(materialManual->getEmissiveColor()));
					break;
				case ape::Event::Type::MATERIAL_MANUAL_TEXTURE:
					{
						if (auto texture = materialManual->getTexture().lock())
						{
							if (texture->getType() == ape::Entity::Type::TEXTURE_MANUAL)
							{
								auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(texture->getName());
								if (!ogreTexture.isNull() && !ogreMaterial.isNull())
								{
									if (!ogreMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates())
										ogreMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
									ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(ogreTexture);
								}
							}
							else if (texture->getType() == ape::Entity::Type::TEXTURE_FILE)
							{
								if (!ogreMaterial.isNull())
								{
									if (!ogreMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates())
									{
										auto ogreTexture = Ogre::TextureManager::getSingleton().load(std::static_pointer_cast<ape::IFileTexture>(texture)->getFileName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
										if (!ogreTexture.isNull())
										{
											ogreMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
											ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(ogreTexture);
										}
									}
									else
									{
										/*std::string oldOgreTextureName = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();
										auto oldOgreTexture = Ogre::TextureManager::getSingleton().getByName(oldOgreTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
										if (!oldOgreTexture.isNull())
										{
											oldOgreTexture->unload();
											auto newOgreTexture = Ogre::TextureManager::getSingleton().load(std::static_pointer_cast<ape::IFileTexture>(texture)->getFileName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
											if (!newOgreTexture.isNull())
											{
												ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(newOgreTexture);
												ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->retryTextureLoad();
											}
										}*/
										auto ogreTexture = Ogre::TextureManager::getSingleton().load(std::static_pointer_cast<ape::IFileTexture>(texture)->getFileName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
										if (!ogreTexture.isNull())
										{
											ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(ogreTexture);
											ogreMaterial->compile();
										}
									}
								}
							}
						}
					}
					break;
				case ape::Event::Type::MATERIAL_MANUAL_CULLINGMODE:
					{
						 ogreMaterial->setCullingMode(ape::ConversionToOgre(materialManual->getCullingMode()));
					}
					break;
				case ape::Event::Type::MATERIAL_MANUAL_DEPTHBIAS:
					{
						ogreMaterial->setDepthBias(materialManual->getDepthBias().x, materialManual->getDepthBias().x);
					}
				break;
				case ape::Event::Type::MATERIAL_MANUAL_LIGHTING:
					{
						ogreMaterial->setLightingEnabled(materialManual->getLightingEnabled());
						if (mpShaderGenerator)
							mpShaderGenerator->removeAllShaderBasedTechniques(ogreMaterial->getName());
						mpShaderGeneratorResolver->appendIgnoreList(ogreMaterial->getName());
					}
				break;
				case ape::Event::Type::MATERIAL_MANUAL_SCENEBLENDING:
					{
						ogreMaterial->setCullingMode(ape::ConversionToOgre(materialManual->getCullingMode()));
						ogreMaterial->setSceneBlending(ape::ConversionToOgre(materialManual->getSceneBlendingType()));
						if (materialManual->getSceneBlendingType() == ape::Material::SceneBlendingType::TRANSPARENT_ALPHA)
							ogreMaterial->setDepthWriteEnabled(false);
					}
					break;
				case ape::Event::Type::MATERIAL_MANUAL_OVERLAY:
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
		else if (event.group == ape::Event::Group::TEXTURE_FILE)
		{
			if (auto textureFile = std::static_pointer_cast<ape::IFileTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case ape::Event::Type::TEXTURE_FILE_CREATE:
					break;
				case ape::Event::Type::TEXTURE_FILE_FILENAME:
				{
					std::string textureFilelName = textureFile->getFileName();
					auto ogreTexture = Ogre::TextureManager::getSingleton().load(textureFilelName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
					;
				}
				break;
				case ape::Event::Type::TEXTURE_FILE_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::TEXTURE_MANUAL)
		{
			if (auto textureManual = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string textureManualName = textureManual->getName();
				switch (event.type)
				{
				case ape::Event::Type::TEXTURE_MANUAL_CREATE:
					break;
				case ape::Event::Type::TEXTURE_MANUAL_PARAMETERS:
					{
						ape::ManualTextureParameters parameters = textureManual->getParameters();
						Ogre::TexturePtr ogreTexture = Ogre::TexturePtr();
						if (parameters.useFsaa)
						{
							ogreTexture = Ogre::TextureManager::getSingleton().createManual(textureManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
								Ogre::TEX_TYPE_2D, Ogre::uint(parameters.width), Ogre::uint(parameters.height), 0, ape::ConversionToOgre(parameters.pixelFormat),
								ape::ConversionToOgre(parameters.usage), nullptr, parameters.gammaCorrection, mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].fsaa, mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].fsaaHint);
						}
						else
						{
							ogreTexture = Ogre::TextureManager::getSingleton().createManual(textureManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
								Ogre::TEX_TYPE_2D, Ogre::uint(parameters.width), Ogre::uint(parameters.height), 0, ape::ConversionToOgre(parameters.pixelFormat),
								ape::ConversionToOgre(parameters.usage), nullptr, parameters.gammaCorrection);
						}
						if (mOgreRenderPluginConfig.renderSystem == "OGL")
						{
							/*GLuint glid;
							ogreTexture->getCustomAttribute("GLID", &glid);
							textureManual->setGraphicsApiID((void*)glid);*/
							textureManual->setGraphicsApiID((void*)static_cast<Ogre::GLTexture*>(Ogre::TextureManager::getSingleton().getByName(textureManualName).getPointer())->getGLID());
						}
						if (mOgreRenderPluginConfig.renderSystem == "DX11")
						{
							textureManual->setGraphicsApiID((void*)static_cast<Ogre::D3D11Texture*>(Ogre::TextureManager::getSingleton().getByName(textureManualName).getPointer())->GetTex2D());
						}
					}
					break;
				case ape::Event::Type::TEXTURE_MANUAL_BUFFER:
				{
					auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(textureManualName);
					if (!ogreTexture.isNull())
					{
						Ogre::HardwarePixelBufferSharedPtr texBuf = ogreTexture->getBuffer();
						texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
						if (textureManual->getParameters().pixelFormat == ape::Texture::PixelFormat::A8R8G8B8 || textureManual->getParameters().pixelFormat == ape::Texture::PixelFormat::R8G8B8A8)
						{
							memcpy(texBuf->getCurrentLock().data, textureManual->getBuffer(), textureManual->getParameters().width * textureManual->getParameters().height * 4);
							//APE_LOG_DEBUG("TEXTURE_MANUAL_BUFFER write begin: " << textureManualName);
						}
						else if (textureManual->getParameters().pixelFormat == ape::Texture::PixelFormat::R8G8B8)
						{
							//APE_LOG_DEBUG("TEXTURE_MANUAL_BUFFER write begin: " << textureManualName);
							memcpy(texBuf->getCurrentLock().data, textureManual->getBuffer(), textureManual->getParameters().width * textureManual->getParameters().height * 3);
						}
						texBuf->unlock();
						/*static int s = 1;
						std::wostringstream oss;
						oss << std::setw(4) << std::setfill(L'0') << s++ << L".bmp";
						ape::SaveVoidBufferToImage(oss.str(), textureManual->getBuffer(), textureManual->getParameters().width, textureManual->getParameters().height);*/
					}
				}
				break;
				case ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA:
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
											{
												ogreViewport->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
											}
											mRttList.push_back(textureManual);
										}
									}
								}
							}
						}
					}
					break;
				case ape::Event::Type::TEXTURE_MANUAL_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::TEXTURE_UNIT)
		{
			if (auto textureUnit = std::static_pointer_cast<ape::IUnitTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string textureUnitName = textureUnit->getName();
				ape::IUnitTexture::Parameters parameters = textureUnit->getParameters();
				Ogre::MaterialPtr ogreMaterial;
				if (auto material = parameters.material.lock())
					ogreMaterial = Ogre::MaterialManager::getSingletonPtr()->getByName(material->getName());
				switch (event.type)
				{
				case ape::Event::Type::TEXTURE_UNIT_CREATE:
					break;
				case ape::Event::Type::TEXTURE_UNIT_PARAMETERS:
					{
						if (!ogreMaterial.isNull())
							ogreMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(parameters.fileName);
					}
				break;
				case ape::Event::Type::TEXTURE_UNIT_SCROLL:
					{
						if (!ogreMaterial.isNull())
						{
							auto ogreTextureUnit = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
							if (ogreTextureUnit)
								ogreTextureUnit->setTextureScroll(textureUnit->getTextureScroll().x, textureUnit->getTextureScroll().y);
						}
					}
				break;
				case ape::Event::Type::TEXTURE_UNIT_ADDRESSING:
					{
						if (!ogreMaterial.isNull())
						{
							auto ogreTextureUnit = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
							if (ogreTextureUnit)
								ogreTextureUnit->setTextureAddressingMode(ape::ConversionToOgre(textureUnit->getTextureAddressingMode()));
						}
					}
				break;
				case ape::Event::Type::TEXTURE_UNIT_FILTERING:
					{
						if (!ogreMaterial.isNull())
						{
							auto ogreTextureUnit = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
							if (ogreTextureUnit)
								ogreTextureUnit->setTextureFiltering(ape::ConversionToOgre(textureUnit->getTextureFiltering().minFilter), ape::ConversionToOgre(textureUnit->getTextureFiltering().magFilter), ape::ConversionToOgre(textureUnit->getTextureFiltering().mipFilter));
						}
					}
				break;
				case ape::Event::Type::TEXTURE_MANUAL_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_RAY)
		{
			if (auto geometryRay = std::static_pointer_cast<ape::IRayGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_RAY_CREATE:
					break;
				case ape::Event::Type::GEOMETRY_RAY_INTERSECTIONQUERY:
					{
						if (auto rayOverlayNode = geometryRay->getParentNode().lock())
						{
							if (auto raySpaceNode = rayOverlayNode->getParentNode().lock())
							{
								Ogre::Ray ray;
								if (mpOgreSceneManager->hasSceneNode(raySpaceNode->getName()))
								{
									auto ogreNode = mpOgreSceneManager->getSceneNode(raySpaceNode->getName());
									Ogre::Vector3 rayDirection = ogreNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
									ray = Ogre::Ray(ogreNode->getPosition(), rayDirection);
								}
								if (raySpaceNode == mpUserInputMacro->getUserNode().lock())
								{
									ray = mOgreCameras[0]->getCameraToViewportRay(rayOverlayNode->getPosition().x / mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].width,
										rayOverlayNode->getPosition().y / mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].height);
								}
								Ogre::RaySceneQuery *raySceneQuery = mpOgreSceneManager->createRayQuery(ray, Ogre::SceneManager::ENTITY_TYPE_MASK);
								if (raySceneQuery != NULL)
								{
									raySceneQuery->setSortByDistance(true);
									raySceneQuery->execute();
									Ogre::RaySceneQueryResult query_result = raySceneQuery->getLastResults();
									std::vector<ape::EntityWeakPtr> intersections;
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
				case ape::Event::Type::GEOMETRY_RAY_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::LIGHT)
		{
			if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				Ogre::Light* ogreLight = nullptr;
				if (mpOgreSceneManager->hasLight(light->getName()))
					ogreLight = mpOgreSceneManager->getLight(light->getName());
				switch (event.type)
				{
				case ape::Event::Type::LIGHT_CREATE:
					mpOgreSceneManager->createLight(light->getName());
					break;
				case ape::Event::Type::LIGHT_ATTENUATION:
					ogreLight->setAttenuation(light->getLightAttenuation().range, light->getLightAttenuation().constant, light->getLightAttenuation().linear, light->getLightAttenuation().quadratic);
					break;
				case ape::Event::Type::LIGHT_DIFFUSE:
					ogreLight->setDiffuseColour(ape::ConversionToOgre(light->getDiffuseColor()));
					break;
				case ape::Event::Type::LIGHT_DIRECTION:
					ogreLight->setDirection(ape::ConversionToOgre(light->getLightDirection()));
					break;
				case ape::Event::Type::LIGHT_SPECULAR:
					ogreLight->setSpecularColour(ape::ConversionToOgre(light->getSpecularColor()));
					break;
				case ape::Event::Type::LIGHT_SPOTRANGE:
					ogreLight->setSpotlightRange(Ogre::Radian(light->getLightSpotRange().innerAngle.toRadian()), Ogre::Radian(light->getLightSpotRange().outerAngle.toRadian()), light->getLightSpotRange().falloff);
					break;
				case ape::Event::Type::LIGHT_TYPE:
					ogreLight->setType(ape::ConversionToOgre(light->getLightType()));
					break;
				case ape::Event::Type::LIGHT_PARENTNODE:
					{
						if (auto parentNode = light->getParentNode().lock())
						{
							if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNode->getName()))
								ogreParentNode->attachObject(ogreLight);
						}
					}
					break;
				case ape::Event::Type::LIGHT_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::SKY)
		{
			if (auto sky = std::static_pointer_cast<ape::ISky>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case ape::Event::Type::SKY_CREATE:
					{

					}
				break;
				case ape::Event::Type::SKY_SIZE:
					{
						mpSkyxBasicController = new SkyX::BasicController();
						mpSkyx = new SkyX::SkyX(mpOgreSceneManager, mpSkyxBasicController);
						mpOgreSceneManager->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
						SkyX::CfgFileManager *skyxCFG = new SkyX::CfgFileManager(mpSkyx, mpSkyxBasicController, sky->getSize());
						skyxCFG->load("SkyXDefault.skx");
						mpSkyx->create();
						mpSkyx->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(1, -1));
						mpRoot->addFrameListener(mpSkyx);
						mRenderWindows[mpCoreConfig->getWindowConfig().name]->addListener(mpSkyx);
						mpSkyx->getGPUManager()->addGroundPass(static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Terrain"))->getTechnique(0)->createPass(), 250, Ogre::SBT_TRANSPARENT_COLOUR);
						//APE_LOG_DEBUG("skyDomeRadius:" << mpSkyx->getMeshManager()->getSkydomeRadius(ogreCamera));
						static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Terrain"))->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uLightY", mpSkyxBasicController->getSunDirection().y);
					}
				break;
				case ape::Event::Type::SKY_TIME:
					{
						ape::ISky::Time time = sky->getTime();
						mpSkyxBasicController->setTime(Ogre::Vector3(time.currentTime, time.sunRiseTime, time.sunSetTime));
					}
				break;
				case ape::Event::Type::SKY_SKYLIGHT:
					{
						;
					}
				break;
				case ape::Event::Type::SKY_SUNLIGHT:
					{
						;
					}
				break;
				case ape::Event::Type::SKY_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::WATER)
		{
			if (auto water = std::static_pointer_cast<ape::IWater>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case ape::Event::Type::WATER_CREATE:
					{
						;
					}
					break;
				case ape::Event::Type::WATER_CAMERAS:
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
				case ape::Event::Type::WATER_SKY:
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
								ape::Color sunColor = sunLight->getSpecularColor();
								mpHydrax->setSunColor(Ogre::Vector3(Ogre::Real(sunColor.r), Ogre::Real(sunColor.g), Ogre::Real(sunColor.b)));
							}
							/*ape::Color waterColor;
							mpHydrax->setWaterColor(Ogre::Vector3(Ogre::Real(waterColor.r), Ogre::Real(waterColor.g), Ogre::Real(waterColor.b)));*/
						}
					}
				break;
				case ape::Event::Type::WATER_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::POINT_CLOUD)
		{
			if (auto pointCloud = std::static_pointer_cast<ape::IPointCloud>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string pointCloudName = pointCloud->getName();
				ape::PointCloudSetParameters pointCloudParameters = pointCloud->getParameters();
				switch (event.type)
				{
				case ape::Event::Type::POINT_CLOUD_CREATE:
				{
					auto ogreMaterial = Ogre::MaterialManager::getSingletonPtr()->create(pointCloudName + "Material", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
					if (!ogreMaterial.isNull())
					{
						if (auto ogreTechnique = ogreMaterial->getTechnique(0))
						{
							if (auto ogrePass = ogreTechnique->getPass(0))
							{
								ogrePass->setLightingEnabled(false);
								ogrePass->setVertexColourTracking(Ogre::TVC_DIFFUSE);
							}
						}
					}
				}
				break;
				case ape::Event::Type::POINT_CLOUD_PARAMETERS:
				{
					int size = pointCloudParameters.points.size() / 3;
					float* points = &pointCloudParameters.points[0];
					float* colors = &pointCloudParameters.colors[0];
					if (auto ogrePointCloudMesh = new ape::OgrePointCloud(pointCloudName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
						size, points, colors, pointCloudParameters.boundigSphereRadius, mpUserInputMacro->getHeadNode(), pointCloud->getParentNode(), pointCloudParameters.pointSize,
						pointCloudParameters.pointScale, pointCloudParameters.pointScaleOffset, pointCloudParameters.unitScaleDistance, pointCloudParameters.scaleFactor))
					{
						if (auto ogreEntity = mpOgreSceneManager->createEntity(pointCloudName, pointCloudName + "Mesh"))
						{
							ogreEntity->setMaterialName(pointCloudName + "Material");
							mOgrePointCloudMeshes[pointCloudName + "Mesh"] = ogrePointCloudMesh;
						}
					}
				}
				break;
				case ape::Event::Type::POINT_CLOUD_POINTS:
				{
					ape::PointCloudPoints points = pointCloud->getCurrentPoints();
					int size = points.size() / 3;
					float* pPoints = &points[0];
					mOgrePointCloudMeshes[pointCloudName + "Mesh"]->updateVertexPositions(size, pPoints);
				}
				break;
				case ape::Event::Type::POINT_CLOUD_COLORS:
				{
					ape::PointCloudColors colors = pointCloud->getCurrentColors();
					int size = colors.size() / 3;
					float* pColors = &colors[0];
					mOgrePointCloudMeshes[pointCloudName + "Mesh"]->updateVertexColours(size, pColors);
				}
				break;
				case ape::Event::Type::POINT_CLOUD_PARENTNODE:
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
				case ape::Event::Type::POINT_CLOUD_DELETE:
					;
					break;
				}
			}
		}
		else if (event.group == ape::Event::Group::CAMERA)
		{
			if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case ape::Event::Type::CAMERA_CREATE:
				{
					mOgreCameras.push_back(mpOgreSceneManager->createCamera(event.subjectName));
					for (int i = 0; i < mOgreRenderPluginConfig.ogreRenderWindowConfigList.size(); i++)
					{
						for (int j = 0; j < mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList.size(); j++)
						{
							for (int k = 0; k < mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].cameras.size(); k++)
							{
								auto cameraSetting = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].cameras[k];
								if (cameraSetting.name == camera->getName())
								{
									camera->setWindow(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].name);
									camera->setFocalLength(1.0f);
									camera->setNearClipDistance(cameraSetting.nearClip);
									camera->setFarClipDistance(cameraSetting.farClip);
									camera->setFOVy(cameraSetting.fovY.toRadian());
								}
							}
						}
					}
				}
				break;
				case ape::Event::Type::CAMERA_WINDOW:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
					{
						if (auto ogreCamera = mpOgreSceneManager->getCamera(event.subjectName))
						{
							for (int i = 0; i < mOgreRenderPluginConfig.ogreRenderWindowConfigList.size(); i++)
							{
								for (int j = 0; j < mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList.size(); j++)
								{
									auto renderWindowSetting = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i];
									auto viewportSetting = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j];
									for (int k = 0; k < mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].cameras.size(); k++)
									{
										auto cameraSetting = mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].cameras[k];
										if (cameraSetting.name == camera->getName())
										{
											int zorder = viewportSetting.zOrder;
											float width = (float)viewportSetting.width / (float)renderWindowSetting.width;
											float height = (float)viewportSetting.height / (float)renderWindowSetting.height;
											float left = (float)viewportSetting.left / (float)renderWindowSetting.width;
											float top = (float)viewportSetting.top / (float)renderWindowSetting.height;
											if (!mRenderWindows[camera->getWindow()]->hasViewportWithZOrder(zorder))
											{
												if (auto ogreViewPort = mRenderWindows[camera->getWindow()]->addViewport(ogreCamera, zorder, left, top, width, height))
												{
													APE_LOG_DEBUG("ogreViewport: " << "zorder: " << zorder << " left: " << left << " top: " << top << " width: " << width << " height: " << height);
													ogreCamera->setAspectRatio(Ogre::Real(ogreViewPort->getActualWidth()) / Ogre::Real(ogreViewPort->getActualHeight()));
													if (mOgreRenderPluginConfig.shading == "perPixel" || mOgreRenderPluginConfig.shading == "")
													{
														ogreViewPort->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
													}
												}
											}
											else if(auto ogreViewPort = mRenderWindows[camera->getWindow()]->getViewportByZOrder(zorder))
											{
												APE_LOG_DEBUG("ogreViewport: " << "zorder: " << zorder << " left: " << left << " top: " << top << " width: " << width << " height: " << height);
												ogreViewPort->setCamera(ogreCamera);
												ogreCamera->setAspectRatio(Ogre::Real(ogreViewPort->getActualWidth()) / Ogre::Real(ogreViewPort->getActualHeight()));
												if (mOgreRenderPluginConfig.shading == "perPixel" || mOgreRenderPluginConfig.shading == "")
												{
													ogreViewPort->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
												}
											}
										}
									}
								}
							}
						}
					}
				}
				break;
				case ape::Event::Type::CAMERA_PARENTNODE:
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
				case ape::Event::Type::CAMERA_DELETE:
					;
					break;
				case ape::Event::Type::CAMERA_FOCALLENGTH:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setFocalLength(camera->getFocalLength());
				}
					break;
				case ape::Event::Type::CAMERA_ASPECTRATIO:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setAspectRatio(camera->getAspectRatio());
				}
					break;
				case ape::Event::Type::CAMERA_AUTOASPECTRATIO:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setAutoAspectRatio(camera->isAutoAspectRatio());
				}
					break;
				case ape::Event::Type::CAMERA_FOVY:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setFOVy(ConversionToOgre(camera->getFOVy()));
				}
					break;
				case ape::Event::Type::CAMERA_FRUSTUMOFFSET:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setFrustumOffset(ape::ConversionToOgre(camera->getFrustumOffset()));
				}
					break;
				case ape::Event::Type::CAMERA_FARCLIP:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setFarClipDistance(camera->getFarClipDistance());
				}
					break;
				case ape::Event::Type::CAMERA_NEARCLIP:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setNearClipDistance(camera->getNearClipDistance());
				}
					break;
				case ape::Event::Type::CAMERA_PROJECTION:
				{
					if (mpOgreSceneManager->hasCamera(event.subjectName))
						mpOgreSceneManager->getCamera(event.subjectName)->setCustomProjectionMatrix(true, ape::ConversionToOgre(camera->getProjection()));
				}
					break;
				case ape::Event::Type::CAMERA_PROJECTIONTYPE:
					{
						if (mpOgreSceneManager->hasCamera(event.subjectName))
							mpOgreSceneManager->getCamera(event.subjectName)->setProjectionType(ConversionToOgre(camera->getProjectionType()));
					}
					break;
				case ape::Event::Type::CAMERA_ORTHOWINDOWSIZE:
					{
						if (mpOgreSceneManager->hasCamera(event.subjectName))
							mpOgreSceneManager->getCamera(event.subjectName)->setOrthoWindow(camera->getOrthoWindowSize().x, camera->getOrthoWindowSize().y);
					}
					break;
				case ape::Event::Type::CAMERA_VISIBILITY:
					{
						if (mpOgreSceneManager->hasCamera(event.subjectName))
						{
							Ogre::Viewport* ogreViewport = mpOgreSceneManager->getCamera(event.subjectName)->getViewport();
							if (ogreViewport)
								ogreViewport->setVisibilityMask(camera->getVisibilityMask());
						}
					}
					break;
				}
			}
		}
		mEventDoubleQueue.pop();
	}
	}
	catch (const Ogre::Exception& ex)
	{
		APE_LOG_DEBUG(ex.getFullDescription());
	}
}

bool ape::OgreRenderPlugin::shouldInject(Ogre::LodWorkQueueRequest* request)
{
	return true;
}

void ape::OgreRenderPlugin::injectionCompleted(Ogre::LodWorkQueueRequest* request)
{
	std::string meshEntityName = mpCurrentlyLoadingMeshEntity->getName();
	mpOgreSceneManager->destroyEntity(mpCurrentlyLoadingMeshEntity);
	mpCurrentlyLoadingMeshEntity = mpOgreSceneManager->createEntity(meshEntityName, mCurrentlyLoadingMeshEntityLodConfig.mesh);
	mpCurrentlyLoadingMeshEntity->setMeshLodBias(mOgreRenderPluginConfig.ogreLodLevelsConfig.bias);
	std::stringstream filePath;
	filePath << mpCoreConfig->getNetworkConfig().resourceLocations[0] << "/" << mCurrentlyLoadingMeshEntityLodConfig.mesh->getName();
	mMeshSerializer.exportMesh(mCurrentlyLoadingMeshEntityLodConfig.mesh.getPointer(), filePath.str());
}

bool ape::OgreRenderPlugin::frameStarted( const Ogre::FrameEvent& evt )
{
	return Ogre::FrameListener::frameStarted( evt );
}

bool ape::OgreRenderPlugin::frameRenderingQueued( const Ogre::FrameEvent& evt )
{
	//std::cout << "FPS: " << mRenderWindows.begin()->second->getLastFPS() << " triangles: " << mRenderWindows.begin()->second->getTriangleCount() << " batches: " << mRenderWindows.begin()->second->getBatchCount() << std::endl;
	processEventDoubleQueue();
	if (mpHydrax && mpSkyxSkylight)
	{
		mpHydrax->setSunPosition(mpSkyxSkylight->getPosition());
		mpHydrax->update(evt.timeSinceLastFrame);
	}
	return Ogre::FrameListener::frameRenderingQueued( evt );
}

bool ape::OgreRenderPlugin::frameEnded( const Ogre::FrameEvent& evt )
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

void ape::OgreRenderPlugin::Stop()
{
	
}

void ape::OgreRenderPlugin::Suspend()
{
	
}

void ape::OgreRenderPlugin::Restart()
{
	
}

void ape::OgreRenderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	try
	{
		mpRoot->startRendering();
	}
	catch (const Ogre::Exception& ex)
	{
		APE_LOG_DEBUG(ex.getFullDescription());
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::OgreRenderPlugin::Step()
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

void ape::OgreRenderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeOgreRenderPlugin.json";
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
				ape::OgreRenderWindowConfig ogreRenderWindowConfig;
				for (rapidjson::Value::MemberIterator renderWindowMemberIterator = 
					renderWindow.MemberBegin(); renderWindowMemberIterator != renderWindow.MemberEnd(); ++renderWindowMemberIterator)
				{
					if (renderWindowMemberIterator->name == "enable")
						ogreRenderWindowConfig.enable = renderWindowMemberIterator->value.GetBool();
					else if (renderWindowMemberIterator->name == "name")
						ogreRenderWindowConfig.name = renderWindowMemberIterator->value.GetString();
					else if (renderWindowMemberIterator->name == "monitorIndex")
						ogreRenderWindowConfig.monitorIndex = renderWindowMemberIterator->value.GetInt();
					else if (renderWindowMemberIterator->name == "hidden")
						ogreRenderWindowConfig.hidden = renderWindowMemberIterator->value.GetBool();
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
							ape::OgreViewPortConfig ogreViewPortConfig;
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
								else if (viewportMemberIterator->name == "cameras")
								{
									rapidjson::Value& cameras = viewport[viewportMemberIterator->name];
									for (auto& camera : cameras.GetArray())
									{
										ape::OgreCameraConfig ogreCameraConfig;
										for (rapidjson::Value::MemberIterator cameraMemberIterator =
											camera.MemberBegin();
											cameraMemberIterator != camera.MemberEnd(); ++cameraMemberIterator)
										{
											if (cameraMemberIterator->name == "name")
												ogreCameraConfig.name = cameraMemberIterator->value.GetString() + mUniqueID;
											else if (cameraMemberIterator->name == "nearClip")
												ogreCameraConfig.nearClip = cameraMemberIterator->value.GetFloat();
											else if (cameraMemberIterator->name == "farClip")
												ogreCameraConfig.farClip = cameraMemberIterator->value.GetFloat();
											else if (cameraMemberIterator->name == "fovY")
												ogreCameraConfig.fovY = cameraMemberIterator->value.GetFloat();
											else if (cameraMemberIterator->name == "positionOffset")
											{
												for (rapidjson::Value::MemberIterator elementMemberIterator =
													viewport[viewportMemberIterator->name][cameraMemberIterator->name].MemberBegin();
													elementMemberIterator != viewport[viewportMemberIterator->name][cameraMemberIterator->name].MemberEnd(); ++elementMemberIterator)
												{
													if (elementMemberIterator->name == "x")
														ogreCameraConfig.positionOffset.x = elementMemberIterator->value.GetFloat();
													else if (elementMemberIterator->name == "y")
														ogreCameraConfig.positionOffset.y = elementMemberIterator->value.GetFloat();
													else if (elementMemberIterator->name == "z")
														ogreCameraConfig.positionOffset.z = elementMemberIterator->value.GetFloat();
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
												ogreCameraConfig.orientationOffset = ape::ConversionFromOgre(orientationOffset);
											}
											else if (cameraMemberIterator->name == "parentNodeName")
											{
												ogreCameraConfig.parentNodeName = cameraMemberIterator->value.GetString();
											}
										}
										ogreViewPortConfig.cameras.push_back(ogreCameraConfig);
									}
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
	lm->createLog("apeOgreRenderPlugin.log", true, false, false);
	mpRoot = new Ogre::Root("", "", "");
	//Ogre::LogManager::getSingleton().createLog("apeOgreRenderPlugin.log", true, false, false);

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
	
	mpRoot->setRenderSystem(renderSystem);
	if (mOgreRenderPluginConfig.renderSystem == "DX9")
	{
		renderSystem->setConfigOption("Resource Creation Policy", "Create on all devices");
		renderSystem->setConfigOption("Multi device memory hint", "Auto hardware buffers management");
	}

	for (auto resourceLocation : mpCoreConfig->getNetworkConfig().resourceLocations)
	{
		if (resourceLocation.find("hydrax"))
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourceLocation, "FileSystem", "Hydrax");
		else if (resourceLocation.find("skyx"))
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourceLocation, "FileSystem", "Skyx");
		else
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourceLocation, "FileSystem");
	}
	
	mpRoot->initialise(false, "ape");
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
			mRenderWindows[winDesc.name]->setHidden(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].hidden);
		}
	}
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	mpOverlaySys = new Ogre::OverlaySystem();
	mpOgreSceneManager->addRenderQueueListener(mpOverlaySys);
	mpHlmsPbsManager = new Ogre::HlmsManager(mpOgreSceneManager);
	mpOgreMovableTextFactory = new ape::OgreMovableTextFactory();
	mpRoot->addMovableObjectFactory(mpOgreMovableTextFactory);
	mpMeshLodGenerator = new  Ogre::MeshLodGenerator();
	mpMeshLodGenerator->_initWorkQueue();
	Ogre::LodWorkQueueInjector::getSingleton().setInjectorListener(this);
	if (mOgreRenderPluginConfig.shading == "perPixel" || mOgreRenderPluginConfig.shading == "")
	{
		if (Ogre::RTShader::ShaderGenerator::initialize())
		{
			mpShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
			mpShaderGenerator->addSceneManager(mpOgreSceneManager);
			mpShaderGeneratorResolver = new ape::ShaderGeneratorResolver(mpShaderGenerator);
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

	int mainWindowID = 0; //first window will be the main window
	Ogre::RenderWindowDescription mainWindowDesc = winDescList[mainWindowID];
	mRenderWindows[mainWindowDesc.name]->getCustomAttribute("WINDOW", &mainWindowHnd);
	std::ostringstream windowHndStr;
	windowHndStr << mainWindowHnd;
	mOgreRenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].windowHandler = windowHndStr.str();
	ape::WindowConfig windowConfig(mainWindowDesc.name, mOgreRenderPluginConfig.renderSystem, mainWindowHnd, mOgreRenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].width,
		mOgreRenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].height);
	mpCoreConfig->setWindowConfig(windowConfig);
	APE_LOG_FUNC_LEAVE();
}
