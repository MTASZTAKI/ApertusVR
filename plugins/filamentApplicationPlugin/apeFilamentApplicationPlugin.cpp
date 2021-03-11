#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "apeFilamentApplicationPlugin.h"

#ifdef __APPLE__
#include <sys/stat.h>
#include "NativeWindowHelper.h"
#endif

using namespace filament;
using namespace filament::math;
using namespace filament::viewer;

using namespace gltfio;
using namespace utils;

ape::FilamentApplicationPlugin::FilamentApplicationPlugin( )
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mEventDoubleQueue = ape::DoubleQueue<Event>();
	mpEventManager = ape::IEventManager::getSingletonPtr();
    mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CLONE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&FilamentApplicationPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mFilamentApplicationPluginConfig = ape::FilamentApplicationPluginConfig();
    mpVlftImgui = new VLFTImgui();
    app.updateinfo.isAdmin = true;
    mpVlftImgui->init(&app.updateinfo);
    mIsStudent = false;
    parseJson();
    initFilament();
    mParsedAnimations = std::vector<Animation>();
    mParsedBookmarkTimes = std::vector<unsigned long long>();
    mIsPauseClicked = false;
    mIsStopClicked = false;
    mIsPlayRunning = false;
    mAnimatedNodeNames = std::vector<std::string>();
    mAttachedUsers = std::vector<ape::NodeWeakPtr>();
    mIsStudentsMovementLogging = false;
    mSpaghettiNodeNames = std::vector<std::string>();
    mIsAllSpaghettiVisible = false;
    mKeyMap = std::map<std::string, SDL_Scancode>();
    initKeyMap();
    initAnimations();
	APE_LOG_FUNC_LEAVE();
}

ape::FilamentApplicationPlugin::~FilamentApplicationPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentApplicationPlugin::initKeyMap(){
    mKeyMap["w"] = SDL_SCANCODE_W;
    mKeyMap["a"] = SDL_SCANCODE_A;
    mKeyMap["s"] = SDL_SCANCODE_S;
    mKeyMap["d"] = SDL_SCANCODE_D;
    mKeyMap["e"] = SDL_SCANCODE_E;
    mKeyMap["q"] = SDL_SCANCODE_Q;
}

void ape::FilamentApplicationPlugin::eventCallBack(const ape::Event& event)
{
	mEventDoubleQueue.push(event);
}

