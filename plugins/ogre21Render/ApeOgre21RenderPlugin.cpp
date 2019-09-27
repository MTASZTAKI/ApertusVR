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
#include "ApeOgre21RenderPlugin.h"
//#include "apeOgreUtilities.h"


ape::Ogre21RenderPlugin::Ogre21RenderPlugin() //constructor
{

	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	//mpMainWindow = ape::IMainWindow::getSingletonPtr(); -> itt is mást kell
	mEventDoubleQueue = ape::DoubleQueue<Event>();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_PBS, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::PASS_MANUAL, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpRoot = nullptr;
	mpSceneMgr = nullptr;
	mRenderWindows = std::map<std::string, Ogre::RenderWindow*>();
	mpHlmsPbsManager = nullptr;
	mOgreRenderPluginConfig = ape::Ogre21RenderPluginConfig();
	mOgreCameras = std::vector<Ogre::Camera*>();
	mCameraCountFromConfig = 0;
	mpActualRenderwindow = nullptr;
	mSkyBoxMaterial = Ogre::MaterialPtr();


	APE_LOG_FUNC_LEAVE();
}

ape::Ogre21RenderPlugin::~Ogre21RenderPlugin() //destructor
{
	std::cout << "OgreRenderPlugin dtor" << std::endl;
	Ogre::CompositorManager2* compositorManager = mpRoot->getCompositorManager2();
	compositorManager->removeAllWorkspaces();
	delete mpRoot;
}

void ape::Ogre21RenderPlugin::eventCallBack(const ape::Event& event)
{
	mEventDoubleQueue.push(event);
}

