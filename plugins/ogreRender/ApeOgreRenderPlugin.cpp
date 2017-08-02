/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "ApeOgreRenderPlugin.h"
#include "ApeOgreUtilities.h"

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

Ape::OgreRenderPlugin::OgreRenderPlugin( )
{
	mpScene = Ape::IScene::getSingletonPtr();
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
	mpRoot = NULL;
	mpSceneMgr = NULL;
	mRenderWindows = std::map<std::string, Ogre::RenderWindow*>();
	mpOverlaySys = NULL;
	mpOgreMovableTextFactory = NULL;
	mpOverlayMgr = NULL;
	mpOverlay = NULL;
	mpOverlayContainer = NULL;
	mpOverlayTextArea = NULL;
	mpOverlayFontManager = NULL;
	mpOverlayFont = NULL;
	mpHlmsPbsManager = NULL;
	mpShaderGenerator = NULL;
	mpShaderGeneratorResolver = NULL;
	mpMeshLodGenerator = NULL;
	mCurrentlyLoadingMeshEntityLodConfig = Ogre::LodConfig();
	mpCurrentlyLoadingMeshEntity = NULL;
	mOgreRenderPluginConfig = Ape::OgreRenderPluginConfig();
	mOgreCameras = std::vector<Ogre::Camera*>();
	mPbsMaterials = std::map<std::string, Ogre::PbsMaterial*>();
}