void ape::FilamentApplicationPlugin::processEventDoubleQueue()
{
	mEventDoubleQueue.swap();
	while (!mEventDoubleQueue.emptyPop())
	{
		ape::Event event = mEventDoubleQueue.front();
		try
		{
		if (event.group == ape::Event::Group::NODE)
		{
			if (auto node = mpSceneManager->getNode(event.subjectName).lock())
			{
				std::string nodeName = node->getName();
                if(nodeName == "Pallet.2")
                    bool stop;
				if (event.type == ape::Event::Type::NODE_CREATE)
				{
                
					auto filamentEntity = app.mpEntityManager->create();
                    app.names->addComponent(filamentEntity);
                    auto nameInstance = app.names->getInstance(filamentEntity);
                    if(nameInstance)
                        app.names->setName(nameInstance, nodeName.c_str());
					app.mpTransformManager->create(filamentEntity);
					auto filamentTransform = app.mpTransformManager->getInstance(filamentEntity);
                    app.mpTransforms[nodeName] = filamentTransform;
                    APE_LOG_DEBUG("nodeName "<<nodeName);
                    app.mpScene->addEntity(filamentEntity);
                }
				else
				{
					switch (event.type)
					{
					case ape::Event::Type::NODE_PARENTNODE:
					{
                        std::string parentNodeName = "";
                        std::vector<utils::Entity> entities;
                        entities.resize(10);
                        if (auto parentNode = node->getParentNode().lock())
                            parentNodeName = parentNode->getName();
                        if (parentNodeName.find_first_of(".") != std::string::npos)
                            {
                                std::string cloneName = parentNodeName.substr(0,parentNodeName.find_last_of("."));
                                std::string subNodeName = parentNodeName.substr(parentNodeName.find_last_of(".")+1);
                                if(app.mpInstancesMap.find(cloneName) == app.mpInstancesMap.end()){
                                    bool foundAsset = false;
                                    auto assetItaretor = app.asset.begin();
                                    while(!foundAsset && assetItaretor != app.asset.end()){
                                        if(assetItaretor->second->getEntitiesByName(subNodeName.c_str(), nullptr, 10) + assetItaretor->second->getEntitiesByName(parentNodeName.c_str(), nullptr, 10)){
                                            foundAsset = true;
                                        }
                                        else{
                                            assetItaretor++;
                                        }
                                    }
                                    if(foundAsset && app.instanceCount[assetItaretor->first] < 10){
                                        int cnt = app.instanceCount[assetItaretor->first]++;
                                        app.mpInstancesMap[event.subjectName] = InstanceData(cnt, assetItaretor->first, app.instances[assetItaretor->first][cnt]);
                                        auto root = app.instances[assetItaretor->first][cnt]->getRoot();
                                        app.names->addComponent(root);
                                        auto nameInstance = app.names->getInstance(root);
                                        if(nameInstance)
                                          app.names->setName(nameInstance, event.subjectName.c_str());
                                    }
                                    else{
                                        
                                        if(app.mpTransforms.find(parentNodeName) != app.mpTransforms.end()){
                                            app.mpTransformManager->setParent(app.mpTransforms[nodeName], app.mpTransforms[parentNodeName]);
                                        }
                                    }
                                }
                                if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end()){
                                    int entitiyIndex = app.mpInstancesMap[cloneName].index;
                                   
                                    int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), entities.data(), app.instanceCount[app.mpInstancesMap[cloneName].assetName]);
                                    if(cnt > 0 ){
                                        auto rinstance = app.mpRenderableManager->getInstance(entities[entitiyIndex]);
                                        if(app.mpTransformManager->hasComponent(entities[entitiyIndex])){
                                            auto entityTransform = app.mpTransformManager->getInstance(entities[entitiyIndex]);
                                            app.mpTransformManager->setParent(app.mpTransforms[nodeName], entityTransform);
                                        }

                                    }
                                    else{
                                        cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(parentNodeName.c_str(), entities.data(), app.instanceCount[app.mpInstancesMap[cloneName].assetName]);
                                        if(cnt > 0 ){
                                            auto rinstance = app.mpRenderableManager->getInstance(entities[entitiyIndex]);
                                            if(app.mpTransformManager->hasComponent(entities[entitiyIndex])){
                                                auto entityTransform = app.mpTransformManager->getInstance(entities[entitiyIndex]);
                                                app.mpTransformManager->setParent(app.mpTransforms[nodeName], entityTransform);
                                            }

                                        }
                                    }
                                    
                                }
                            }
                        
					}
						break;
					case ape::Event::Type::NODE_DETACH:
					{
						;
					}
						break;
					case ape::Event::Type::NODE_POSITION:
					{
                        if(app.mpTransforms.find(nodeName) != app.mpTransforms.end()){
                            auto nodePosition = node->getPosition();
                            auto nodeScale = node->getScale();
                            auto nodeTransforms = app.mpTransformManager->getTransform(app.mpTransforms[nodeName]);
                            float divider = 1.0;
                            auto filamentTransform = filament::math::mat4f(
                                nodeTransforms[0][0], nodeTransforms[0][1], nodeTransforms[0][2], nodeTransforms[0][3],
                                nodeTransforms[1][0], nodeTransforms[1][1], nodeTransforms[1][2], nodeTransforms[1][3],
                                nodeTransforms[2][0], nodeTransforms[2][1], nodeTransforms[2][2], nodeTransforms[2][3],
                                nodePosition.getX()/divider, nodePosition.getY()/divider, nodePosition.getZ()/divider, nodeTransforms[3][3]);
                            app.mpTransformManager->setTransform(app.mpTransforms[nodeName], filamentTransform);
                                if (nodeName.find_first_of(".") != std::string::npos)
                                {
                                    std::string cloneName = nodeName.substr(0,nodeName.find_last_of("."));
                                    std::string subNodeName = nodeName.substr(nodeName.find_last_of(".")+1);
                                    if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end()){
                                        int entitiyIndex = app.mpInstancesMap[cloneName].index;
                                        std::vector<utils::Entity> entities;
                                        entities.resize(10);
                                        int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), entities.data(), 10);
                                        if(cnt > 0 ){
                                            if(app.mpTransformManager->hasComponent(entities[entitiyIndex])){
                                                auto entityTransform = app.mpTransformManager->getInstance(entities[entitiyIndex]);
                                                app.mpTransformManager->setTransform(entityTransform,filamentTransform);
                                            }

                                        }
                                    }
                                }
                            
                        }
                       
					}
						break;
					case ape::Event::Type::NODE_ORIENTATION:
					{
                            auto nodeOrientation= node->getModelMatrix().transpose();
                            if(nodeName == "Pallet.2"){
                                auto ori = node->getOrientation();
                                bool stop=false;
                            }
                            auto nodeTransforms = app.mpTransformManager->getTransform(app.mpTransforms[nodeName]);
                            math::mat4f transform;
                            auto filamentTransform = math::mat4f(
                                nodeOrientation[0][0], nodeOrientation[0][1], nodeOrientation[0][2], nodeTransforms[0][3],
                                nodeOrientation[1][0], nodeOrientation[1][1], nodeOrientation[1][2], nodeTransforms[1][3],
                                nodeOrientation[2][0], nodeOrientation[2][1], nodeOrientation[2][2], nodeTransforms[2][3],
                                nodeTransforms[3][0], nodeTransforms[3][1], nodeTransforms[3][2], nodeTransforms[3][3]);
                            app.mpTransformManager->setTransform(app.mpTransforms[nodeName], filamentTransform);
					}
						break;
					case ape::Event::Type::NODE_SCALE:
                        {
                            ;
                        }
						break;
					case ape::Event::Type::NODE_CHILDVISIBILITY:
                        {
                            if(node->getChildrenVisibility()){
                                if(app.mpInstancesMap.find(nodeName) != app.mpInstancesMap.end()){
                                    auto instance = app.mpInstancesMap[nodeName].mpInstance;
                                    if(!app.mpScene->hasEntity(instance->getEntities()[0])){
                                        app.mpScene->addEntities(instance->getEntities(), instance->getEntityCount());
                                    }
                                    
                                }
                                
                            }
                            else{
                                if(app.mpInstancesMap.find(nodeName) != app.mpInstancesMap.end()){
                                    auto instance = app.mpInstancesMap[nodeName].mpInstance;
                                    if(app.mpScene->hasEntity(instance->getEntities()[0])){
                                        app.mpScene->removeEntities(instance->getEntities(), instance->getEntityCount());
                                    }
                                    
                                }
                            }
                        }
						break;
					case ape::Event::Type::NODE_VISIBILITY:
                        {
                            APE_LOG_DEBUG("Node visibility")
                        }
						break;
					case ape::Event::Type::NODE_FIXEDYAW:
						;
						break;
					case ape::Event::Type::NODE_INHERITORIENTATION:
						;
						break;
					case ape::Event::Type::NODE_INITIALSTATE:
						;
						break;
					case ape::Event::Type::NODE_SHOWBOUNDINGBOX:
						;
						break;
					case ape::Event::Type::NODE_HIDEBOUNDINGBOX:
						;
						break;
					}
				}
			}
			else if (event.type == ape::Event::Type::NODE_DELETE)
			{
				;
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_FILE)
		{
            
			if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string geometryName = geometryFile->getName();
				std::string fileName = geometryFile->getFileName();
                float unitScale = geometryFile->getUnitScale();
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
                    if(app.mpLoadedAssets.find(fileName) == app.mpLoadedAssets.end())
                    {
                            APE_LOG_DEBUG("The asset connected to the parent nocde has not been loaded yet: " << fileName);
                    }
                    else{
                        auto filamentAssetRootEntity = app.mpLoadedAssets[fileName]->getRoot();
                        auto filamentAssetRootTransform = app.mpTransformManager->getInstance(filamentAssetRootEntity);
                        app.mpTransformManager->setParent(filamentAssetRootTransform, app.mpTransforms[parentNodeName]);
                    }
				}
					break;
				case ape::Event::Type::GEOMETRY_FILE_FILENAME:
				{
					if (fileName.find_first_of(".") != std::string::npos)
					{
						std::string fileExtension = fileName.substr(fileName.find_last_of("."));
						if (fileExtension == ".mesh")
						{
							;
						}
						if (fileExtension == ".glb")
						{
							;
						}
						if (fileExtension == ".gltf" || fileExtension == ".glb")
						{
							std::stringstream filePath;
							std::size_t found = fileName.find(":");
							if (found != std::string::npos)
							{
								filePath << fileName;
							}
							else
							{
								std::string separator = "../";
								found = fileName.find(separator);
								if (found != std::string::npos)
								{
									struct stat info;
									if (stat(fileName.c_str(), &info) == -1)
									{
										auto found_it = std::find_end(fileName.begin(), fileName.end(), separator.begin(), separator.end());
										size_t foundPos = found_it - fileName.begin();
										std::stringstream resourceLocationPath;
										resourceLocationPath << APE_SOURCE_DIR << fileName.substr(foundPos + 2);
										filePath << resourceLocationPath.str();
									}
									else
									{
										filePath << fileName;
									}
								}
								else
								{
                                    std::string absolutePath = "/Users";
                                    found = fileName.find(absolutePath);
                                    if(found != std::string::npos)
                                    {
                                        filePath << fileName;
                                    }
                                    else{
                                        std::stringstream resourceLocationPath;
                                        resourceLocationPath << APE_SOURCE_DIR << fileName;
                                        filePath << resourceLocationPath.str();
                                    }
								}
							}
							std::ifstream in(filePath.str().c_str(), std::ifstream::ate | std::ifstream::binary);
							long contentSize = static_cast<long>(in.tellg());
							if (contentSize <= 0)
							{
								APE_LOG_DEBUG("Unable to open " << filePath.str());
							}
							else
							{
								APE_LOG_DEBUG(filePath.str() << " was opened");
							}
							std::ifstream inBin(filePath.str().c_str(), std::ifstream::binary | std::ifstream::in);
							std::vector<uint8_t> buffer(static_cast<unsigned long>(contentSize));
							if (!inBin.read((char*)buffer.data(), contentSize))
							{
								APE_LOG_DEBUG("Unable to read " << filePath.str());
							}
							else
							{
								APE_LOG_DEBUG(filePath.str() << " was read");
							}
                            if (app.asset.find(geometryName) != app.asset.end())
                            {
                                app.mpScene->removeEntities(app.asset[geometryName]->getEntities(), app.asset[geometryName]->getEntityCount());
                            }
                            app.instances[geometryName].resize(10);
                            app.instanceCount[geometryName] = 0;
                            app.asset[geometryName] = app.loader->createInstancedAsset(buffer.data(), buffer.size(), app.instances[geometryName].data(), app.instances[geometryName].size());
 							buffer.clear();
							buffer.shrink_to_fit();
							if (!app.asset[geometryName])
							{
								APE_LOG_DEBUG("Unable to parse " << filePath.str());
							}
							else
							{

								APE_LOG_DEBUG(filePath.str() << " was parsed");
                                
                                gltfio::ResourceConfiguration resourceConfiguration;
                                resourceConfiguration.engine = app.engine;
								auto resourceLocation = filePath.str();
								resourceConfiguration.gltfPath = resourceLocation.c_str();
								resourceConfiguration.normalizeSkinningWeights = true;
								resourceConfiguration.recomputeBoundingBoxes = true;
                                if(app.resourceLoader)
                                    delete app.resourceLoader;
								app.resourceLoader = new gltfio::ResourceLoader(resourceConfiguration);
								if (app.resourceLoader->loadResources(app.asset[geometryName]))
								{
									APE_LOG_DEBUG("resources load OK");
//                                    auto nodeTransforms = app.mpTransformManager->getTransform(app.mpTransforms[geometryName]);
//                                    auto filamentTransform = filament::math::mat4f(
//                                       1/1000,0,0,0,
//                                       0,1/1000,0,0,
//                                       0,0,1/1000,0,
//                                       0,0,0,1);
                                    app.mpLoadedAssets[fileName] = app.asset[geometryName];
								}
								else
								{
									APE_LOG_DEBUG("resources load FAILED");
								}
							}
						}
					}
				}
					break;
				case ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES:
				{
					;
				}
				break;
				case ape::Event::Type::GEOMETRY_FILE_EXPORT:
				{
					;
				}
				break;
				case ape::Event::Type::GEOMETRY_FILE_MATERIAL:
				{
					;
				}
				break;
				case ape::Event::Type::GEOMETRY_FILE_VISIBILITY:
				{
					;
				}
				break;
				}
			}
			else if (event.type == ape::Event::Type::GEOMETRY_FILE_DELETE)
			{
				;
			}
		}
        else if(event.group == ape::Event::Group::GEOMETRY_CLONE)
        {
            if (auto cloneGeometry = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
            {
                std::string sourceFileName = cloneGeometry->getSourceGeometryGroupName();
                
                std::string parentNodeName = "";
                if (auto parentNode = cloneGeometry->getParentNode().lock())
                    parentNodeName = parentNode->getName();
                
                switch (event.type)
                {
                    case ape::Event::Type::GEOMETRY_CLONE_CREATE:
                    {
                        ;
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_PARENTNODE:
                    {
                        //app.mpScene->remove(app.asset[sourceFileName]->getWireframe());
                        //app.mpScene->removeEntities(app.asset[sourceFileName]->getEntities(), app.asset[sourceFileName]->getEntityCount());
                        auto filamentAssetRootEntity = app.mpInstancesMap[event.subjectName].mpInstance->getRoot();
                        auto filamentAssetRootTransform = app.mpTransformManager->getInstance(filamentAssetRootEntity);
                        app.mpTransformManager->setParent(filamentAssetRootTransform, app.mpTransforms[parentNodeName]);
                        auto nodeTransforms = app.mpTransformManager->getTransform(app.mpTransforms[parentNodeName]);
                        auto filamentTransform = filament::math::mat4f(
                                 nodeTransforms[0][0], nodeTransforms[0][1], nodeTransforms[0][2], nodeTransforms[0][3],
                                 nodeTransforms[1][0], nodeTransforms[1][1], nodeTransforms[1][2], nodeTransforms[1][3],
                                 nodeTransforms[2][0], nodeTransforms[2][1], nodeTransforms[2][2], nodeTransforms[2][3],
                                 nodeTransforms[3][0], nodeTransforms[3][1], nodeTransforms[3][2], nodeTransforms[3][3]);
                        app.mpTransformManager->setTransform(app.mpTransforms[parentNodeName], filamentTransform);
                        for(auto  const& x: app.mpTransforms){
                            std::string nodeName = x.first;
                            if (nodeName.find_first_of(".") != std::string::npos)
                            {
                                std::string cloneName = nodeName.substr(0,nodeName.find_last_of("."));
                                std::string subNodeName = nodeName.substr(nodeName.find_last_of(".")+1);
                                if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end()){
                                    int entitiyIndex = app.mpInstancesMap[cloneName].index;
                                    std::vector<utils::Entity> entities;
                                    entities.resize(10);
                                    int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), entities.data(), 10);
                                    if(cnt > 0 ){
                                        auto rinstance = app.mpRenderableManager->getInstance(entities[entitiyIndex]);
                                        
                                        
                                        if(app.mpTransformManager->hasComponent(entities[entitiyIndex])){
                                            auto entityTransform = app.mpTransformManager->getInstance(entities[entitiyIndex]);
                                            app.mpTransformManager->setTransform(entityTransform,app.mpTransformManager->getTransform(x.second));
                                        }

                                    }
                                }
                            }
                        }
                        app.mpScene->addEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRY:
                    {
                        ;
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME:
                    {
                        if(app.instances.find(sourceFileName) != app.instances.end()){
                            if(app.instanceCount[sourceFileName] < 10 ){
                                int cnt = app.instanceCount[sourceFileName]++;
                                app.mpInstancesMap[event.subjectName] =  InstanceData(cnt, sourceFileName, app.instances[sourceFileName][cnt]);
                                auto root = app.instances[sourceFileName][cnt]->getRoot();
                                app.names->addComponent(root);
                                auto nameInstance = app.names->getInstance(root);
                                if(nameInstance)
                                  app.names->setName(nameInstance, event.subjectName.c_str());
                                if(sourceFileName == "characterModel"){
                                    auto cam = &app.view->getCamera();
                                    auto camTM = app.mpTransformManager->getInstance(cam->getEntity());
                                    app.mpTransformManager->setParent(app.mpTransforms["characterNode"], camTM);
                                }
                            }
                            else{
                                  FilamentInstance* instance = app.loader->createInstance(app.asset[sourceFileName]);
                                  auto root = instance->getRoot();
                                  app.names->addComponent(root);
                                  auto nameInstance = app.names->getInstance(root);
                                  if(nameInstance)
                                    app.names->setName(nameInstance, event.subjectName.c_str());

                            }
                        }
                        else{
                            APE_LOG_ERROR("The clone's sourcefile has not been loaded yet.")
                        }
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_DELETE:
                    {
                        ;
                    }
                    break;
                }
            }
        }
		else if (event.group == ape::Event::Group::LIGHT)
		{
			if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				if (event.type == ape::Event::Type::LIGHT_CREATE)
				{
					;
				}
				else
				{
					switch (event.type)
					{
					case ape::Event::Type::LIGHT_ATTENUATION:
						break;
					case ape::Event::Type::LIGHT_DIFFUSE:
						break;
					case ape::Event::Type::LIGHT_DIRECTION:
						break;
					case ape::Event::Type::LIGHT_POWER:
						break;
					case ape::Event::Type::LIGHT_SPECULAR:
						break;
					case ape::Event::Type::LIGHT_SPOTRANGE:
						break;
					case ape::Event::Type::LIGHT_TYPE:
						break;
					case ape::Event::Type::LIGHT_PARENTNODE:
					{
						;
					}
					break;
					case ape::Event::Type::LIGHT_DELETE:
						;
						break;
					}
				}
			}
		}
		else if (event.group == ape::Event::Group::CAMERA)
		{
			if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				if (event.type == ape::Event::Type::CAMERA_CREATE)
				{
//					mpFilamentCamera = mpFilamentEngine->createCamera(mpFilamentEntityManager->create());
//					for (int i = 0; i < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList.size(); i++)
//					{
//						for (int j = 0; j < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList.size(); j++)
//						{
//							for (int k = 0; k < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].cameras.size(); k++)
//							{
//								auto cameraSetting = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].cameras[k];
//								if (cameraSetting.name == camera->getName())
//								{
//									float aspectRatio = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].width / mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].height;
//									camera->setWindow(mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].name);
//									camera->setFocalLength(1.0f);
//									camera->setNearClipDistance(cameraSetting.nearClip);
//									camera->setFarClipDistance(cameraSetting.farClip);
//									camera->setFOVy(cameraSetting.fovY.toRadian());
//									//mpFilamentCamera->setProjection(cameraSetting.fovY.degree, aspectRatio, cameraSetting.nearClip, cameraSetting.farClip);
//								}
//							}
//						}
//					}
				}
				else
				{
					switch (event.type)
					{
					case ape::Event::Type::CAMERA_WINDOW:
                    {
//						for (int i = 0; i < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList.size(); i++)
//						{
//							for (int j = 0; j < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList.size(); j++)
//							{
//								auto renderWindowSetting = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i];
//								auto viewportSetting = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j];
//								for (int k = 0; k < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].cameras.size(); k++)
//								{
//									auto cameraSetting = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].cameras[k];
//									if (cameraSetting.name == camera->getName())
//									{
//										int zorder = viewportSetting.zOrder;
//										float width = (float)viewportSetting.width / (float)renderWindowSetting.width;
//										float height = (float)viewportSetting.height / (float)renderWindowSetting.height;
//										float left = (float)viewportSetting.left / (float)renderWindowSetting.width;
//										float top = (float)viewportSetting.top / (float)renderWindowSetting.height;
//
//										if (mpFilamentView)
//										{
//											APE_LOG_DEBUG("filamentViewport: " << "zorder: " << zorder << " left: " << left << " top: " << top << " width: " << width << " height: " << height);
//											mpFilamentView->setCamera(mpFilamentCamera);
//											mpFilamentView->setViewport({ viewportSetting.left, viewportSetting.top, (unsigned int)viewportSetting.width, (unsigned int)viewportSetting.height });}
//											mpFilamentView->setScene(mpFilamentScene);
//											filament::View::AmbientOcclusionOptions ambientOcclusionOptions;
//											ambientOcclusionOptions.upsampling = filament::View::QualityLevel::HIGH;
//											mpFilamentView->setAmbientOcclusionOptions(ambientOcclusionOptions);
//									}
//								}
//							}
//						}
                        ;
					}
					break;
					case ape::Event::Type::CAMERA_PARENTNODE:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_DELETE:
						;
						break;
					case ape::Event::Type::CAMERA_FOCALLENGTH:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_ASPECTRATIO:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_AUTOASPECTRATIO:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_FOVY:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_FRUSTUMOFFSET:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_FARCLIP:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_NEARCLIP:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_PROJECTION:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_PROJECTIONTYPE:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_ORTHOWINDOWSIZE:
					{
						;
					}
					break;
					case ape::Event::Type::CAMERA_VISIBILITY:
					{
						;
					}
					break;
					}
				}
			}
		}
		}
		catch (std::exception exp)
		{
			APE_LOG_DEBUG("");
		}
		mEventDoubleQueue.pop();
	}
}
void ape::FilamentApplicationPlugin::initFilament(){
    
    
}

