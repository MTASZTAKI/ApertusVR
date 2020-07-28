#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "apeOgre21RenderPlugin.h"
#include "apeOgre21Utilities.h"

ape::Ogre21RenderPlugin::Ogre21RenderPlugin( )
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
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
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CLONE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpRoot = nullptr;
	mpOgreSceneManager = nullptr;
	mRenderWindows = std::map<std::string, Ogre::RenderWindow*>();
	mpGlTFLoaderInterface = nullptr;
	mOgre21RenderPluginConfig = ape::Ogre21RenderPluginConfig();
	mOgreCameras = std::vector<Ogre::Camera*>();
	mpWindowEventUtilities = nullptr;
	mRttList = std::vector<ape::ManualTextureWeakPtr>();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::Ogre21RenderPlugin::~Ogre21RenderPlugin()
{
	APE_LOG_FUNC_ENTER();
	delete mpRoot;
	APE_LOG_FUNC_LEAVE();
}

void ape::Ogre21RenderPlugin::eventCallBack(const ape::Event& event)
{
	mEventDoubleQueue.push(event);
}

void ape::Ogre21RenderPlugin::processEventDoubleQueue()
{
	mEventDoubleQueue.swap();
	//while (!mEventDoubleQueue.emptyPop())
	//{
	//	ape::Event event = mEventDoubleQueue.front();
	//	try
	//	{
	//	if (event.group == ape::Event::Group::NODE)
	//	{
	//		if (auto node = mpSceneManager->getNode(event.subjectName).lock())
	//		{
	//			std::string nodeName = node->getName();
	//			if (event.type == ape::Event::Type::NODE_CREATE)
	//			{
	//				mpOgreSceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
	//			}
	//			else 
	//			{
	//				Ogre::SceneNode* ogreNode = nullptr;
	//				if (mpOgreSceneManager->hasSceneNode(nodeName))
	//					ogreNode = mpOgreSceneManager->getSceneNode(nodeName);
	//				if (ogreNode)
	//				{
	//					switch (event.type)
	//					{
	//					case ape::Event::Type::NODE_PARENTNODE:
	//					{
	//						if (auto parentNode = node->getParentNode().lock())
	//						{
	//							auto ogreOldParentNode = ogreNode->getParentSceneNode();
	//							if (ogreOldParentNode)
	//								ogreOldParentNode->removeChild(ogreNode);
	//							if (mpOgreSceneManager->hasSceneNode(parentNode->getName()))
	//							{
	//								auto ogreNewParentNode = mpOgreSceneManager->getSceneNode(parentNode->getName());
	//								ogreNewParentNode->addChild(ogreNode);

	//								//TODO intresting how to manage visibility in multi-user mode
	//								ogreNewParentNode->setVisible(parentNode->getChildrenVisibility(), true);
	//							}
	//						}
	//					}
	//						break;
	//					case ape::Event::Type::NODE_DETACH:
	//					{
	//						auto ogreParentNode = ogreNode->getParentSceneNode();
	//						if (ogreParentNode)
	//						{
	//							ogreParentNode->removeChild(ogreNode);
	//							mpOgreSceneManager->getRootSceneNode()->addChild(ogreNode);
	//						}
	//					}
	//						break;
	//					case ape::Event::Type::NODE_POSITION:
	//						ogreNode->setPosition(ape::ConversionToOgre(node->getPosition()));
	//						break;
	//					case ape::Event::Type::NODE_ORIENTATION:
	//						ogreNode->setOrientation(ape::ConversionToOgre(node->getOrientation()));
	//						break;
	//					case ape::Event::Type::NODE_SCALE:
	//						ogreNode->setScale(ape::ConversionToOgre(node->getScale()));
	//						break;
	//					case ape::Event::Type::NODE_CHILDVISIBILITY:
	//						ogreNode->setVisible(node->getChildrenVisibility());
	//						break;
	//					case ape::Event::Type::NODE_VISIBILITY:
	//					{
	//						ogreNode->setVisible(node->isVisible(), false);
	//						//APE_LOG_DEBUG(nodeName << " visibility: " << node->isVisible() << " numChild: " << ogreNode->numChildren() << " numEntities: " << ogreNode->numAttachedObjects());
	//					}
	//						break;
	//					case ape::Event::Type::NODE_FIXEDYAW:
	//						ogreNode->setFixedYawAxis(node->isFixedYaw());
	//						break;
	//					case ape::Event::Type::NODE_INHERITORIENTATION:
	//						ogreNode->setInheritOrientation(node->isInheritOrientation());
	//						break;
	//					case ape::Event::Type::NODE_INITIALSTATE:
	//						ogreNode->setInitialState();
	//						break;
	//					case ape::Event::Type::NODE_SHOWBOUNDINGBOX:
	//						ogreNode->showBoundingBox(true);
	//						break;
	//					case ape::Event::Type::NODE_HIDEBOUNDINGBOX:
	//						ogreNode->showBoundingBox(false);
	//						break;
	//					}
	//				}
	//			}
	//		}
	//		else if (event.type == ape::Event::Type::NODE_DELETE)
	//		{
	//			Ogre::SceneNode* ogreNode = nullptr;
	//			if (mpOgreSceneManager->hasSceneNode(event.subjectName))
	//				ogreNode = mpOgreSceneManager->getSceneNode(event.subjectName);
	//			if (ogreNode)
	//			{
	//				ogreNode->removeAndDestroyAllChildren();
	//				//APE_LOG_DEBUG("delete: " << event.subjectName);
	//			}
	//		}
	//	}
	//	else if (event.group == ape::Event::Group::GEOMETRY_FILE)
	//	{
	//		if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
	//		{
	//			std::string geometryName = geometryFile->getName();
	//			std::string fileName = geometryFile->getFileName();
	//			std::string parentNodeName = "";
	//			if (auto parentNode = geometryFile->getParentNode().lock())
	//				parentNodeName = parentNode->getName();
	//			switch (event.type)
	//			{
	//			case ape::Event::Type::GEOMETRY_FILE_CREATE:
	//				;
	//				break;
	//			case ape::Event::Type::GEOMETRY_FILE_PARENTNODE:
	//			{
	//				if (mpOgreSceneManager->hasEntity(geometryName))
	//				{
	//					if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
	//					{
	//						if (mpOgreSceneManager->hasSceneNode(parentNodeName))
	//						{
	//							if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNodeName))
	//							{
	//								ogreParentNode->attachObject(ogreEntity);
	//								//TODO intresting how to manage visibility in multi-user mode
	//								if (auto parentNode = geometryFile->getParentNode().lock())
	//								{
	//									ogreParentNode->setVisible(parentNode->getChildrenVisibility(), true);
	//								}
	//							}
	//						}
	//					}
	//				}
	//			}
	//				break;
	//			case ape::Event::Type::GEOMETRY_FILE_FILENAME:
	//			{
	//				if (fileName.find_first_of(".") != std::string::npos)
	//				{
	//					std::string fileExtension = fileName.substr(fileName.find_last_of("."));
	//					if (fileExtension == ".mesh")
	//					{
	//						createAutoGeneratedLodLevelsIfNeeded(geometryName);
	//					}
	//				}
	//			}
	//				break;
	//			case ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES:
	//			{
	//				if (geometryFile->isMergeSubMeshes())
	//				{
	//					if (mpOgreSceneManager->hasEntity(geometryName))
	//					{
	//						if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
	//						{
	//							auto ogreStaticGeometry = mpOgreSceneManager->createStaticGeometry(geometryName + "SG");
	//							ogreStaticGeometry->addEntity(ogreEntity, Ogre::Vector3::ZERO);
	//							ogreStaticGeometry->build();
	//						}
	//					}
	//				}
	//			}
	//			break;
	//			case ape::Event::Type::GEOMETRY_FILE_EXPORT:
	//			{
	//				if (geometryFile->isExportMesh())
	//				{
	//					if (fileName.find_first_of(".") != std::string::npos)
	//					{
	//						std::string fileExtension = fileName.substr(fileName.find_last_of("."));
	//						if (fileExtension == ".mesh")
	//						{
	//							auto mesh = Ogre::MeshManager::getSingleton().getByName(fileName);
	//							if (!mesh.isNull())
	//								mMeshSerializer.exportMesh(mesh.getPointer(), fileName);
	//						}
	//					}
	//					else if (mpOgreSceneManager->hasManualObject(geometryName))
	//					{
	//						auto ogreManual = mpOgreSceneManager->getManualObject(geometryName);
	//						std::stringstream meshName;
	//						meshName << geometryName << ".mesh";
	//						if (Ogre::MeshManager::getSingleton().getByName(meshName.str()).isNull())
	//						{
	//							auto mesh = ogreManual->convertToMesh(meshName.str());
	//							if (!mesh.isNull())
	//							{
	//								mMeshSerializer.exportMesh(mesh.getPointer(), meshName.str());
	//								Ogre::Mesh::SubMeshIterator subMeshIterator = mesh->getSubMeshIterator();
	//								Ogre::SubMesh* subMesh = nullptr;
	//								while (subMeshIterator.hasMoreElements())
	//								{
	//									subMesh = subMeshIterator.getNext();
	//									std::string materialName = subMesh->getMaterialName();
	//									auto ogreMaterial = Ogre::MaterialManager::getSingletonPtr()->getByName(materialName);
	//									if (!ogreMaterial.isNull())
	//									{
	//										std::string filePath = ogreManual->getName();
	//										std::size_t found = filePath.find_last_of("/\\");
	//										filePath = filePath.substr(0, found + 1);
	//										std::string materialFileName = materialName;
	//										//TODO_apeOgre21RenderPlugin automatic filesystem check for filenames and coding conversion
	//										materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), '<'), materialFileName.end());
	//										materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), '>'), materialFileName.end());
	//										materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), '/'), materialFileName.end());
	//										materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), '/\\'), materialFileName.end());
	//										materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), ':'), materialFileName.end());
	//										materialFileName.erase(std::remove(materialFileName.begin(), materialFileName.end(), ','), materialFileName.end());
	//										//std::wstring wMaterialFileName = utf8_decode(materialFileName.c_str());
	//										//std::wcout << wMaterialFileName << std::endl;
	//										std::size_t materialFileNameHash = std::hash<std::string>{}(materialFileName);
	//										std::stringstream materialFilePath;
	//										materialFilePath << filePath << materialFileNameHash << ".material";
	//										std::ifstream materialFile(materialFilePath.str());
	//										if (!materialFile)
	//											mMaterialSerializer.exportMaterial(ogreMaterial, materialFilePath.str());
	//									}
	//								}
	//								mpOgreSceneManager->destroyManualObject(ogreManual);
	//								auto ogreEntity = mpOgreSceneManager->createEntity(geometryName, meshName.str());
	//							}
	//						}
	//					}
	//				}
	//			}
	//			break;
	//			case ape::Event::Type::GEOMETRY_FILE_MATERIAL:
	//				{
	//					if (mpOgreSceneManager->hasEntity(geometryName))
	//					{
	//						if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
	//						{
	//							if (auto material = geometryFile->getMaterial().lock())
	//							{
	//								auto ogreMaterial = Ogre::MaterialManager::getSingleton().getByName(material->getName());
	//								if (!ogreMaterial.isNull())
	//									ogreEntity->setMaterial(ogreMaterial);
	//							}
	//						}
	//					}
	//				}
	//				break;
	//			case ape::Event::Type::GEOMETRY_FILE_VISIBILITY:
	//				{
	//					if (mpOgreSceneManager->hasEntity(geometryName))
	//					{
	//						if (auto ogreEntity = mpOgreSceneManager->getEntity(geometryName))
	//						{
	//							ogreEntity->setVisibilityFlags(geometryFile->getVisibilityFlag());
	//						}
	//					}
	//				}
	//				break;
	//			}
	//		}
	//		else if (event.type == ape::Event::Type::GEOMETRY_FILE_DELETE)
	//		{
	//			//APE_LOG_DEBUG("GEOMETRY_FILE_DELETE: " << event.subjectName);
	//			if (mpOgreSceneManager->hasEntity(event.subjectName))
	//			{
	//				if (auto ogreEntity = mpOgreSceneManager->getEntity(event.subjectName))
	//				{
	//					//APE_LOG_DEBUG("GEOMETRY_FILE_DELETE BEGIN on entity: " << event.subjectName);
	//					ogreEntity->detachFromParent();
	//					mpOgreSceneManager->destroyEntity(event.subjectName);
	//					//APE_LOG_DEBUG("GEOMETRY_FILE_DELETE END on entity: " << event.subjectName);
	//				}
	//			}
	//			if (mpOgreSceneManager->hasManualObject(event.subjectName))
	//			{
	//				//APE_LOG_DEBUG("GEOMETRY_FILE_DELETE BEGIN on manual: " << event.subjectName);
	//				mpOgreSceneManager->destroyManualObject(event.subjectName);
	//				//APE_LOG_DEBUG("GEOMETRY_FILE_DELETE END on manual: " << event.subjectName);
	//			}
	//		}
	//	}
	//	else if (event.group == ape::Event::Group::LIGHT)
	//	{
	//		if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->getEntity(event.subjectName).lock()))
	//		{
	//			Ogre::Light* ogreLight = nullptr;
	//			if (mpOgreSceneManager->hasLight(light->getName()))
	//				ogreLight = mpOgreSceneManager->getLight(light->getName());
	//			switch (event.type)
	//			{
	//			case ape::Event::Type::LIGHT_CREATE:
	//				mpOgreSceneManager->createLight(light->getName());
	//				break;
	//			case ape::Event::Type::LIGHT_ATTENUATION:
	//				ogreLight->setAttenuation(light->getLightAttenuation().range, light->getLightAttenuation().constant, light->getLightAttenuation().linear, light->getLightAttenuation().quadratic);
	//				break;
	//			case ape::Event::Type::LIGHT_DIFFUSE:
	//				ogreLight->setDiffuseColour(ape::ConversionToOgre(light->getDiffuseColor()));
	//				break;
	//			case ape::Event::Type::LIGHT_DIRECTION:
	//				ogreLight->setDirection(ape::ConversionToOgre(light->getLightDirection()));
	//				break;
	//			case ape::Event::Type::LIGHT_SPECULAR:
	//				ogreLight->setSpecularColour(ape::ConversionToOgre(light->getSpecularColor()));
	//				break;
	//			case ape::Event::Type::LIGHT_SPOTRANGE:
	//				ogreLight->setSpotlightRange(Ogre::Radian(light->getLightSpotRange().innerAngle.toRadian()), Ogre::Radian(light->getLightSpotRange().outerAngle.toRadian()), light->getLightSpotRange().falloff);
	//				break;
	//			case ape::Event::Type::LIGHT_TYPE:
	//				ogreLight->setType(ape::ConversionToOgre(light->getLightType()));
	//				break;
	//			case ape::Event::Type::LIGHT_PARENTNODE:
	//				{
	//					if (auto parentNode = light->getParentNode().lock())
	//					{
	//						if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNode->getName()))
	//							ogreParentNode->attachObject(ogreLight);
	//					}
	//				}
	//				break;
	//			case ape::Event::Type::LIGHT_DELETE:
	//				;
	//				break;
	//			}
	//		}
	//	}
	//	else if (event.group == ape::Event::Group::CAMERA)
	//	{
	//		if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
	//		{
	//			switch (event.type)
	//			{
	//			case ape::Event::Type::CAMERA_CREATE:
	//			{
	//				mOgreCameras.push_back(mpOgreSceneManager->createCamera(event.subjectName));
	//				for (int i = 0; i < mOgre21RenderPluginConfig.ogreRenderWindowConfigList.size(); i++)
	//				{
	//					for (int j = 0; j < mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].viewportList.size(); j++)
	//					{
	//						for (int k = 0; k < mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].cameras.size(); k++)
	//						{
	//							auto cameraSetting = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].cameras[k];
	//							if (cameraSetting.name == camera->getName())
	//							{
	//								camera->setWindow(mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].name);
	//								camera->setFocalLength(1.0f);
	//								camera->setNearClipDistance(cameraSetting.nearClip);
	//								camera->setFarClipDistance(cameraSetting.farClip);
	//								camera->setFOVy(cameraSetting.fovY.toRadian());
	//							}
	//						}
	//					}
	//				}
	//			}
	//			break;
	//			case ape::Event::Type::CAMERA_WINDOW:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//				{
	//					if (auto ogreCamera = mpOgreSceneManager->getCamera(event.subjectName))
	//					{
	//						for (int i = 0; i < mOgre21RenderPluginConfig.ogreRenderWindowConfigList.size(); i++)
	//						{
	//							for (int j = 0; j < mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].viewportList.size(); j++)
	//							{
	//								auto renderWindowSetting = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i];
	//								auto viewportSetting = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j];
	//								for (int k = 0; k < mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].cameras.size(); k++)
	//								{
	//									auto cameraSetting = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].cameras[k];
	//									if (cameraSetting.name == camera->getName())
	//									{
	//										int zorder = viewportSetting.zOrder;
	//										float width = (float)viewportSetting.width / (float)renderWindowSetting.width;
	//										float height = (float)viewportSetting.height / (float)renderWindowSetting.height;
	//										float left = (float)viewportSetting.left / (float)renderWindowSetting.width;
	//										float top = (float)viewportSetting.top / (float)renderWindowSetting.height;
	//										if (!mRenderWindows[camera->getWindow()]->hasViewportWithZOrder(zorder))
	//										{
	//											if (auto ogreViewPort = mRenderWindows[camera->getWindow()]->addViewport(ogreCamera, zorder, left, top, width, height))
	//											{
	//												APE_LOG_DEBUG("ogreViewport: " << "zorder: " << zorder << " left: " << left << " top: " << top << " width: " << width << " height: " << height);
	//												ogreCamera->setAspectRatio(Ogre::Real(ogreViewPort->getActualWidth()) / Ogre::Real(ogreViewPort->getActualHeight()));
	//												if (mOgre21RenderPluginConfig.shading == "perPixel" || mOgre21RenderPluginConfig.shading == "")
	//												{
	//													ogreViewPort->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
	//												}
	//											}
	//										}
	//										else if(auto ogreViewPort = mRenderWindows[camera->getWindow()]->getViewportByZOrder(zorder))
	//										{
	//											APE_LOG_DEBUG("ogreViewport: " << "zorder: " << zorder << " left: " << left << " top: " << top << " width: " << width << " height: " << height);
	//											ogreViewPort->setCamera(ogreCamera);
	//											ogreCamera->setAspectRatio(Ogre::Real(ogreViewPort->getActualWidth()) / Ogre::Real(ogreViewPort->getActualHeight()));
	//											if (mOgre21RenderPluginConfig.shading == "perPixel" || mOgre21RenderPluginConfig.shading == "")
	//											{
	//												ogreViewPort->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
	//											}
	//										}
	//									}
	//								}
	//							}
	//						}
	//					}
	//				}
	//			}
	//			break;
	//			case ape::Event::Type::CAMERA_PARENTNODE:
	//			{
	//				if (auto ogreCamera = mpOgreSceneManager->getCamera(camera->getName()))
	//				{
	//					if (auto parentNode = camera->getParentNode().lock())
	//					{
	//						if (mpOgreSceneManager->hasSceneNode(parentNode->getName()))
	//						{
	//							if (auto ogreParentNode = mpOgreSceneManager->getSceneNode(parentNode->getName()))
	//							{
	//								if (ogreCamera->getParentNode())
	//									ogreCamera->detachFromParent();
	//								ogreParentNode->attachObject(ogreCamera);
	//							}
	//						}
	//					}
	//				}
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_DELETE:
	//				;
	//				break;
	//			case ape::Event::Type::CAMERA_FOCALLENGTH:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					mpOgreSceneManager->getCamera(event.subjectName)->setFocalLength(camera->getFocalLength());
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_ASPECTRATIO:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					mpOgreSceneManager->getCamera(event.subjectName)->setAspectRatio(camera->getAspectRatio());
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_AUTOASPECTRATIO:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					mpOgreSceneManager->getCamera(event.subjectName)->setAutoAspectRatio(camera->isAutoAspectRatio());
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_FOVY:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					mpOgreSceneManager->getCamera(event.subjectName)->setFOVy(ConversionToOgre(camera->getFOVy()));
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_FRUSTUMOFFSET:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					mpOgreSceneManager->getCamera(event.subjectName)->setFrustumOffset(ape::ConversionToOgre(camera->getFrustumOffset()));
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_FARCLIP:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					mpOgreSceneManager->getCamera(event.subjectName)->setFarClipDistance(camera->getFarClipDistance());
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_NEARCLIP:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					mpOgreSceneManager->getCamera(event.subjectName)->setNearClipDistance(camera->getNearClipDistance());
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_PROJECTION:
	//			{
	//				if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					mpOgreSceneManager->getCamera(event.subjectName)->setCustomProjectionMatrix(true, ape::ConversionToOgre(camera->getProjection()));
	//			}
	//				break;
	//			case ape::Event::Type::CAMERA_PROJECTIONTYPE:
	//				{
	//					if (mpOgreSceneManager->hasCamera(event.subjectName))
	//						mpOgreSceneManager->getCamera(event.subjectName)->setProjectionType(ConversionToOgre(camera->getProjectionType()));
	//				}
	//				break;
	//			case ape::Event::Type::CAMERA_ORTHOWINDOWSIZE:
	//				{
	//					if (mpOgreSceneManager->hasCamera(event.subjectName))
	//						mpOgreSceneManager->getCamera(event.subjectName)->setOrthoWindow(camera->getOrthoWindowSize().x, camera->getOrthoWindowSize().y);
	//				}
	//				break;
	//			case ape::Event::Type::CAMERA_VISIBILITY:
	//				{
	//					if (mpOgreSceneManager->hasCamera(event.subjectName))
	//					{
	//						Ogre::Viewport* ogreViewport = mpOgreSceneManager->getCamera(event.subjectName)->getViewport();
	//						if (ogreViewport)
	//							ogreViewport->setVisibilityMask(camera->getVisibilityMask());
	//					}
	//				}
	//				break;
	//			}
	//		}
	//	}
	//	}
	//	catch (const Ogre::Exception& ex)
	//	{
	//		APE_LOG_DEBUG(ex.getFullDescription());
	//	}
	//	mEventDoubleQueue.pop();
	//}
}