Ape::OgreRenderPlugin::~OgreRenderPlugin()
{
	std::cout << "OgreRenderPlugin dtor" << std::endl;
	delete mpRoot;
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
	mpCurrentlyLoadingMeshEntity = mpSceneMgr->createEntity(geometryName, fileName);
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
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				std::string nodeName = node->getName();
				if (event.type == Ape::Event::Type::NODE_CREATE)
					mpSceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
				else 
				{
					Ogre::SceneNode* ogreNode = nullptr;
					if (mpSceneMgr->hasSceneNode(nodeName))
						ogreNode = mpSceneMgr->getSceneNode(nodeName);
					if (ogreNode)
					{
						switch (event.type)
						{
						case Ape::Event::Type::NODE_PARENTNODE:
						{
							if (auto parentNode = node->getParentNode().lock())
							{
								Ogre::SceneNode* ogreParentNode = ogreNode->getParentSceneNode();
								if (ogreParentNode)
									ogreParentNode->removeChild(ogreNode);
								if (mpSceneMgr->hasSceneNode(parentNode->getName()))
								{
									ogreParentNode = mpSceneMgr->getSceneNode(parentNode->getName());
									ogreParentNode->addChild(ogreNode);
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
						}
					}
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_FILE)
		{
			if (auto geometryFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = geometryFile->getName();
				std::string fileName = geometryFile->getfFileName();
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
					if (mpSceneMgr->hasEntity(geometryName))
					{
						if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
						{
							if (mpSceneMgr->hasSceneNode(parentNodeName))
							{
								if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
									ogreParentNode->attachObject(ogreEntity);
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
					else if (mpSceneMgr->hasManualObject(fileName))
					{
						auto ogreManual = mpSceneMgr->getManualObject(fileName);
						std::stringstream meshName;
						meshName << fileName << ".mesh";
						if (Ogre::MeshManager::getSingleton().getByName(meshName.str()).isNull())
							ogreManual->convertToMesh(meshName.str());

						mpSceneMgr->createEntity(geometryName, meshName.str());
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_FILE_EXPORT:
				{
					if (mpSceneMgr->hasManualObject(geometryName))
					{
						auto ogreManual = mpSceneMgr->getManualObject(geometryName);
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
								mpSceneMgr->destroyManualObject(ogreManual);
								mpSceneMgr->createEntity(geometryName, meshName.str());
							}
						}
					}
				}
				break;
				case Ape::Event::Type::GEOMETRY_FILE_MATERIAL:
				{
					if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
					{
						if (auto material = geometryFile->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
							if (auto pass = material->getPass().lock())
							{
								if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
								{
									size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
									for (size_t i = 0; i < ogreSubEntitxCount; i++)
									{
										Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
										mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
									}
								}
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
			if (auto primitive = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->getEntity(event.subjectName).lock()))
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
					if (auto ogreGeometry = mpSceneMgr->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_PLANE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_PLANE_MATERIAL:
				{
					if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
							if (auto pass = material->getPass().lock())
							{
								if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
								{
									size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
									for (size_t i = 0; i < ogreSubEntitxCount; i++)
									{
										Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
										mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
									}
								}
							}
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
			if (auto primitive = std::static_pointer_cast<Ape::IBoxGeometry>(mpScene->getEntity(event.subjectName).lock()))
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
					if (auto ogreGeometry = mpSceneMgr->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_BOX_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_BOX_MATERIAL:
				{
					if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
							if (auto pass = material->getPass().lock())
							{
								if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
								{
									size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
									for (size_t i = 0; i < ogreSubEntitxCount; i++)
									{
										Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
										mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
									}
								}
							}
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
			if (auto primitive = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->getEntity(event.subjectName).lock()))
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
					if (auto ogreGeometry = mpSceneMgr->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_SPHERE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_SPHERE_MATERIAL:
				{
					if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
							if (auto pass = material->getPass().lock())
							{
								if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
								{
									size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
									for (size_t i = 0; i < ogreSubEntitxCount; i++)
									{
										Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
										mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
									}
								}
							}
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
			if (auto primitive = std::static_pointer_cast<Ape::ICylinderGeometry>(mpScene->getEntity(event.subjectName).lock()))
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
					if (auto ogreGeometry = mpSceneMgr->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_CYLINDER_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_CYLINDER_MATERIAL:
				{
					if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
							if (auto pass = material->getPass().lock())
							{
								if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
								{
									size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
									for (size_t i = 0; i < ogreSubEntitxCount; i++)
									{
										Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
										mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
									}
								}
							}
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
			if (auto primitive = std::static_pointer_cast<Ape::ITorusGeometry>(mpScene->getEntity(event.subjectName).lock()))
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
					if (auto ogreGeometry = mpSceneMgr->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TORUS_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TORUS_MATERIAL:
				{
					if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
							if (auto pass = material->getPass().lock())
							{
								if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
								{
									size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
									for (size_t i = 0; i < ogreSubEntitxCount; i++)
									{
										Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
										mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
									}
								}
							}
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
			if (auto primitive = std::static_pointer_cast<Ape::IConeGeometry>(mpScene->getEntity(event.subjectName).lock()))
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
					if (auto ogreGeometry = mpSceneMgr->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_CONE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_CONE_MATERIAL:
				{
					if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
							if (auto pass = material->getPass().lock())
							{
								if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
								{
									size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
									for (size_t i = 0; i < ogreSubEntitxCount; i++)
									{
										Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
										mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
									}
								}
							}
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
			if (auto primitive = std::static_pointer_cast<Ape::ITubeGeometry>(mpScene->getEntity(event.subjectName).lock()))
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
					if (auto ogreGeometry = mpSceneMgr->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TUBE_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TUBE_MATERIAL:
				{
					if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreEntity->setMaterial(ogreMaterial);
							if (auto pass = material->getPass().lock())
							{
								if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
								{
									size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
									for (size_t i = 0; i < ogreSubEntitxCount; i++)
									{
										Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
										mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
									}
								}
							}
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
			if (auto manual = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpScene->getEntity(event.subjectName).lock()))
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
					if (mpSceneMgr->hasManualObject(geometryName))
					{
						if (auto ogreManual = mpSceneMgr->getManualObject(geometryName))
						{
							if (mpSceneMgr->hasSceneNode(parentNodeName))
							{
								if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
								{
									ogreParentNode->attachObject(ogreManual);
									//std::cout << "c++: " << ogreParentNode->getName() << std::endl;
								}
							}
						}
					}
					else if (mpSceneMgr->hasEntity(geometryName))
					{
						if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
						{
							if (mpSceneMgr->hasSceneNode(parentNodeName))
							{
								if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
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
					if (auto ogreEntity = mpSceneMgr->hasEntity(geometryName))
					{
						if (auto ogreEntity = mpSceneMgr->getEntity(geometryName))
						{
							if (auto material = manual->getMaterial().lock())
							{
								auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
								ogreEntity->setMaterial(ogreMaterial);
								if (auto pass = material->getPass().lock())
								{
									if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
									{
										size_t ogreSubEntitxCount = ogreEntity->getNumSubEntities();
										for (size_t i = 0; i < ogreSubEntitxCount; i++)
										{
											Ogre::SubEntity* ogreSubEntity = ogreEntity->getSubEntity(i);
											mpHlmsPbsManager->bind(ogreSubEntity, ogrePbsMaterial, pass->getName());
										}
									}
								}
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
					if (!mpSceneMgr->hasManualObject(geometryName))
					{
						auto ogreManual = mpSceneMgr->createManualObject(geometryName);
						ogreManual->setDynamic(true);
					}
					if (mpSceneMgr->hasManualObject(geometryName))
					{
						if (auto ogreManual = mpSceneMgr->getManualObject(geometryName))
						{
							std::vector<Ogre::Vector3> normals = std::vector<Ogre::Vector3>();
							if (parameters.normals.size() == 0 && parameters.generateNormals == true)
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
								if (auto pass = material->getPass().lock())
								{
									if (auto ogrePbsMaterial = mPbsMaterials[pass->getName()])
									{
										auto ogreCurrentManualSection = ogreManual->getSection(ogreManual->getNumSections() - 1);
										mpHlmsPbsManager->bind(ogreCurrentManualSection, ogrePbsMaterial, pass->getName());
									}
								}
							}
							else
							{
								ogreManual->begin("FlatVertexColorLighting", Ogre::RenderOperation::OperationType::OT_TRIANGLE_LIST);
							}
							for (int i = 0; i < parameters.coordinates.size(); i = i + 3)
							{
								ogreManual->position(parameters.coordinates[i], parameters.coordinates[i + 1], parameters.coordinates[i + 2]);
								if (parameters.normals.size() == 0 && normals.size() > 0)
								{
									normals[i / 3].normalise();
									ogreManual->normal(normals[i / 3]);
								}
								else if (parameters.normals.size() > 0)
								{
									ogreManual->normal(Ogre::Vector3(parameters.normals[i], parameters.normals[i + 1], parameters.normals[i + 2]));
								}
								if (parameters.textureCoordinates.size() != 0)
								{
									int textCoordIndex = (i / 3) * 6;
									ogreManual->textureCoord(parameters.textureCoordinates[textCoordIndex], parameters.textureCoordinates[textCoordIndex + 1]);
									ogreManual->textureCoord(parameters.textureCoordinates[textCoordIndex + 2], parameters.textureCoordinates[textCoordIndex + 3]);
									ogreManual->textureCoord(parameters.textureCoordinates[textCoordIndex + 4], parameters.textureCoordinates[textCoordIndex + 5]);
								}
								if (parameters.colors.size() != 0)
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
			if (auto manual = std::static_pointer_cast<Ape::IIndexedLineSetGeometry>(mpScene->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = manual->getName();
				std::string parentNodeName = "";
				if (auto parentNode = manual->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_INDEXEDLINESET_CREATE:
					mpSceneMgr->createManualObject(geometryName);
					break;
				case Ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARENTNODE:
				{
					if (auto ogreGeometry = mpSceneMgr->getEntity(geometryName))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
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
					if (auto ogreManual = mpSceneMgr->getManualObject(geometryName))
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
						mpSceneMgr->createEntity(geometryName, geometryName);
						mpSceneMgr->destroyManualObject(geometryName);
					}

				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_TEXT)
		{
			if (auto geometryText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = geometryText->getName();
				std::string parentNodeName = "";
				if (auto parentNode = geometryText->getParentNode().lock())
					parentNodeName = parentNode->getName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_TEXT_CREATE:
				{
					if (auto ogreText = (Ape::OgreMovableText*)mpSceneMgr->createMovableObject(geometryName, "MovableText"))
					{
						ogreText->setTextAlignment(Ape::OgreMovableText::H_CENTER, Ape::OgreMovableText::V_ABOVE);
						ogreText->showOnTop(true);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_PARENTNODE:
				{
					if (auto ogreTextGeometry = (Ape::OgreMovableText*)mpSceneMgr->getMovableObject(geometryName, "MovableText"))
					{
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNodeName))
							ogreParentNode->attachObject(ogreTextGeometry);
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_OFFSET:
				{
					if (auto ogreText = (Ape::OgreMovableText*)mpSceneMgr->getMovableObject(geometryName, "MovableText"))
					{
						if (auto textGeometry = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->getEntity(geometryName).lock()))
							ogreText->setLocalTranslation(Ape::ConversionToOgre(textGeometry->getOffset()));
					}
				}
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_CAPTION:
				{
					if (auto ogreText = (Ape::OgreMovableText*)mpSceneMgr->getMovableObject(geometryName, "MovableText"))
					{
						if (auto textGeometry = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->getEntity(geometryName).lock()))
							ogreText->setCaption(textGeometry->getCaption());
					}
				}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::MATERIAL_FILE)
		{
			if (auto materialFile = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->getEntity(event.subjectName).lock()))
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
						mpSceneMgr->setSkyBox(true, materialName);
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
			if (auto materialManual = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->getEntity(event.subjectName).lock()))
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
				}
			}
		}
		else if (event.group == Ape::Event::Group::PASS_PBS)
		{
			if (auto passPbs = std::static_pointer_cast<Ape::IPbsPass>(mpScene->getEntity(event.subjectName).lock()))
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
			if (auto passManual = std::static_pointer_cast<Ape::IManualPass>(mpScene->getEntity(event.subjectName).lock()))
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
		else if (event.group == Ape::Event::Group::TEXTURE_MANUAL)
		{
			if (auto textureManual = std::static_pointer_cast<Ape::IManualTexture>(mpScene->getEntity(event.subjectName).lock()))
			{
				std::string textureManualName = textureManual->getName();
				switch (event.type)
				{
				case Ape::Event::Type::TEXTURE_MANUAL_CREATE:
					break;
				case Ape::Event::Type::TEXTURE_MANUAL_PARAMETERS:
					{
						Ape::ManualTextureParameters parameters = textureManual->getParameters();
						Ogre::TextureManager::getSingleton().createManual(textureManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
							Ogre::TEX_TYPE_2D, parameters.width, parameters.height, 0, Ape::ConversionToOgre(parameters.pixelFormat),
							Ape::ConversionToOgre(parameters.usage));
					}
					break;
				case Ape::Event::Type::TEXTURE_MANUAL_BUFFER:
				{
					auto ogreTexture = Ogre::TextureManager::getSingleton().getByName(textureManualName);
					if (!ogreTexture.isNull())
					{
						//std::cout << "ApeOgreRenderPlugin::TEXTURE_MANUAL_BUFFER write begin" << std::endl;
						Ogre::HardwarePixelBufferSharedPtr texBuf = ogreTexture->getBuffer();
						texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
						memcpy(texBuf->getCurrentLock().data, textureManual->getBuffer(), textureManual->getParameters().width * textureManual->getParameters().height * 4);
						texBuf->unlock();
						/*static int s = 1;
						std::wostringstream oss;
						oss << std::setw(4) << std::setfill(L'0') << s++ << L".bmp";
						Ape::SaveVoidBufferToImage(oss.str(), textureManual->getBuffer(), textureManual->getParameters().width, textureManual->getParameters().height);*/
						//std::cout << "ApeOgreRenderPlugin::TEXTURE_MANUAL_BUFFER write end" << std::endl;
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
								if (auto ogreCamera = mpSceneMgr->getCamera(camera->getName()))
								{
									if (auto ogreRenderTexture = ogreTexture->getBuffer()->getRenderTarget())
									{
										if (auto ogreViewport = ogreRenderTexture->addViewport(ogreCamera))
										{
											ogreViewport->setClearEveryFrame(true);
											ogreViewport->setBackgroundColour(Ogre::ColourValue::Black);
											ogreViewport->setOverlaysEnabled(false);
											ogreViewport->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
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
		else if (event.group == Ape::Event::Group::LIGHT)
		{
			if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->getEntity(event.subjectName).lock()))
			{
				Ogre::Light* ogreLight = nullptr;
				if (mpSceneMgr->hasLight(light->getName()))
					ogreLight = mpSceneMgr->getLight(light->getName());
				switch (event.type)
				{
				case Ape::Event::Type::LIGHT_CREATE:
					mpSceneMgr->createLight(light->getName());
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
							if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNode->getName()))
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
		else if (event.group == Ape::Event::Group::CAMERA)
		{
			if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case Ape::Event::Type::CAMERA_CREATE:
				{
					mpSceneMgr->createCamera(event.subjectName);
				}
					break;
				case Ape::Event::Type::CAMERA_WINDOW:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
					{
						if (auto ogreCamera = mpSceneMgr->getCamera(event.subjectName))
						{
							if (auto viewPort = mRenderWindows[camera->getWindow()]->addViewport(ogreCamera))
							{
								//TODO why it is working instead of in the init phase?
								ogreCamera->setAspectRatio(Ogre::Real(viewPort->getActualWidth()) / Ogre::Real(viewPort->getActualHeight()));
								mOgreCameras.push_back(ogreCamera);
								if (mOgreRenderPluginConfig.shading == "perPixel" || mOgreRenderPluginConfig.shading == "")
								{
									if (mOgreCameras.size() == 1)
									{
										if (Ogre::RTShader::ShaderGenerator::initialize())
										{
											mpShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
											mpShaderGenerator->addSceneManager(mpSceneMgr);
											mpShaderGeneratorResolver = new Ape::ShaderGeneratorResolver(mpShaderGenerator);
											Ogre::MaterialManager::getSingleton().addListener(mpShaderGeneratorResolver);
											Ogre::RTShader::RenderState* pMainRenderState = mpShaderGenerator->createOrRetrieveRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME).first;
											pMainRenderState->reset();
											pMainRenderState->addTemplateSubRenderState(mpShaderGenerator->createSubRenderState(Ogre::RTShader::PerPixelLighting::Type));
											mpShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
										}
										else
											std::cout << "Problem in the RTSS init" << std::endl;
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
					if (auto ogreCamera = mpSceneMgr->getCamera(camera->getName()))
					{
						if (auto parentNode = camera->getParentNode().lock())
						{
							if (mpSceneMgr->hasSceneNode(parentNode->getName()))
							{
								if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentNode->getName()))
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
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setFocalLength(camera->getFocalLength());
				}
					break;
				case Ape::Event::Type::CAMERA_ASPECTRATIO:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setAspectRatio(camera->getAspectRatio());
				}
					break;
				case Ape::Event::Type::CAMERA_FOVY:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setFOVy(ConversionToOgre(camera->getFOVy()));
				}
					break;
				case Ape::Event::Type::CAMERA_FRUSTUMOFFSET:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setFrustumOffset(Ape::ConversionToOgre(camera->getFrustumOffset()));
				}
					break;
				case Ape::Event::Type::CAMERA_FARCLIP:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setFarClipDistance(camera->getFarClipDistance());
				}
					break;
				case Ape::Event::Type::CAMERA_NEARCLIP:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setNearClipDistance(camera->getNearClipDistance());
				}
					break;
				case Ape::Event::Type::CAMERA_PROJECTION:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setCustomProjectionMatrix(true, Ape::ConversionToOgre(camera->getProjection()));
				}
					break;
				case Ape::Event::Type::CAMERA_POSITION:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setPosition(ConversionToOgre(camera->getPosition()));
				}
					break;
				case Ape::Event::Type::CAMERA_ORIENTATION:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setOrientation(ConversionToOgre(camera->getOrientation()));
				}
					break;
				case Ape::Event::Type::CAMERA_PROJECTIONTYPE:
					{
						if (mpSceneMgr->hasCamera(event.subjectName))
							mpSceneMgr->getCamera(event.subjectName)->setProjectionType(ConversionToOgre(camera->getProjectionType()));
					}
					break;
				case Ape::Event::Type::CAMERA_ORTHOWINDOWSIZE:
				{
					if (mpSceneMgr->hasCamera(event.subjectName))
						mpSceneMgr->getCamera(event.subjectName)->setOrthoWindow(camera->getOrthoWindowSize().x, camera->getOrthoWindowSize().y);
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
	mpSceneMgr->destroyEntity(mpCurrentlyLoadingMeshEntity);
	mpCurrentlyLoadingMeshEntity = mpSceneMgr->createEntity(meshEntityName, mCurrentlyLoadingMeshEntityLodConfig.mesh);
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
	if (mRenderWindows.size() > 0)
		std::cout << "FPS: " << mRenderWindows.begin()->second->getLastFPS() << " triangles: " << mRenderWindows.begin()->second->getTriangleCount() << std::endl;

	processEventDoubleQueue();
	
	return Ogre::FrameListener::frameRenderingQueued( evt );
}

bool Ape::OgreRenderPlugin::frameEnded( const Ogre::FrameEvent& evt )
{
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
	try
	{
        mpRoot->renderOneFrame();
        mpRoot->startRendering();
	}
	catch (const Ogre::RenderingAPIException& ex)
	{
		std::cout << ex.getFullDescription() << std::endl;
	}
	catch (const Ogre::Exception& ex)
	{
		std::cout << ex.getFullDescription() << std::endl;
	}
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
		std::cout << ex.getFullDescription() << std::endl;
	}
	catch (const Ogre::Exception& ex)
	{
		std::cout << ex.getFullDescription() << std::endl;
	}
}

void Ape::OgreRenderPlugin::Init()
{
	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

	if (mpSystemConfig->getSceneSessionConfig().participantType == Ape::SceneSession::ParticipantType::HOST || mpSystemConfig->getSceneSessionConfig().participantType == Ape::SceneSession::ParticipantType::GUEST)
	{
		if (auto userNode = mUserNode.lock())
		{
			if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity(userNode->getName(), Ape::Entity::GEOMETRY_TEXT).lock()))
			{
				userNameText->setCaption(userNode->getName());
				userNameText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
				userNameText->setParentNode(userNode);
			}
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
	
	mpRoot = new Ogre::Root("", "", "ApeOgreRenderPlugin.log");
    
	Ogre::LogManager::getSingleton().createLog("ApeOgreRenderPlugin.log", true, false, false);

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
		Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
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

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/fonts",				 "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/materials", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/pbs", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/Cg", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/GLSL", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/GLSL150", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/GLSLES", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/HLSL", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/rtss/materials", "FileSystem");
	for (auto resourceLocation : mpSystemConfig->getSceneSessionConfig().sessionResourceLocation)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourceLocation, "FileSystem");
	
	mpRoot->initialise(false, "Ape");
	mpSceneMgr = mpRoot->createSceneManager(Ogre::ST_GENERIC);

	mpRoot->addFrameListener(this);

	Ogre::RenderWindowList renderWindowList;
	Ogre::RenderWindowDescriptionList winDescList;
	int enabledWindowCount = 0;
	for (int i = 0; i < mOgreRenderPluginConfig.ogreRenderWindowConfigList.size(); i++)
	{
		if (mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].enable)
		{
			enabledWindowCount++;
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
			/*winDesc.miscParams["Allow NVPersHUD"] = "No";
			winDesc.miscParams["Driver type"] = "Hardware";
			winDesc.miscParams["Information Queue Exceptions Bottom Level"] = "Info (exception on any message)";
			winDesc.miscParams["Max Requested Feature Levels"] = "11.0";
			winDesc.miscParams["Min Requested Feature Levels"] = "9.1";
			winDesc.miscParams["Floating-point mode"] = "Fastest";
			winDesc.miscParams["sRGB Gamma Conversion"] = "No";
			winDescList.push_back(winDesc);*/

			if (mpSystemConfig->getMainWindowConfig().creator == THIS_PLUGINNAME)
			{
				mRenderWindows[winDesc.name] = mpRoot->createRenderWindow(winDesc.name, winDesc.width, winDesc.height, winDesc.useFullScreen, &winDesc.miscParams);
				mRenderWindows[winDesc.name]->setDeactivateOnFocusChange(false);
				if (enabledWindowCount == 1)
				{
					void* windowHnd = 0;
					mRenderWindows[winDesc.name]->getCustomAttribute("WINDOW", &windowHnd);
					std::ostringstream windowHndStr;
					windowHndStr << windowHnd;
					mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].windowHandler = windowHndStr.str();
					mpMainWindow->setName(winDesc.name);
					mpMainWindow->setHandle(windowHnd);
					mpMainWindow->setWidth(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].width);
					mpMainWindow->setHeight(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].height);
				}
				if (mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList.size() > 0)
				{
					auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->createEntity(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[0].camera.name, Ape::Entity::Type::CAMERA).lock());
					if (camera)
					{
						//TODO why it is not ok
						//camera->setAspectRatio((float)mOgreRenderWindowConfigList[i].width / (float)mOgreRenderWindowConfigList[i].height);
						camera->setWindow(winDesc.name);
						camera->setFocalLength(1.0f);
						camera->setNearClipDistance(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[0].camera.nearClip);
						camera->setFarClipDistance(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[0].camera.farClip);
						camera->setFOVy(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[0].camera.fovY.toRadian());
						if (auto userNode = mUserNode.lock())
							camera->setParentNode(mUserNode);
					}
				}
			}
		}
	}
	
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mpOverlaySys = new Ogre::OverlaySystem();
	mpSceneMgr->addRenderQueueListener(mpOverlaySys);

	mpHlmsPbsManager = new Ogre::HlmsManager(mpSceneMgr);

	mpOgreMovableTextFactory = new Ape::OgreMovableTextFactory();
	mpRoot->addMovableObjectFactory(mpOgreMovableTextFactory);

	mpMeshLodGenerator = new  Ogre::MeshLodGenerator();
	mpMeshLodGenerator->_initWorkQueue();
	Ogre::LodWorkQueueInjector::getSingleton().setInjectorListener(this);
}