void ape::Ogre21RenderPlugin::processEventDoubleQueue()
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
					auto ogreNode = mpSceneMgr->getRootSceneNode()->createChildSceneNode();
					ogreNode->setName(nodeName);
				}
				else
				{
					Ogre::SceneNode* ogreNode = nullptr;
					auto ogreNodeList = mpSceneMgr->findSceneNodes(nodeName);
					if (!ogreNodeList.empty())
					{
						ogreNode = mpSceneMgr->getSceneNode(ogreNodeList[0]->getId());
					}
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
								{
									ogreOldParentNode->removeChild(ogreNode);
								}
								auto ogreNodeList = mpSceneMgr->findSceneNodes(parentNode->getName());
								if (ogreNodeList[0] != nullptr)
								{
									auto ogreNewParentNode = mpSceneMgr->getSceneNode(ogreNodeList[0]->getId());
									ogreNewParentNode->addChild(ogreNode);
								}
									

							}
						}
						break;
						case ape::Event::Type::NODE_DELETE:
							;
							break;
						case ape::Event::Type::NODE_POSITION:
							ogreNode->setPosition(ape::ConversionToOgre21(node->getPosition()));
							break;
						case ape::Event::Type::NODE_ORIENTATION:
							ogreNode->setOrientation(ape::ConversionToOgre21(node->getOrientation()));
							break;
						case ape::Event::Type::NODE_SCALE:
							ogreNode->setScale(ape::ConversionToOgre21(node->getScale()));
							break;
						case ape::Event::Type::NODE_CHILDVISIBILITY:
							ogreNode->setVisible(node->getChildrenVisibility());
							break;
						case ape::Event::Type::NODE_FIXEDYAW:
							ogreNode->setFixedYawAxis(node->isInheritOrientation());
							break;
						case ape::Event::Type::NODE_INITIALSTATE:
							;//ilyen nincs az ogre2.1ben
							break;
						case ape::Event::Type::NODE_SHOWBOUNDINGBOX:
							;
							//ogreNode->showBoundingBox(true); ilyensincs
							break;
						case ape::Event::Type::NODE_HIDEBOUNDINGBOX:
							;
							//ogreNode->showBoundingBox(false); ilyen sincs
							break;
						}
					}
				}
			}
		}
		else if (event.group == ape::Event::Group::MATERIAL_FILE)
		{
			if (auto materialFile = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string materialName = materialFile->getName();
				Ogre::MaterialPtr ogreMaterial;
				if (Ogre::MaterialManager::getSingleton().resourceExists(materialName))
				{
					ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
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
					if (mSkyBoxMaterial.isNull())
					{
						mSkyBoxMaterial = Ogre::MaterialManager::getSingletonPtr()->getByName(materialName);
					}
					/*if (mpActualWorkSpace->isValid())
					{
						//Ogre::Material* material = materialPtr.getPointer();
						Ogre::TextureUnitState* tex = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
						tex->setCubicTextureName(materialName+".dds", true);
						tex->setGamma(2.0);
						material->compile();
						mpActualWorkSpace->setEnabled(true);
					}*/
					
					
				}
				break;
				case ape::Event::Type::MATERIAL_FILE_TEXTURE:
				{
					if (auto texture = materialFile->getPassTexture().lock())
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
							for (auto passGpuParameter : materialFile->getPassGpuParameters())
								ogreGpuParameters->setNamedConstant(passGpuParameter.name, ConversionToOgre21(passGpuParameter.value));
						}
					}
				}
				break;
				}
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
				{
					parentNodeName = parentNode->getName();
				}
				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_FILE_CREATE:
					;
					break;
				case ape::Event::Type::GEOMETRY_FILE_PARENTNODE:
				{
					if (mItemList[geometryName] != nullptr)
					{
						auto ogreEntity = mItemList[geometryName];
						auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = parentList[0])
						{
							ogreParentNode->attachObject(ogreEntity);
						}

					}
				}
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
							//---------------------
							Ogre::MeshPtr v2Mesh;

							
							v2Mesh = Ogre::MeshManager::getSingleton().load(
								fileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

							
							Ogre::Item *item = mpSceneMgr->createItem(fileName,
								Ogre::ResourceGroupManager::
								AUTODETECT_RESOURCE_GROUP_NAME,
								Ogre::SCENE_DYNAMIC);


							//---------------------


							//Ogre::Item* Item = mpSceneMgr->createItem(geometryName, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC);
							mItemList[geometryName] = item;
						}
					} 
					else if(mManualObjectList[fileName])
					{
						auto ogreManual = mManualObjectList[fileName];
						std::stringstream meshName;
						meshName << fileName << ".mesh";
						if (Ogre::MeshManager::getSingleton().getByName(meshName.str()).isNull())
						{
							//ogreManual->convertToMesh(meshName.str());
							Ogre::MeshPtr v2Mesh;
							v2Mesh = Ogre::MeshManager::getSingleton().createManual(fileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
							
							
						}
						if (!mItemList[geometryName])
						{
							auto item = mpSceneMgr->createItem(geometryName);
							mItemList[geometryName] = item;
						}
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES:
				{
					//Ogre 2.1 not support
				}
				break;
				case ape::Event::Type::GEOMETRY_FILE_EXPORT:
				{
					if (geometryFile->isExportMesh())
					{
						Ogre::MeshSerializer mMeshSerializer(0);
						if (fileName.find_first_of(".") != std::string::npos)
						{
							std::string fileExtension = fileName.substr(fileName.find_last_of("."));
							if (fileExtension == ".mesh")
							{
								auto mesh = Ogre::MeshManager::getSingleton().getByName(fileName);
								if (!mesh.isNull())
								{
									mMeshSerializer.exportMesh(mesh.getPointer(), fileName);
								}
							}
						}
						else if (mManualObjectList[geometryName])
						{
							auto ogreManual = mManualObjectList[geometryName];
							std::stringstream meshName;
							meshName << geometryName << ".mesh";
							if (Ogre::MeshManager::getSingleton().getByName(meshName.str()).isNull())
							{
								
								Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual(meshName.str(),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
								if (!mesh.isNull())
								{
									mMeshSerializer.exportMesh(mesh.getPointer(), meshName.str());
									//Ogre::Mesh::SubMeshIterator subMeshIterator = mesh->getSubMeshIterator();
									Ogre::SubMesh* subMesh = nullptr;
									int i = 0;
									auto subMeshNum = mesh->getNumSubMeshes();
									auto subMeshVec = mesh->getSubMeshes();
									while (i<subMeshNum)
									{
										subMesh = subMeshVec[i];
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
									mpSceneMgr->destroyManualObject(ogreManual);
									auto ogreItem = mpSceneMgr->createItem(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
									mItemList[geometryName] = ogreItem;
								}
							}
						}
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_FILE_MATERIAL:
				{
					if (mItemList[geometryName])
					{
						auto ogreItem = mItemList[geometryName];
						if (auto material = geometryFile->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							if (!ogreMaterial.isNull())
								ogreItem->setMaterial(ogreMaterial);
						}
						
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_FILE_VISIBILITY:
				{
					if (mItemList[geometryName])
					{
						auto ogreItem = mItemList[geometryName];
						ogreItem->setVisibilityFlags(geometryFile->getVisibilityFlag());
						
					}
				}
				break;

				}
			}
			
		}
		else if (event.group == ape::Event::Group::LIGHT)
		{
			if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				Ogre::Light* ogreLight = nullptr;
				
				if (mLightList[light->getName()] != nullptr)
					ogreLight = mLightList[light->getName()];

				

				switch (event.type)
				{
				case ape::Event::Type::LIGHT_CREATE:
				{
 					ogreLight = mpSceneMgr->createLight();
					ogreLight->setName(light->getName());
					mLightList[light->getName()] = ogreLight;
					Ogre::SceneNode* lightNode = mpSceneMgr->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
					lightNode->attachObject(ogreLight);
				}
					break;
				case ape::Event::Type::LIGHT_ATTENUATION:
					ogreLight->setAttenuation(light->getLightAttenuation().range, light->getLightAttenuation().constant, light->getLightAttenuation().linear, light->getLightAttenuation().quadratic);
					
					break;
				case ape::Event::Type::LIGHT_DIFFUSE:
					ogreLight->setDiffuseColour(ape::ConversionToOgre21(light->getDiffuseColor()));
					
					break;
				case ape::Event::Type::LIGHT_DIRECTION:
					ogreLight->setDirection(ape::ConversionToOgre21(light->getLightDirection()));
					;
					break;
				case ape::Event::Type::LIGHT_SPECULAR:
					ogreLight->setSpecularColour(ape::ConversionToOgre21(light->getSpecularColor()));
					
					break;
				case ape::Event::Type::LIGHT_SPOTRANGE:
					ogreLight->setSpotlightRange(Ogre::Radian(light->getLightSpotRange().innerAngle.toRadian()), Ogre::Radian(light->getLightSpotRange().outerAngle.toRadian()), light->getLightSpotRange().falloff);
					
					break;
				case ape::Event::Type::LIGHT_TYPE:
					ogreLight->setType(ape::ConversionToOgre21(light->getLightType()));
					
					break;
				case ape::Event::Type::LIGHT_PARENTNODE:
				{
					if (auto parentNode = light->getParentNode().lock())
					{
						auto parentList = mpSceneMgr->findSceneNodes(parentNode->getName());
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentList[0]->getId()))
						{
							ogreLight->detachFromParent();
							ogreParentNode->attachObject(ogreLight);
						}
							
					}
					ogreLight->setDirection(ape::ConversionToOgre21(light->getLightDirection()));
				}
				break;
				case ape::Event::Type::LIGHT_DELETE:
					;
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
					if (auto ogreGeometry = mItemList[geometryName])
					{
						auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = mpSceneMgr->getSceneNode(parentList[0]->getId()))
							ogreParentNode->attachObject(ogreGeometry);						
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_PLANE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_PLANE_MATERIAL:
				{
					
					if (Ogre::Item* ogreItem = mItemList[geometryName])
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreItem->setMaterial(ogreMaterial);
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
						// procedural not yet implemented
						//Procedural::PlaneGenerator().setNumSegX(parameters.numSeg.x).setNumSegY(parameters.numSeg.y).setSizeX(parameters.size.x).setSizeY(parameters.size.y)
							//.setUTile(parameters.tile.x).setVTile(parameters.tile.y).realizeMesh(meshFileName.str());

					}
					Ogre::Item* planeItem = mpSceneMgr->createItem(geometryName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,Ogre::SCENE_DYNAMIC);
					mItemList[geometryName] = planeItem;

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
					if (auto ogreGeometry = mItemList[geometryName])
					{
						auto ParentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = ParentList[0])
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_BOX_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_BOX_MATERIAL:
				{
					if (auto ogreItem = mItemList[geometryName])
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreItem->setMaterial(ogreMaterial);
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
						// procedural not yet implemented
						//Procedural::BoxGenerator().setSizeX(parameters.dimensions.x).setSizeY(parameters.dimensions.x).setSizeZ(parameters.dimensions.x)
						//	.realizeMesh(meshFileName.str());
					}
					Ogre::Item* boxItem = mpSceneMgr->createItem(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC);
					mItemList[geometryName] = boxItem;
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
					if (auto ogreGeometry = mItemList[geometryName])
					{
						auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = parentList[0])
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_SPHERE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_SPHERE_MATERIAL:
				{
					if (auto ogreItem = mItemList[geometryName])
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreItem->setMaterial(ogreMaterial);
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
						// procedural not yet implemented
						//Procedural::SphereGenerator().setRadius(parameters.radius)
						//	.setUTile(parameters.tile.x).setVTile(parameters.tile.y)
						//	.realizeMesh(meshFileName.str());
					}
					Ogre::Item* sphereItem = mpSceneMgr->createItem(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC);
					mItemList[geometryName] = sphereItem;

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
					if (auto ogreGeometry = mItemList[geometryName])
					{
						auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = parentList[0])
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_CYLINDER_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_CYLINDER_MATERIAL:
				{
					if (auto ogreItem = mItemList[geometryName])
					{
						if (auto material = primitive->getMaterial().lock())
						{
							auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
							ogreItem->setMaterial(ogreMaterial);
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
						//not implemented
						//Procedural::CylinderGenerator().setHeight(parameters.height)
						//	.setRadius(parameters.radius)
						//	.setUTile(parameters.tile)
						//	.realizeMesh(meshFileName.str());
					}
					Ogre::Item* cylinderItem = mpSceneMgr->createItem(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC);
					mItemList[geometryName] = cylinderItem;

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
					if (auto ogreGeometry = mItemList[geometryName])
					{
						auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = parentList[0])
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_TORUS_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_TORUS_MATERIAL:
				{
					if (auto ogreEntity = mItemList[geometryName])
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
						//not implemted
						//Procedural::TorusGenerator().setRadius(parameters.radius)
						//	.setSectionRadius(parameters.sectionRadius)
						//	.setUTile(parameters.tile.x).setVTile(parameters.tile.y)
						//	.realizeMesh(meshFileName.str());
					}
					Ogre::Item* torusItem = mpSceneMgr->createItem(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC);
					mItemList[geometryName] = torusItem;

				}
				break;
				}
			}
		}
		/*else if (event.group == ape::Event::Group::GEOMETRY_CONE)
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
					if (auto ogreGeometry = mItemList[geometryName])
					{
						auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = parentList[0])
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_CONE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_CONE_MATERIAL:
				{
					if (auto ogreEntity = mItemList[geometryName])
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
						//not implemented
						//Procedural::ConeGenerator().setRadius(parameters.radius)
						//	.setHeight(parameters.height)
						//	//.setNumSegBase(parameters.numSeg.x).setNumSegHeight(parameters.numSeg.y)
						//	//.setUTile(parameters.tile)
						//	.realizeMesh(meshFileName.str());
					}
					Ogre::Item* coneItem = mpSceneMgr->createItem(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC);
					mItemList[geometryName] = coneItem;

				}
				break;
				}
			}
		}*/
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
					if (auto ogreGeometry = mItemList[geometryName])
					{
						auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = parentList[0])
							ogreParentNode->attachObject(ogreGeometry);
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_TUBE_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_TUBE_MATERIAL:
				{
					if (auto ogreEntity = mItemList[geometryName])
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
						//not implemented
						//Procedural::TubeGenerator().setHeight(parameters.height)
						//	.setUTile(parameters.tile)
						//	.realizeMesh(meshFileName.str());
					}
					Ogre::Item* tubeItem = mpSceneMgr->createItem(meshFileName.str(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC);
					mItemList[geometryName] = tubeItem;

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
				Ogre::MeshPtr pmeshv2;
				if (mMeshPtrList[geometryName])
				{
					pmeshv2 = mMeshPtrList[geometryName];
				}else
				{
					pmeshv2 = Ogre::MeshManager::getSingleton().createManual(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
					mMeshPtrList[geometryName] = pmeshv2;
				}

				switch (event.type)
				{
				case ape::Event::Type::GEOMETRY_INDEXEDFACESET_CREATE:
				{

				}
				break;
				case ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARENTNODE:
				{
					if (mManualObjectList[geometryName])
					{
						if (auto ogreManual = mManualObjectList[geometryName])
						{
							auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
							if (auto ogreParentNode = parentList[0])
							{
								ogreParentNode->attachObject(ogreManual);
							}
						}
					}
					else if (mItemList[geometryName])
					{
						if (auto ogreItem = mItemList[geometryName])
						{
							auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
							if (auto ogreParentNode = parentList[0])
								ogreParentNode->attachObject(ogreItem);					
						}
					}
					else if (mMeshPtrList[geometryName])
					{
						auto item = mpSceneMgr->createItem(pmeshv2);
						mItemList[geometryName] = item;
						auto parentList = mpSceneMgr->findSceneNodes(parentNodeName);
						if (auto ogreParentNode = parentList[0])
							ogreParentNode->attachObject(item);
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_INDEXEDFACESET_DELETE:
					;
					break;
				case ape::Event::Type::GEOMETRY_INDEXEDFACESET_MATERIAL:
				{
					if (!mItemList[geometryName])
					{
						auto item = mpSceneMgr->createItem(pmeshv2);
						mItemList[geometryName] = item;
					}
					if (auto ogreItem = mItemList[geometryName])
					{
						if (auto material = manual->getMaterial().lock())
						{
							//Ogre::HlmsPbsDatablock ogreMaterial = mpRoot->getHlmsManager()->getDatablock(material->getName());
							Ogre::HlmsPbsDatablock* ogreMaterial = mPbsDataBlockList[material->getName()];
							//ogreItem->setDatablock(ogreMaterial);
							auto till = ogreItem->getNumSubItems();

							for (size_t i = 0; i < till; i++)
							{
								ogreItem->getSubItem(i)->setDatablock(ogreMaterial);
								/*mItemList["Cube_d.mesh"]->setDatablock("Rocks");
								mItemList["Sphere1000.mesh"]->setDatablock("Rocks");*/
							}
							

						
						}	
					}
				}
				break;
				case ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS:
				{
					Ogre::VaoManager *vaoManager = Ogre::Root::getSingleton().getRenderSystem()->getVaoManager();
					// create vertex buffer
					unsigned short numreal = 3;
					Ogre::VertexElement2Vec velements;
					velements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
					if (manual->getHasNormals())
					{
						numreal += 3;
						velements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));
					}if (manual->getHasTangents())
					{
						numreal += 3;
						velements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_TANGENT));
					}
					if (manual->getHasTextureCoords()) 
					{
						numreal += 3;
						velements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_TEXTURE_COORDINATES));
					}
					if (manual->getHasVertexColors()) 
					{
						numreal += 3;
						velements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_DIFFUSE));
					}
					//parameters.faces.faceVectors.size();
					Ogre::Real *vertexdata = reinterpret_cast<Ogre::Real *> (OGRE_MALLOC_SIMD(
						sizeof(Ogre::Real) * numreal * parameters.faces.faceVectors.size(), Ogre::MEMCATEGORY_GEOMETRY));
					Ogre::FreeOnDestructor vdataPtr(vertexdata);
					// fill vertexdata manually
					// prime pointers to vertex related data

					std::vector<ape::Vector3> uvvec = manual->getUvs();
					//ape::Vector3 *uv = uvvec[0];
					ape::Vector4 *col = manual->getCols();

					ape::Vector3 vect3 = parameters.faces.faceVectors[0];
					Ogre::Aabb subAABB(Ogre::Vector3(vect3.x, vect3.y, vect3.z), Ogre::Vector3::ZERO);
					int normalhelper = 0;
					int tangenthelper = 0;
					int cordHelp = 0;
					for (unsigned int i = 0, offset = 0; i < parameters.faces.faceVectors.size(); ++i)
					{
						// position
						vect3 = parameters.faces.faceVectors[i];
						vertexdata[offset + 0] = vect3.x;
						vertexdata[offset + 1] = vect3.y;
						vertexdata[offset + 2] = vect3.z;
						//vertexdata[offset + 0] = parameters.coordinates[cordHelp + 0];
						//vertexdata[offset + 1] = parameters.coordinates[cordHelp + 1];
						//vertexdata[offset + 2] = parameters.coordinates[cordHelp + 2];
						
						offset += 3;
						subAABB.merge(Ogre::Vector3(vect3.x, vect3.y, vect3.z));
						//subAABB.merge(Ogre::Vector3(parameters.coordinates[cordHelp + 0], parameters.coordinates[cordHelp + 1], parameters.coordinates[cordHelp + 2]));
						cordHelp += 3;
						// normal
						if (manual->getHasNormals())
						{
							
							vertexdata[offset + 0] = parameters.normals[normalhelper + 0];
							vertexdata[offset + 1] = parameters.normals[normalhelper + 1];
							vertexdata[offset + 2] = parameters.normals[normalhelper + 2];
							offset += 3;
							normalhelper += 3;
						}
						//tangent
						if (manual->getHasTangents())
						{
							vertexdata[offset + 0] = parameters.tangents[tangenthelper + 0];
							vertexdata[offset + 1] = parameters.tangents[tangenthelper + 1];
							vertexdata[offset + 2] = parameters.tangents[tangenthelper + 2];
							offset += 3;
							tangenthelper += 3;
						}

						// uv
						if (manual->getHasTextureCoords())
						{
							vertexdata[offset + 0] = uvvec[i].x;
							vertexdata[offset + 1] = uvvec[i].y;
							vertexdata[offset + 2] = uvvec[i].z;
							offset += 3;
						}
						// color
						if (manual->getHasVertexColors())
						{
							vertexdata[offset + 0] = col[i].x;//r
							vertexdata[offset + 1] = col[i].y;//g
							vertexdata[offset + 2] = col[i].z;//b
							offset += 3;
						}
						
						
					}

					Ogre::VertexBufferPacked *vertexBuffer = 0;

					try
					{
						//Create the actual vertex buffer.
						vertexBuffer = vaoManager->createVertexBuffer(velements, parameters.faces.faceVectors.size(),
							Ogre::BT_IMMUTABLE,
							vertexdata, false);
					}
					catch (Ogre::Exception &e)
					{
						// When keepAsShadow = true, the memory will be freed when the index buffer is destroyed.
						// However if for some weird reason there is an exception raised, the memory will
						// not be freed, so it is up to us to do so.
						// The reasons for exceptions are very rare. But we're doing this for correctness.
						OGRE_FREE_SIMD(vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
						vertexBuffer = 0;
						throw e;
					}
					//We'll just use one vertex buffer source (multi-source not working yet)

					Ogre::VertexBufferPackedVec vertexBuffers;
					vertexBuffers.push_back(vertexBuffer);

					// currently consider 16-bit indices array only
					Ogre::uint16 * indexdata = reinterpret_cast<Ogre::uint16 *>(OGRE_MALLOC_SIMD(
						sizeof(Ogre::uint16) * 3 * parameters.faces.face.size(), Ogre::MEMCATEGORY_GEOMETRY));
					Ogre::FreeOnDestructor iddataPtr(indexdata);

					for (unsigned int i = 0, offset = 0; i < parameters.indices.size()/3; ++i)
					{
						indexdata[offset + 0] = (Ogre::uint16)parameters.indices[offset + 0];
						indexdata[offset + 1] = (Ogre::uint16)parameters.indices[offset + 1];
						indexdata[offset + 2] = (Ogre::uint16)parameters.indices[offset + 2];
						offset += 3;
					}

					Ogre::IndexBufferPacked *indexBuffer = 0;
					try
					{
						indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
							3 * parameters.indices.size()/3, // number of indices
							Ogre::BT_IMMUTABLE,
							indexdata, false);
					}
					catch (Ogre::Exception &e)
					{
						// When keepAsShadow = true, the memory will be freed when the index buffer is destroyed.
						// However if for some weird reason there is an exception raised, the memory will
						// not be freed, so it is up to us to do so.
						// The reasons for exceptions are very rare. But we're doing this for correctness.
						OGRE_FREE_SIMD(indexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
						indexBuffer = 0;
						throw e;
					}
					
					Ogre::SubMesh * submesh = pmeshv2->createSubMesh(manual->getIndex());
					Ogre::VertexArrayObject *vao = vaoManager->createVertexArrayObject(
						vertexBuffers, indexBuffer, Ogre::OT_TRIANGLE_LIST);

					//Each Vao pushed to the vector refers to an LOD level.
					//Must be in sync with mesh->mLodValues & mesh->mNumLods if you use more than one level
					submesh->mVao[Ogre::VpNormal].push_back(vao);
					//Use the same geometry for shadow casting.
					submesh->mVao[Ogre::VpShadow].push_back(vao);
					// AABB
					if (pmeshv2->getNumSubMeshes() > 0) // not first
					{
						subAABB.merge(pmeshv2->getAabb());
					}
					pmeshv2->_setBounds(subAABB, false); // merging AABB from all sub-meshes, pad need to be FALSE
					pmeshv2->_setBoundingSphereRadius(subAABB.getRadius());
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
				{
					Ogre::ManualObject* manObj = mpSceneMgr->createManualObject();
					manObj->setName(geometryName);
					mManualObjectList[geometryName] = manObj;
				}
					break;
				case ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARENTNODE:
				{
					if (auto ogreGeometry = mItemList[geometryName])
					{
						auto parentList = mpSceneMgr->findSceneNodes(geometryName);
						if (auto ogreParentNode =parentList[0])
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
					if (auto ogreManual = mManualObjectList[geometryName])
					{
						// Ogre v1
						ogreManual->begin("FlatVertexColorNoLighting");
						for (int coordinateIndex = 0; coordinateIndex < parameters.coordinates.size(); coordinateIndex = coordinateIndex + 3)
						{
							ogreManual->position(parameters.coordinates[coordinateIndex], parameters.coordinates[coordinateIndex + 1], parameters.coordinates[coordinateIndex + 2]);
							ogreManual->colour(ape::ConversionToOgre21(parameters.color));
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
						Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
						auto item =  mpSceneMgr->createItem(geometryName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
						mpSceneMgr->destroyManualObject(mManualObjectList[geometryName]);
						mManualObjectList.erase(geometryName);
						mItemList[geometryName] = item;
					}

				}
				break;
				}
			}
		}
		/*else if (event.group == ape::Event::Group::GEOMETRY_TEXT)
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
		}*/
		else if (event.group == ape::Event::Group::MATERIAL_PBS)
		{
		if (auto materialPbs = std::static_pointer_cast<ape::IPbsMaterial>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			//--
				//**********
			/*Ogre::String basename;
			basename = materialPbs->getName();
			Ogre::MaterialManager* omatMgr = Ogre::MaterialManager::getSingletonPtr();
			Ogre::ResourceManager::ResourceCreateOrRetrieveResult status = omatMgr->createOrRetrieve(basename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
			
			Ogre::MaterialPtr omat = status.first.staticCast<Ogre::Material>();
			mMatList[basename] = omat;*/

			//omat->setAmbient(clr.r, clr.g, clr.b);

				//**********

			std::string materialName = materialPbs->getName();
			auto HlmsPbs = static_cast<Ogre::HlmsPbs*>(Ogre::Root::getSingleton().getHlmsManager()->getHlms(Ogre::HlmsTypes::HLMS_PBS));
			Ogre::HlmsPbsDatablock* datablock;
			if (mPbsDataBlockList[materialName] == nullptr)
			{
				datablock = static_cast<Ogre::HlmsPbsDatablock*>(HlmsPbs->createDatablock(Ogre::IdString(materialName),
								materialName,
								Ogre::HlmsMacroblock(),
								Ogre::HlmsBlendblock(),
								Ogre::HlmsParamVec()));
				datablock->setWorkflow(Ogre::HlmsPbsDatablock::Workflows::MetallicWorkflow);
				mPbsDataBlockList[materialName] = datablock;
			}
			else
			{
				datablock = static_cast<Ogre::HlmsPbsDatablock*>(HlmsPbs->getDatablock(Ogre::IdString(materialName)));
			}
			//datablock
			//---
			switch (event.type)
			{
			case ape::Event::Type::MATERIAL_PBS_CREATE:
				;
				break;
			case ape::Event::Type::MATERIAL_PBS_DELETE:
				;
				break;
			case ape::Event::Type::MATERIAL_PBS_DIFFUSE: //base color + alfa
			{
				datablock->setDiffuse(ConversionToOgre21_Alfaless(materialPbs->getDiffuseColor()));
				float alpha = (ConversionToOgre21_Alfa(materialPbs->getDiffuseColor()));
				auto transparentMode = (alpha == 1) ? Ogre::HlmsPbsDatablock::None : Ogre::HlmsPbsDatablock::Transparent;
				datablock->setTransparency(alpha, transparentMode);
				/*ape::Color clr = materialPbs->getDiffuseColor();
				omat->setDiffuse(clr.r, clr.g, clr.b,clr.a);*/

			}
				
				break;
			case ape::Event::Type::MATERIAL_PBS_SPECULAR:
			{
				/*ape::Color clr = materialPbs->getSpecularColor();
				omat->setSpecular(clr.r, clr.g, clr.b, clr.a);*/
				datablock->setSpecular(ConversionToOgre21_SCPECULAR(materialPbs->getSpecularColor()));
			}
				break;
			case ape::Event::Type::MATERIAL_PBS_AMBIENT:
			{
				/*ape::Color clr = materialPbs->getAmbientColor();
				omat->setAmbient(clr.r, clr.g, clr.b);*/
			}
				break;
			case ape::Event::Type::MATERIAL_PBS_SHADINGMODE:
			{
				/*if (materialPbs->getShadingMode() == "SO_GOURAUD")
				{
					omat->setShadingMode(Ogre::SO_GOURAUD);
				}
				else if (materialPbs->getShadingMode() == "SO_FLAT")
				{
					omat->setShadingMode(Ogre::SO_FLAT);
				}*/
			}
				break;
			case ape::Event::Type::MATERIAL_PBS_SHININESS:
			{
				//omat->setShininess(Ogre::Real(materialPbs->getShininess()));
			}
			case ape::Event::Type::MATERIAL_PBS_EMISSIVE:
			{
				/*ape::Color clr = materialPbs->getEmissiveColor();
				omat->setSelfIllumination(clr.r, clr.g, clr.b);*/
				datablock->setEmissive(ConversionToOgre21_Alfaless(materialPbs->getEmissiveColor()));
			}
				break;
			case ape::Event::Type::MATERIAL_PBS_ALPHAMODE:
			{
				if (materialPbs->getAlphaMode() == "BLEND")
				{
					auto blendBlock = *datablock->getBlendblock();
					blendBlock.setBlendType(Ogre::SBT_TRANSPARENT_ALPHA);
					datablock->setBlendblock(blendBlock);
				}
				else if (materialPbs->getAlphaMode() == "MASK")
				{
					datablock->setAlphaTest(Ogre::CMPF_GREATER_EQUAL);
				}
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_BASECOLOR_TEXTURE:
			{
				
				/*auto texture = materialPbs->getBaseColorTexture();
				Ogre::TexturePtr texptr = mpRoot->getTextureManager()->getByName(texture);
				datablock->setTexture(Ogre::PbsTextureTypes::PBSM_DIFFUSE, 0, texptr);*/


				/*static Ogre::uint8 s_RGB[] = { 128, 0, 255, 128, 0, 255, 128, 0, 255, 128, 0, 255 };
				// attempt to load the image
				Ogre::Image image;
				std::string path = materialPbs->getBaseColorTexture();
				// possibly if we fail to actually find it, pop up a box?
				Ogre::String pathname(materialPbs->getBaseColorTexture());

				std::ifstream imgstream;
				imgstream.open(path.data(), std::ios::binary);
				if (!imgstream.is_open())
					imgstream.open(Ogre::String(path + Ogre::String("\\") + Ogre::String(path.data())).c_str(), std::ios::binary);

				if (imgstream.is_open())
				{
					// Wrap as a stream
					Ogre::DataStreamPtr strm(OGRE_NEW Ogre::FileStreamDataStream(path.data(), &imgstream, false));

					if (!strm->size() || strm->size() == 0xffffffff)
					{
						// fall back to our very simple and very hardcoded hot-pink version
						Ogre::DataStreamPtr altStrm(OGRE_NEW Ogre::MemoryDataStream(s_RGB, sizeof(s_RGB)));
						image.loadRawData(altStrm, 2, 2, 1, Ogre::PF_R8G8B8);
						
					}
					else
					{
						// extract extension from filename
						size_t pos = pathname.find_last_of('.');
						Ogre::String ext = pathname.substr(pos + 1);
						image.load(strm, ext);
						imgstream.close();
					}
				}
				else {
					// fall back to our very simple and very hardcoded hot-pink version
					Ogre::DataStreamPtr altStrm(OGRE_NEW Ogre::MemoryDataStream(s_RGB, sizeof(s_RGB)));
					image.loadRawData(altStrm, 2, 2, 1, Ogre::PF_R8G8B8);
					
				}

				// Ogre::TextureManager::getSingleton().loadImage(Ogre::String(szPath.data), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, image);
				//TODO: save this to materials/textures ?
				//Ogre::TextureUnitState* texUnitState = omat->getTechnique(0)->getPass(0)->createTextureUnitState(basename);
				
				const auto renderSystem = Ogre::Root::getSingleton().getRenderSystem();
				const auto renderSystemGammaConversionConfigOption = renderSystem->getConfigOptions().at("sRGB Gamma Conversion");
				bool isHardwareGammaEnabled;
				if (renderSystemGammaConversionConfigOption.currentValue != "Yes")
				{
					isHardwareGammaEnabled = true;
				}
				else
				{
					isHardwareGammaEnabled = false;
				}
				const auto pixelFormat = [&] {
					if (!(image.getHasAlpha())) return Ogre::PF_BYTE_RGB;
					if (image.getHasAlpha()) return Ogre::PF_BYTE_RGBA;
				}();
				Ogre::TexturePtr BaseTex = mpRoot->getTextureManager()->createManual(materialPbs->getMetallicRoughnessTexture(),
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					Ogre::TextureType::TEX_TYPE_2D_ARRAY, image.getWidth(),
					image.getHeight(),
					1,
					1,
					pixelFormat,
					Ogre::TU_DEFAULT,
					nullptr,
					isHardwareGammaEnabled);*/


				Ogre::Image img = Ogre::Image();
				img.load(materialPbs->getBaseColorTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				//Ogre::TexturePtr metalTex = mpRoot->getTextureManager()->loadImage(materialPbs->getMetallicRoughnessTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, img);

				const auto pixelFormat = [&] {
					if (!(img.getHasAlpha())) return Ogre::PF_BYTE_RGB;
					if (img.getHasAlpha()) return Ogre::PF_BYTE_RGBA;
				}();

				const auto renderSystem = Ogre::Root::getSingleton().getRenderSystem();
				bool isHardwareGammaEnabled;
				const auto renderSystemGammaConversionConfigOption = renderSystem->getConfigOptions().at("sRGB Gamma Conversion");
				if (renderSystemGammaConversionConfigOption.currentValue != "Yes")
				{
					isHardwareGammaEnabled = true;
				}
				else
				{
					isHardwareGammaEnabled = false;
				}



				Ogre::TexturePtr BaseTex = mpRoot->getTextureManager()->createManual(materialPbs->getMetallicRoughnessTexture(),
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					Ogre::TextureType::TEX_TYPE_2D_ARRAY, img.getWidth(),
					img.getHeight(),
					1,
					1,
					pixelFormat,
					Ogre::TU_DEFAULT,
					nullptr,
					isHardwareGammaEnabled);
				
				BaseTex->loadImage(img);
				datablock->setTexture(Ogre::PbsTextureTypes::PBSM_DIFFUSE, 0, BaseTex);
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_METALLICROUGHNESS_TEXTURE:
			{
				Ogre::Image img = Ogre::Image();
				img.load(materialPbs->getMetallicRoughnessTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				//Ogre::TexturePtr metalTex = mpRoot->getTextureManager()->loadImage(materialPbs->getMetallicRoughnessTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, img);

				const auto pixelFormat = [&] {
					if (!(img.getHasAlpha())) return Ogre::PF_BYTE_RGB;
					if (img.getHasAlpha()) return Ogre::PF_BYTE_RGBA;
				}();

				const auto renderSystem = Ogre::Root::getSingleton().getRenderSystem();
				bool isHardwareGammaEnabled;
				const auto renderSystemGammaConversionConfigOption = renderSystem->getConfigOptions().at("sRGB Gamma Conversion");
				if (renderSystemGammaConversionConfigOption.currentValue != "Yes")
				{
					isHardwareGammaEnabled = true;
				}
				else
				{
					isHardwareGammaEnabled = false;
				}
					
				

				Ogre::TexturePtr metalTex = mpRoot->getTextureManager()->createManual(materialPbs->getMetallicRoughnessTexture(),
																					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
																					Ogre::TextureType::TEX_TYPE_2D_ARRAY, img.getWidth(),
																					img.getHeight(),
																					1,
																					1,
																					pixelFormat,
																					Ogre::TU_DEFAULT,
																					nullptr,
																					isHardwareGammaEnabled);

				metalTex->loadImage(img);
				if (metalTex)
				{
					//OgreLog("metalness greyscale texture extracted by textureImporter : " + metalTexure->getName());
					datablock->setTexture(Ogre::PbsTextureTypes::PBSM_METALLIC, 0, metalTex);
				}
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_BASECOLOR:
			{
				//datablock->setFresnel(ConversionToOgre21_Alfaless(materialPbs->getF0()),false);
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_METALNESS:
			{
				datablock->setMetalness(materialPbs->getMetalness());
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_ROUGHNESS:
			{
				datablock->setRoughness(materialPbs->getRoughness());
			}
			break; 
			case ape::Event::Type::MATERIAL_PBS_NORMAL_TEXTURE:
			{
				Ogre::Image img = Ogre::Image();
				img.load(materialPbs->getNormalTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				//Ogre::TexturePtr metalTex = mpRoot->getTextureManager()->loadImage(materialPbs->getMetallicRoughnessTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, img);

				const auto pixelFormat = [&] {
					if (!(img.getHasAlpha())) return Ogre::PF_BYTE_RGB;
					if (img.getHasAlpha()) return Ogre::PF_BYTE_RGBA;
				}();

				const auto renderSystem = Ogre::Root::getSingleton().getRenderSystem();
				bool isHardwareGammaEnabled;
				const auto renderSystemGammaConversionConfigOption = renderSystem->getConfigOptions().at("sRGB Gamma Conversion");
				if (renderSystemGammaConversionConfigOption.currentValue != "Yes")
				{
					isHardwareGammaEnabled = true;
				}
				else
				{
					isHardwareGammaEnabled = false;
				}

				
				//auto BaseTex = mpRoot->getTextureManager()->create(materialPbs->getMetallicRoughnessTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TextureType::TEX_TYPE_2D_ARRAY);
				Ogre::TexturePtr BaseTex = mpRoot->getTextureManager()->createManual(materialPbs->getMetallicRoughnessTexture(),
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					Ogre::TextureType::TEX_TYPE_2D_ARRAY, img.getWidth(),
					img.getHeight(),
					1,
					1,
					pixelFormat,
					Ogre::TU_DEFAULT,
					nullptr,
					isHardwareGammaEnabled);
////--
//				Ogre::HlmsTextureManager::TextureLocation texLocation =
//					mpRoot->getHlmsManager()->getTextureManager()->createOrRetrieveTexture(materialPbs->getBaseColorTexture(), materialPbs->getBaseColorTexture(), Ogre::HlmsTextureManager::TEXTURE_TYPE_DIFFUSE);
//				
//				texLocation.texture = BaseTex;
//
//
////--
				BaseTex->loadImage(img);
				BaseTex->load();
				datablock->setTexture(Ogre::PbsTextureTypes::PBSM_DIFFUSE,0 , BaseTex);
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_EMISSIVE_TEXTURE:
			{
				Ogre::Image img = Ogre::Image();
				img.load(materialPbs->getEmissiveTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				//Ogre::TexturePtr metalTex = mpRoot->getTextureManager()->loadImage(materialPbs->getMetallicRoughnessTexture(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, img);

				const auto pixelFormat = [&] {
					if (!(img.getHasAlpha())) return Ogre::PF_BYTE_RGB;
					if (img.getHasAlpha()) return Ogre::PF_BYTE_RGBA;
				}();

				const auto renderSystem = Ogre::Root::getSingleton().getRenderSystem();
				bool isHardwareGammaEnabled;
				const auto renderSystemGammaConversionConfigOption = renderSystem->getConfigOptions().at("sRGB Gamma Conversion");
				if (renderSystemGammaConversionConfigOption.currentValue != "Yes")
				{
					isHardwareGammaEnabled = true;
				}
				else
				{
					isHardwareGammaEnabled = false;
				}



				Ogre::TexturePtr EmissiveTex = mpRoot->getTextureManager()->createManual(materialPbs->getMetallicRoughnessTexture(),
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					Ogre::TextureType::TEX_TYPE_2D_ARRAY, img.getWidth(),
					img.getHeight(),
					1,
					1,
					pixelFormat,
					Ogre::TU_DEFAULT,
					nullptr,
					isHardwareGammaEnabled);

				EmissiveTex->loadImage(img);

				datablock->setTexture(Ogre::PbsTextureTypes::PBSM_EMISSIVE, 0, EmissiveTex);
				
			}
			break;
			/*case ape::Event::Type::MATERIAL_PBS_TEXTURE:
			{

			}
			break;
			case ape::Event::Type::MATERIAL_PBS_CULLINGMODE:
			{
				
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_DEPTHBIAS:
			{
				
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_LIGHTING:
			{
				
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_SCENEBLENDING:
			{
				
			}
			break;
			case ape::Event::Type::MATERIAL_PBS_OVERLAY:
			{
				
			}
			break;*/
			}
			
		}
//		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);
		}
		else if (event.group == ape::Event::Group::MATERIAL_MANUAL)
		{
			if (auto materialManual = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string materialName = materialManual->getName();
				auto result = Ogre::MaterialManager::getSingleton().createOrRetrieve(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				//--
				
			
				//---
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
					ogreMaterial->setDiffuse(ConversionToOgre21(materialManual->getDiffuseColor()));
					//datablock->setDiffuse;
					break;
				case ape::Event::Type::MATERIAL_MANUAL_SPECULAR:
					ogreMaterial->setSpecular(ConversionToOgre21(materialManual->getSpecularColor()));
					break;
				case ape::Event::Type::MATERIAL_MANUAL_AMBIENT:
					ogreMaterial->setAmbient(ConversionToOgre21(materialManual->getAmbientColor()));
					break;
				case ape::Event::Type::MATERIAL_MANUAL_EMISSIVE:
					ogreMaterial->setSelfIllumination(ConversionToOgre21(materialManual->getEmissiveColor()));
					break;
				case ape::Event::Type::MATERIAL_MANUAL_PASS:
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
				case ape::Event::Type::MATERIAL_MANUAL_TEXTURE:
				{
					if (auto texture = materialManual->getPassTexture().lock())
					{
						Ogre::TexturePtr ogreTexture = Ogre::TextureManager::getSingleton().getByName(texture->getName(), Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
						Ogre::HlmsTextureManager* texmgr = mpRoot->getHlmsManager()->getTextureManager();
						//texmgr->createOrRetrieveTexture(texture->getName(),)
						if (!ogreTexture.isNull() && !ogreMaterial.isNull())
						{
							if (!ogreMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates())
								ogreMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
							ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(ogreTexture);

						}
					}
				}
				break;
				case ape::Event::Type::MATERIAL_MANUAL_CULLINGMODE:
				{
					// Ogre v1
					//ogreMaterial->setCullingMode(ape::ConversionToOgre21(materialManual->getCullingMode()));
				}
				break;
				case ape::Event::Type::MATERIAL_MANUAL_DEPTHBIAS:
				{
					// Ogre v1
					//ogreMaterial->setDepthBias(materialManual->getDepthBias().x, materialManual->getDepthBias().x);
				}
				break;
				case ape::Event::Type::MATERIAL_MANUAL_LIGHTING:
				{
					//ogre v1
					/*ogreMaterial->setLightingEnabled(materialManual->getLightingEnabled());
					if (mpShaderGenerator)
						mpShaderGenerator->removeAllShaderBasedTechniques(ogreMaterial->getName());
					mpShaderGeneratorResolver->appendIgnoreList(ogreMaterial->getName());*/
				}
				break;
				case ape::Event::Type::MATERIAL_MANUAL_SCENEBLENDING:
				{
					//ogre v1
					/*ogreMaterial->setCullingMode(ape::ConversionToOgre(materialManual->getCullingMode()));
					ogreMaterial->setSceneBlending(ape::ConversionToOgre(materialManual->getSceneBlendingType()));
					if (materialManual->getSceneBlendingType() == ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA)
						ogreMaterial->setDepthWriteEnabled(false);*/
				}
				break;
				case ape::Event::Type::MATERIAL_MANUAL_OVERLAY:
				{
					//Ogre v1
					/*auto overlay = Ogre::OverlayManager::getSingleton().getByName(materialName);
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
					else if (overlay)
						overlay->hide();*/
				}
				break;
				}
			}
			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);
		}
		/*else if (event.group == ape::Event::Group::PASS_PBS)
		{
			if (auto passPbs = std::static_pointer_cast<ape::IPbsPass>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string passPbsName = passPbs->getName();
				auto result = Ogre::MaterialManager::getSingleton().createOrRetrieve(passPbsName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				Ogre::MaterialPtr ogrePbsPassMaterial = result.first.staticCast<Ogre::Material>();
				if (!ogrePbsPassMaterial.isNull())
				{
					switch (event.type)
					{
					case ape::Event::Type::MATERIAL_PBS_CREATE:
					{
						ogrePbsPassMaterial->createTechnique()->createPass();
						Ogre::PbsMaterial* ogrePbsMaterial = new Ogre::PbsMaterial();
						mPbsMaterials[passPbsName] = ogrePbsMaterial;
					}
					break;
					case ape::Event::Type::MATERIAL_PBS_AMBIENT:
						ogrePbsPassMaterial->setAmbient(ConversionToOgre21(passPbs->getAmbientColor()));
						break;
					case ape::Event::Type::MATERIAL_PBS_DIFFUSE:
						ogrePbsPassMaterial->setDiffuse(ConversionToOgre21(passPbs->getDiffuseColor()));
						break;
					case ape::Event::Type::MATERIAL_PBS_EMISSIVE:
						ogrePbsPassMaterial->setSelfIllumination(ConversionToOgre21(passPbs->getEmissiveColor()));
						break;
					case ape::Event::Type::MATERIAL_PBS_SPECULAR:
						ogrePbsPassMaterial->setSpecular(ConversionToOgre21(passPbs->getSpecularColor()));
						break;
					case ape::Event::Type::MATERIAL_PBS_SHININESS:
						ogrePbsPassMaterial->setShininess(passPbs->getShininess());
						break;
					case ape::Event::Type::MATERIAL_PBS_ALBEDO:
						mPbsMaterials[passPbsName]->setAlbedo(ConversionToOgre21(passPbs->getAlbedo()));
						break;
					case ape::Event::Type::MATERIAL_PBS_F0:
						mPbsMaterials[passPbsName]->setF0(ConversionToOgre21(passPbs->getF0()));
						break;
					case ape::Event::Type::MATERIAL_PBS_ROUGHNESS:
						mPbsMaterials[passPbsName]->setRoughness(passPbs->getRoughness());
						break;
					case ape::Event::Type::MATERIAL_PBS_LIGHTROUGHNESSOFFSET:
						mPbsMaterials[passPbsName]->setLightRoughnessOffset(passPbs->getLightRoughnessOffset());
						break;
					case ape::Event::Type::MATERIAL_PBS_DELETE:
						;
						break;
					}
				}
			}
		}*/

		else if (event.group == ape::Event::Group::PASS_MANUAL)
		{
			if (auto passManual = std::static_pointer_cast<ape::IManualPass>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string passManualName = passManual->getName();
				auto result = Ogre::MaterialManager::getSingleton().createOrRetrieve(passManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				Ogre::MaterialPtr ogreManualPassMaterial = result.first.staticCast<Ogre::Material>();
				if (!ogreManualPassMaterial.isNull())
				{
					switch (event.type)
					{
					case ape::Event::Type::PASS_MANUAL_CREATE:
						ogreManualPassMaterial->createTechnique()->createPass();
						break;
					case ape::Event::Type::PASS_MANUAL_AMBIENT:
						ogreManualPassMaterial->setAmbient(ConversionToOgre21(passManual->getAmbientColor()));
						break;
					case ape::Event::Type::PASS_MANUAL_DIFFUSE:
						ogreManualPassMaterial->setDiffuse(ConversionToOgre21(passManual->getDiffuseColor()));
						break;
					case ape::Event::Type::PASS_MANUAL_EMISSIVE:
						ogreManualPassMaterial->setSelfIllumination(ConversionToOgre21(passManual->getEmissiveColor()));
						break;
					case ape::Event::Type::PASS_MANUAL_SPECULAR:
						ogreManualPassMaterial->setSpecular(ConversionToOgre21(passManual->getSpecularColor()));
						break;
					case ape::Event::Type::PASS_MANUAL_SHININESS:
						ogreManualPassMaterial->setShininess(passManual->getShininess());
						break;
					case ape::Event::Type::PASS_MANUAL_SCENEBLENDING:
					{
						//ogre v1
						/*ogreManualPassMaterial->setSceneBlending(ConversionToOgre21(passManual->getSceneBlendingType()));
						if (passManual->getSceneBlendingType() == ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA)
							ogreManualPassMaterial->setDepthWriteEnabled(false);*/
					}
					break;
					case ape::Event::Type::PASS_MANUAL_TEXTURE:
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
					case ape::Event::Type::PASS_MANUAL_GPUPARAMETERS:
					{
						Ogre::GpuProgramParametersSharedPtr ogreGpuParameters = ogreManualPassMaterial->getTechnique(0)->getPass(0)->getVertexProgramParameters();
						if (!ogreGpuParameters.isNull())
						{
							for (auto passGpuParameter : passManual->getPassGpuParameters())
								ogreGpuParameters->setNamedConstant(passGpuParameter.name, ConversionToOgre21(passGpuParameter.value));
						}
					}
					break;
					case ape::Event::Type::PASS_MANUAL_DELETE:
						;
						break;
					}
				}
			}
		}
		else if (event.group == ape::Event::Group::TEXTURE_FILE)
		{
			if (auto textureManual = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string textureManualName = textureManual->getName();
				switch (event.type)
				{
				case ape::Event::Type::TEXTURE_FILE_CREATE:
					break;
				case ape::Event::Type::TEXTURE_FILE_FILENAME:
				{
					auto ogreTexture = Ogre::TextureManager::getSingleton().createOrRetrieve(textureManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
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
					auto ogreTexture = Ogre::TextureManager::getSingleton().createManual(textureManualName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
						Ogre::TEX_TYPE_2D, Ogre::uint(parameters.width), Ogre::uint(parameters.height), 0, ape::ConversionToOgre21(parameters.pixelFormat),
						ape::ConversionToOgre21(parameters.usage), nullptr, true, mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].fsaa, mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].fsaaHint);
					if (mOgreRenderPluginConfig.renderSystem == "OGL")
					{
						/*GLuint glid;
						ogreTexture->getCustomAttribute("GLID", &glid);
						textureManual->setGraphicsApiID((void*)glid);*/
						bool outIsFsaa;
						textureManual->setGraphicsApiID((void*)static_cast<Ogre::GL3PlusTexture*>(Ogre::TextureManager::getSingleton().getByName(textureManualName).getPointer())->getGLID(outIsFsaa));
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
						// Ogre v1
						////APE_LOG_DEBUG("TEXTURE_MANUAL_BUFFER write begin");
						//Ogre::HardwarePixelBufferSharedPtr texBuf = ogreTexture->getBuffer();
						//texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
						//memcpy(texBuf->getCurrentLock().data, textureManual->getBuffer(), textureManual->getParameters().width * textureManual->getParameters().height * 4);
						//texBuf->unlock();
						///*static int s = 1;
						//std::wostringstream oss;
						//oss << std::setw(4) << std::setfill(L'0') << s++ << L".bmp";
						//ape::SaveVoidBufferToImage(oss.str(), textureManual->getBuffer(), textureManual->getParameters().width, textureManual->getParameters().height);*/
						////APE_LOG_DEBUG("TEXTURE_MANUAL_BUFFER write end");
						
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
							if (auto ogreCamera = mpSceneMgr->findCamera(camera->getName()))
							{
								if (auto ogreRenderTexture = ogreTexture->getBuffer()->getRenderTarget())
								{

									//ogreRenderTexture->setAutoUpdated(true);
									if (auto ogreViewport = ogreRenderTexture->addViewport())
									{
										//ogreViewport->setClearEveryFrame(true);
										//ogreViewport->setAutoUpdated(true);
										if (mOgreRenderPluginConfig.shading == "perPixel" || mOgreRenderPluginConfig.shading == "")
										{
											// Ogre v1
											//ogreViewport->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
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
					//Ogre v1
					/*if (!ogreMaterial.isNull())
					{
						auto ogreTextureUnit = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
						if (ogreTextureUnit)
							ogreTextureUnit->setTextureAddressingMode(ape::ConversionToOgre21(textureUnit->getTextureAddressingMode()));
					}*/
				}
				break;
				case ape::Event::Type::TEXTURE_UNIT_FILTERING:
				{
					//Ogre v1
					/*if (!ogreMaterial.isNull())
					{
						auto ogreTextureUnit = ogreMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
						if (ogreTextureUnit)
							ogreTextureUnit->setTextureFiltering(ape::ConversionToOgre21(textureUnit->getTextureFiltering().minFilter), ape::ConversionToOgre21(textureUnit->getTextureFiltering().magFilter), ape::ConversionToOgre21(textureUnit->getTextureFiltering().mipFilter));
					}
					*/
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
							Ogre::Ray ray = mOgreCameras[0]->getCameraToViewportRay(rayOverlayNode->getPosition().x / mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].width,
								rayOverlayNode->getPosition().y / mOgreRenderPluginConfig.ogreRenderWindowConfigList[0].height); //TODO_apeOgreRenderPlugin check enabled window in ogreRenderWindowConfigList
							Ogre::RaySceneQuery *raySceneQuery = mpSceneMgr->createRayQuery(ray);
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
		
		else if (event.group == ape::Event::Group::CAMERA)
		{
			if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				switch (event.type)
				{
				case ape::Event::Type::CAMERA_CREATE:
				{
					mOgreCameras.push_back(mpSceneMgr->createCamera(event.subjectName));
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
					if (mpSceneMgr->findCamera(event.subjectName))
					{
						Ogre::Camera* ogreCamera = mpSceneMgr->findCamera(event.subjectName);
						if (ogreCamera != nullptr)
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

											if (auto ogreViewPort = mRenderWindows[camera->getWindow()]->addViewport(left,top,width,height))
											{
												APE_LOG_DEBUG("ogreViewport: " << "zorder: " << zorder << " left: " << left << " top: " << top << " width: " << width << " height: " << height);
													
												ogreCamera->setAspectRatio(Ogre::Real(ogreViewPort->getActualWidth()) / Ogre::Real(ogreViewPort->getActualHeight()));
												/*if (mOgreRenderPluginConfig.shading == "perPixel" || mOgreRenderPluginConfig.shading == "")
												{
													ogreViewPort->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
												}*///nincs
												//------------------
												mpActualRenderwindow = mRenderWindows[camera->getWindow()];
												if (!mSkyBoxMaterial.isNull())
												{
													Ogre::CompositorManager2* compositorManager = mpRoot->getCompositorManager2();
													//Ogre::IdString workspaceName("MyOwnWorkspace");
												
													//if (!compositorManager->hasWorkspaceDefinition(workspaceName))
													//	compositorManager->createBasicWorkspaceDef("MyOwnWorkspace", Ogre::ColourValue(0.6f, 0.0f, 0.6f));

													//compositorManager->addWorkspace(mpSceneMgr, mRenderWindows[camera->getWindow()], ogreCamera,
													//	"MyOwnWorkspace", true);
													//compositorManager->addWorkspaceDefinition("WorkSpace01");
													mpActualWorkSpace = compositorManager->addWorkspace(mpSceneMgr, mRenderWindows[camera->getWindow()], ogreCamera,
														"SkyPostprocessWorkspace", true);
													mpActualWorkSpace->setEnabled(true);
												}
												
												
												//--------
												
												//--------
												//change sky
												/*Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingletonPtr()->getByName("SkyPostprocess");
												if (materialPtr.isNull())
													return;

												Ogre::Material* material = materialPtr.getPointer();
												Ogre::TextureUnitState* tex = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
												tex->setCubicTextureName("SkyBoxNone.dds", true);
												tex->setGamma(2.0);
												material->compile();
												*/

												
												//--------

												
											}
										}
									}
								}
							}
						}
						//ogreCamera->lookAt(Ogre::Vector3(0, 0, 0));
					}
				}
					break;
				case ape::Event::Type::CAMERA_PARENTNODE:
				{
					if (auto ogreCamera = mpSceneMgr->findCamera(camera->getName()))
					{
						if (auto parentNode = camera->getParentNode().lock())
						{
							auto hasSceneNodeList = mpSceneMgr->findSceneNodes(parentNode->getName());
							if (!hasSceneNodeList.empty())
							{
								if (auto ogreParentNode = mpSceneMgr->getSceneNode(hasSceneNodeList[0]->getId()))
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
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setFocalLength(camera->getFocalLength());
				}
				break;
				case ape::Event::Type::CAMERA_ASPECTRATIO:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setAspectRatio(camera->getAspectRatio());
				}
				break;
				case ape::Event::Type::CAMERA_AUTOASPECTRATIO:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setAutoAspectRatio(camera->isAutoAspectRatio());
				}
				break;
				case ape::Event::Type::CAMERA_FOVY:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setFOVy(ConversionToOgre21(camera->getFOVy()));
				}
				break;
				case ape::Event::Type::CAMERA_FRUSTUMOFFSET:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setFrustumOffset(ape::ConversionToOgre21(camera->getFrustumOffset()));
				}
				break;
				case ape::Event::Type::CAMERA_FARCLIP:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setFarClipDistance(camera->getFarClipDistance());
				}
				break;
				case ape::Event::Type::CAMERA_NEARCLIP:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setNearClipDistance(camera->getNearClipDistance());
				}
				break;
				case ape::Event::Type::CAMERA_PROJECTION:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setCustomProjectionMatrix(true, ape::ConversionToOgre21(camera->getProjection()));
				}
				break;
				case ape::Event::Type::CAMERA_PROJECTIONTYPE:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setProjectionType(ConversionToOgre21(camera->getProjectionType()));
				}
				break;
				case ape::Event::Type::CAMERA_ORTHOWINDOWSIZE:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
						mpSceneMgr->findCamera(event.subjectName)->setOrthoWindow(camera->getOrthoWindowSize().x, camera->getOrthoWindowSize().y);
				}
				break;
				case ape::Event::Type::CAMERA_VISIBILITY:
				{
					if (mpSceneMgr->findCamera(event.subjectName))
					{
						Ogre::Viewport* ogreViewport = mpSceneMgr->findCamera(event.subjectName)->getLastViewport();
						//ogrev1
						//if (ogreViewport)
							//ogreViewport->setVisibilityMask(camera->getVisibilityMask());
					}
				}
				break;
				}
			}
		}
		mEventDoubleQueue.pop();
	}
}


bool ape::Ogre21RenderPlugin::frameStarted(const Ogre::FrameEvent& evt)
{
	return Ogre::FrameListener::frameStarted(evt);
}

bool ape::Ogre21RenderPlugin::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	processEventDoubleQueue();
	return Ogre::FrameListener::frameRenderingQueued(evt);
}


bool ape::Ogre21RenderPlugin::frameEnded(const Ogre::FrameEvent& evt)
{

	
	return Ogre::FrameListener::frameEnded(evt);
}


void ape::Ogre21RenderPlugin::Stop()
{

}


void ape::Ogre21RenderPlugin::Suspend()
{

}


void ape::Ogre21RenderPlugin::Restart()
{

}


void ape::Ogre21RenderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	try
	{
		//mpRoot->renderOneFrame();
		mpRoot->startRendering();
	}
	catch (const Ogre::RenderingAPIException& ex)
	{
		std::cout << ex.getFullDescription() << std::endl;
		APE_LOG_ERROR(ex.getFullDescription());
	}
	catch (const Ogre::Exception& ex)
	{
		std::cout << ex.getFullDescription() << std::endl;
		APE_LOG_ERROR(ex.getFullDescription());
	}
	APE_LOG_FUNC_LEAVE();



}


void ape::Ogre21RenderPlugin::Step()
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
		APE_LOG_ERROR(ex.getFullDescription());
	}
	catch (const Ogre::Exception& ex)
	{
		std::cout << ex.getFullDescription() << std::endl;
		APE_LOG_ERROR(ex.getFullDescription());
	}
}









void ape::Ogre21RenderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	/*mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();*/
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeOgre21RenderPlugin.json";
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
												ogreCameraConfig.name = cameraMemberIterator->value.GetString();
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
												ogreCameraConfig.orientationOffset = ape::ConversionFromOgre21(orientationOffset);
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

	mpRoot = OGRE_NEW Ogre::Root("", "", "apeOgre21RenderPlugin.log");


	Ogre::LogManager::getSingleton().createLog("apeOgre21RenderPlugin.log", true, false, false);

	//mpOverlaySys = OGRE_NEW Ogre::v1::OverlaySystem();

#if defined (_DEBUG)
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
	if (mOgreRenderPluginConfig.renderSystem == "DX11")
		mpRoot->loadPlugin("RenderSystem_Direct3D11_d");
	else
		mpRoot->loadPlugin("RenderSystem_GL3Plus_d");
#else
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
	if (mOgreRenderPluginConfig.renderSystem == "DX11")
		mpRoot->loadPlugin("RenderSystem_Direct3D11");
	else
		mpRoot->loadPlugin("RenderSystem_GL3Plus");
#endif




	Ogre::RenderSystem* renderSystem = nullptr;
	if (mOgreRenderPluginConfig.renderSystem == "DX11")
		renderSystem = mpRoot->getRenderSystemByName("Direct3D11 Rendering Subsystem");
	else
		renderSystem = mpRoot->getRenderSystemByName("Open_GL3Plus Rendering Subsystem");



	std::stringstream mediaFolder;
	mediaFolder << APE_SOURCE_DIR << "/plugins/ogre21Render/media";

	mpRoot->setRenderSystem(renderSystem);

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/Hlms", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/models", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/modelsV2", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/scripts", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/PbsMaterials", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/textures", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/textures/Cubemaps", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	/*Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/DebugPack.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/cubemap.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/cubemapsJS.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/dragon.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/fresneldemo.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/OgreCore.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/ogredance.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/profiler.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/SdkTrays.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/Sinbad.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/packs/skybox.zip", "Zip", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);*/
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/scripts/Compositors", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/scripts/materials/TutorialSky_Postprocess", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/apeSky", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);

	for (auto resourceLocation : mpCoreConfig->getNetworkConfig().resourceLocations)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourceLocation, "FileSystem");

	mpRoot->getRenderSystem()->setConfigOption("sRGB Gamma Conversion", "Yes");
	mpRoot->initialise(false, "ape");

	Ogre::InstancingThreadedCullingMethod instancingThreadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD; //-> új miatt

#if OGRE_DEBUG_MODE
	const size_t numThreads = 1;
#else
	const size_t numThreads = std::max<size_t>(1, Ogre::PlatformInformation::getNumLogicalCores());
	if (numThreads > 1)
		instancingThreadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
#endif

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
			//--
			winDesc.miscParams["gamma"] = "true";
			//--
			winDescList.push_back(winDesc);
			APE_LOG_DEBUG("winDesc:" << " name=" << winDesc.name << " width=" << winDesc.width << " height=" << winDesc.height << " fullScreen=" << winDesc.useFullScreen);

			mRenderWindows[winDesc.name] = mpRoot->createRenderWindow(winDesc.name, winDesc.width, winDesc.height, winDesc.useFullScreen, &winDesc.miscParams);
			mRenderWindows[winDesc.name]->setDeactivateOnFocusChange(false);
			mRenderWindows[winDesc.name]->setHidden(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].hidden);


			mpSceneMgr = mpRoot->createSceneManager(Ogre::ST_GENERIC, numThreads, instancingThreadedCullingMethod);// ->új miatt

		}
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

	//Must be after creating Rendering Window
	registerHlms();



	// Initialise, parse scripts etc
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);


	//------makessome ground
	
	/*


	Ogre::v1::MeshPtr planeMeshV1 = Ogre::v1::MeshManager::getSingleton().createPlane("Plane v1",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::Plane(Ogre::Vector3::UNIT_Y, 10.0f), 500.0f, 500.0f,
		1, 1, true, 1, 4.0f, 4.0f, Ogre::Vector3::UNIT_Z,
		Ogre::v1::HardwareBuffer::HBU_STATIC,
		Ogre::v1::HardwareBuffer::HBU_STATIC);

	Ogre::MeshPtr planeMesh = Ogre::MeshManager::getSingleton().createManual(
		"Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	planeMesh->importV1(planeMeshV1.get(), true, true, true);

	{
		Ogre::Item *item = mpSceneMgr->createItem(planeMesh, Ogre::SCENE_DYNAMIC);
		item->setDatablock("Marble");
		Ogre::SceneNode *sceneNode = mpSceneMgr->getRootSceneNode(Ogre::SCENE_DYNAMIC)->
			createChildSceneNode(Ogre::SCENE_DYNAMIC);
		sceneNode->setPosition(0, -25, 0);
		sceneNode->attachObject(item);

		//Change the addressing mode of the roughness map to wrap via code.
		//Detail maps default to wrap, but the rest to clamp.
		assert(dynamic_cast<Ogre::HlmsPbsDatablock*>(item->getSubItem(0)->getDatablock()));
		Ogre::HlmsPbsDatablock *datablock = static_cast<Ogre::HlmsPbsDatablock*>(
			item->getSubItem(0)->getDatablock());
		//Make a hard copy of the sampler block
		Ogre::HlmsSamplerblock samplerblock(*datablock->getSamplerblock(Ogre::PBSM_ROUGHNESS));
		samplerblock.mU = Ogre::TAM_WRAP;
		samplerblock.mV = Ogre::TAM_WRAP;
		samplerblock.mW = Ogre::TAM_WRAP;
		//Set the new samplerblock. The Hlms system will
		//automatically create the API block if necessary
		datablock->setSamplerblock(Ogre::PBSM_ROUGHNESS, samplerblock);
	}
	//-------------
	//***
	
			Ogre::String meshName;

			
			
				meshName = "Cube_d.mesh";
				Ogre::MeshPtr v2Mesh;
				v2Mesh = Ogre::MeshManager::getSingleton().load(
					meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			Ogre::Item *item = mpSceneMgr->createItem(meshName,
				Ogre::ResourceGroupManager::
				AUTODETECT_RESOURCE_GROUP_NAME,
				Ogre::SCENE_DYNAMIC);
			
				item->setDatablock("Rocks");
			


				//item->setDatablock("Rocks");
			//item->setVisibilityFlags(0x000000001);

			
				mItemList[meshName] = item;
			auto mSceneNode = mpSceneMgr->getRootSceneNode(Ogre::SCENE_DYNAMIC)->
				createChildSceneNode(Ogre::SCENE_DYNAMIC);

			mSceneNode->setPosition(50.0,10.0,10.0);
			mSceneNode->setScale(10.65f, 10.65f, 10.65f);

			//mSceneNode->roll(Ogre::Radian((Ogre::Real)idx));

			mSceneNode->attachObject(item);

			meshName = "Sphere1000.mesh";
			Ogre::MeshPtr v2Mesh2;
			v2Mesh2 = Ogre::MeshManager::getSingleton().load(
				meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			Ogre::Item *item2 = mpSceneMgr->createItem(meshName,
				Ogre::ResourceGroupManager::
				AUTODETECT_RESOURCE_GROUP_NAME,
				Ogre::SCENE_DYNAMIC);

			mItemList[meshName] = item2;
			auto mSceneNode2 = mpSceneMgr->getRootSceneNode(Ogre::SCENE_DYNAMIC)->
				createChildSceneNode(Ogre::SCENE_DYNAMIC);

			mSceneNode2->setPosition(100.0, 100.0, 10.0);
			mSceneNode2->setScale(100.65f, 100.65f, 100.65f);

			//mSceneNode->roll(Ogre::Radian((Ogre::Real)idx));

			mSceneNode2->attachObject(item2);			
			*/
	//***
	APE_LOG_FUNC_LEAVE();
}