void ape::Ogre21RenderPlugin::windowResized(Ogre::RenderWindow* rw)
{
	//APE_LOG_DEBUG(rw->getViewport(0)->getActualWidth());
	auto oldWindowConfig = mpCoreConfig->getWindowConfig();
	//ape::WindowConfig windowConfig(oldWindowConfig.name, oldWindowConfig.renderSystem, oldWindowConfig.handle, oldWindowConfig.device, rw->getViewport(0)->getActualWidth(), rw->getViewport(0)->getActualHeight());
	//mpCoreConfig->setWindowConfig(windowConfig);
}

bool ape::Ogre21RenderPlugin::frameStarted( const Ogre::FrameEvent& evt )
{
	return Ogre::FrameListener::frameStarted( evt );
}

bool ape::Ogre21RenderPlugin::frameRenderingQueued( const Ogre::FrameEvent& evt )
{
	//APE_LOG_DEBUG("FPS: " << mRenderWindows.begin()->second->getLastFPS() << " triangles: " << mRenderWindows.begin()->second->getTriangleCount() << " batches: " << mRenderWindows.begin()->second->getBatchCount());
	processEventDoubleQueue();
	return Ogre::FrameListener::frameRenderingQueued( evt );
}

bool ape::Ogre21RenderPlugin::frameEnded( const Ogre::FrameEvent& evt )
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
		mpRoot->startRendering();
	}
	catch (const Ogre::Exception& ex)
	{
		APE_LOG_DEBUG(ex.getFullDescription());
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
		APE_LOG_ERROR(ex.getFullDescription());
	}
	catch (const Ogre::Exception& ex)
	{
		APE_LOG_ERROR(ex.getFullDescription());
	}
}

