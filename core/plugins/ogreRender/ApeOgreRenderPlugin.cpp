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

Ape::OgreRenderPlugin::OgreRenderPlugin()
{
	mpScene = Ape::IScene::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mEventDoubleQueue = Ape::DoubleQueue<Event>(true);
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::SCENEPROPERTY, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::LIGHT, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::MATERIAL, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&OgreRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpRoot = NULL;
	mpSceneMgr = NULL;
	mpRenderWindow = NULL;
	mpOverlaySys = NULL;
	mpOgreMovableTextFactory = NULL;
	mpOverlayMgr = NULL;
	mpOverlay = NULL;
	mpOverlayContainer = NULL;
	mpOverlayTextArea = NULL;
	mpOverlayFontManager = NULL;
	mpOverlayFont = NULL;
	mpShaderGenerator = NULL;
	mOgreRenderWindowConfigList = Ape::OgreRenderWindowConfigList();
	mOgreCameras = std::vector<Ogre::Camera*>();
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

void Ape::OgreRenderPlugin::processEventDoubleQueue()
{
	mEventDoubleQueue.swap();
	while (!mEventDoubleQueue.emptyPop())
	{
		Ape::Event event = mEventDoubleQueue.front();
		if (event.group == Ape::Event::Group::SCENEPROPERTY)
		{
			if (auto sceneProperty = mpScene->getProperty().lock())
			{
				switch (event.type)
				{
				case Ape::Event::Type::SCENEPROPERTY_ADDRESOURCEFOLDER:
					{
						auto resourceFolders = sceneProperty->getResourceFolders();
						for (auto resourceFolder : resourceFolders)
						{
							if (!Ogre::ResourceGroupManager::getSingleton().resourceLocationExists(resourceFolder))
							{
								std::string resourceGroupName = "OTF";
								/*Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourceFolder, "FileSystem", resourceGroupName);
								Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceGroupName);
								std::cout << "Parsing resource location " << resourceFolder << " please wait......" << std::endl;
								while (!Ogre::ResourceGroupManager::getSingleton().isResourceGroupInitialised(resourceGroupName))
									std::this_thread::sleep_for(std::chrono::milliseconds(100));
								std::cout << "Resource location " << resourceFolder << " parsed successfully" << std::endl;*/
							}
						}
					}
					break;
				case Ape::Event::Type::SCENEPROPERTY_SKYBOXMATERIAL:
					{
						std::string skyBoxMaterialName = sceneProperty->getSkyBoxMaterialName();
						if (Ogre::MaterialManager::getSingleton().resourceExists(skyBoxMaterialName))
							mpSceneMgr->setSkyBox(true, skyBoxMaterialName);
					}
					break;
				case Ape::Event::Type::SCENEPROPERTY_AMBIENTCOLOR:
					mpSceneMgr->setAmbientLight(ConversionToOgre(sceneProperty->getAmbientColor()));
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::NODE)
		{
			if (auto node = mpScene->getNode(event.subjectName).lock())
			{
				std::string nodeName = node->getName();
				switch (event.type)
				{
				case Ape::Event::Type::NODE_CREATE:
					mpSceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
					break;
				case Ape::Event::Type::NODE_DELETE:
					; 
					break;
				case Ape::Event::Type::NODE_POSITION:
					{
						if (mpSceneMgr->hasSceneNode(nodeName))
							mpSceneMgr->getSceneNode(nodeName)->setPosition(ConversionToOgre(node->getPosition()));
					}
					break;
				case Ape::Event::Type::NODE_ORIENTATION:
					{
						if (mpSceneMgr->hasSceneNode(nodeName))
							mpSceneMgr->getSceneNode(nodeName)->setOrientation(ConversionToOgre(node->getOrientation()));
					}
					break;
				}		
			}
		}
		else if (event.group == Ape::Event::Group::GEOMETRY)
		{			
			if (auto geometry = mpScene->getEntity(event.subjectName).lock())
			{
				std::string geometryName = geometry->getName();
				std::string parentNodeName = geometry->getParentNodeName();
				switch (event.type)
				{
				case Ape::Event::Type::GEOMETRY_FILE_CREATE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_FILE_DELETE:
					; 
					break;
				case Ape::Event::Type::GEOMETRY_FILE_FILENAME:
					{
						std::string fileExtension = geometryName.substr(geometryName.find_first_of("."));
						if (fileExtension == ".mesh")
						{
							if (auto ogreEntity = mpSceneMgr->createEntity(geometryName, geometryName))
							{
								if (mpSceneMgr->hasSceneNode(parentNodeName))
									mpSceneMgr->getSceneNode(parentNodeName)->attachObject(ogreEntity);
							}
						}
					}
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_CREATE:
					{
						if (auto ogreText = (Ape::OgreMovableText*)mpSceneMgr->createMovableObject(geometryName, "MovableText"))
						{
							ogreText->setTextAlignment(Ape::OgreMovableText::H_CENTER, Ape::OgreMovableText::V_ABOVE);
							ogreText->showOnTop(true);
							if (mpSceneMgr->hasSceneNode(parentNodeName))
								mpSceneMgr->getSceneNode(parentNodeName)->attachObject(ogreText);
						}
					}
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_DELETE:
					;
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_OFFSET:
					{
						if (auto ogreText = (Ape::OgreMovableText*)mpSceneMgr->getMovableObject(geometry->getName(), "MovableText"))
						{
							if (auto textGeometry = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->getEntity(geometryName).lock()))
								ogreText->setLocalTranslation(Ape::ConversionToOgre(textGeometry->getOffset()));
						}
					}
					break;
				case Ape::Event::Type::GEOMETRY_TEXT_CAPTION:
					{
						if (auto ogreText = (Ape::OgreMovableText*)mpSceneMgr->getMovableObject(geometry->getName(), "MovableText"))
						{
							if (auto textGeometry = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->getEntity(geometryName).lock()))
								ogreText->setCaption(textGeometry->getCaption());
						}
					}
					break;
				}
			}
		}
		else if (event.group == Ape::Event::Group::MATERIAL)
		{
			if (auto material = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->getEntity(event.subjectName).lock()))
			{
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
				}
			}
		}
		else if (event.group == Ape::Event::Group::LIGHT)
		{
			if (auto light = mpScene->getEntity(event.subjectName).lock())
			{
				switch (event.type)
				{
				case Ape::Event::Type::LIGHT_CREATE:
					; 
					break;
				case Ape::Event::Type::LIGHT_DELETE:
					; 
					break;
				default:
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
						if (auto ogreCamera = mpSceneMgr->createCamera(event.subjectName))
						{
							if (auto viewPort = mpRenderWindow->addViewport(ogreCamera))
							{
								if (mpSceneMgr->hasSceneNode(camera->getParentNodeName()))
									mpSceneMgr->getSceneNode(camera->getParentNodeName())->attachObject(ogreCamera);
								ogreCamera->setAspectRatio(Ogre::Real(viewPort->getActualWidth()) / Ogre::Real(viewPort->getActualHeight()));
								mOgreCameras.push_back(ogreCamera);
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
							mpSceneMgr->getCamera(event.subjectName)->setFrustumOffset(camera->getFrustumOffset());
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
				}
			}
		}
		mEventDoubleQueue.pop();
	}
}

bool Ape::OgreRenderPlugin::frameStarted( const Ogre::FrameEvent& evt )
{
	return Ogre::FrameListener::frameStarted( evt );
}

bool Ape::OgreRenderPlugin::frameRenderingQueued( const Ogre::FrameEvent& evt )
{
	std::stringstream ss;
	ss << mpRenderWindow->getLastFPS();
	//TODO overlay
	//mpOverlayTextArea->setCaption(ss.str());

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
			rapidjson::Value& renderWindows = jsonDocument["renderWindows"];
			for (auto& renderWindow : renderWindows.GetArray())
			{
				Ape::OgreRenderWindowConfig ogreRenderWindowConfig;
				for (rapidjson::Value::MemberIterator renderWindowMemberIterator = 
					renderWindow.MemberBegin(); renderWindowMemberIterator != renderWindow.MemberEnd(); ++renderWindowMemberIterator)
				{
					if (renderWindowMemberIterator->name == "monitorIndex")
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
								if (viewportMemberIterator->name == "name")
									ogreViewPortConfig.name = viewportMemberIterator->value.GetString();
								else if (viewportMemberIterator->name == "zOrder")
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
										if (cameraMemberIterator->name == "nearClip")
											ogreViewPortConfig.camera.nearClip = cameraMemberIterator->value.GetFloat();
										else if (cameraMemberIterator->name == "farClip")
											ogreViewPortConfig.camera.farClip = cameraMemberIterator->value.GetFloat();
										else if (cameraMemberIterator->name == "fovY")
											ogreViewPortConfig.camera.fovY = cameraMemberIterator->value.GetFloat();

									}
								}
							}
							ogreRenderWindowConfig.viewportList.push_back(ogreViewPortConfig);
						}
					}
				}
				mOgreRenderWindowConfigList.push_back(ogreRenderWindowConfig);
			}
		}
		fclose(apeOgreRenderPluginConfigFile);
	}	
	
	mpRoot = new Ogre::Root("", "", "ApeOgreRenderPlugin.log");