void ape::Ogre21RenderPlugin::registerHlms()
{
	static const Ogre::String OGRE_RENDERSYSTEM_DIRECTX11 = "Direct3D11 Rendering Subsystem";
	static const Ogre::String OGRE_RENDERSYSTEM_OPENGL3PLUS = "OpenGL 3+ Rendering Subsystem";
	static const Ogre::String OGRE_RENDERSYSTEM_METAL = "Metal Rendering Subsystem";

	std::stringstream mediaFolder;
	mediaFolder << APE_SOURCE_DIR << "/plugins/ogre21Render/media";

	Ogre::String dataFolder = mediaFolder.str() + "/";
	Ogre::RenderSystem* renderSystem =  mpRoot->getRenderSystem();

	Ogre::String shaderSyntax = "GLSL";
	if (renderSystem->getName() == OGRE_RENDERSYSTEM_DIRECTX11)
		shaderSyntax = "HLSL";
	else if (renderSystem->getName() == OGRE_RENDERSYSTEM_METAL)
		shaderSyntax = "Metal";

	Ogre::Archive* archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(
		dataFolder + "Hlms/Common/" + shaderSyntax,
		"FileSystem", true);
	Ogre::Archive* archiveLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(
		dataFolder + "Hlms/Common/Any",
		"FileSystem", true);
	Ogre::Archive* archivePbsLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(
		dataFolder + "Hlms/Pbs/Any",
		"FileSystem", true);
	Ogre::Archive* archiveUnlitLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(
		dataFolder + "Hlms/Unlit/Any",
		"FileSystem", true);

	Ogre::ArchiveVec library;
	library.push_back(archiveLibrary);
	library.push_back(archiveLibraryAny);

	Ogre::Archive* archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(
		dataFolder + "Hlms/Unlit/" + shaderSyntax,
		"FileSystem", true);

	library.push_back(archiveUnlitLibraryAny);
	Ogre::HlmsUnlit* hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(archiveUnlit, &library);
	mpRoot->getHlmsManager()->registerHlms(hlmsUnlit);
	library.pop_back();

	Ogre::Archive* archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(
		dataFolder + "Hlms/Pbs/" + shaderSyntax,
		"FileSystem", true);

	library.push_back(archivePbsLibraryAny);
	Ogre::HlmsPbs* hlmsPbs = OGRE_NEW Ogre::HlmsPbs(archivePbs, &library);
	mpRoot->getHlmsManager()->registerHlms(hlmsPbs);
	library.pop_back();

	if (renderSystem->getName() == "Direct3D11 Rendering Subsystem")
	{
		//Set lower limits 512kb instead of the default 4MB per Hlms in D3D 11.0
		//and below to avoid saturating AMD's discard limit (8MB) or
		//saturate the PCIE bus in some low end machines.
		bool supportsNoOverwriteOnTextureBuffers;
		renderSystem->getCustomAttribute("MapNoOverwriteOnDynamicBufferSRV",
			&supportsNoOverwriteOnTextureBuffers);

		if (!supportsNoOverwriteOnTextureBuffers)
		{
			hlmsPbs->setTextureBufferDefaultSize(512 * 1024);
			hlmsUnlit->setTextureBufferDefaultSize(512 * 1024);
		}
	}

}