void ape::Ogre21RenderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeOgre21RenderPlugin.json";
	FILE* apeOgre21RenderPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeOgre21RenderPluginConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeOgre21RenderPluginConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& renderSystem = jsonDocument["renderSystem"];
			mOgre21RenderPluginConfig.renderSystem = renderSystem.GetString();
			rapidjson::Value& lodLevels = jsonDocument["lodLevels"];
			for (rapidjson::Value::MemberIterator lodLevelsMemberIterator =
				lodLevels.MemberBegin(); lodLevelsMemberIterator != lodLevels.MemberEnd(); ++lodLevelsMemberIterator)
			{
				if (lodLevelsMemberIterator->name == "autoGenerateAndSave")
					mOgre21RenderPluginConfig.ogreLodLevelsConfig.autoGenerateAndSave = lodLevelsMemberIterator->value.GetBool();
				else if (lodLevelsMemberIterator->name == "bias")
					mOgre21RenderPluginConfig.ogreLodLevelsConfig.bias = lodLevelsMemberIterator->value.GetFloat();
			}
			if (jsonDocument.HasMember("shading"))
			{
				rapidjson::Value& shading = jsonDocument["shading"];
				mOgre21RenderPluginConfig.shading = shading.GetString();
			}
			rapidjson::Value& renderWindows = jsonDocument["renderWindows"];
			for (auto& renderWindow : renderWindows.GetArray())
			{
				ape::Ogre21RenderWindowConfig ogreRenderWindowConfig;
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
							ape::Ogre21ViewPortConfig ogreViewPortConfig;
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
										ape::Ogre21CameraConfig ogreCameraConfig;
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
				mOgre21RenderPluginConfig.ogreRenderWindowConfigList.push_back(ogreRenderWindowConfig);
			}
		}
		fclose(apeOgre21RenderPluginConfigFile);
	}	
	
	Ogre::LogManager* lm = new Ogre::LogManager();
	lm->createLog("apeOgre21RenderPlugin.log", true, false, false);
	mpRoot = OGRE_NEW Ogre::Root("", "", "");