#if defined (__APPLE__)
    Ogre::LogManager::getSingleton().createLog("ApeOgreRenderPlugin.log", true, false, false);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
    mpRoot->loadPlugin( "RenderSystem_GL" );
    mpRoot->loadPlugin( "Plugin_ParticleFX");
#endif
    
#if defined (_WIN32)
    
    #if defined (_DEBUG)
        Ogre::LogManager::getSingleton().createLog("ApeOgreRenderPlugin.log", true, false, false);
        Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
//		mpRoot->loadPlugin( "RenderSystem_Direct3D11_d" );
        mpRoot->loadPlugin( "RenderSystem_GL_d" );
       // mpRoot->loadPlugin( "Plugin_ParticleFX_d");
		// mpRoot->loadPlugin( "Plugin_CgProgramManager_d");
		
    #else
        Ogre::LogManager::getSingleton().createLog("ApeOgreRenderPlugin.log", true, false, false);
        Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
//      mpRoot->loadPlugin( "RenderSystem_Direct3D11" );
		mpRoot->loadPlugin( "RenderSystem_GL" );
        mpRoot->loadPlugin( "Plugin_ParticleFX" );
		mpRoot->loadPlugin( "Plugin_CgProgramManager" );
    #endif
#endif

#if defined (WIN32)
	Ogre::RenderSystem *renderSystem = mpRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