template <class id_t>
Ogre::IndexBufferPacked* ape::Ogre21RenderPlugin::inflateIndexBufferPacked(ape::GeometryIndexedFaceSetParameters parameters, Ogre::VaoManager* vaomgr,
	Ogre::IndexBufferPacked::IndexType typenum, id_t* buffer)
{
	unsigned int idnum = 3 * parameters.faces.face.size();
	buffer = reinterpret_cast<id_t*>(OGRE_MALLOC_SIMD(
		sizeof(id_t) * idnum, Ogre::MEMCATEGORY_GEOMETRY));
	Ogre::FreeOnDestructor idDtor(buffer);

	for (unsigned int i = 0, offset = 0; i < parameters.faces.face.size(); ++i, offset += 3)
	{
		if (parameters.faces.face[i].size() == 4)
		{
			buffer[offset + 0] = parameters.faces.face[i][0];
			buffer[offset + 1] = parameters.faces.face[i][1];
			buffer[offset + 2] = parameters.faces.face[i][2];
			buffer[offset + 3] = parameters.faces.face[i][3];
		}
		else
		{
			buffer[offset + 0] = parameters.faces.face[i][0];
			buffer[offset + 1] = parameters.faces.face[i][1];
			buffer[offset + 2] = parameters.faces.face[i][2];
		}
		
	}

	Ogre::IndexBufferPacked *indexBuffer = 0;
	try
	{
		indexBuffer = vaomgr->createIndexBuffer(typenum,
			idnum, // number of indices
			Ogre::BT_IMMUTABLE,
			buffer, false);
	}
	catch (Ogre::Exception &e)
	{
		// When keepAsShadow = true, the memory will be freed when the index buffer is destroyed.
		// However if for some weird reason there is an exception raised, the memory will
		// not be freed, so it is up to us to do so.
		// The reasons for exceptions are very rare. But we're doing this for correctness.
		OGRE_FREE_SIMD(indexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
		indexBuffer = 0;
		throw e;
	}

	return indexBuffer;
}