void ape::FilamentApplicationPlugin::parseJson(){
    APE_LOG_FUNC_ENTER();
//    std::stringstream fileFullPath;
//    fileFullPath << mpCoreConfig->getConfigFolderPath() << "/apeFilamentApplicationPlugin.json";
//    FILE* apeFilamentApplicationPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
//    char readBuffer[65536];
//    if (apeFilamentApplicationPluginConfigFile)
//    {
//        rapidjson::FileReadStream jsonFileReaderStream(apeFilamentApplicationPluginConfigFile, readBuffer, sizeof(readBuffer));
//        rapidjson::Document jsonDocument;
//        jsonDocument.ParseStream(jsonFileReaderStream);
//        if (jsonDocument.IsObject())
//        {
//            rapidjson::Value& renderSystem = jsonDocument["renderSystem"];
//            mFilamentApplicationPluginConfig.renderSystem = renderSystem.GetString();
//            rapidjson::Value& lodLevels = jsonDocument["lodLevels"];
//            for (rapidjson::Value::MemberIterator lodLevelsMemberIterator =
//                lodLevels.MemberBegin(); lodLevelsMemberIterator != lodLevels.MemberEnd(); ++lodLevelsMemberIterator)
//            {
//                if (lodLevelsMemberIterator->name == "autoGenerateAndSave")
//                    mFilamentApplicationPluginConfig.filamentLodLevelsConfig.autoGenerateAndSave = lodLevelsMemberIterator->value.GetBool();
//                else if (lodLevelsMemberIterator->name == "bias")
//                    mFilamentApplicationPluginConfig.filamentLodLevelsConfig.bias = lodLevelsMemberIterator->value.GetFloat();
//            }
//            if (jsonDocument.HasMember("shading"))
//            {
//                rapidjson::Value& shading = jsonDocument["shading"];
//                mFilamentApplicationPluginConfig.shading = shading.GetString();
//            }
//            rapidjson::Value& renderWindows = jsonDocument["renderWindows"];
//            for (auto& renderWindow : renderWindows.GetArray())
//            {
//                ape::FilamentWindowsRenderWindowConfig filamentWindowsRenderWindowConfig;
//                for (rapidjson::Value::MemberIterator renderWindowMemberIterator =
//                    renderWindow.MemberBegin(); renderWindowMemberIterator != renderWindow.MemberEnd(); ++renderWindowMemberIterator)
//                {
//                    if (renderWindowMemberIterator->name == "enable")
//                        filamentWindowsRenderWindowConfig.enable = renderWindowMemberIterator->value.GetBool();
//                    else if (renderWindowMemberIterator->name == "name")
//                        filamentWindowsRenderWindowConfig.name = renderWindowMemberIterator->value.GetString();
//                    else if (renderWindowMemberIterator->name == "monitorIndex")
//                        filamentWindowsRenderWindowConfig.monitorIndex = renderWindowMemberIterator->value.GetInt();
//                    else if (renderWindowMemberIterator->name == "hidden")
//                        filamentWindowsRenderWindowConfig.hidden = renderWindowMemberIterator->value.GetBool();
//                    else if (renderWindowMemberIterator->name == "resolution")
//                    {
//                        for (rapidjson::Value::MemberIterator resolutionMemberIterator =
//                            renderWindow[renderWindowMemberIterator->name].MemberBegin();
//                            resolutionMemberIterator != renderWindow[renderWindowMemberIterator->name].MemberEnd(); ++resolutionMemberIterator)
//                        {
//                            if (resolutionMemberIterator->name == "width")
//                                filamentWindowsRenderWindowConfig.width = resolutionMemberIterator->value.GetInt();
//                            else if (resolutionMemberIterator->name == "height")
//                                filamentWindowsRenderWindowConfig.height = resolutionMemberIterator->value.GetInt();
//                            else if (resolutionMemberIterator->name == "fullScreen")
//                                filamentWindowsRenderWindowConfig.fullScreen = resolutionMemberIterator->value.GetBool();
//                        }
//                    }
//                    else if (renderWindowMemberIterator->name == "miscParams")
//                    {
//                        for (rapidjson::Value::MemberIterator miscParamsMemberIterator =
//                            renderWindow[renderWindowMemberIterator->name].MemberBegin();
//                            miscParamsMemberIterator != renderWindow[renderWindowMemberIterator->name].MemberEnd(); ++miscParamsMemberIterator)
//                        {
//                            if (miscParamsMemberIterator->name == "vSync")
//                                filamentWindowsRenderWindowConfig.vSync = miscParamsMemberIterator->value.GetBool();
//                            else if (miscParamsMemberIterator->name == "vSyncInterval")
//                                filamentWindowsRenderWindowConfig.vSyncInterval = miscParamsMemberIterator->value.GetInt();
//                            else if (miscParamsMemberIterator->name == "colorDepth")
//                                filamentWindowsRenderWindowConfig.colorDepth = miscParamsMemberIterator->value.GetInt();
//                            else if (miscParamsMemberIterator->name == "FSAA")
//                                filamentWindowsRenderWindowConfig.fsaa = miscParamsMemberIterator->value.GetInt();
//                            else if (miscParamsMemberIterator->name == "FSAAHint")
//                                filamentWindowsRenderWindowConfig.fsaaHint = miscParamsMemberIterator->value.GetString();
//                        }
//                    }
//                    else if (renderWindowMemberIterator->name == "viewports")
//                    {
//                        rapidjson::Value& viewports = renderWindow[renderWindowMemberIterator->name];
//                        for (auto& viewport : viewports.GetArray())
//                        {
//                            ape::FilamentWindowsViewPortConfig filamentViewPortConfig;
//                            for (rapidjson::Value::MemberIterator viewportMemberIterator =
//                                viewport.MemberBegin();
//                                viewportMemberIterator != viewport.MemberEnd(); ++viewportMemberIterator)
//                            {
//                                if (viewportMemberIterator->name == "zOrder")
//                                    filamentViewPortConfig.zOrder = viewportMemberIterator->value.GetInt();
//                                else if (viewportMemberIterator->name == "left")
//                                    filamentViewPortConfig.left = viewportMemberIterator->value.GetInt();
//                                else if (viewportMemberIterator->name == "top")
//                                    filamentViewPortConfig.top = viewportMemberIterator->value.GetInt();
//                                else if (viewportMemberIterator->name == "width")
//                                    filamentViewPortConfig.width = viewportMemberIterator->value.GetInt();
//                                else if (viewportMemberIterator->name == "height")
//                                    filamentViewPortConfig.height = viewportMemberIterator->value.GetInt();
//                                else if (viewportMemberIterator->name == "cameras")
//                                {
//                                    rapidjson::Value& cameras = viewport[viewportMemberIterator->name];
//                                    for (auto& camera : cameras.GetArray())
//                                    {
//                                        ape::FilamentWindowsCameraConfig filamentCameraConfig;
//                                        for (rapidjson::Value::MemberIterator cameraMemberIterator =
//                                            camera.MemberBegin();
//                                            cameraMemberIterator != camera.MemberEnd(); ++cameraMemberIterator)
//                                        {
//                                            if (cameraMemberIterator->name == "name")
//                                                filamentCameraConfig.name = cameraMemberIterator->value.GetString() + mUniqueID;
//                                            else if (cameraMemberIterator->name == "nearClip")
//                                                filamentCameraConfig.nearClip = cameraMemberIterator->value.GetFloat();
//                                            else if (cameraMemberIterator->name == "farClip")
//                                                filamentCameraConfig.farClip = cameraMemberIterator->value.GetFloat();
//                                            else if (cameraMemberIterator->name == "fovY")
//                                                filamentCameraConfig.fovY = cameraMemberIterator->value.GetFloat();
//                                            else if (cameraMemberIterator->name == "positionOffset")
//                                            {
//                                                for (rapidjson::Value::MemberIterator elementMemberIterator =
//                                                    viewport[viewportMemberIterator->name][cameraMemberIterator->name].MemberBegin();
//                                                    elementMemberIterator != viewport[viewportMemberIterator->name][cameraMemberIterator->name].MemberEnd(); ++elementMemberIterator)
//                                                {
//                                                    if (elementMemberIterator->name == "x")
//                                                        filamentCameraConfig.positionOffset.x = elementMemberIterator->value.GetFloat();
//                                                    else if (elementMemberIterator->name == "y")
//                                                        filamentCameraConfig.positionOffset.y = elementMemberIterator->value.GetFloat();
//                                                    else if (elementMemberIterator->name == "z")
//                                                        filamentCameraConfig.positionOffset.z = elementMemberIterator->value.GetFloat();
//                                                }
//                                            }
//                                            else if (cameraMemberIterator->name == "orientationOffset")
//                                            {
//                                                ;
//                                            }
//                                            else if (cameraMemberIterator->name == "parentNodeName")
//                                            {
//                                                filamentCameraConfig.parentNodeName = cameraMemberIterator->value.GetString();
//                                            }
//                                        }
//                                        filamentViewPortConfig.cameras.push_back(filamentCameraConfig);
//                                    }
//                                }
//                            }
//                            filamentWindowsRenderWindowConfig.viewportList.push_back(filamentViewPortConfig);
//                        }
//                    }
//                }
//                mFilamentApplicationPluginConfig.filamentRenderWindowConfigList.push_back(filamentWindowsRenderWindowConfig);
//            }
//        }
//        fclose(apeFilamentApplicationPluginConfigFile);
//    }
    APE_LOG_FUNC_LEAVE();
}
void ape::FilamentApplicationPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentApplicationPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