#else
	Ogre::RenderSystem *renderSystem = mpRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
    
#endif
	std::stringstream mediaFolder;
	mediaFolder << APE_SOURCE_DIR << "/core/plugins/ogreRender/media";

	mpRoot->setRenderSystem(renderSystem);

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/fonts",				 "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/materials/scripts", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/materials/textures", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/materials/programs", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/models", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/compositors", "FileSystem");
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/oculus", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/shaders", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/shaders/Cg", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/shaders/GLSL", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/shaders/GLSL150", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/shaders/GLSLES", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/shaders/HLSL", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mediaFolder.str() + "/shaders/materials", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mpSystemConfig->getSceneSessionConfig().sessionResourceLocation, "FileSystem");
	

	mpRoot->initialise(false, "Ape");
	mpSceneMgr = mpRoot->createSceneManager(Ogre::ST_GENERIC);

	mpRoot->addFrameListener(this);

	Ogre::RenderWindowList renderWindowList;
	Ogre::RenderWindowDescriptionList winDescList;
	for (int i = 0; i < mOgreRenderWindowConfigList.size(); i++)
	{
		Ogre::RenderWindowDescription winDesc;
		if (i == 0)
			winDesc.name = mpSystemConfig->getMainWindowConfig().name;
		else
		{
			std::stringstream ss;
			ss << mpSystemConfig->getMainWindowConfig().name << i;
			winDesc.name = ss.str();
		}
		winDesc.height = mOgreRenderWindowConfigList[i].height;
		winDesc.width = mOgreRenderWindowConfigList[i].width;
		winDesc.useFullScreen = mOgreRenderWindowConfigList[i].fullScreen;
		winDesc.miscParams["colourDepth"] = mOgreRenderWindowConfigList[i].colorDepth;
		winDesc.miscParams["vsync"] = mOgreRenderWindowConfigList[i].vSync ? "Yes" : "No";
		std::stringstream vsyncIntervalSS;
		vsyncIntervalSS << mOgreRenderWindowConfigList[i].vSyncInterval;
		winDesc.miscParams["vsyncInterval"] = vsyncIntervalSS.str().c_str();
		winDesc.miscParams["FSAA"] = mOgreRenderWindowConfigList[i].fsaa;
		winDesc.miscParams["FSAAHint"] = mOgreRenderWindowConfigList[i].fsaaHint;
        #if defined (__APPLE__)
            winDesc.miscParams["macAPI"] = "cocoa";
            winDesc.miscParams["macAPICocoaUseNSView"] = "true";
        #endif
		std::stringstream monitorIndexSS;
		monitorIndexSS << mOgreRenderWindowConfigList[i].monitorIndex;
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
			mpRenderWindow = mpRoot->createRenderWindow(winDesc.name, winDesc.width, winDesc.height, winDesc.useFullScreen, &winDesc.miscParams);
			mpRenderWindow->setDeactivateOnFocusChange(false);
			auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->createEntity(mOgreRenderWindowConfigList[i].viewportList[0].name, mpSystemConfig->getSceneSessionConfig().generatedUniqueUserName, Ape::Entity::Type::CAMERA).lock());
			if (camera)
			{
				camera->setNearClipDistance(mOgreRenderWindowConfigList[i].viewportList[0].camera.nearClip);
				camera->setFarClipDistance(mOgreRenderWindowConfigList[i].viewportList[0].camera.farClip);
				camera->setFOVy(mOgreRenderWindowConfigList[i].viewportList[0].camera.fovY.toRadian());
			}

			//TODO somhow no backfaces for that and create a manual instead and animating when zoomin and etc.
			//if (auto frustumMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("frustum.mesh", mpSystemConfig->getSceneSessionConfig().generatedUniqueUserName, Ape::Entity::GEOMETRY_FILE).lock()))
				//frustumMeshFile->setFileName("frustum.mesh");

			void* windowHnd = 0;
			mpRenderWindow->getCustomAttribute("WINDOW", &windowHnd);
			std::ostringstream windowHndStr;
			windowHndStr << windowHnd;
			mOgreRenderWindowConfigList[0].windowHandler = windowHndStr.str();

			mpMainWindow->setHandle(windowHnd);
			mpMainWindow->setWidth(mOgreRenderWindowConfigList[0].width);
			mpMainWindow->setHeight(mOgreRenderWindowConfigList[0].height);
		}
	}
	
	mpOverlaySys = new Ogre::OverlaySystem();
	mpSceneMgr->addRenderQueueListener(mpOverlaySys);

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mpOgreMovableTextFactory = new Ape::OgreMovableTextFactory();
	mpRoot->addMovableObjectFactory(mpOgreMovableTextFactory);
}