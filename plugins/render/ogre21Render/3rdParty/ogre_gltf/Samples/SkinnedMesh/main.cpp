#include "SamplesCommon.h"

Ogre::CompositorWorkspace* SetupCompositor(Ogre::Root* root, Ogre::RenderWindow* const window, Ogre::SceneManager* smgr, Ogre::Camera* camera)
{
	//Setup rendering pipeline
	auto compositor			   = root->getCompositorManager2();
	const char workspaceName[] = "workspace0";
	compositor->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue { 0.2f, 0.3f, 0.4f });
	auto workspace = compositor->addWorkspace(smgr, window, camera, workspaceName, true);

	return workspace;
}

int main()
{
#ifdef Ogre_glTF_STATIC
	// Must instantiate before Root so that it'll be destroyed afterwards. 
	// Otherwise we get a crash on Ogre::Root::shutdownPlugins()
#if __linux__
    auto glPlugin = std::make_unique<Ogre::GL3PlusPlugin>();
#endif
#endif

	//Init Ogre
	auto root = std::make_unique<Ogre::Root>();
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_BOREME);

#ifdef Ogre_glTF_STATIC
#if __linux__
    root->installPlugin(glPlugin.get());
#endif
#else
	root->loadPlugin(GL_RENDER_PLUGIN);
#ifdef _WIN32
	root->loadPlugin(D3D11_RENDER_PLUGIN);
#endif
#endif
	root->showConfigDialog();
	root->getRenderSystem()->setConfigOption("FSAA", "16");
	root->getRenderSystem()->setConfigOption("sRGB Gamma Conversion", "Yes");
	root->initialise(false);

	//Create a window and a scene
	Ogre::NameValuePairList params;
	params["FSAA"]	= "16";
	const auto window = root->createRenderWindow("glTF test!", 800, 600, false, &params);

	auto smgr		  = root->createSceneManager(Ogre::ST_GENERIC, 2, Ogre::INSTANCING_CULLING_THREADED);
	smgr->setForward3D(true, 4, 4, 5, 96, 3, 200);
	auto camera = smgr->createCamera("cam");
	camera->setNearClipDistance(0.001f);
	camera->setFarClipDistance(100);
	camera->setAutoAspectRatio(true);
	camera->setPosition(2, 2, 2);
	camera->lookAt(0, 1, 0);

	//Load workspace and hlms
	auto workspace = SetupCompositor(root.get(), window, smgr, camera);

	DeclareHlmsLibrary("./Media");

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/gltfFiles.zip", "Zip");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);

	auto gltf = std::make_unique<Ogre_glTF::glTFLoader>();
	Ogre::SceneNode* objectNode = nullptr;

	try
	{
		auto adapter = gltf->loadGlbResource("CesiumMan.glb");
		objectNode = adapter.getFirstSceneNode(smgr);
	}
	catch(std::exception& e)
	{
		Ogre::LogManager::getSingleton().logMessage(e.what());
		return -1;
	}

	//Add light
	auto light = smgr->createLight();
	light->setType(Ogre::Light::LightTypes::LT_POINT);
	light->setPowerScale(5);
	auto lightNode = smgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);
	lightNode->setPosition(1, 1, 1);

	//Setup animation and update it over time
	Ogre::SkeletonAnimation* animation = nullptr;
	// Find a node that contains an item that contains an skeleton
	auto childIt = objectNode->getChildIterator();

	while(childIt.hasMoreElements()) 
	{
		auto sceneNode = static_cast<Ogre::SceneNode*>(childIt.getNext());
		if(sceneNode->numAttachedObjects() > 0) 
		{
			auto itemIt = sceneNode->getAttachedObjectIterator();

			while(itemIt.hasMoreElements())
			{
				auto item = static_cast<Ogre::Item*>(itemIt.getNext());
				auto skeleton = item->getSkeletonInstance();

				if(skeleton)
				{
					const auto animationName = (skeleton->getAnimations().front().getName());
					animation = item->getSkeletonInstance()->getAnimation(animationName);
					animation->setEnabled(true);
					animation->setLoop(true);
					break;
				}
			}
		}
	}


	auto timer	= root->getTimer();
	auto now	  = timer->getMilliseconds();
	auto last	 = now;
	auto duration = now - last;

	//Update
	while(!window->isClosed())
	{
		//Add time to animation
		now		 = timer->getMilliseconds();
		duration = (now - last);
		animation->addTime(Ogre::Real(duration) / 1000.0f);
		last = now;

		//Render
		root->renderOneFrame();
		Ogre::WindowEventUtilities::messagePump();
	}

	return 0;
}