std::ifstream::pos_type ape::FilamentApplicationPlugin::getFileSize(const char* filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

bool compareAnimationTime(ape::FilamentApplicationPlugin::Animation animation1, ape::FilamentApplicationPlugin::Animation animation2)
{
    return (animation1.time < animation2.time);
}

bool ape::FilamentApplicationPlugin::attach2NewAnimationNode(const std::string& parentNodeName, const ape::NodeSharedPtr& node)
{
    if (auto newParentNode = mpSceneManager->getNode(parentNodeName).lock())
    {
        if (auto currentParentNode = node->getParentNode().lock())
        {
            if (newParentNode != currentParentNode)
            {
                node->setParentNode(newParentNode);
                return true;
            }
        }
        else
        {
            node->setParentNode(newParentNode);
            return true;
        }
    }
    return false;
}

void ape::FilamentApplicationPlugin::initAnimations(){
    APE_LOG_FUNC_ENTER();
    mStartTime = -1.0;
    mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
    std::stringstream fileFullPath;
    fileFullPath << mpCoreConfig->getConfigFolderPath() << "/apeVLFTAnimationPlayerPlugin.json";
    FILE* apeVLFTAnimationPlayerPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
    mAnimations = nlohmann::json::parse(apeVLFTAnimationPlayerPluginConfigFile);
    animationQuicktype::Context* context = &mAnimations.get_mutable_context();
    if (context)
        mIsAllSpaghettiVisible = mAnimations.get_context().get_asset_trail();
    for (const auto& node : mAnimations.get_nodes())
    {
        mAnimatedNodeNames.push_back(node.get_name());
        for (const auto& action : node.get_actions())
        {
            if (action.get_trigger().get_type() == animationQuicktype::TriggerType::TIMESTAMP)
            {
                if (action.get_event().get_type() == animationQuicktype::EventType::SHOW)
                {
                    Animation animation;
                    animation.type = action.get_event().get_type();
                    animation.nodeName = node.get_name();
                    if (action.get_event().get_placement_rel_to())
                    {
                        animation.parentNodeName = *action.get_event().get_placement_rel_to();
                    }
                    if (action.get_event().get_position())
                    {
                        auto position = *action.get_event().get_position();
                        animation.position = ape::Vector3(position[0], position[1], position[2]);
                    }
                    if (action.get_event().get_rotation())
                    {
                        auto orientation = *action.get_event().get_rotation();
                        animation.orientation = ape::Quaternion(orientation[0], orientation[1], orientation[2], orientation[3]);
                    }
                    animation.time = atoi(action.get_trigger().get_data().c_str());
                    mParsedAnimations.push_back(animation);
                }
                if (action.get_event().get_type() == animationQuicktype::EventType::HIDE)
                {
                    Animation animation;
                    animation.type = action.get_event().get_type();
                    animation.nodeName = node.get_name();
                    animation.parentNodeName = "";
                    animation.time = atoi(action.get_trigger().get_data().c_str());
                    mParsedAnimations.push_back(animation);
                }
                if (action.get_event().get_type() == animationQuicktype::EventType::STATE)
                {
                    Animation animation;
                    animation.type = action.get_event().get_type();
                    animation.nodeName = node.get_name();
                    animation.parentNodeName = "";
                    animation.time = atoi(action.get_trigger().get_data().c_str());
                    if (action.get_event().get_data())
                        animation.modelName = *action.get_event().get_data();
                    mParsedAnimations.push_back(animation);
                }
                if (action.get_event().get_type() == animationQuicktype::EventType::LINK)
                {
                    Animation animation;
                    animation.type = action.get_event().get_type();
                    animation.nodeName = node.get_name();
                    animation.parentNodeName = "";
                    animation.time = atoi(action.get_trigger().get_data().c_str());
                    if (action.get_event().get_data())
                        animation.fileName = *action.get_event().get_data();
                    if (action.get_event().get_url())
                        animation.url = *action.get_event().get_data();
                    if (action.get_event().get_descr())
                        animation.descr = *action.get_event().get_data();
                    mParsedAnimations.push_back(animation);
                }
                if (action.get_event().get_type() == animationQuicktype::EventType::TRAIL)
                {
                    Animation animation;
                    animation.type = action.get_event().get_type();
                    animation.nodeName = node.get_name();
                    if (action.get_event().get_value())
                        animation.trail = *action.get_event().get_value();
                    mParsedAnimations.push_back(animation);
                }
                if (action.get_event().get_type() == animationQuicktype::EventType::ANIMATION)
                {
                    std::string fileNamePath = mpCoreConfig->getConfigFolderPath().substr(0, mpCoreConfig->getConfigFolderPath().find("virtualLearningFactory") + 23) + *action.get_event().get_data();
                    std::ifstream file(fileNamePath);
                    std::string dataCount;
                    std::getline(file, dataCount);
                    std::string fps;
                    std::getline(file, fps);
                    std::vector<Animation> currentAnimations;
                    for (int i = 0; i < atoi(dataCount.c_str()); i++)
                    {
                        std::string postionData;
                        std::getline(file, postionData);
                        auto posX = postionData.find_first_of(",");
                        float x = atof(postionData.substr(1, posX).c_str());
                        postionData = postionData.substr(posX + 1, postionData.length());
                        auto posY = postionData.find_first_of(",");
                        float y = atof(postionData.substr(0, posY).c_str());
                        postionData = postionData.substr(posY + 1, postionData.length());
                        auto posZ = postionData.find_first_of("]");
                        float z = atof(postionData.substr(0, posZ).c_str());
                        Animation animation;
                        animation.type = action.get_event().get_type();
                        animation.nodeName = node.get_name();
                        if (action.get_event().get_placement_rel_to())
                            animation.parentNodeName = *action.get_event().get_placement_rel_to();
                        else
                            animation.parentNodeName = "";
                        animation.time = atoi(action.get_trigger().get_data().c_str()) + ((1.0f / atoi(fps.c_str()) * 1000) * i);
                        animation.position = ape::Vector3(x, y, z);
                        currentAnimations.push_back(animation);
                    }
                    for (auto currentAnimation : currentAnimations)
                    {
                        std::string orientationData;
                        std::getline(file, orientationData);
                        auto posW = orientationData.find_first_of(",");
                        float w = atof(orientationData.substr(1, posW).c_str());
                        orientationData = orientationData.substr(posW + 1, orientationData.length());
                        auto posX = orientationData.find_first_of(",");
                        float x = atof(orientationData.substr(0, posX).c_str());
                        orientationData = orientationData.substr(posX + 1, orientationData.length());
                        auto posY = orientationData.find_first_of(",");
                        float y = atof(orientationData.substr(0, posY).c_str());
                        orientationData = orientationData.substr(posY + 1, orientationData.length());
                        auto posZ = orientationData.find_first_of("]");
                        float z = atof(orientationData.substr(0, posZ).c_str());
                        currentAnimation.orientation = ape::Quaternion(w, x, y, z);
                        mParsedAnimations.push_back(currentAnimation);
                    }
                }
                if (action.get_event().get_type() == animationQuicktype::EventType::ANIMATION_ADDITIVE)
                {
                    std::string fileNamePath = mpCoreConfig->getConfigFolderPath().substr(0, mpCoreConfig->getConfigFolderPath().find("virtualLearningFactory") + 23) + *action.get_event().get_data();
                    std::ifstream file(fileNamePath);
                    std::string dataCount;
                    std::getline(file, dataCount);
                    std::string fps;
                    std::getline(file, fps);
                    std::vector<Animation> currentAnimations;
                    for (int i = 0; i < atoi(dataCount.c_str()); i++)
                    {
                        std::string translateData;
                        std::getline(file, translateData);
                        auto posX = translateData.find_first_of(",");
                        float x = atof(translateData.substr(1, posX).c_str());
                        translateData = translateData.substr(posX + 1, translateData.length());
                        auto posY = translateData.find_first_of(",");
                        float y = atof(translateData.substr(0, posY).c_str());
                        translateData = translateData.substr(posY + 1, translateData.length());
                        auto posZ = translateData.find_first_of("]");
                        float z = atof(translateData.substr(0, posZ).c_str());
                        Animation animation;
                        animation.type = action.get_event().get_type();
                        animation.nodeName = node.get_name();
                        if (action.get_event().get_placement_rel_to())
                            animation.parentNodeName = *action.get_event().get_placement_rel_to();
                        else
                            animation.parentNodeName = "";
                        animation.time = atoi(action.get_trigger().get_data().c_str()) + ((1.0f / atoi(fps.c_str()) * 1000) * i);
                        animation.translate = ape::Vector3(x, y, z);
                        currentAnimations.push_back(animation);
                    }
                    for (auto currentAnimation : currentAnimations)
                    {
                        std::string rotationData;
                        std::getline(file, rotationData);
                        auto posW = rotationData.find_first_of(",");
                        float angle = atof(rotationData.substr(1, posW).c_str());
                        rotationData = rotationData.substr(posW + 1, rotationData.length());
                        auto posX = rotationData.find_first_of(",");
                        float x = atof(rotationData.substr(0, posX).c_str());
                        rotationData = rotationData.substr(posX + 1, rotationData.length());
                        auto posY = rotationData.find_first_of(",");
                        float y = atof(rotationData.substr(0, posY).c_str());
                        rotationData = rotationData.substr(posY + 1, rotationData.length());
                        auto posZ = rotationData.find_first_of("]");
                        float z = atof(rotationData.substr(0, posZ).c_str());
                        currentAnimation.rotationAngle = angle;
                        currentAnimation.rotationAxis = ape::Vector3(x, y, z);
                        mParsedAnimations.push_back(currentAnimation);
                    }
                }
            }
        }
    }
    for (const auto& bookmark : mAnimations.get_bookmarks())
    {
        mParsedBookmarkTimes.push_back(atoi(bookmark.get_time().c_str()));
    }
    std::sort(mParsedBookmarkTimes.begin(), mParsedBookmarkTimes.end());
    std::sort(mParsedAnimations.begin(), mParsedAnimations.end(), compareAnimationTime);
    /*for (auto parsedAnimations : mParsedAnimations)
    {
        APE_LOG_DEBUG("animation: " << parsedAnimations.time);
    }*/
    APE_LOG_FUNC_LEAVE();
}

void ape::FilamentApplicationPlugin::playAnimations(double now){
    app.updateinfo.isPlayRunning = true;
    if(app.updateinfo.StartTime >= 0){
        app.updateinfo.pauseTime = (now-app.updateinfo.StartTime)*1000.0;
        for (int i = 0; i < mParsedAnimations.size(); i++)
        {
            if(!(app.updateinfo.playedAnimation[i]) && app.updateinfo.pauseTime >= mParsedAnimations[i].time){
                if (auto node = mpSceneManager->getNode(mParsedAnimations[i].nodeName).lock())
                {
                    if (mParsedAnimations[i].type == animationQuicktype::EventType::SHOW)
                    {
                        attach2NewAnimationNode(mParsedAnimations[i].parentNodeName, node);
                        node->setChildrenVisibility(true);
                        auto ori = mParsedAnimations[i].orientation;
                       
                        if(mParsedAnimations[i].parentNodeName == ""){
                            auto derived = node->getDerivedPosition();
                            auto pos = mParsedAnimations[i].position;
                            auto newPos = Vector3(pos.getX()-derived.getX(),pos.getY()-derived.getY(), pos.getZ()-derived.getZ());
                            node->setPosition(newPos);
                        }
                        else
                            node->setPosition(mParsedAnimations[i].position);
                        node->setOrientation(mParsedAnimations[i].orientation);
                    }
                    else if (mParsedAnimations[i].type == animationQuicktype::EventType::HIDE)
                    {
                        node->setChildrenVisibility(false);
                    }
                }
                app.updateinfo.playedAnimation[i] = true;
            }
        }
    }
}

void ape::FilamentApplicationPlugin::Step()
{

    app.config.title = "Filament";
    app.config.iblDirectory = FilamentApp::getRootAssetsPath() + DEFAULT_IBL;
    utils::Path filename;
    int num_args = 1;
    if (num_args >= 1) {
        filename = "/Users/erik/Documents/ApertusVR/ApertusVR/plugins/scene/photorealisticScene/resources/Conveyor.gltf";
        if (!filename.exists()) {
            std::cerr << "file " << filename << " not found!" << std::endl;
            //return 1;
        }
        if (filename.isDirectory()) {
            auto files = filename.listContents();
            for (auto file : files) {
                if (file.getExtension() == "gltf" || file.getExtension() == "glb") {
                    filename = file;
                    break;
                }
            }
            if (filename.isDirectory()) {
                std::cerr << "no glTF file found in " << filename << std::endl;
                //return 1;
            }
        }
    }

    auto loadAsset = [this](utils::Path filename) {
        // Peek at the file size to allow pre-allocation.
        long contentSize = static_cast<long>(getFileSize(filename.c_str()));
        if (contentSize <= 0) {
            std::cerr << "Unable to open " << filename << std::endl;
            exit(1);
        }

        // Consume the glTF file.
        std::ifstream in(filename.c_str(), std::ifstream::binary | std::ifstream::in);
        std::vector<uint8_t> buffer(static_cast<unsigned long>(contentSize));
        if (!in.read((char*) buffer.data(), contentSize)) {
            std::cerr << "Unable to read " << filename << std::endl;
            exit(1);
        }

        // Parse the glTF file and create Filament entities.
        app.instances[filename].resize(3);
        app.instanceCount[filename] = 0;
        if (filename.getExtension() == "glb") {
            app.asset[filename] = app.loader->createAssetFromBinary(buffer.data(), buffer.size());
        } else {
            app.asset[filename] = app.loader->createInstancedAsset(buffer.data(), buffer.size(), app.instances[filename].data(), app.instances[filename].size());
        }
        buffer.clear();
        buffer.shrink_to_fit();

        if (app.asset.find(filename) == app.asset.end()) {
            std::cerr << "Unable to parse " << filename << std::endl;
            exit(1);
        }
    };

    auto loadResources = [this] (utils::Path filename) {
        // Load external textures and buffers.
        std::string gltfPath = filename.getAbsolutePath();
        ResourceConfiguration configuration = {};
        configuration.engine = app.engine;
        configuration.gltfPath = gltfPath.c_str();
        configuration.recomputeBoundingBoxes = true;
        configuration.normalizeSkinningWeights = true;
        if (!app.resourceLoader) {
            app.resourceLoader = new gltfio::ResourceLoader(configuration);
        }
        if ( app.resourceLoader->asyncBeginLoad(app.asset[filename]))
        {
            APE_LOG_DEBUG("resources load OK");
            app.mpScene->addEntities(app.asset[filename]->getEntities(), app.asset[filename]->getEntityCount());
        }
       
        
        auto ibl = FilamentApp::get().getIBL();
        if (ibl) {
            using namespace filament::math;
            filament::math::float3 const* sh3 = ibl->getSphericalHarmonics();
            filament::IndirectLight* ibl_light;
            app.settings.view.fog.color = sh3[0];
            app.indirectLight = ibl->getIndirectLight();
            if (ibl) {
                float3 d = filament::IndirectLight::getDirectionEstimate(sh3);
                float4 c = filament::IndirectLight::getColorEstimate(sh3, d);
                app.sunlightDirection = d;
                app.sunlightColor = c.rgb;
                app.sunlightIntensity = c[3] * app.indirectLight->getIntensity();
                using namespace filament::math;
                if (app.indirectLight) {
                    app.indirectLight->setIntensity(app.IblIntensity);
                    app.indirectLight->setRotation(mat3f::rotation(app.IblRotation, float3{ 0, 1, 0 }));
                }
            }
        }
    };

   
    auto setup = [&](Engine* engine, View* view, Scene* scene) {
        app.engine = engine;
        app.view = view;
        app.mpScene = scene;
        app.config.title = "Filament";
        app.config.iblDirectory = FilamentApp::getRootAssetsPath() + "default_env";
        app.mpEntityManager = &utils::EntityManager::get();
        app.mpTransformManager = &app.engine->getTransformManager();
        app.mpRenderableManager = &app.engine->getRenderableManager();
        app.names = new NameComponentManager(EntityManager::get());

        app.materials = (app.materialSource == GENERATE_SHADERS) ?
                createMaterialGenerator(engine) : createUbershaderLoader(engine);
        app.loader = AssetLoader::create({engine, app.materials, app.names });
        app.mainCamera = &view->getCamera();
        
        app.currentCamera = 0;
        
        auto mNode = mpSceneManager->createNode("characterNode", true, mpCoreConfig->getNetworkGUID());
        if (auto node = mNode.lock())
        {
            if (auto gltfNode = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("characterModel", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
            {
                gltfNode->setFileName("/plugins/filamentApplicationPlugin/resources/MC_Char_1.glb");
                gltfNode->setParentNode(node);
                node->setPosition(ape::Vector3(0.0, 0.25, 0.2));
                
                if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity("characterModel1", ape::Entity::Type::GEOMETRY_CLONE, true, mpCoreConfig->getNetworkGUID()).lock()))
                {
                    
                    geometryClone->setSourceGeometryGroupName(gltfNode->getName());
                    geometryClone->setParentNode(node);
                }
            }
            
        }
        
    };

    auto cleanup = [this](Engine* engine, View*, Scene*) {
        for(auto const& x : app.asset)
            app.loader->destroyAsset(x.second);

        engine->destroy(app.scene.groundPlane);
        engine->destroy(app.colorGrading);

        delete app.materials;
        delete app.names;
        AssetLoader::destroy(&app.loader);
    };

   
    auto resize = [this](Engine* engine, View* view) {
        filament::Camera& camera = view->getCamera();
        if (&camera == app.mainCamera) {
            // Don't adjut the aspect ratio of the main camera, this is done inside of
            // FilamentApp.cpp
            return;
        }
        const Viewport& vp = view->getViewport();
        double aspectRatio = (double) vp.width / vp.height;
        camera.setScaling(double4 {1.0 / aspectRatio, 1.0, 1.0, 1.0});
    };

    auto gui = [this](Engine* engine, View* view) {
        //copy and edit apecoreJson to vfgame2
        auto& tm = engine->getTransformManager();
        auto& rm = engine->getRenderableManager();
        auto& lm = engine->getLightManager();
        
        auto renderableTreeItem = [this, &rm](utils::Entity entity) {
            bool rvis = app.mpScene->hasEntity(entity);
            ImGui::Checkbox("visible", &rvis);
            if (rvis) {
                app.mpScene->addEntity(entity);
            } else {
                app.mpScene->remove(entity);
            }
            auto instance = rm.getInstance(entity);
            size_t numPrims = rm.getPrimitiveCount(instance);
            for (size_t prim = 0; prim < numPrims; ++prim) {
                const char* mname = rm.getMaterialInstanceAt(instance, prim)->getName();
                if (mname) {
                    ImGui::Text("prim %zu: material %s", prim, mname);
                } else {
                    ImGui::Text("prim %zu: (unnamed material)", prim);
                }
            }
        };

        auto lightTreeItem = [this, &lm](utils::Entity entity) {
//            bool lvis =  app.mpScene->hasEntity(entity);
//            ImGui::Checkbox("visible", &lvis);
//
//            if (lvis) {
//                app.mpScene->addEntity(entity);
//            } else {
//                app.mpScene->remove(entity);
//            }

            auto instance = lm.getInstance(entity);
            bool lcaster = lm.isShadowCaster(instance);
            ImGui::Checkbox("shadow caster", &lcaster);
            lm.setShadowCaster(instance, lcaster);
        };
        
        std::function<void(utils::Entity)> treeNode;
        FilamentAsset* mAsset;
        treeNode = [&](utils::Entity entity) {
            auto tinstance = tm.getInstance(entity);
            auto rinstance = rm.getInstance(entity);
            auto linstance = lm.getInstance(entity);
            intptr_t treeNodeId = 1 + entity.getId();
            
            const char* name = mAsset->getName(entity);
            auto getLabel = [&name, &rinstance, &linstance]() {
                if (name) {
                    return name;
                }
                if (rinstance) {
                    return "Mesh";
                }
                if (linstance) {
                    return "Light";
                }
                return "Node";
            };
            const char* label = getLabel();

            ImGuiTreeNodeFlags flags = 0; // rinstance ? 0 : ImGuiTreeNodeFlags_DefaultOpen;
            std::vector<utils::Entity> children(tm.getChildCount(tinstance));
            if (ImGui::TreeNodeEx((const void*) treeNodeId, flags, "%s", label)) {
                if (rinstance) {
                    renderableTreeItem(entity);
                }
                if (linstance) {
                    lightTreeItem(entity);
                }
                tm.getChildren(tinstance, children.data(), children.size());
                for (auto ce : children) {
                    treeNode(ce);
                }
                ImGui::TreePop();
            }
        };
        if(app.updateinfo.inRoom){
            const float width = ImGui::GetIO().DisplaySize.x;
            const float height = ImGui::GetIO().DisplaySize.y;
            
            ImGui::SetNextWindowPos(ImVec2(width-320, height-320),ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
            ImGui::SetNextWindowSizeConstraints(ImVec2(200, 150), ImVec2(500, 500));
           
            ImGui::Begin("Hierarchy", nullptr);
            for(auto  const& x: app.asset){
                auto instances = x.second->getAssetInstances();
                size_t cnt = x.second->getAssetInstanceCount();
                mAsset = x.second;
                if (ImGui::CollapsingHeader(x.first.c_str())) {
                    for(size_t i = 0; i < cnt; i++){
                            ImGui::Indent();
                            treeNode(instances[i]->getRoot());
                            ImGui::Unindent();
                    }
                }
            }
            ImGui::End();
        }
        FilamentApp::get().setSidebarWidth(0);
        mpVlftImgui->update();
    };

    auto preRender = [this](Engine* engine, View* view, Scene* scene, Renderer* renderer) {
        auto& rcm = engine->getRenderableManager();
        auto instance = rcm.getInstance(app.scene.groundPlane);
        rcm.setLayerMask(instance,
                0xff, app.viewOptions.groundPlaneEnabled ? 0xff : 0x00);
        if(app.updateinfo.setPosition){
            auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
            auto tmInstance = app.mpTransformManager->getInstance(instance->getRoot());
            auto transforms = app.mpTransformManager->getTransform(tmInstance);
            auto worldTm = app.mpTransformManager->getWorldTransform(tmInstance);
            transforms[3][0] = app.updateinfo.position.getX();
            transforms[3][1] = app.updateinfo.position.getY();
            transforms[3][2] = app.updateinfo.position.getZ();
            app.mpTransformManager->setTransform(tmInstance, transforms);
            app.updateinfo.setPosition = false;
        }
        if(app.updateinfo.setRotation){
            auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
            auto tmInstance = app.mpTransformManager->getInstance(instance->getRoot());
            auto transforms = app.mpTransformManager->getTransform(tmInstance);
            auto setOri = math::details::TQuaternion<float>(app.updateinfo.orientation.getW(),
                                              app.updateinfo.orientation.getX(),
                                              app.updateinfo.orientation.getY(),
                                              app.updateinfo.orientation.getZ());
            auto orientation = setOri*transforms.toQuaternion();
            math::mat4f newLocal(orientation);
            newLocal[3][0] =  transforms[3][0];
            newLocal[3][1] =  transforms[3][1];
            newLocal[3][2] =  transforms[3][2];
            app.mpTransformManager->setTransform(tmInstance, newLocal);
            app.updateinfo.setRotation = false;
        }
        if(app.updateinfo.deleteSelected){
            auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
            scene->removeEntities(instance->getEntities(), instance->getEntityCount());
            scene->remove(app.boxEntity.first);
            app.updateinfo.selectedItem = "";
            app.updateinfo.rootOfSelected = "";
            app.updateinfo.deleteSelected = false;
        }
        if(app.updateinfo.drop){
            auto parentTm = app.mpTransformManager->getInstance(app.parentOfPicked);
            auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
            auto tmInstance = app.mpTransformManager->getInstance(instance->getRoot());
            auto parentWorldMatrix = app.mpTransformManager->getTransform(tmInstance);
            parentWorldMatrix[3][1] += 0.4;
            parentWorldMatrix[3][2] += 2.0;
            app.mpTransformManager->setTransform(tmInstance, parentWorldMatrix);
            app.mpTransformManager->setParent(tmInstance,parentTm);
            app.updateinfo.pickedItem = "";
            app.updateinfo.drop = false;
            scene->remove(app.boxEntity.first);
        }
        else if(app.updateinfo.pickUp || app.updateinfo.pickedItem != "" ){
            if(app.updateinfo.selectedItem != ""){
                
                auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
                auto tmInstance = app.mpTransformManager->getInstance(instance->getRoot());
                auto camInstance = app.mpTransformManager->getInstance(view->getCamera().getEntity());
                auto parentWorldMatrix = app.mpTransformManager->getTransform(tmInstance);
                auto worldTransform = app.mpTransformManager->getWorldTransform(tmInstance);
                
               
                //auto parent = app.mpTransformManager->getParent(tmInstance);
                //auto tmParent =app.mpTransformManager->getInstance(parent);
                
                if(app.updateinfo.pickedItem == ""){
                    app.parentOfPicked = app.mpTransformManager->getParent(tmInstance);
                    auto name = app.asset[app.mpInstancesMap[app.rootOfSelected.second].assetName]->getName(app.parentOfPicked);
                    app.mpTransformManager->setParent(tmInstance, camInstance);
                    parentWorldMatrix = mat4f(1);
                    parentWorldMatrix[3][1] = -0.4;
                    parentWorldMatrix[3][2] = -2.0;
                    app.mpTransformManager->setTransform(tmInstance, parentWorldMatrix);
                    app.updateinfo.pickedItem = app.updateinfo.selectedItem;
                    app.updateinfo.pickUp = false;
                }else if(app.updateinfo.pickUp){
                    auto parentTm = app.mpTransformManager->getInstance(app.parentOfPicked);
                    parentWorldMatrix = app.mpTransformManager->getWorldTransform(parentTm);
                    
                    auto camWorldMatrix = app.mpTransformManager->getWorldTransform(tmInstance);
                    app.mpTransformManager->setParent(tmInstance,parentTm);
                    auto orientation = inverse(parentWorldMatrix.toQuaternion());
                    auto invTrans = orientation*(camWorldMatrix[3].xyz-parentWorldMatrix[3].xyz);
                    auto invertedOri = orientation* camWorldMatrix.toQuaternion();
                    math::mat4f newLocal(invertedOri);
                    
                    newLocal[3].xyz = invTrans;
                    
                    app.mpTransformManager->setTransform(tmInstance, newLocal);
                    app.updateinfo.pickedItem = "";
                    app.updateinfo.pickUp = false;
                    scene->remove(app.boxEntity.first);
                }
            }
        }
        filament::Camera& camera = view->getCamera();
        auto viewMatrix =  camera.getModelMatrix();
//        if(mIsStudent){
//            camera.setModelMatrix(filament::math::mat4f(
//                                viewMatrix[0][0],viewMatrix[0][1],viewMatrix[0][2],viewMatrix[0][3],
//                                viewMatrix[1][0],viewMatrix[1][1],viewMatrix[1][2],viewMatrix[1][3],
//                                viewMatrix[2][0],viewMatrix[2][1],viewMatrix[2][2],viewMatrix[2][3],
//                                viewMatrix[3][0]+0.2, 1.5,viewMatrix[3][2]+3.0,viewMatrix[3][3]
//                                             ));
//        }
//        else{
//            camera.setModelMatrix(filament::math::mat4f(
//                                viewMatrix[0][0],viewMatrix[0][1],viewMatrix[0][2],viewMatrix[0][3],
//                                viewMatrix[1][0],viewMatrix[1][1],viewMatrix[1][2],viewMatrix[1][3],
//                                viewMatrix[2][0],viewMatrix[2][1],viewMatrix[2][2],viewMatrix[2][3],
//                                viewMatrix[3][0]+0.2,  viewMatrix[3][1]+1.5, viewMatrix[3][2]+3.0,viewMatrix[3][3]
//                                             ));
//        }
        
        //view->setCamera(app.mainCamera);
        
        
        camera.setExposure(
                app.viewOptions.cameraAperture,
                1.0f / app.viewOptions.cameraSpeed,
                app.viewOptions.cameraISO);
        auto ibl = FilamentApp::get().getIBL();
        if (ibl) {
            ibl->getSkybox()->setLayerMask(0xff, app.viewOptions.skyboxEnabled ? 0xff : 0x00);
        }
        
        view->setColorGrading(nullptr);
        processEventDoubleQueue();
    };

    auto postRender = [this](Engine* engine, View* view, Scene* scene, Renderer* renderer) {
        ;
    };
    auto userInput = [&](Engine* engine, View* view, Scene* scene, filament::camutils::Manipulator<float>* manipulator,int x, int y, SDL_Event event, int width, int height, double now){
        
        app.updateinfo.now = now;
        filament::math::vec3<float> origin;
        filament::math::vec3<float> dir;
        filament::math::vec3<float> mtarget;
        filament::math::vec3<float> upward;
        auto* camera = &view->getCamera();
        float tnear = camera->getNear()/2;
        float tfar = camera->getCullingFar()/10;
        float fov = camera->getFieldOfViewInDegrees(filament::Camera::Fov::VERTICAL);
//        auto frustum = camera->getFrustum();
        //app.mpRenderableManager->getAxisAlignedBoundingBox(<#Instance instance#>)
        auto keyCode = event.key.keysym.scancode;
        auto a = event.key.keysym.sym;
        auto keyState = SDL_GetModState();
        if(!app.updateinfo.inSettings || app.updateinfo.changedKey){
            switch(event.type){
                case SDL_MOUSEBUTTONDOWN:{
                    if(!app.updateinfo.inSettings){
                        if(event.button.button == SDL_BUTTON_LEFT && app.updateinfo.pickedItem == ""){
                            auto viewport = view->getViewport();
                            width = viewport.width;
                            height = viewport.height;
                            
                            manipulator->getLookAt(&origin, &mtarget, &upward);
                            math::vec3<float>  gaze = normalize(mtarget - origin);
                            math::vec3<float> right = normalize(cross(gaze, math::vec3<float>(0,1,0)));
                            upward = cross(right, gaze);
                            fov = fov * F_PI / 180.0;

                            // Remap the grid coordinate into [-1, +1] and shift it to the pixel center.
                            float u = 2.0 * (0.5+x) / width - 1.0;
                            float v = 1.0-2.0 * (0.5+y) / height;
                            // Compute the tangent of the field-of-view angle as well as the aspect ratio.
                            float tangent = tan(fov/2.0);
                            float aspectRatio = (float)width / height;

                            // Adjust the gaze so it goes through the pixel of interest rather than the grid center.
                            dir = gaze;
                            dir += right * (tangent * u * aspectRatio);
                            dir += upward * (tangent * v);
                            dir = normalize(dir);
                            origin += dir*tnear;
                            
                            app.rayIntersectedEntities.clear();
                            app.boxEntity.second = 1000000.0;
                            math::mat4f worldTm;
                            
                            for(auto  const& asset: app.asset){
                                auto* entities = asset.second->getEntities();
                                size_t cnt = asset.second->getEntityCount();
                                for(size_t i = 0; i < cnt; i++){
                                    if(app.mpRenderableManager->hasComponent(entities[i]) && scene->hasEntity(entities[i])){
                                        auto instance = app.mpRenderableManager->getInstance(entities[i]);
                                        auto tmInstance = app.mpTransformManager->getInstance(entities[i]);
                                        worldTm = app.mpTransformManager->getWorldTransform(tmInstance);
                                        auto box = app.mpRenderableManager->getAxisAlignedBoundingBox(instance);
                                        
                                        math::vec3<float> T_1;
                                        math::vec3<float> T_2;
                                        float t_near = -FLT_MIN;
                                        float t_far = FLT_MAX;
                                        math::float3 boxMin = box.getMin();
                                        math::float3 boxMax = box.getMax();
                                        if(boxMax.x != boxMin.x && boxMax.y != boxMin.y && boxMax.z != boxMin.z){
                                            box = rigidTransform(box, worldTm);
                                            boxMin = box.getMin();
                                            boxMax = box.getMax();
                                            float t[9];
                                            t[1] = (boxMin.x - origin.x)/dir.x;
                                            t[2] = (boxMax.x - origin.x)/dir.x;
                                            t[3] = (boxMin.y - origin.y)/dir.y;
                                            t[4] = (boxMax.y - origin.y)/dir.y;
                                            t[5] = (boxMin.z - origin.z)/dir.z;
                                            t[6] = (boxMax.z - origin.z)/dir.z;
                                            t[7] = fmax(fmax(fmin(t[1], t[2]), fmin(t[3], t[4])), fmin(t[5], t[6]));
                                            t[8] = fmin(fmin(fmax(t[1], t[2]), fmax(t[3], t[4])), fmax(t[5], t[6]));
                                            if(t[8] >= 0 && t[7] <= t[8] && t[7] >= 0){
                                                app.rayIntersectedEntities.push_back(std::pair(entities[i], t[7]));
                                                if(abs(t[7]) < app.boxEntity.second){
                                                    if(app.mpScene->hasEntity(app.boxEntity.first)){
                                                        app.mpScene->remove(app.boxEntity.first);
                                                    }
                                                    if(app.mpEntityManager->isAlive(app.boxEntity.first)){
                                                        app.engine->destroy(app.boxEntity.first);
                                                        app.mpEntityManager->destroy(app.boxEntity.first);
                                                        if(app.mpTransformManager->hasComponent(app.boxEntity.first))
                                                            app.mpTransformManager->destroy(app.boxEntity.first);
                                                        app.engine->destroy(app.boxVertexBuffer);
                                                        app.engine->destroy(app.boxIndexBuffer);
                                                        app.engine->destroy(app.boxEntity.first);
                                                            
                                                    }
                                                        
                                                    size_t boxVertCount = 8;
                                                    size_t boxIndCount = 24;
                                                    
                                                    app.boxVerts[0] = math::float3(boxMin.x, boxMin.y, boxMin.z);
                                                    app.boxVerts[1] = math::float3(boxMin.x, boxMin.y, boxMax.z);
                                                    app.boxVerts[2] = math::float3(boxMin.x, boxMax.y, boxMin.z);
                                                    app.boxVerts[3] = math::float3(boxMin.x, boxMax.y, boxMax.z);
                                                    app.boxVerts[4] = math::float3(boxMax.x, boxMin.y, boxMin.z);
                                                    app.boxVerts[5] = math::float3(boxMax.x, boxMin.y, boxMax.z);
                                                    app.boxVerts[6] = math::float3(boxMax.x, boxMax.y, boxMin.z);
                                                    app.boxVerts[7] = math::float3(boxMax.x, boxMax.y, boxMax.z);
                                                   
                                                    app.boxInds[0] = 0;
                                                    app.boxInds[1] = 1;
                                                    app.boxInds[2] = 1;
                                                    app.boxInds[3] = 3;
                                                    app.boxInds[4] = 3;
                                                    app.boxInds[5] = 2;
                                                    app.boxInds[6] = 2;
                                                    app.boxInds[7] = 0;
                                                    // Generate 4 lines around face at +X.
                                                    app.boxInds[ 8] = 4;
                                                    app.boxInds[ 9] = 5;
                                                    app.boxInds[10] = 5;
                                                    app.boxInds[11] = 7;
                                                    app.boxInds[12] = 7;
                                                    app.boxInds[13] = 6;
                                                    app.boxInds[14] = 6;
                                                    app.boxInds[15] = 4;
                                                    // Generate 2 horizontal lines at -Z.
                                                    app.boxInds[16] = 0;
                                                    app.boxInds[17] = 4;
                                                    app.boxInds[18] = 2;
                                                    app.boxInds[19] = 6;
                                                    // Generate 2 horizontal lines at +Z.
                                                    app.boxInds[20] = 1;
                                                    app.boxInds[21] = 5;
                                                    app.boxInds[22] = 3;
                                                    app.boxInds[23] = 7;
                                                    
                                                    
                                                    app.boxVertexBuffer = VertexBuffer::Builder()
                                                        .bufferCount(1)
                                                        .vertexCount(boxVertCount)
                                                        .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
                                                        .build(*engine);

                                                    app.boxIndexBuffer = IndexBuffer::Builder()
                                                        .indexCount(boxIndCount)
                                                        .bufferType(IndexBuffer::IndexType::UINT)
                                                        .build(*engine);

                                                    app.boxVertexBuffer->setBufferAt(*engine, 0, VertexBuffer::BufferDescriptor(
                                                                        app.boxVerts, app.boxVertexBuffer->getVertexCount() * sizeof(float3), nullptr));

                                                    app.boxIndexBuffer->setBuffer(*engine, IndexBuffer::BufferDescriptor(
                                                            app.boxInds, app.boxIndexBuffer->getIndexCount() * sizeof(uint32_t), nullptr));
                                                    
                                                    app.boxEntity = std::pair(EntityManager::get().create(), t[7]);
                                                    RenderableManager::Builder(1)
                                                        .culling(false)
                                                        .castShadows(false)
                                                        .receiveShadows(false)
                                                        .geometry(0, RenderableManager::PrimitiveType::LINES, app.boxVertexBuffer, app.boxIndexBuffer)
                                                        .build(*engine, app.boxEntity.first);
                                                    if(!app.mpScene->hasEntity(app.boxEntity.first)){
                                                        app.mpScene->addEntity(app.boxEntity.first);
                                                        app.selectedNode =std::pair(entities[i], asset.first);
                                                    }
                                                       
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            if(app.boxEntity.second < 1000000.0){
                                auto tmInstance = app.mpTransformManager->getInstance(app.selectedNode.first);
                                auto parent = app.mpTransformManager->getParent(tmInstance);
                                bool found = false;
                                auto sceneNodes = mpSceneManager->getNodes();
                                std::string parentName = "";
                                std::string rootName = app.asset[app.selectedNode.second]->getName(parent);
                                while(app.mpInstancesMap.find(rootName) == app.mpInstancesMap.end()){
                                    tmInstance = app.mpTransformManager->getInstance(parent);
                                    parent = app.mpTransformManager->getParent(tmInstance);
                                    rootName = app.asset[app.selectedNode.second]->getName(parent);
                                }
                                app.rootOfSelected.first = parent;
                                app.rootOfSelected.second = rootName;
                                app.updateinfo.rootOfSelected = rootName;
                                tmInstance = app.mpTransformManager->getInstance(app.selectedNode.first);
                                parent = app.mpTransformManager->getParent(tmInstance);
                                std::string sceneNodeName = "";
                                while(parent && !found){
                                    if(app.asset[app.selectedNode.second]->getName(parent)){
                                        parentName = app.asset[app.selectedNode.second]->getName(parent);
                                        for(auto  const& x: sceneNodes){
                                            sceneNodeName = x.first;
                                            if(sceneNodeName == parentName){
                                                app.selectedNode.first = parent;
                                                app.selectedNode.second = parentName;
                                                app.updateinfo.selectedItem = parentName;
                                                found = true;
                                                break;
                                            }
                                            else if (sceneNodeName.find_first_of(".") != std::string::npos)
                                            {
                                                std::string cloneName = sceneNodeName.substr(0,sceneNodeName.find_last_of("."));
                                                std::string nodeName = sceneNodeName.substr(sceneNodeName.find_last_of(".")+1);
                                                if(parentName == nodeName && rootName == cloneName){
                                                    app.updateinfo.selectedItem = sceneNodeName;
                                                    app.updateinfo.rootOfSelected = cloneName;
                                                    app.selectedNode.first = parent;
                                                    found = true;
                                                    break;
                                                }
                                                    
                                            }
                                        }
                                    }
                                    if(!found){
                                        tmInstance = app.mpTransformManager->getInstance(parent);
                                        parent = app.mpTransformManager->getParent(tmInstance);
                                    }
                                }
                                if(!found)
                                    app.updateinfo.selectedItem = rootName;
                            }
                            isSelected = true;
                        }
                        app.animationData.mouseDown = true;
                        app.animationData.animatedClick = false;
                        app.animationData.mouseStartTime = now;
                        break;
                    }
                }
                case SDL_MOUSEBUTTONUP:{
                    if(!app.updateinfo.inSettings){
                        if(event.button.button == SDL_BUTTON_LEFT){
                            app.animationData.mouseDown = false;
                        }
                        break;
                    }
                }
                case SDL_KEYDOWN:{
                    if(app.updateinfo.changedKey){
                        bool sameKey = false;
                        for(auto const& x : mKeyMap){
                            if(x.second == keyCode){
                                sameKey = true;
                                break;
                            }
                        }
                        if(!sameKey){
                            mKeyMap[app.updateinfo.changeKeyCode] = keyCode;
                            app.updateinfo.keyLabel[app.updateinfo.changeKeyCode] = event.text.text[8];
                        }
                        app.updateinfo.changedKey = false;
                    }
                    else if(!app.updateinfo.inSettings){
                        bool moved;
                        if(keyCode == mKeyMap["w"]){
                            moved = true;
                            manipulator->keyDown(filament::camutils::Manipulator<float>::Key::FORWARD);
                        }
                        if(keyCode == mKeyMap["s"]){
                            moved = true;
                            manipulator->keyDown(filament::camutils::Manipulator<float>::Key::BACKWARD);
                        }
                        if(keyCode == mKeyMap["a"]){
                            moved = true;
                            manipulator->keyDown(filament::camutils::Manipulator<float>::Key::LEFT);
                        }
                        if(keyCode == mKeyMap["d"]){
                            moved = true;
                            manipulator->keyDown(filament::camutils::Manipulator<float>::Key::RIGHT);
                        }
                        if(keyCode == mKeyMap["e"]){
                            moved = true;
                            manipulator->keyDown(filament::camutils::Manipulator<float>::Key::UP);
                        }
                        if(keyCode == mKeyMap["q"]){
                            moved = true;
                            manipulator->keyDown(filament::camutils::Manipulator<float>::Key::DOWN);
                        }
                        if(moved){
                            
                            if(app.animationData.animatedKey){
                                app.animationData.keyStartTime = now;
                                app.animationData.animatedKey = false;
                                app.animationData.keysDown = true;
                            }
                            if(keyState == KMOD_RSHIFT || keyState == KMOD_LSHIFT){
                                if(app.animationData.keyCurrentAnimation == 0){
                                    auto animator = app.instances["characterModel"][0]->getAnimator();
                                    animator->applyAnimation(0, 0);
                                }
                                app.animationData.keyCurrentAnimation = 1;
                                
                            }
                            else{
                                if(app.animationData.keyCurrentAnimation == 1){
                                    auto animator = app.instances["characterModel"][0]->getAnimator();
                                    animator->applyAnimation(1, 0);
                                }
                                app.animationData.keyCurrentAnimation = 0;
                            }
                            app.animationData.keyDown[keyCode] = true;
                        }
                    }
                    break;
                }
                case SDL_KEYUP:{
                    if(!app.updateinfo.inSettings){
                        if(keyCode == mKeyMap["w"]){
                            manipulator->keyUp(filament::camutils::Manipulator<float>::Key::FORWARD);
                        }
                        if(keyCode == mKeyMap["s"]){
                            manipulator->keyUp(filament::camutils::Manipulator<float>::Key::BACKWARD);
                        }
                        if(keyCode == mKeyMap["a"]){
                            manipulator->keyUp(filament::camutils::Manipulator<float>::Key::LEFT);
                        }
                        if(keyCode == mKeyMap["d"]){
                            manipulator->keyUp(filament::camutils::Manipulator<float>::Key::RIGHT);
                        }
                        if(keyCode == mKeyMap["e"]){
                            manipulator->keyUp(filament::camutils::Manipulator<float>::Key::UP);
                        }
                        if(keyCode == mKeyMap["q"]){
                            manipulator->keyUp(filament::camutils::Manipulator<float>::Key::DOWN);
                        }
                        app.animationData.keyDown[keyCode] = false;
                        if(!app.animationData.keyDown[mKeyMap["w"]] && !app.animationData.keyDown[mKeyMap["a"]] &&
                           !app.animationData.keyDown[mKeyMap["s"]] && !app.animationData.keyDown[mKeyMap["d"]] &&
                           !app.animationData.keyDown[mKeyMap["e"]] && !app.animationData.keyDown[mKeyMap["q"]]){
                            app.animationData.keysDown = false;
                        }
                        break;
                    }
                }
            }
        }
    };

    auto animate = [this](Engine* engine, View* view, double now) {
        app.updateinfo.now = now;
        if(!app.updateinfo.isPlayRunning && app.updateinfo.IsPlayClicked){
            app.updateinfo.StartTime = now-app.updateinfo.pauseTime/1000;
            playAnimations(now);
        }
        else if(app.updateinfo.IsPlayClicked){
            playAnimations(now);
        }
        if(app.instances.find("characterModel") != app.instances.end()){
            auto animator = app.instances["characterModel"][0]->getAnimator();
            double timeDiff;
            if(!app.animationData.animatedClick){
                timeDiff = now - app.animationData.mouseStartTime;
                
                if( timeDiff > animator->getAnimationDuration(3) && app.animationData.mouseDown){
                    app.animationData.mouseStartTime +=  animator->getAnimationDuration(3);
                    timeDiff = now - app.animationData.mouseStartTime;
                }
                if(timeDiff <= animator->getAnimationDuration(3)){
                    animator->applyAnimation(3, timeDiff);
                }else{
                    app.animationData.animatedClick = true;
                    animator->applyAnimation(3, 0);
                }
            }
            if(!app.animationData.animatedKey){
                timeDiff = now - app.animationData.keyStartTime;
                if(!app.animationData.keysDown){
                    app.animationData.animatedKey = true;
                    animator->applyAnimation(3, 0);
                }
                else{
                    if( timeDiff > animator->getAnimationDuration(app.animationData.keyCurrentAnimation) && app.animationData.keysDown){
                        app.animationData.keyStartTime +=  animator->getAnimationDuration(app.animationData.keyCurrentAnimation);
                        timeDiff = now - app.animationData.keyStartTime;
                    }
                    if(timeDiff <= animator->getAnimationDuration(app.animationData.keyCurrentAnimation)){
                        animator->applyAnimation(app.animationData.keyCurrentAnimation, timeDiff);
                    }
                }
                
            }
            animator->updateBoneMatrices();
        }
       
    };
    
    FilamentApp& filamentApp = FilamentApp::get();
    filamentApp.animate(animate);
    filamentApp.resize(resize);
    filamentApp.setDropHandler([&] (std::string path) {
        loadAsset(path);
        loadResources(path);
    });
    app.config.cameraMode = filament::camutils::Mode::FREE_FLIGHT;
    filamentApp.run(app.config, setup, cleanup, gui, preRender, postRender, userInput);
    
    //return 0;
}

void ape::FilamentApplicationPlugin::Stop()
{

}

void ape::FilamentApplicationPlugin::Suspend()
{

}

void ape::FilamentApplicationPlugin::Restart()
{

}