#if defined (_DEBUG)
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
	if (mOgre21RenderPluginConfig.renderSystem == "DX11")
		mpRoot->loadPlugin("RenderSystem_Direct3D11_d");
	else
		mpRoot->loadPlugin("RenderSystem_GL3Plus_d");
	mpRoot->loadPlugin("ogre_gltf_d");
#else
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
	if (mOgre21RenderPluginConfig.renderSystem == "DX11")
		mpRoot->loadPlugin("RenderSystem_Direct3D11");
	else
		mpRoot->loadPlugin("RenderSystem_GL3Plus");
	mpRoot->loadPlugin("ogre_gltf");
#endif
    
	Ogre::RenderSystem* renderSystem = nullptr;
	if (mOgre21RenderPluginConfig.renderSystem == "DX11")
		renderSystem = mpRoot->getRenderSystemByName("Direct3D11 Rendering Subsystem");
	else
		renderSystem = mpRoot->getRenderSystemByName("Open_GL3Plus Rendering Subsystem");

	mpRoot->setRenderSystem(renderSystem);

	for (auto resourceLocation : mpCoreConfig->getNetworkConfig().resourceLocations)
	{
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourceLocation, "FileSystem");
	}

	mpRoot->initialise(false, "ape");

	Ogre::InstancingThreadedCullingMethod instancingThreadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD; 

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
	void* device = 0;
	for (int i = 0; i < mOgre21RenderPluginConfig.ogreRenderWindowConfigList.size(); i++)
	{
		if (mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].enable)
		{
			Ogre::RenderWindowDescription winDesc;
			std::stringstream ss;
			ss << mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].name;
			winDesc.name = ss.str();
			winDesc.height = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].height;
			winDesc.width = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].width;
			winDesc.useFullScreen = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].fullScreen;
			std::stringstream colourDepthSS;
			colourDepthSS << mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].colorDepth;
			winDesc.miscParams["colourDepth"] = colourDepthSS.str().c_str();
			winDesc.miscParams["vsync"] = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].vSync ? "Yes" : "No";
			std::stringstream vsyncIntervalSS;
			vsyncIntervalSS << mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].vSyncInterval;
			winDesc.miscParams["vsyncInterval"] = vsyncIntervalSS.str().c_str();
			std::stringstream fsaaSS;
			fsaaSS << mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].fsaa;
			winDesc.miscParams["FSAA"] = fsaaSS.str().c_str();
			winDesc.miscParams["FSAAHint"] = mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].fsaaHint;
			std::stringstream monitorIndexSS;
			monitorIndexSS << mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].monitorIndex;
			winDesc.miscParams["monitorIndex"] = monitorIndexSS.str().c_str();
			winDescList.push_back(winDesc);
			APE_LOG_DEBUG("winDesc:" << " name=" << winDesc.name << " width=" << winDesc.width << " height=" << winDesc.height << " fullScreen=" << winDesc.useFullScreen);
			mRenderWindows[winDesc.name] = mpRoot->createRenderWindow(winDesc.name, winDesc.width, winDesc.height, winDesc.useFullScreen, &winDesc.miscParams);
			mRenderWindows[winDesc.name]->setDeactivateOnFocusChange(false);
			mRenderWindows[winDesc.name]->setHidden(mOgre21RenderPluginConfig.ogreRenderWindowConfigList[i].hidden);
		}
	}

	mpGlTFLoaderInterface = Ogre_glTF::gltfPluginAccessor::findPlugin()->getLoader();

	registerHlms();

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);

	int mainWindowID = 0; //first window will be the main window
	Ogre::RenderWindowDescription mainWindowDesc = winDescList[mainWindowID];
	mpWindowEventUtilities = new Ogre::WindowEventUtilities();
	mpWindowEventUtilities->addWindowEventListener(mRenderWindows[mainWindowDesc.name], this);
	mRenderWindows[mainWindowDesc.name]->getCustomAttribute("WINDOW", &mainWindowHnd);
	std::ostringstream windowHndStr;
	windowHndStr << mainWindowHnd;
	mOgre21RenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].windowHandler = windowHndStr.str();
	if (mOgre21RenderPluginConfig.renderSystem == "DX11")
	{
		mRenderWindows[mainWindowDesc.name]->getCustomAttribute("D3DDEVICE", &device);
	}
	ape::WindowConfig windowConfig(mainWindowDesc.name, mOgre21RenderPluginConfig.renderSystem, mainWindowHnd, device, mOgre21RenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].width,
		mOgre21RenderPluginConfig.ogreRenderWindowConfigList[mainWindowID].height);
	mpCoreConfig->setWindowConfig(windowConfig);
	APE_LOG_FUNC_LEAVE();
}

