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
//#include "ApeOgreUtilities.h"


Ape::Ogre21RenderPlugin::Ogre21RenderPlugin( ) //constructor
{
	mpScene = Ape::IScene::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mEventDoubleQueue = Ape::DoubleQueue<Event>();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::LIGHT, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TEXT, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_PLANE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_BOX, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_CONE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TUBE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_SPHERE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TORUS, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::MATERIAL_FILE, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::MATERIAL_MANUAL, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::PASS_PBS, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::PASS_MANUAL, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_MANUAL, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_UNIT, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::SKY, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::WATER, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&Ogre21RenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpRoot = nullptr;
	mpSceneMgr = nullptr;
	mRenderWindows = std::map<std::string, Ogre::RenderWindow*>();
	mpHlmsPbsManager = nullptr;
	mOgreRenderPluginConfig = Ape::Ogre21RenderPluginConfig();
	mOgreCameras = std::vector<Ogre::Camera*>();
	mCameraCountFromConfig = 0;
}

Ape::Ogre21RenderPlugin::~Ogre21RenderPlugin() //destructor
{
	std::cout << "OgreRenderPlugin dtor" << std::endl;
	delete mpRoot;
}

void Ape::Ogre21RenderPlugin::eventCallBack(const Ape::Event& event)
{
	mEventDoubleQueue.push(event);
}



void Ape::Ogre21RenderPlugin::processEventDoubleQueue()
{
	mEventDoubleQueue.swap();
	while (!mEventDoubleQueue.emptyPop())
	{
		Ape::Event event = mEventDoubleQueue.front();
		if (event.group == Ape::Event::Group::NODE)
		{
			
		}
		else if (event.group == Ape::Event::Group::GEOMETRY_FILE)
		{
			
		}
		mEventDoubleQueue.pop();
	}
}


bool Ape::Ogre21RenderPlugin::frameStarted( const Ogre::FrameEvent& evt )
{
	return Ogre::FrameListener::frameStarted( evt );
}

bool Ape::Ogre21RenderPlugin::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	processEventDoubleQueue();
	return Ogre::FrameListener::frameRenderingQueued(evt);
}


bool Ape::Ogre21RenderPlugin::frameEnded( const Ogre::FrameEvent& evt )
{
	return Ogre::FrameListener::frameEnded( evt );
}


void Ape::Ogre21RenderPlugin::Stop()
{
	
}


void Ape::Ogre21RenderPlugin::Suspend()
{
	
}


void Ape::Ogre21RenderPlugin::Restart()
{
	
}


void Ape::Ogre21RenderPlugin::Run()
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


void Ape::Ogre21RenderPlugin::Step()
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









void Ape::Ogre21RenderPlugin::Init()
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
	fileFullPath << mpSystemConfig->getFolderPath() << "\\ApeOgre21RenderPlugin.json";
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
											ogreViewPortConfig.camera.orientationOffset = Ape::ConversionFromOgre21(orientationOffset);
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
	
	mpRoot = new Ogre::Root("", "", "ApeOgre21RenderPlugin.log");
    
	Ogre::LogManager::getSingleton().createLog("ApeOgre21RenderPlugin.log", true, false, false);

	#if defined (_DEBUG)
		Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
		if (mOgreRenderPluginConfig.renderSystem == "DX11")
			mpRoot->loadPlugin( "RenderSystem_Direct3D11_d" );
		else if (mOgreRenderPluginConfig.renderSystem == "DX9")
			mpRoot->loadPlugin("RenderSystem_Direct3D9_d");
		else 
			mpRoot->loadPlugin( "RenderSystem_GL_d" );
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
	mediaFolder << APE_SOURCE_DIR << "/plugins/ogre21Render/media";

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
	//mpSceneMgr = mpRoot->createSceneManager(Ogre::ST_GENERIC);

	mpRoot->addFrameListener(this);

	Ogre::RenderWindowList renderWindowList;
	Ogre::RenderWindowDescriptionList winDescList;
	int enabledWindowCount = 0;
	void* mainWindowHnd = 0;
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
				
				if (enabledWindowCount == 1)
				{
					std::cout << winDesc.width << std::endl;
					mRenderWindows[winDesc.name] = mpRoot->createRenderWindow(winDesc.name, winDesc.width, winDesc.height, winDesc.useFullScreen, &winDesc.miscParams);
					mRenderWindows[winDesc.name]->setDeactivateOnFocusChange(false);
					for (int j=0; j<mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList.size(); j++)
					{
						auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->createEntity(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].camera.name, Ape::Entity::Type::CAMERA).lock());
						if (camera)
						{
							//TODO why it is not ok
							//camera->setAspectRatio((float)mOgreRenderWindowConfigList[i].width / (float)mOgreRenderWindowConfigList[i].height);
							camera->setWindow(winDesc.name);
							camera->setFocalLength(1.0f);
							camera->setNearClipDistance(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].camera.nearClip);
							camera->setFarClipDistance(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].camera.farClip);
							camera->setFOVy(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].viewportList[j].camera.fovY.toRadian());
							if (auto userNode = mUserNode.lock())
								camera->setParentNode(mUserNode);
						}
					}

					mRenderWindows[winDesc.name]->getCustomAttribute("WINDOW", &mainWindowHnd);
					std::ostringstream windowHndStr;
					windowHndStr << mainWindowHnd;
					mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].windowHandler = windowHndStr.str();
					mpMainWindow->setName(winDesc.name);
					mpMainWindow->setWidth(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].width);
					mpMainWindow->setHeight(mOgreRenderPluginConfig.ogreRenderWindowConfigList[i].height);

					//Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

					
					//mpHlmsPbsManager = new Ogre::HlmsManager(mpSceneMgr);


					/*Has to be the last call because of the sync if needed*/
					mpMainWindow->setHandle(mainWindowHnd);
				}
			}
		}
	}
}