void ape::Ogre21RenderPlugin::registerHlms()
{
	static const Ogre::String OGRE_RENDERSYSTEM_DIRECTX11 = "Direct3D11 Rendering Subsystem";
	static const Ogre::String OGRE_RENDERSYSTEM_OPENGL3PLUS = "OpenGL 3+ Rendering Subsystem";
	static const Ogre::String OGRE_RENDERSYSTEM_METAL = "Metal Rendering Subsystem";

	Ogre::RenderSystem* renderSystem = mpRoot->getRenderSystem();

	Ogre::HlmsUnlit* hlmsUnlit = nullptr;
	Ogre::HlmsPbs* hlmsPbs = nullptr;

	Ogre::ArchiveVec library;

	Ogre::String shaderSyntax = "GLSL";
	if (renderSystem->getName() == OGRE_RENDERSYSTEM_DIRECTX11)
		shaderSyntax = "HLSL";
	else if (renderSystem->getName() == OGRE_RENDERSYSTEM_METAL)
		shaderSyntax = "Metal";
	for (auto resourceLocation : mpCoreConfig->getNetworkConfig().resourceLocations)
	{
		if (resourceLocation.find("Hlms/Common") != std::string::npos && resourceLocation.find(shaderSyntax) != std::string::npos)
		{
			auto archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(resourceLocation, "FileSystem", true);
			library.push_back(archiveLibrary);
		}
		if (resourceLocation.find("Any") != std::string::npos)
		{
			auto archiveLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(resourceLocation, "FileSystem", true);
			library.push_back(archiveLibraryAny);
		}
		if (resourceLocation.find("Pbs") != std::string::npos && resourceLocation.find(shaderSyntax) != std::string::npos && resourceLocation.find("Any") == std::string::npos)
		{
			auto archiveLibraryPbs = Ogre::ArchiveManager::getSingletonPtr()->load(resourceLocation, "FileSystem", true);
			hlmsPbs = OGRE_NEW Ogre::HlmsPbs(archiveLibraryPbs, &library);
			mpRoot->getHlmsManager()->registerHlms(hlmsPbs);
			library.push_back(archiveLibraryPbs);
		}
		if (resourceLocation.find("Unlit") != std::string::npos && resourceLocation.find(shaderSyntax) != std::string::npos && resourceLocation.find("Any") == std::string::npos)
		{
			auto archiveLibraryUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(resourceLocation, "FileSystem", true);
			hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(archiveLibraryUnlit, &library);
			mpRoot->getHlmsManager()->registerHlms(hlmsUnlit);
			library.push_back(archiveLibraryUnlit);
		}
	}


	if (renderSystem->getName() == "Direct3D11 Rendering Subsystem")
	{
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
