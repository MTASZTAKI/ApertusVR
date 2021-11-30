#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "apeFilamentApplicationPlugin.h"


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
    mIsStudent = false;
    parseJson();
    initFilament();
    mParsedAnimations = std::vector<Animation>();
    mParsedBookmarkTimes = std::vector<unsigned long long>();
    mIsPauseClicked = false;
    mIsStopClicked = false;
    mIsPlayRunning = false;
    mIsAllSpaghettiVisible = false;
    mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
    mpUserInputMacro->registerCallbackForHmdMovedValue(std::bind(&FilamentApplicationPlugin::hmdMovedEventCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    mAnimatedNodeNames = std::set<std::string>();
    mAttachedUsers = std::vector<ape::NodeWeakPtr>();
    mSpaghettiNodeNames = std::set<std::string>();
    mstateNodeNames = std::set<std::string>();
    mstateGeometryNames = std::set<std::string>();
    mSpaghettiNodeNames = std::set<std::string>();
    mStudents = std::vector<ape::NodeWeakPtr>();
    mStudentsMovementLoggingFile = std::ofstream();
    mKeyMap = std::map<std::string, SDL_Scancode>();
    idGltfMap = std::map<std::string, std::string>();
    mCamManipulator =  filament::camutils::Manipulator<float>::Builder()
    .flightStartPosition(2.5, 1.5, 1)
    .build(filament::camutils::Mode::FREE_FLIGHT);
    mCameraBookmark = mCamManipulator->getCurrentBookmark();
    logoAnimTime = 0;
    mUserName = "DefaultUser";
    mPostUserName = "";
    initKeyMap();
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
    mKeyMap["o"] = SDL_SCANCODE_O;
    mKeyMap["f2"] = SDL_SCANCODE_F2;
    mKeyMap["f3"] = SDL_SCANCODE_F3;
}

void ape::FilamentApplicationPlugin::eventCallBack(const ape::Event& event)
{
	mEventDoubleQueue.push(event);
    //APE_LOG_DEBUG(event.subjectName);
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
                std::string glftNodeName =  idGltfMap[nodeName];
				if (event.type == ape::Event::Type::NODE_CREATE)
				{
                
					auto filamentEntity = app.mpEntityManager->create();
                    app.mpEntities[nodeName] = filamentEntity;
                    app.names->addComponent(filamentEntity);
                    auto nameInstance = app.names->getInstance(filamentEntity);
                    if(nameInstance)
                        app.names->setName(nameInstance, nodeName.c_str());
					app.mpTransformManager->create(filamentEntity);
					auto filamentTransform = app.mpTransformManager->getInstance(filamentEntity);
                    app.mpTransforms[nodeName] = filamentTransform;
                    app.mpScene->addEntity(filamentEntity);
//                    std::string postName = "_vlftStudent";
//                    if(app.updateinfo.isAdmin)
//                        postName = "_vlftTeacher";
//                    std::size_t pos = nodeName.find(postName);
//                    if(pos != std::string::npos){
//                        if(node->getOwner() == mpCoreConfig->getNetworkGUID()){
//                            if(app.mpTransforms.find(nodeName) != app.mpTransforms.end()){
//                                auto camEntity = app.mainCamera->getEntity();
//                                auto camTm = app.mpTransformManager->getInstance(camEntity);
//                                app.mpTransformManager->setParent(camTm, app.mpTransforms[nodeName]);
//                                auto filaTm = filament::math::mat4f(1,0,0,0,
//                                                      0,1,0,0,
//                                                      0,0,1,0,
//                                                      0,0,0,1);
//                                app.mpTransformManager->setTransform(camTm, filaTm);
//                            }
//                        }
//                    }
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
                        
                        std::size_t pos = nodeName.find("_vlftStudent");
                        if(pos != std::string::npos){
                            if(node->getOwner() != mpCoreConfig->getNetworkGUID()){
                                if (auto parentNode = node->getParentNode().lock()){
                                    if(app.mpTransforms.find(parentNodeName) != app.mpTransforms.end()){
                                        auto camEntity = app.mainCamera->getEntity();
                                        auto camTm = app.mpTransformManager->getInstance(camEntity);
                                        app.mpTransformManager->setParent(camTm, app.mpTransforms[parentNodeName]);
                                        auto filaTm = filament::math::mat4f(1,0,0,0,
                                                              0,1,0,0,
                                                              0,0,1,0,
                                                              0,-0.0,-0.05,1);
                                        app.mpTransformManager->setTransform(camTm, filaTm);
                                        //app.mainCamera->setModelMatrix(filaTm);
                                    }
                                }
                            }
                        }
                        if(app.mpTransforms.find(nodeName) != app.mpTransforms.end()){
                            if(app.mpTransforms.find(parentNodeName) != app.mpTransforms.end())
                            {
                                app.mpTransformManager->setParent(app.mpTransforms[nodeName], app.mpTransforms[parentNodeName]);
                            }
                            if(parentNodeName.find_first_of(".") != std::string::npos){
                                std::string parentClone = parentNodeName.substr(0,parentNodeName.find_last_of("."));
                                std::string parentGltfName = parentNodeName.substr(parentNodeName.find_last_of(".")+1);
                                if(idGltfMap.find(parentNodeName) != idGltfMap.end())
                                    parentGltfName = idGltfMap[parentNodeName].substr( idGltfMap[parentNodeName].find_last_of(".")+1);
                                
                                if(app.mpInstancesMap.find(parentClone) != app.mpInstancesMap.end() && app.mpInstancesMap[parentClone].index > -1){
                                    int parentIndex = app.mpInstancesMap[parentClone].index;
                                    std::vector<utils::Entity> parentEntities;
                                    parentEntities.resize(10);
                                    int cnt2 = app.asset[app.mpInstancesMap[parentClone].assetName]->getEntitiesByName(parentGltfName.c_str(), parentEntities.data(), 10);
                                    if(cnt2 > 0 ){
                                        if(app.mpTransformManager->hasComponent(parentEntities[parentIndex])){
                                            auto parentTM = app.mpTransformManager->getInstance(parentEntities[parentIndex]);
                                            app.mpTransformManager->setParent(app.mpTransforms[nodeName], parentTM);
                                        }
                                    }
                                }
                                
                            }
                        }
                        if(nodeName.find_first_of(".") != std::string::npos){
                            std::string nodeClone = nodeName.substr(0,nodeName.find_last_of("."));
                            std::string nodeGltfName = nodeName.substr(nodeName.find_last_of(".")+1);
                            if(idGltfMap.find(parentNodeName) != idGltfMap.end())
                                nodeGltfName = idGltfMap[nodeName].substr( idGltfMap[nodeName].find_last_of(".")+1);
                            
                            if(app.mpInstancesMap.find(nodeClone) != app.mpInstancesMap.end() && app.mpInstancesMap[nodeClone].index > -1){
                                int nodeIndex = app.mpInstancesMap[nodeClone].index;
                                std::vector<utils::Entity> nodeEntities;
                                nodeEntities.resize(10);
                                int cnt2 = app.asset[app.mpInstancesMap[nodeClone].assetName]->getEntitiesByName(nodeGltfName.c_str(), nodeEntities.data(), 10);
                                if(cnt2 > 0 ){
                                    if(app.mpTransformManager->hasComponent(nodeEntities[nodeIndex])){
                                        auto nodeTM = app.mpTransformManager->getInstance(nodeEntities[nodeIndex]);
                                        if(app.mpTransforms.find(parentNodeName) != app.mpTransforms.end())
                                        {
                                            app.mpTransformManager->setParent(nodeTM, app.mpTransforms[parentNodeName]);
                                        }
                                        if(parentNodeName.find_first_of("." ) != std::string::npos){
                                            std::string parentClone = parentNodeName.substr(0,parentNodeName.find_last_of("."));
                                            std::string parentGltfName = parentNodeName.substr(parentNodeName.find_last_of(".")+1);
                                            if(idGltfMap.find(parentNodeName) != idGltfMap.end())
                                                parentGltfName = idGltfMap[parentNodeName].substr( idGltfMap[parentNodeName].find_last_of(".")+1);
                                            
                                            if(app.mpInstancesMap.find(parentClone) != app.mpInstancesMap.end() && app.mpInstancesMap[parentClone].index > -1){
                                                int parentIndex = app.mpInstancesMap[parentClone].index;
                                                std::vector<utils::Entity> parentEntities;
                                                parentEntities.resize(10);
                                                int cnt = app.asset[app.mpInstancesMap[parentClone].assetName]->getEntitiesByName(parentGltfName.c_str(), parentEntities.data(), 10);
                                                if(cnt > 0 ){
                                                    if(app.mpTransformManager->hasComponent(parentEntities[parentIndex])){
                                                        auto parentTM = app.mpTransformManager->getInstance(parentEntities[parentIndex]);
                                                        app.mpTransformManager->setParent(nodeTM, parentTM);
                                                    }
                                                }
                                            }
                                            
                                        }
                                    }
                                }
                            }
                        }
                           
//                        bool parentSet = false;
//                        if (parentNodeName.find_first_of(".") != std::string::npos)
//                            {
//
//                                std::string parentglftNodeName = parentNodeName;
//                                if(idGltfMap.find(parentNodeName) != idGltfMap.end())
//                                    parentglftNodeName = idGltfMap[parentNodeName];
//                                std::string cloneName = parentglftNodeName.substr(0,parentglftNodeName.find_last_of("."));
//                                std::string subNodeName = parentglftNodeName.substr(parentglftNodeName.find_last_of(".")+1);
//                                if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end() && app.mpInstancesMap[cloneName].index != -1){
//                                    int parentIndex = app.mpInstancesMap[cloneName].index;
//                                    std::vector<utils::Entity> parentEntities;
//                                    parentEntities.resize(10);
//                                    int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), parentEntities.data(), 10);
//                                    if(cnt > 0 ){
//                                        if(app.mpTransformManager->hasComponent(parentEntities[parentIndex])){
//                                            auto parentTM = app.mpTransformManager->getInstance(parentEntities[parentIndex]);
//
//                                            if(glftNodeName.find_first_of(".") != std::string::npos){
//                                                std::string nodeClone = glftNodeName.substr(0,glftNodeName.find_last_of("."));
//                                                std::string nodeGltfName = glftNodeName.substr(glftNodeName.find_last_of(".")+1);
//                                                if(app.mpInstancesMap.find(nodeClone) != app.mpInstancesMap.end() && app.mpInstancesMap[nodeClone].index > -1){
//                                                    int nodeIndex = app.mpInstancesMap[nodeClone].index;
//                                                    std::vector<utils::Entity> nodeEntities;
//                                                    nodeEntities.resize(10);
//                                                    int cnt2 = app.asset[app.mpInstancesMap[nodeClone].assetName]->getEntitiesByName(nodeGltfName.c_str(), nodeEntities.data(), 10);
//                                                    if(cnt2 > 0 ){
//                                                        if(app.mpTransformManager->hasComponent(nodeEntities[nodeIndex])){
//                                                            auto nodeTM = app.mpTransformManager->getInstance(nodeEntities[nodeIndex]);
//                                                            app.mpTransformManager->setParent(nodeTM, parentTM);
//                                                            parentSet = true;
//                                                        }
//                                                    }
//                                                }
//                                            }
//                                        }
//
//                                    }
//                                }
//                                //last resort
//                                if(!parentSet && (app.mpInstancesMap.find(cloneName) == app.mpInstancesMap.end() || app.mpInstancesMap[cloneName].index == -1)){
//                                    bool foundAsset = false;
//                                    auto assetItaretor = app.asset.begin();
//                                    while(!foundAsset && assetItaretor != app.asset.end()){
//                                        if(assetItaretor->second->getEntitiesByName(subNodeName.c_str(), nullptr, 10) + assetItaretor->second->getEntitiesByName(parentNodeName.c_str(), nullptr, 10)){
//                                            foundAsset = true;
//                                        }
//                                        else{
//                                            assetItaretor++;
//                                        }
//                                    }
//                                    if(foundAsset && app.instanceCount[assetItaretor->first] < 10){
//                                        int cnt = app.instanceCount[assetItaretor->first]++;
//                                        app.mpInstancesMap[cloneName] = InstanceData(cnt, assetItaretor->first, app.instances[assetItaretor->first][cnt]);
//                                        auto root = app.instances[assetItaretor->first][cnt]->getRoot();
//                                        app.names->addComponent(root);
//                                        auto nameInstance = app.names->getInstance(root);
//                                        if(nameInstance)
//                                          app.names->setName(nameInstance, cloneName.c_str());
//                                    }
//                                    else{
//
//                                        if(app.mpTransforms.find(parentNodeName) != app.mpTransforms.end()){
//                                            app.mpTransformManager->setParent(app.mpTransforms[nodeName], app.mpTransforms[parentNodeName]);
//                                            parentSet = true;
//                                        }
//                                    }
//                                }
//                                if(!parentSet && app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end() && app.mpInstancesMap[cloneName].index > -1){
//                                    int entitiyIndex = app.mpInstancesMap[cloneName].index;
//
//                                    int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), entities.data(), app.instanceCount[app.mpInstancesMap[cloneName].assetName]);
//                                    if(cnt > 0 ){
//                                        auto rinstance = app.mpRenderableManager->getInstance(entities[entitiyIndex]);
//                                        if(app.mpTransformManager->hasComponent(entities[entitiyIndex])){
//                                            auto entityTransform = app.mpTransformManager->getInstance(entities[entitiyIndex]);
//                                            app.mpTransformManager->setParent(app.mpTransforms[nodeName], entityTransform);
//                                            parentSet = true;
//                                        }
//
//                                    }
//                                    else if(app.mpTransforms.find(parentNodeName) != app.mpTransforms.end()){
//                                        app.mpTransformManager->setParent(app.mpTransforms[nodeName], app.mpTransforms[parentNodeName]);
//                                        parentSet = true;
//                                    }
//
//                                }
//                            }
//                        if(!parentSet && app.mpTransforms.find(parentNodeName) != app.mpTransforms.end()){
//
//                            if(glftNodeName.find_first_of(".") != std::string::npos){
//                                std::string nodeClone = glftNodeName.substr(0,glftNodeName.find_last_of("."));
//                                std::string nodeGltfName = glftNodeName.substr(glftNodeName.find_last_of(".")+1);
//                                if(app.mpInstancesMap.find(nodeClone) != app.mpInstancesMap.end() && app.mpInstancesMap[nodeClone].index > -1){
//                                    int nodeIndex = app.mpInstancesMap[nodeClone].index;
//                                    std::vector<utils::Entity> nodeEntities;
//                                    nodeEntities.resize(10);
//                                    int cnt2 = app.asset[app.mpInstancesMap[nodeClone].assetName]->getEntitiesByName(nodeGltfName.c_str(), nodeEntities.data(), 10);
//                                    if(cnt2 > 0 ){
//                                        if(app.mpTransformManager->hasComponent(nodeEntities[nodeIndex])){
//                                            auto nodeTM = app.mpTransformManager->getInstance(nodeEntities[nodeIndex]);
//                                            app.mpTransformManager->setParent(nodeTM, app.mpTransforms[parentNodeName]);
//                                            parentSet = true;
//                                        }
//                                    }
//                                }
//                            }
//                            if(!parentSet)
//                                app.mpTransformManager->setParent(app.mpTransforms[nodeName], app.mpTransforms[parentNodeName]);
//                        }
                        
					}
						break;
					case ape::Event::Type::NODE_DETACH:
					{
                        app.mpTransformManager->setParent(app.mpTransforms[nodeName], NULL);
                        std::size_t pos = nodeName.find("_vlftStudent");
                        if(pos != std::string::npos){
                            node->setOwner(node->getCreator());
                                if(app.mpTransforms.find(nodeName) != app.mpTransforms.end()){
                                    auto camEntity = app.mainCamera->getEntity();
                                    auto camTm = app.mpTransformManager->getInstance(camEntity);
                                    app.mpTransformManager->setParent(camTm, NULL);
                                    auto filaTm = filament::math::mat4f(1,0,0,0,
                                                          0,1,0,0,
                                                          0,0,1,0,
                                                          0,0,0,1);
                                    app.mpTransformManager->setTransform(camTm, filaTm);
                                }
                        }
					}
						break;
					case ape::Event::Type::NODE_POSITION:
					{
                        std::size_t pos = nodeName.find("_vlftStudent");
                        std::size_t pos2 = nodeName.find("_vlftTeacher");
                        auto nodePosition = node->getPosition();
                        auto nodeWorldPos = node->getDerivedPosition();
                        auto nodeScale = node->getScale();
                        auto nodeTransforms = app.mpTransformManager->getTransform(app.mpTransforms[nodeName]);
                        float divider = 1.0;
                        filament::math::mat4f filamentTransform;
                        
                        if((pos != std::string::npos || pos2 != std::string::npos) && nodeName.find(mUserName+mPostUserName) == std::string::npos){
                                filamentTransform = filament::math::mat4f(
                                    nodeTransforms[0][0], nodeTransforms[0][1], nodeTransforms[0][2], nodeTransforms[0][3],
                                    nodeTransforms[1][0], nodeTransforms[1][1], nodeTransforms[1][2], nodeTransforms[1][3],
                                    nodeTransforms[2][0], nodeTransforms[2][1], nodeTransforms[2][2], nodeTransforms[2][3],
                                    nodePosition.getX()/divider, nodePosition.getY()/divider, nodePosition.getZ()/divider, nodeTransforms[3][3]);
                                app.mpTransformManager->setTransform(app.mpTransforms[nodeName], filamentTransform);

                               
                        }
                        else if(app.mpTransforms.find(nodeName) != app.mpTransforms.end()){
                            filamentTransform = filament::math::mat4f(
                                nodeTransforms[0][0], nodeTransforms[0][1], nodeTransforms[0][2], nodeTransforms[0][3],
                                nodeTransforms[1][0], nodeTransforms[1][1], nodeTransforms[1][2], nodeTransforms[1][3],
                                nodeTransforms[2][0], nodeTransforms[2][1], nodeTransforms[2][2], nodeTransforms[2][3],
                                nodePosition.getX()/divider, nodePosition.getY()/divider, nodePosition.getZ()/divider, nodeTransforms[3][3]);
                            app.mpTransformManager->setTransform(app.mpTransforms[nodeName], filamentTransform);
                            if (nodeName.find_first_of(".") != std::string::npos)
                            {
                                std::string cloneName = glftNodeName.substr(0,glftNodeName.find_last_of("."));
                                std::string subNodeName = glftNodeName.substr(glftNodeName.find_last_of(".")+1);
                                if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end() &&app.mpInstancesMap[cloneName].index > -1){
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
                        for (auto studentWP : mStudents)
                                {
                                    if (auto student = studentWP.lock())
                                    {
                                        if (event.subjectName == student->getName())
                                        {
                                            std::chrono::milliseconds timeStamp = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
                                            std::stringstream data;
                                            std::size_t pos = event.subjectName.find("_vlftStudent");
                                            std::string studentName = event.subjectName;
                                            if(pos != std::string::npos){
                                                studentName = event.subjectName.substr(0,pos);
                                            }
                                            data << std::to_string(timeStamp.count()) << " name: " << studentName << " pos: " << student->getDerivedPosition().toString() << " ori: "  << student->getDerivedOrientation().toString() << std::endl;
                                            mStudentsMovementLoggingFile << data.str();
                                        }
                                    }
                                }
                        if (app.updateinfo.playerMapPositions.find(event.subjectName) != app.updateinfo.playerMapPositions.end() && nodeName.find(mUserName + mPostUserName) == std::string::npos) {
                            auto cam = app.mainCamera->getPosition();
                            //auto camWorld = app.mainCamera->getModelMatrix();
                            auto nodeWorldTransforms = app.mpTransformManager->getWorldTransform(app.mpTransforms[nodeName]);
                            app.updateinfo.playerMapPositions[nodeName][0] = nodeWorldTransforms[3][0] - cam.x;
                            app.updateinfo.playerMapPositions[nodeName][1] = cam.z - nodeWorldTransforms[3][2];
                            /*if (abs(nodeWorldTransforms[3][0] - cam.x) < 25 && abs(cam.z - nodeWorldTransforms[3][2]) < 25) {
                                if (!app.mpScene->hasEntity(app.worldMap.playerTriangles[nodeName]) && app.updateinfo.isMapVisible)
                                    app.mpScene->addEntity(app.worldMap.playerTriangles[nodeName]);

                                auto playerTM = app.mpTransformManager->getInstance(app.worldMap.playerTriangles[nodeName]);
                                auto playerTransform = app.mpTransformManager->getTransform(playerTM);
                                playerTransform[3][0] = (nodeWorldTransforms[3][0] - cam.x)/2000;
                                playerTransform[3][1] = (cam.z - nodeWorldTransforms[3][2])/2000;
                                double yaw = 0.0;
                                if (nodeWorldTransforms[0][0] == 1.0f || nodeWorldTransforms[0][0] == -1.0f)
                                {
                                    yaw = atan2f(nodeWorldTransforms[0][2], nodeWorldTransforms[2][3]);
                                }
                                else
                                {
                                    yaw = atan2(-nodeWorldTransforms[2][0], nodeWorldTransforms[0][0]);
                                }
                                double camYaw = 0.0;
                                if (camWorld[0][0] == 1.0f || camWorld[0][0] == -1.0f)
                                {
                                    camYaw = atan2f(camWorld[0][2], camWorld[2][3]);
                                }
                                else
                                {
                                    camYaw = atan2(-camWorld[2][0], camWorld[0][0]);
                                }
                                auto rot = math::mat4f::eulerYXZ(0, 0, -yaw);
                                auto newTransform = filament::math::mat4f(1, 0, 0, 0,
                                                                            0, 1, 0, 0,
                                                                            0, 0, 1, 0,
                                    playerTransform[3][0], playerTransform[3][1], playerTransform[3][2], 1);
                                app.mpTransformManager->setTransform(playerTM, newTransform*rot);

                            }
                            else {
                                if (app.mpScene->hasEntity(app.worldMap.playerTriangles[nodeName]))
                                    app.mpScene->remove(app.worldMap.playerTriangles[nodeName]);
                                app.updateinfo.playerMapPositions.erase(nodeName);
                            }*/
                            float tnear = app.mainCamera->getNear()/2;
                            float fov = app.mainCamera->getFieldOfViewInDegrees(filament::Camera::Fov::VERTICAL);
                            fov = fov * F_PI / 180.0;
                            
                            auto viewport = app.view->getViewport();
                            auto width = viewport.width;
                            auto height = viewport.height;
                            
                            // Compute the tangent of the field-of-view angle as well as the aspect ratio.
                            float tangent = tan(fov / 2.0);
                            float aspectRatio = (float)width / height;
                            //auto &nodeWorldTransforms = app.mpTransformManager->getWorldTransform(app.mpTransforms[nodeName]);
                            auto camPos = app.mainCamera->getModelMatrix();
                            
                            auto worldToLocalTM = inverse(camPos)*nodeWorldTransforms;
                            auto xPrime = worldToLocalTM[3][0];
                            auto yPrime = worldToLocalTM[3][1];
                            auto zPrime = -worldToLocalTM[3][2];

                            auto m = tangent * tnear;
                            auto mPrime = tangent * (tnear + zPrime);
                            auto w = tangent * tnear*aspectRatio;
                            auto wPrime = tangent * (tnear + zPrime) * aspectRatio;

                            auto asd = m / w;
                            auto x = xPrime * w / wPrime;
                            auto wScale = width / w / 2;
                            x = x * wScale + width / 2;

                            auto y = yPrime * m / mPrime;
                            auto hScale = height / m / 2;
                            y = -y * hScale + height / 2;

                            //APE_LOG_DEBUG(zPrime << " zPrime");
                            if (zPrime > 21 || zPrime < 0.65 || x < 0 || y < 0 || x > width || y > height) {
                                if (app.updateinfo.playerNamePositions.find(nodeName.substr(0, nodeName.find("_vlft"))) != app.updateinfo.playerNamePositions.end())
                                    app.updateinfo.playerNamePositions.erase(nodeName.substr(0, nodeName.find("_vlft")));
                            }
                            else {
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][0] = x;
                                float yOffset = 10;
#ifdef __APPLE__
                                yOffset = 35;
#endif
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][1] = y - yOffset - 200 / zPrime;
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][2] = 1.1 + (5 - zPrime) / 5;
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][3] = width;
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][4] = height;

                            }

                        }
                        else if (nodeName.find(mUserName + mPostUserName) != std::string::npos) {
                            auto nodeWorldTransforms = app.mpTransformManager->getWorldTransform(app.mpTransforms[nodeName]);
                            //auto playerTM = app.mpTransformManager->getInstance(app.worldMap.playerTriangles[nodeName]);
                            //auto playerTransform = app.mpTransformManager->getTransform(playerTM);
                            double yaw = 0.0;
                            if (nodeWorldTransforms[0][0] == 1.0f || nodeWorldTransforms[0][0] == -1.0f)
                            {
                                yaw = atan2f(nodeWorldTransforms[0][2], nodeWorldTransforms[2][3]);
                            }
                            else
                            {
                                yaw = atan2(-nodeWorldTransforms[2][0], nodeWorldTransforms[0][0]);
                            }
                            app.updateinfo.playerRotation = yaw;
                            //double camYaw = 0.0;
                            //auto rot = math::mat4f::eulerYXZ(0, 0, -yaw);
                            //auto newTransform = filament::math::mat4f(1, 0, 0, 0,
                            //    0, 1, 0, 0,
                            //    0, 0, 1, 0,
                            //    0, 0, playerTransform[3][2], 1);
                            //app.mpTransformManager->setTransform(playerTM, rot * newTransform);
                        }
					}
						break;
					case ape::Event::Type::NODE_ORIENTATION:
					{
                        if(auto node = mpSceneManager->getNode(mUserName+mPostUserName).lock()){
                            if(node->getOwner() != mpCoreConfig->getNetworkGUID()){
                                std::size_t pos = nodeName.find("_vlftTeacher");
                                if (pos != std::string::npos)
                                {
                                    auto pos = node->getDerivedPosition();
                                    auto ori = node->getDerivedOrientation();
                                }
                            }
                        }
                        
                        auto nodeOrientation= node->getModelMatrix().transpose();
                        auto nodeTransforms = app.mpTransformManager->getTransform(app.mpTransforms[nodeName]);
                        math::mat4f transform;
                        auto filamentTransform = math::mat4f(
                            nodeOrientation[0][0], nodeOrientation[0][1], nodeOrientation[0][2], nodeTransforms[0][3],
                            nodeOrientation[1][0], nodeOrientation[1][1], nodeOrientation[1][2], nodeTransforms[1][3],
                            nodeOrientation[2][0], nodeOrientation[2][1], nodeOrientation[2][2], nodeTransforms[2][3],
                            nodeTransforms[3][0], nodeTransforms[3][1], nodeTransforms[3][2], nodeTransforms[3][3]);
                        app.mpTransformManager->setTransform(app.mpTransforms[nodeName], filamentTransform);
                        if (nodeName.find_first_of(".") != std::string::npos)
                        {
                            std::string cloneName = glftNodeName.substr(0,glftNodeName.find_last_of("."));
                            std::string subNodeName = glftNodeName.substr(glftNodeName.find_last_of(".")+1);
                            if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end() &&app.mpInstancesMap[cloneName].index > -1){
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
                        if (app.updateinfo.playerMapPositions.find(event.subjectName) != app.updateinfo.playerMapPositions.end() && nodeName.find(mUserName + mPostUserName) == std::string::npos) {
                            auto cam = app.mainCamera->getPosition();
                            //auto camWorld = app.mainCamera->getModelMatrix();
                            auto nodeWorldTransforms = app.mpTransformManager->getWorldTransform(app.mpTransforms[nodeName]);
                           /* if (abs(nodeWorldTransforms[3][0] - cam.x) < 25 && abs(cam.z - nodeWorldTransforms[3][2]) < 25) {
                                if (!app.mpScene->hasEntity(app.worldMap.playerTriangles[nodeName]))
                                    app.mpScene->addEntity(app.worldMap.playerTriangles[nodeName]);
                                auto playerTM = app.mpTransformManager->getInstance(app.worldMap.playerTriangles[nodeName]);
                                auto playerTransform = app.mpTransformManager->getTransform(playerTM);
                                playerTransform[3][0] = (nodeWorldTransforms[3][0] - cam.x) / 2000;
                                playerTransform[3][1] = (cam.z - nodeWorldTransforms[3][2]) / 2000;
                                double yaw = 0.0;
                                if (nodeWorldTransforms[0][0] == 1.0f || nodeWorldTransforms[0][0] == -1.0f)
                                {
                                    yaw = atan2f(nodeWorldTransforms[0][2], nodeWorldTransforms[2][3]);
                                }
                                else
                                {
                                    yaw = atan2(-nodeWorldTransforms[2][0], nodeWorldTransforms[0][0]);
                                }
                                double camYaw = 0.0;
                                if (camWorld[0][0] == 1.0f || camWorld[0][0] == -1.0f)
                                {
                                    camYaw = atan2f(camWorld[0][2], camWorld[2][3]);
                                }
                                else
                                {
                                    camYaw = atan2(-camWorld[2][0], camWorld[0][0]);
                                }
                                auto rot = math::mat4f::eulerYXZ(0, 0, -yaw);
                                auto newTransform = filament::math::mat4f(1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, 0,
                                    playerTransform[3][0], playerTransform[3][1], playerTransform[3][2], 1);
                                app.mpTransformManager->setTransform(playerTM, newTransform * rot);

                            }
                            else {
                                if (app.mpScene->hasEntity(app.worldMap.playerTriangles[nodeName]))
                                    app.mpScene->remove(app.worldMap.playerTriangles[nodeName]);
                            }*/
                            float tnear = app.mainCamera->getNear() / 2;
                            float fov = app.mainCamera->getFieldOfViewInDegrees(filament::Camera::Fov::VERTICAL);
                            fov = fov * F_PI / 180.0;

                            auto viewport = app.view->getViewport();
                            auto width = viewport.width;
                            auto height = viewport.height;

                            // Compute the tangent of the field-of-view angle as well as the aspect ratio.
                            float tangent = tan(fov / 2.0);
                            float aspectRatio = (float)width / height;
                            //auto &nodeWorldTransforms = app.mpTransformManager->getWorldTransform(app.mpTransforms[nodeName]);
                            auto camPos = app.mainCamera->getModelMatrix();

                            auto worldToLocalTM = inverse(camPos) * nodeWorldTransforms;
                            auto xPrime = worldToLocalTM[3][0];
                            auto yPrime = worldToLocalTM[3][1];
                            auto zPrime = -worldToLocalTM[3][2];

                            auto m = tangent * tnear;
                            auto mPrime = tangent * (tnear + zPrime);
                            auto w = tangent * tnear * aspectRatio;
                            auto wPrime = tangent * (tnear + zPrime) * aspectRatio;

                            auto asd = m / w;
                            auto x = xPrime * w / wPrime;
                            auto wScale = width / w / 2;
                            x = x * wScale + width / 2;

                            auto y = yPrime * m / mPrime;
                            auto hScale = height / m / 2;
                            y = -y * hScale + height / 2;

                            //APE_LOG_DEBUG(zPrime << " zPrime");
                            if (zPrime > 21 || zPrime < 0.65 || x < 0 || y < 0 || x > width || y > height) {
                                if (app.updateinfo.playerNamePositions.find(nodeName.substr(0, nodeName.find("_vlft"))) != app.updateinfo.playerNamePositions.end())
                                    app.updateinfo.playerNamePositions.erase(nodeName.substr(0, nodeName.find("_vlft")));
                            }
                            else {
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][0] = x;
                                float yOffset = 10;
#ifdef __APPLE__
                                yOffset = 35;
#endif
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][1] = y - yOffset - 200 / zPrime;
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][2] = 1.1 + (5 - zPrime) / 5;
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][3] = width;
                                app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][4] = height;

                            }

                        }
                        else if (nodeName.find(mUserName + mPostUserName) != std::string::npos) {
                            auto nodeWorldTransforms = app.mpTransformManager->getWorldTransform(app.mpTransforms[nodeName]);
                            //auto playerTM = app.mpTransformManager->getInstance(app.worldMap.playerTriangles[nodeName]);
                            //auto playerTransform = app.mpTransformManager->getTransform(playerTM);
                            double yaw = 0.0;
                            if (nodeWorldTransforms[0][0] == 1.0f || nodeWorldTransforms[0][0] == -1.0f)
                            {
                                yaw = atan2f(nodeWorldTransforms[0][2], nodeWorldTransforms[2][3]);
                            }
                            else
                            {
                                yaw = atan2(-nodeWorldTransforms[2][0], nodeWorldTransforms[0][0]);
                            }
                            app.updateinfo.playerRotation = yaw;
                        //    auto rot = math::mat4f::eulerYXZ(0, 0, -yaw);
                        //    auto newTransform = filament::math::mat4f(1, 0, 0, 0,
                        //        0, 1, 0, 0,
                        //        0, 0, 1, 0,
                        //        0, 0, playerTransform[3][2], 1);
                        //    app.mpTransformManager->setTransform(playerTM, rot * newTransform);
                        }
					}
						break;
					case ape::Event::Type::NODE_SCALE:
                        {
                            ;
                        }
						break;
					case ape::Event::Type::NODE_CHILDVISIBILITY:
                        {
                            if(node->getChildrenVisibility() && nodeName.find(mUserName) == std::string::npos){
                                if(app.mpInstancesMap.find(nodeName) != app.mpInstancesMap.end() && app.mpInstancesMap[nodeName].index > -1){
                                    auto instance = app.mpInstancesMap[nodeName].mpInstance;
                                    if(auto root = instance->getRoot())
                                        if(!app.mpScene->hasEntity(root)){
                                            app.mpScene->addEntities(instance->getEntities(), instance->getEntityCount());
                                        }
                                    
                                }
                                else if(app.mpTransforms.find(nodeName) != app.mpTransforms.end() && nodeName.find(mUserName) == std::string::npos){
                                    std::vector<utils::Entity> children;
                                    size_t cnt = app.mpTransformManager->getChildCount(app.mpTransforms[nodeName]);
                                    if(cnt > 0){
                                        children.resize(cnt);
                                        app.mpTransformManager->getChildren(app.mpTransforms[nodeName], children.data(), cnt);
                                        app.mpScene->addEntities(children.data(), cnt);
                                    }
                                }
                                if(app.spaghettiLines.find(nodeName) != app.spaghettiLines.end()){
                                    if(!app.mpScene->hasEntity(app.spaghettiLines[nodeName].lineEntity))
                                        app.mpScene->addEntity(app.spaghettiLines[nodeName].lineEntity);
                                }
                                
                                if (nodeName.find_first_of(".") != std::string::npos)
                                {
                                    std::string cloneName = glftNodeName.substr(0,glftNodeName.find_last_of("."));
                                    std::string subNodeName = glftNodeName.substr(glftNodeName.find_last_of(".")+1);
                                    if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end() &&app.mpInstancesMap[cloneName].index > -1){
                                        int entitiyIndex = app.mpInstancesMap[cloneName].index;
                                        std::vector<utils::Entity> entities;
                                        entities.resize(10);
                                        int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), entities.data(), 10);
                                        if(cnt > 0 ){
                                            std::function<void(utils::Entity)> addToScene;
                                            addToScene = [&](utils::Entity nodeEntity){
                                                auto entityTransform = app.mpTransformManager->getInstance(nodeEntity);
                                                std::vector<utils::Entity> children(app.mpTransformManager->getChildCount(entityTransform));
                                                app.mpTransformManager->getChildren(entityTransform, children.data(), children.size());
                                                for (auto ce : children) {
                                                    if(!app.mpScene->hasEntity(entities[entitiyIndex])){
                                                        app.mpScene->addEntity(ce);
                                                    }
                                                    addToScene(ce);
                                                }
                                            };
                                        }
                                    }
                                }
                            }
                            else{
                                if(app.mpInstancesMap.find(nodeName) != app.mpInstancesMap.end()  && app.mpInstancesMap[nodeName].index > -1){
                                    auto instance = app.mpInstancesMap[nodeName].mpInstance;
                                    if(app.mpScene->hasEntity(instance->getEntities()[0])){
                                        app.mpScene->removeEntities(instance->getEntities(), instance->getEntityCount());
                                    }
                                    
                                }
                                else if(app.mpTransforms.find(nodeName) != app.mpTransforms.end()){
                                    std::vector<utils::Entity> children;
                                    size_t cnt = app.mpTransformManager->getChildCount(app.mpTransforms[nodeName]);
                                    if(cnt > 0){
                                        children.resize(cnt);
                                        app.mpTransformManager->getChildren(app.mpTransforms[nodeName], children.data(), cnt);
                                        app.mpScene->removeEntities(children.data(), cnt);
                                    }
                                }
                                if(app.spaghettiLines.find(nodeName) != app.spaghettiLines.end()){
                                    if(app.mpScene->hasEntity(app.spaghettiLines[nodeName].lineEntity))
                                        app.mpScene->remove(app.spaghettiLines[nodeName].lineEntity);
                                }
                                if (nodeName.find_first_of(".") != std::string::npos)
                                {
                                    std::string cloneName = glftNodeName.substr(0,glftNodeName.find_last_of("."));
                                    std::string subNodeName = glftNodeName.substr(glftNodeName.find_last_of(".")+1);
                                    if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end() &&app.mpInstancesMap[cloneName].index > -1){
                                        int entitiyIndex = app.mpInstancesMap[cloneName].index;
                                        std::vector<utils::Entity> entities;
                                        entities.resize(10);
                                        int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), entities.data(), 10);
                                        if(cnt > 0 )
                                        {
                                            std::function<void(utils::Entity)> removeFromScene;
                                            removeFromScene = [&](utils::Entity nodeEntity){
                                                auto entityTransform = app.mpTransformManager->getInstance(nodeEntity);
                                                std::vector<utils::Entity> children(app.mpTransformManager->getChildCount(entityTransform));
                                                app.mpTransformManager->getChildren(entityTransform, children.data(), children.size());
                                                for (auto ce : children) {
                                                    if(app.mpScene->hasEntity(ce)){
                                                        app.mpScene->remove(ce);
                                                    }
                                                    removeFromScene(ce);
                                                }
                                            };
                                            removeFromScene(entities[entitiyIndex]);
                                        }
                                    }
                                }
                            }
                        }
						break;
					case ape::Event::Type::NODE_VISIBILITY:
                        {
                            if(node->isVisible()){
                                if(app.mpInstancesMap.find(nodeName) != app.mpInstancesMap.end() && app.mpInstancesMap[nodeName].index > -1){
                                    auto instance = app.mpInstancesMap[nodeName].mpInstance;
                                    auto root = instance->getRoot();
                                    if(!app.mpScene->hasEntity(root)){
                                        app.mpScene->addEntities(instance->getEntities(), instance->getEntityCount());
                                    }
                                    
                                }
                                if(app.mpTransforms.find(nodeName) != app.mpTransforms.end()){
                                    //APE_LOG_DEBUG("FoundTM: " << nodeName);
                                    if(app.mpEntities.find(nodeName) != app.mpEntities.end() && !app.mpScene->hasEntity(app.mpEntities[nodeName]))
                                    {
                                        //APE_LOG_DEBUG("FoundEnt: " << nodeName);
                                        app.mpScene->addEntity(app.mpEntities[nodeName]);
                                        if(app.mpTransformManager->hasComponent(app.mpEntities[nodeName])){
                                            //APE_LOG_DEBUG("HasTM: " << nodeName);
                                            std::vector<utils::Entity> children;
                                            size_t cnt = app.mpTransformManager->getChildCount(app.mpTransforms[nodeName]);
                                            if(cnt > 0){
                                                app.mpTransformManager->getChildren(app.mpTransforms[nodeName], children.data(), cnt);
                                                if(!app.mpScene->hasEntity(children[0]))
                                                    app.mpScene->addEntities(children.data(), cnt);
                                            }
                                        }
                                    }
                                   
                                }
                                if (nodeName.find_first_of(".") != std::string::npos)
                                {
                                    std::string cloneName = nodeName.substr(0,nodeName.find_last_of("."));
                                    std::string subNodeName = nodeName.substr(nodeName.find_last_of(".")+1);
                                    if(idGltfMap.find(nodeName) != idGltfMap.end())
                                        subNodeName = idGltfMap[nodeName].substr( idGltfMap[nodeName].find_last_of(".")+1);
                                    //APE_LOG_DEBUG("Search: " << cloneName << " " << subNodeName);
                                    if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end() &&app.mpInstancesMap[cloneName].index > -1){
                                        int entitiyIndex = app.mpInstancesMap[cloneName].index;
                                        std::vector<utils::Entity> entities;
                                        entities.resize(10);
                                        int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), entities.data(), 10);
                                        //APE_LOG_DEBUG("Get entitites: " << cloneName << " " << subNodeName << " " << cnt);
                                        if(cnt > 0 )
                                        {
                                            if(!app.mpScene->hasEntity(entities[entitiyIndex]))
                                                app.mpScene->addEntity(entities[entitiyIndex]);
                                        }
                                    }
                                }
                                if (app.spaghettiLines.find(nodeName) != app.spaghettiLines.end()) {
                                    if (!app.mpScene->hasEntity(app.spaghettiLines[nodeName].lineEntity))
                                        app.mpScene->addEntity(app.spaghettiLines[nodeName].lineEntity);
                                }
                                
                            }
                            else{
                                if(app.mpInstancesMap.find(nodeName) != app.mpInstancesMap.end() && app.mpInstancesMap[nodeName].index > -1){
                                    auto instance = app.mpInstancesMap[nodeName].mpInstance;
                                    if(app.mpScene->hasEntity(instance->getEntities()[0])){
                                        app.mpScene->removeEntities(instance->getEntities(), instance->getEntityCount());
                                    }
                                    if (nodeName.find_first_of(".") != std::string::npos)
                                    {
                                        std::string cloneName = glftNodeName.substr(0,glftNodeName.find_last_of("."));
                                        std::string subNodeName = glftNodeName.substr(glftNodeName.find_last_of(".")+1);
                                        if(app.mpInstancesMap.find(cloneName) != app.mpInstancesMap.end() &&app.mpInstancesMap[cloneName].index > -1){
                                            int entitiyIndex = app.mpInstancesMap[cloneName].index;
                                            std::vector<utils::Entity> entities;
                                            entities.resize(10);
                                            int cnt = app.asset[app.mpInstancesMap[cloneName].assetName]->getEntitiesByName(subNodeName.c_str(), entities.data(), 10);
                                            if(cnt > 0 )
                                            {
                                                if(app.mpScene->hasEntity(entities[entitiyIndex]))
                                                    app.mpScene->remove(entities[entitiyIndex]);
                                            }
                                        }
                                    }
                                    if (app.spaghettiLines.find(nodeName) != app.spaghettiLines.end()) {
                                        if (app.mpScene->hasEntity(app.spaghettiLines[nodeName].lineEntity))
                                            app.mpScene->remove(app.spaghettiLines[nodeName].lineEntity);
                                    }
                                    
                                }
                            }
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
                if (app.mpInstancesMap.find(event.subjectName) != app.mpInstancesMap.end() && app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount() >= 0) {
                    auto cloneRoot = app.mpInstancesMap[event.subjectName].mpInstance->getRoot();
                    if (app.mpScene->hasEntity(cloneRoot)) {
                        app.names->removeComponent(cloneRoot);
                        app.mpScene->removeEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());
                    }
                    else {
                        int cnt = app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount();
                        if (cnt > 0 && app.mpScene->hasEntity(app.mpInstancesMap[event.subjectName].mpInstance->getEntities()[0])) {
                            app.mpScene->removeEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());
                        }
                    }
                }
                if (app.mpInstancesMap.find(event.subjectName) != app.mpInstancesMap.end()) {
                    app.instanceCount[app.mpInstancesMap[event.subjectName].assetName]--;
                    app.mpInstancesMap.erase(event.subjectName);
                }
                if (app.updateinfo.playerNamePositions.find(event.subjectName.substr(0, event.subjectName.find("_vlft"))) != app.updateinfo.playerNamePositions.end()) {
                    app.updateinfo.playerNamePositions.erase(event.subjectName.substr(0, event.subjectName.find("_vlft")));
                    app.updateinfo.newChatMessage = true;
                }

                app.playerNamesToShow.erase(event.subjectName);

                if(app.mpEntities.find(event.subjectName) != app.mpEntities.end()){
                    if(app.mpScene->hasEntity(app.mpEntities[event.subjectName])){
                        app.mpScene->remove(app.mpEntities[event.subjectName]);
                    }
                    if(app.mpTransforms.find(event.subjectName) != app.mpTransforms.end()){
                        app.mpTransformManager->destroy(app.mpEntities[event.subjectName]);
                        app.mpTransforms.erase(event.subjectName);
                    }
                    app.engine->destroy(app.mpEntities[event.subjectName]);

                }
                if(event.subjectName.find("_vlft") != std::string::npos && event.subjectName.find("_TextNode") == std::string::npos && event.subjectName.find("_StateNode") == std::string::npos)
                    app.updateinfo.newMessage.push_back(event.subjectName.substr(0, event.subjectName.find("_vlft")) + " left the room");
                app.playerAnimations.erase(event.subjectName);
                app.updateinfo.playerMapPositions.erase(event.subjectName);
                APE_LOG_DEBUG("Node deleted: " << event.subjectName);
               
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
                            APE_LOG_DEBUG("The asset connected to the parent node has not been loaded yet: " << fileName);
                    }
                    else{
                        auto filamentAssetRootEntity = app.mpLoadedAssets[fileName]->getRoot();
                        auto filamentAssetRootTransform = app.mpTransformManager->getInstance(filamentAssetRootEntity);
                        app.mpTransformManager->setParent(filamentAssetRootTransform, app.mpTransforms[parentNodeName]);
                    }
				}
					break;
                case ape::Event::Type::GEOMETRY_FILE_PLAYANIMATION:
                {
                    if (parentNodeName != mUserName + mPostUserName) {
                        app.playerAnimations[parentNodeName];
                        auto runningAnimation = std::stoi(geometryFile->getRunningAnimation());
                        if (runningAnimation == 3) {
                            app.playerAnimations[parentNodeName].mouseDown = true;
                            app.playerAnimations[parentNodeName].animatedClick = false;
                            app.playerAnimations[parentNodeName].mouseStartTime = app.currentTime;
                            app.playerAnimations[parentNodeName].keysDown = false;
                        }
                        else if (runningAnimation == 0 || runningAnimation == 1) {
                            if (app.playerAnimations[parentNodeName].animatedKey) {
                                app.playerAnimations[parentNodeName].keyStartTime = app.currentTime;
                                app.playerAnimations[parentNodeName].animatedKey = false;
                                app.playerAnimations[parentNodeName].keysDown = true;
                                app.playerAnimations[parentNodeName].keyCurrentAnimation = runningAnimation;
                                app.playerAnimations[parentNodeName].mouseDown = false;
                            }
                        }

                    }
                }
                break;
                case ape::Event::Type::GEOMETRY_FILE_STOPANIMATION:
                {
                    if (parentNodeName != mUserName + mPostUserName) {
                        auto runningAnimation = std::stoi(geometryFile->getStoppedAnimation());
                        if (runningAnimation == 3) {
                            app.playerAnimations[parentNodeName].mouseDown = false;
                        }
                        else if (runningAnimation == 0 || runningAnimation == 1) {
                            app.playerAnimations[parentNodeName].keysDown = false;
                            app.playerAnimations[parentNodeName].keyCurrentAnimation = runningAnimation;
                        }
                        else {
                            app.playerAnimations[parentNodeName].mouseDown = false;
                            app.playerAnimations[parentNodeName].keysDown = false;
                        }
                    }
                    
                }
                break;
				case ape::Event::Type::GEOMETRY_FILE_FILENAME:
				{
                    //if(fileName.find("Warehouse") == std::string::npos)
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
								std::string separator = "./";
								found = fileName.find(separator);
								if (found != std::string::npos)
								{
//                                    auto found_it = std::find_end(fileName.begin(), fileName.end(), separator.begin(), separator.end());
//                                    size_t foundPos = found_it - fileName.begin();
//                                    std::stringstream resourceLocationPath;
//                                    resourceLocationPath << APE_SOURCE_DIR << fileName.substr(foundPos + 2);
//                                    filePath << resourceLocationPath.str();
                                      filePath << fileName;
								}
								else
								{
                                   
                                    std::string absolutePath = "c:/";
                                    #ifdef __APPLE__
                                    absolutePath = "/Users";
                                    #endif
                                    found = fileName.find(absolutePath);
                                    if(found != std::string::npos)
                                    {
                                        filePath << fileName;
                                    }
                                    else{
                                        std::stringstream resourceLocationPath;
                                        resourceLocationPath << "../../samples/virtualLearningFactory/" << fileName;
                                        filePath << resourceLocationPath.str();
                                    }
								}
							}
                            if(app.mpLoadedAssets.find(fileName) == app.mpLoadedAssets.end()){
                                std::ifstream in(filePath.str().c_str(), std::ifstream::ate | std::ifstream::binary);
                                long contentSize = static_cast<long>(in.tellg());
                                if (contentSize <= 0)
                                {
                                    APE_LOG_DEBUG("Unable to open " << filePath.str());
                                }
                                /*else
                                {
                                    APE_LOG_DEBUG(filePath.str() << " was opened");
                                }*/
                                std::ifstream inBin(filePath.str().c_str(), std::ifstream::binary | std::ifstream::in);
                                std::vector<uint8_t> buffer(static_cast<unsigned long>(contentSize));
                                if (!inBin.read((char*)buffer.data(), contentSize))
                                {
                                    APE_LOG_DEBUG("Unable to read " << filePath.str());
                                }
                               /* else
                                {
                                    APE_LOG_DEBUG(filePath.str() << " was read");
                                }*/
                                    if (app.asset.find(geometryName) != app.asset.end())
                                    {
                                        app.mpScene->removeEntities(app.asset[geometryName]->getEntities(), app.asset[geometryName]->getEntityCount());
                                    }
                                    if (fileName.find("Warehouse") == std::string::npos)
                                        app.instances[geometryName].resize(10);
                                    else
                                        app.instances[geometryName].resize(1);
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

                                        //APE_LOG_DEBUG(filePath.str() << " was parsed");
                                        
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
                                            //APE_LOG_DEBUG("resources load OK "<<fileName);
        //                                    auto nodeTransforms = app.mpTransformManager->getTransform(app.mpTransforms[geometryName]);
        //                                    auto filamentTransform = filament::math::mat4f(
        //                                       1/1000,0,0,0,
        //                                       0,1/1000,0,0,
        //                                       0,0,1/1000,0,
        //                                       0,0,0,1);
                                            app.mpLoadedAssets[fileName] = app.asset[geometryName];
                                            app.geometryNameMap[fileName].push_back( geometryName);
                                        }
                                        else
                                        {
                                            APE_LOG_DEBUG("resources load FAILED");
                                        }
                                    }
                            }else{
                                app.geometryNameMap[fileName].push_back(geometryName);
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
                //Destroy not needed
                if(app.resourceLoader)
                    delete app.resourceLoader;
                app.resourceLoader = nullptr;
                app.loader->destroyAsset(app.asset[event.subjectName]);
                app.asset.erase(event.subjectName);
			}
		}
        else if(event.group == ape::Event::Group::GEOMETRY_INDEXEDLINESET){
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
                                    ;
                                    
                                }
                                    break;
                                case ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARENTNODE:
                                {
                                    app.spaghettiLines[parentNodeName];
                                    app.names->addComponent(app.spaghettiLines[parentNodeName].lineEntity);
                                    auto nameInstance = app.names->getInstance(app.spaghettiLines[parentNodeName].lineEntity);
                                    if(nameInstance)
                                        app.names->setName(nameInstance, geometryName.c_str());
                                    app.mpTransformManager->create(app.spaghettiLines[parentNodeName].lineEntity);
                                    auto filamentTransform = app.mpTransformManager->getInstance(app.spaghettiLines[parentNodeName].lineEntity);
                                    app.mpTransforms[parentNodeName] = filamentTransform;
                                    app.spaghettiLines[parentNodeName].mat = filament::Material::Builder()
                                    .package(RESOURCES_BAKEDCOLOR_DATA, RESOURCES_BAKEDCOLOR_SIZE)
                                    .build(*app.engine);
                                    app.spaghettiLines[parentNodeName].lineName = geometryName;
                                    mspaghettiLineNames[event.subjectName] = parentNodeName;
//                                    if(app.spaghettiLines.find(parentNodeName) != app.spaghettiLines.end()){
//                                        if(app.mpTransforms.find(parentNodeName) != app.mpTransforms.end()){
//                                            auto lineTm = app.mpTransformManager->getInstance(app.spaghettiLines[geometryName].lineEntity);
//                                            app.mpTransformManager->setParent(lineTm, app.mpTransforms[parentNodeName]);
//                                        }
//                                    }
                                }
                                    break;
                                case ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARAMETERS:
                                {
                                    ape::GeometryIndexedLineSetParameters parameters = manual->getParameters();
                                    if (app.spaghettiLines.find(parentNodeName) != app.spaghettiLines.end())
                                    {
                                        
                                        app.engine->destroy(app.spaghettiLines[parentNodeName].lineVertexBuffer);
                                        app.engine->destroy(app.spaghettiLines[parentNodeName].lineIndexBuffer);
                                        auto col = parameters.getColor();
                                        int r = 255*col.getR();
                                        int g = 255*col.getG();
                                        int b = 255*col.getB();
                                        auto lineVert = LineVertex();
                                        for (int coordinateIndex = app.spaghettiLines[parentNodeName].size*3; coordinateIndex < parameters.coordinates.size(); coordinateIndex = coordinateIndex + 3)
                                        {
                                            uint32_t color = ((255 & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + ((b & 0xff));
                                            uint32_t baseColor = 0xff0000ffu;
                                            lineVert.pos =float3(parameters.coordinates[coordinateIndex],
                                                                 parameters.coordinates[coordinateIndex + 1],
                                                                 parameters.coordinates[coordinateIndex + 2]);
                                            lineVert.color = baseColor;
                                            app.spaghettiLines[parentNodeName].lineVertices.push_back(lineVert);
                                            app.spaghettiLines[parentNodeName].size++;
                                        }
                                        int indexIndex = app.spaghettiLines[parentNodeName].lineIndices.size();
                                        while (indexIndex < parameters.indices.size())
                                        {
                                            while (indexIndex < parameters.indices.size() && parameters.indices[indexIndex] != -1)
                                            {
                                                app.spaghettiLines[parentNodeName].lineIndices.push_back(parameters.indices[indexIndex]);
                                                indexIndex++;
                                            }
                                            indexIndex++;
                                        }
                                    
                                        app.spaghettiLines[parentNodeName].lineVertexBuffer = VertexBuffer::Builder()
                                        .vertexCount(app.spaghettiLines[parentNodeName].size)
                                        .bufferCount(1)
                                        .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3,0, 16)
                                        .attribute(VertexAttribute::COLOR, 0, VertexBuffer::AttributeType::UBYTE4, 12, 16)
                                        .normalized(VertexAttribute::COLOR)
                                        .build(*app.engine);
                                        
                                        app.spaghettiLines[parentNodeName].lineVertexBuffer->setBufferAt(*app.engine, 0, VertexBuffer::BufferDescriptor(app.spaghettiLines[parentNodeName].lineVertices.data(), app.spaghettiLines[parentNodeName].size*16,nullptr));
                                        
                                        
                                        app.spaghettiLines[parentNodeName].lineIndexBuffer = IndexBuffer::Builder()
                                        .indexCount(app.spaghettiLines[parentNodeName].lineIndices.size())
                                        .bufferType(IndexBuffer::IndexType::UINT)
                                        .build(*app.engine);
                                        
                                        app.spaghettiLines[parentNodeName].lineIndexBuffer->setBuffer(*app.engine, IndexBuffer::BufferDescriptor(app.spaghettiLines[parentNodeName].lineIndices.data(), app.spaghettiLines[parentNodeName].lineIndices.size()*sizeof(uint32_t),nullptr));
                                        
                                        RenderableManager::Builder(1)
                                                .material(0,  app.spaghettiLines[parentNodeName].mat->getDefaultInstance())
                                                .geometry(0, RenderableManager::PrimitiveType::LINES, app.spaghettiLines[parentNodeName].lineVertexBuffer,
                                                    app.spaghettiLines[parentNodeName].lineIndexBuffer)
                                                .culling(false)
                                                .receiveShadows(false)
                                                .castShadows(false)
                                                .build(*app.engine, app.spaghettiLines[parentNodeName].lineEntity);
                                        if (auto parentNode = manual->getParentNode().lock()){
                                            if(parentNode->getChildrenVisibility() || parentNode->isVisible())
                                                app.mpScene->addEntity(app.spaghettiLines[parentNodeName].lineEntity);
                                        }
                                    }
                                    
                                }
                                    break;
                            }
                        }
            else if(event.type == ape::Event::Type::GEOMETRY_INDEXEDLINESET_DELETE)
            {
                std::string lineNodeName = mspaghettiLineNames[event.subjectName];
                if(app.mpScene->hasEntity(app.spaghettiLines[lineNodeName].lineEntity)){
                    app.mpScene->remove(app.spaghettiLines[lineNodeName].lineEntity);
                }
                if(app.mpEntityManager->isAlive(app.spaghettiLines[lineNodeName].lineEntity)){
                    app.engine->destroy(app.spaghettiLines[lineNodeName].lineEntity);
                    app.mpEntityManager->destroy(app.spaghettiLines[lineNodeName].lineEntity);
                    if(app.mpTransformManager->hasComponent(app.spaghettiLines[lineNodeName].lineEntity))
                        app.mpTransformManager->destroy(app.spaghettiLines[lineNodeName].lineEntity);
                    app.engine->destroy(app.spaghettiLines[lineNodeName].lineVertexBuffer);
                    app.engine->destroy(app.spaghettiLines[lineNodeName].lineIndexBuffer);
                        
                }
                app.spaghettiLines.erase(lineNodeName);
            }
                        
        }
        else if(event.group == ape::Event::Group::GEOMETRY_CLONE)
        {
            if (auto cloneGeometry = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
            {
                std::string sourceFileName = "", sourceGeometryName = "";
                if(auto sourceGeometry = std::static_pointer_cast<ape::IFileGeometry>(cloneGeometry->getSourceGeometry().lock())){
                    sourceGeometryName = sourceGeometry->getName();
                    sourceFileName = sourceGeometry->getFileName();
                }
                std::string parentNodeName = "";
                if (auto parentNode = cloneGeometry->getParentNode().lock())
                    parentNodeName = parentNode->getName();
                switch (event.type)
                {
                    case ape::Event::Type::GEOMETRY_CLONE_CREATE:
                    {
                        app.mpInstancesMap[event.subjectName];
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_PARENTNODE:
                    {
                        if(app.mpInstancesMap.find(event.subjectName) != app.mpInstancesMap.end() && app.mpInstancesMap[event.subjectName].index > -1){
                            auto filamentAssetRootEntity = app.mpInstancesMap[event.subjectName].mpInstance->getRoot();
                            auto filamentAssetRootTransform = app.mpTransformManager->getInstance(filamentAssetRootEntity);
                            app.mpTransformManager->setParent(filamentAssetRootTransform, app.mpTransforms[parentNodeName]);
                            if(auto node = mpSceneManager->getNode(parentNodeName).lock()){
                                if(node->getChildrenVisibility() && parentNodeName.find(mUserName+mPostUserName) == std::string::npos){
                                    auto rcm = app.mpRenderableManager;
                                    auto entities = app.mpInstancesMap[event.subjectName].mpInstance->getEntities();
                                    if (sourceFileName.find("Warehouse") == std::string::npos)
                                    {
                                        for (int i = 0; i < app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount(); i++) {
                                            auto rm = rcm->getInstance(entities[i]);

                                            if (rcm->hasComponent(entities[i])) {
                                                rcm->setCastShadows(rm, true);
                                                rcm->setReceiveShadows(rm, true);
                                            }
                                        }
                                    }
                                    else {
                                        for (int i = 0; i < app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount(); i++) {
                                            auto rm = rcm->getInstance(entities[i]);
                                            if (rcm->hasComponent(entities[i])) {
                                                rcm->setCastShadows(rm, false);
                                                rcm->setReceiveShadows(rm, true);
                                            }
                                        }
                                        if (app.mpTransformManager->hasComponent(filamentAssetRootEntity)) {
                                            auto transformMatrix = app.mpTransformManager->getTransform(filamentAssetRootTransform);
                                                transformMatrix[3][1] -= 0.055;
                                                transformMatrix[3][2] -= 5.1;
                                                app.mpTransformManager->setTransform(filamentAssetRootTransform, transformMatrix);
                                        }
                                    }
                                    if(node->isVisible())
                                        app.mpScene->addEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());

                                }
                                else if(parentNodeName.find(mUserName+mPostUserName) != std::string::npos){
                                    app.mpScene->removeEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());
                                }
                            }
                        }
                        else{
                            
                            APE_LOG_ERROR("The clone does not exist yet "<<event.subjectName);
                        }
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRY:
                    {
                        bool exists  =false;
                        std::string nameOfGeometry = "";
                        std::string nameOfFile = "";
                        if(app.instances.find(sourceGeometryName) == app.instances.end()){
                            if(app.geometryNameMap.find(sourceFileName) != app.geometryNameMap.end()){
                                auto names = app.geometryNameMap[sourceFileName];
                                for(size_t i = 0; i < names.size(); i++){
                                    if(app.instances.find(names[i]) != app.instances.end()){
                                        exists = true;
                                        nameOfGeometry = names[i];
                                        break;
                                    }
                                }
                            }
                        }
                        if(!exists)
                            nameOfGeometry = sourceGeometryName;
                        if(app.instances.find(nameOfGeometry) != app.instances.end() && app.mpInstancesMap[event.subjectName].index == -1){
                            if(app.instanceCount[nameOfGeometry] < 10){
                                int cnt = app.instanceCount[nameOfGeometry]++;
                                app.mpInstancesMap[event.subjectName] =  InstanceData(cnt, nameOfGeometry, app.instances[nameOfGeometry][cnt]);
                                auto root = app.instances[nameOfGeometry][cnt]->getRoot();
                                app.names->addComponent(root);
                                auto nameInstance = app.names->getInstance(root);
                                if(nameInstance)
                                  app.names->setName(nameInstance, event.subjectName.c_str());
                                if(auto node = mpSceneManager->getNode(parentNodeName).lock()){
                                    if(node->isVisible() && parentNodeName.find(mUserName+mPostUserName) == std::string::npos){
                                        app.mpScene->addEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());
                                    }
                                    else if(parentNodeName.find(mUserName+mPostUserName) != std::string::npos){
                                        app.mpScene->removeEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());
                                    }
                                }
//                                if(nameOfGeometry == mUserName+"characterModel"){
//                                    auto cam = &app.view->getCamera();
//                                    auto camTM = app.mpTransformManager->getInstance(cam->getEntity());
//                                    std::string postName = "_vlftStudent";
//                                    if(app.updateinfo.isAdmin)
//                                        postName = "_vlftTeacher";
//                                    app.mpTransformManager->setParent(app.mpTransforms[mUserName+postName], camTM);
//                                }
                            }
                            else{
                                  FilamentInstance* instance = app.loader->createInstance(app.asset[nameOfGeometry]);
                                  auto root = instance->getRoot();
                                  app.names->addComponent(root);
                                  auto nameInstance = app.names->getInstance(root);
                                  if(nameInstance)
                                    app.names->setName(nameInstance, event.subjectName.c_str());

                            }
                            if (nameOfGeometry.find("characterModel") != std::string::npos) {
                                /*app.worldMap.playerTriangles[event.subjectName] = EntityManager::get().create();
                                RenderableManager::Builder(1)
                                    .boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
                                    .material(0, app.worldMap.mapMaterial->getDefaultInstance())
                                    .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, app.worldMap.playerVertexBuffer, app.worldMap.playerIndexBuffer, 0, 6)
                                    .culling(true)
                                    .receiveShadows(false)
                                    .castShadows(false)
                                    .build(*app.engine, app.worldMap.playerTriangles[event.subjectName]);
                                app.mpScene->addEntity(app.worldMap.playerTriangles[event.subjectName]);
                                auto mapTM = app.mpTransformManager->getInstance(app.worldMap.mapReferencePoint);
                                auto playerTM = app.mpTransformManager->getInstance(app.worldMap.playerTriangles[event.subjectName]);
                                app.mpTransformManager->setParent(playerTM, mapTM);*/
                               
                                if (event.subjectName.find(mUserName + mPostUserName) == std::string::npos)
                                {
                                    app.updateinfo.playerMapPositions[event.subjectName];
                                    app.playerNamesToShow.insert(event.subjectName);
                                }
                               /* auto playerTransform = app.mpTransformManager->getTransform(playerTM);
                                playerTransform[3][0] = 0.0125;
                                playerTransform[3][1] = -0.0125;
                                playerTransform[3][2] = 0.0001;
                                app.mpTransformManager->setTransform(playerTM, playerTransform);*/
                            }
                        }
                        else if(app.instances.find(nameOfGeometry) == app.instances.end()){
                            
                            APE_LOG_ERROR("The clone's sourcefile has not been loaded yet "<<event.subjectName);
                        }
                        else {
                            APE_LOG_DEBUG("A clone with the same name already exists, repoint it "<< event.subjectName);
                            auto cnt = app.mpInstancesMap[event.subjectName].index;
                            auto root = app.instances[nameOfGeometry][cnt]->getRoot();
                            app.names->addComponent(root);
                            auto nameInstance = app.names->getInstance(root);
                            if (nameInstance)
                                app.names->setName(nameInstance, event.subjectName.c_str());
                        }
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME:
                    {
                        APE_LOG_DEBUG("GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME "<<event.subjectName);
                        app.updateinfo.loadingRoom = false;
                    }
                    break;
                }
            }
            else if(event.type == ape::Event::Type::GEOMETRY_CLONE_DELETE)
            {
                
                if(app.mpInstancesMap.find(event.subjectName) != app.mpInstancesMap.end() && app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount() >= 0){
                    auto cloneRoot= app.mpInstancesMap[event.subjectName].mpInstance->getRoot();
                    if(app.mpScene->hasEntity(cloneRoot)){
                        app.names->removeComponent(cloneRoot);
                        app.mpScene->removeEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());
                    }else{
                        int cnt = app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount();
                        if(cnt >0 && app.mpScene->hasEntity(app.mpInstancesMap[event.subjectName].mpInstance->getEntities()[0])){
                            app.mpScene->removeEntities(app.mpInstancesMap[event.subjectName].mpInstance->getEntities(), app.mpInstancesMap[event.subjectName].mpInstance->getEntityCount());
                        }
                    }
                }
                if (app.mpInstancesMap.find(event.subjectName) != app.mpInstancesMap.end()) {
                    app.instanceCount[app.mpInstancesMap[event.subjectName].assetName]--;
                    app.mpInstancesMap.erase(event.subjectName);
                }
                if (app.updateinfo.playerNamePositions.find(event.subjectName.substr(0, event.subjectName.find("_vlft"))) != app.updateinfo.playerNamePositions.end()) {
                    app.updateinfo.playerNamePositions.erase(event.subjectName.substr(0, event.subjectName.find("_vlft")));
                    app.updateinfo.newMessage.push_back(event.subjectName.substr(0, event.subjectName.find("_vlft")) + " left the room");
                    app.updateinfo.newChatMessage = true;
                    app.updateinfo.playerMapPositions.erase(event.subjectName);
                }
               
                app.playerNamesToShow.erase(event.subjectName);
                app.playerAnimations.erase(event.subjectName);
             /*   if (app.mpScene->hasEntity(app.worldMap.playerTriangles[event.subjectName])) {
                    app.mpScene->remove(app.worldMap.playerTriangles[event.subjectName]);
                    app.worldMap.playerTriangles.erase(event.subjectName);
                    app.updateinfo.playerNamePositions.erase(event.subjectName);
                    app.updateinfo.playerMapPositions.erase(event.subjectName);
                }*/
                   
            }
        }
        else if (event.group == ape::Event::Group::GEOMETRY_TEXT){
            if (auto textGeometry = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->getEntity(event.subjectName).lock())){
                auto name = textGeometry->getName();
                    switch (event.type)
                    {
                            
                        case ape::Event::Type::GEOMETRY_TEXT_CREATE:{
                            ;
                        }
                        break;
                        case ape::Event::Type::GEOMETRY_TEXT_DELETE:{
                            ;
                        }
                        break;
                        case ape::Event::Type::GEOMETRY_TEXT_VISIBLE:{
                            ;
                        }
                        break;
                        case ape::Event::Type::GEOMETRY_TEXT_SHOWONTOP:{
                            ;
                        }
                        break;
                        case ape::Event::Type::GEOMETRY_TEXT_CAPTION:{
                            if(name != mUserName+mPostUserName+"_text" && name.find("_text") != std::string::npos){
                                auto newCaption = textGeometry->getCaption();
                                app.updateinfo.newMessage.push_back(newCaption);
                                app.updateinfo.newChatMessage = true;
                                if (newCaption.find(":") == std::string::npos && newCaption.find("left the room") != std::string::npos) {
                                    if (auto node = mpSceneManager->getNode(mUserName + mPostUserName).lock()) {
                                        if (name.find("_vlftTeacher") != std::string::npos) {
                                            node->detachFromParentNode();
                                            node->setPosition(ape::Vector3(0, 1.5, 0));
                                            node->setOrientation(ape::Quaternion(0, 0, 1, 0));
                                        }
                                    }
                                }
                            }
                            else if(name != mUserName + mPostUserName + "_StateText" && name.find("_StateText") != std::string::npos) {
                                //name != mUserName + mPostUserName + "_StateText" &&
                                std::vector<std::string> words;
                                size_t start;
                                size_t end = 0;
                                auto str = textGeometry->getCaption();
                                while ((start = str.find_first_not_of(" ", end)) != std::string::npos)
                                {
                                    end = str.find(" ", start);
                                    words.push_back(str.substr(start, end - start));
                                }
                                if (words.size() == 3) {
                                    app.updateinfo.stateOfObjects.push_back(words[0]);
                                    app.updateinfo.nameOfState.push_back(words[1]);
                                    app.updateinfo.timeOfState.push_back(std::stof(words[2]));
                                }
                            }
                        }
                        break;
                        case ape::Event::Type::GEOMETRY_TEXT_PARENTNODE:{
                            ;
                        }
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
        else if (event.group == ape::Event::Group::TEXTURE_MANUAL) {

            if (auto textureManual = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
            {
                std::string textureManualName = textureManual->getName();
                switch (event.type)
                {
                case ape::Event::Type::TEXTURE_MANUAL_CREATE:
                    break;
                case ape::Event::Type::TEXTURE_MANUAL_GRAPHICSAPIID:
                    break;
                case ape::Event::Type::TEXTURE_MANUAL_CONTEXTID:
                    break;
                case ape::Event::Type::TEXTURE_MANUAL_PARAMETERS:
                    ape::ManualTextureParameters parameters = textureManual->getParameters();
                    break;
                }
            }

        }
		}
		catch (std::exception exp)
		{
			//APE_LOG_DEBUG("");
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
    //Step();
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
    
    std::stringstream fileFullPath;
    fileFullPath << mpCoreConfig->getConfigFolderPath() << "/apeVLFTAnimationPlayerPlugin.json";
    FILE* apeVLFTAnimationPlayerPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
    mAnimations = nlohmann::json::parse(apeVLFTAnimationPlayerPluginConfigFile);
    animationQuicktype::Context* context = &mAnimations.get_mutable_context();
    if (context)
        app.updateinfo.spaghettiVisible = mAnimations.get_context().get_asset_trail();
    for (const auto& node : mAnimations.get_nodes())
    {
        mAnimatedNodeNames.insert(node.get_name());
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
                if (action.get_event().get_type() == animationQuicktype::EventType::ATTACH)
                {
                    Animation animation;
                    animation.type = action.get_event().get_type();
                    animation.nodeName = node.get_name();
                    animation.parentNodeName = "";
                    if (action.get_event().get_placement_rel_to())
                    {
                        animation.parentNodeName = *action.get_event().get_placement_rel_to();
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
                    if (action.get_event().get_descr())
                        animation.descr = *action.get_event().get_descr();
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
                        animation.url = *action.get_event().get_url();
                    if (action.get_event().get_descr())
                        animation.descr = *action.get_event().get_descr();
                    mParsedAnimations.push_back(animation);
                }
                if (action.get_event().get_type() == animationQuicktype::EventType::TRAIL)
                {
                    Animation animation;
                    animation.type = action.get_event().get_type();
                    animation.nodeName = node.get_name();
                    animation.time = atoi(action.get_trigger().get_data().c_str());
                    if (action.get_event().get_value())
                        animation.trail = *action.get_event().get_value();
                    mParsedAnimations.push_back(animation);
                }
                if (action.get_event().get_type() == animationQuicktype::EventType::ANIMATION)
                {
                    std::string fileNamePath = mpCoreConfig->getConfigFolderPath()+ "/" + *action.get_event().get_data();
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
                    std::string fileNamePath = mpCoreConfig->getConfigFolderPath()+ "/" + *action.get_event().get_data();
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
    for(auto animationName: mAnimatedNodeNames){
        if(auto node = mpSceneManager->getNode(animationName).lock()){
            node->setInitalState();
        }
    }
    std::sort(mParsedBookmarkTimes.begin(), mParsedBookmarkTimes.end());
    std::sort(mParsedAnimations.begin(), mParsedAnimations.end(), compareAnimationTime);
    /*for (auto parsedAnimations : mParsedAnimations)
    {
        APE_LOG_DEBUG("animation: " << parsedAnimations.time);
    }*/
    APE_LOG_FUNC_LEAVE();
}

void ape::FilamentApplicationPlugin::showSpaghetti(std::string name, bool show)
{
    if (auto spaghettiNode = mpSceneManager->getNode(name + "_spaghettiNode").lock())
    {
        spaghettiNode->setChildrenVisibility(show);
    }
}

void ape::FilamentApplicationPlugin::hmdMovedEventCallback(const ape::Vector3& hmdMovedValuePos, const ape::Quaternion& hmdMovedValueOri, const ape::Vector3& hmdMovedValueScl)
{
    APE_LOG_DEBUG(hmdMovedValuePos.toString());
    APE_LOG_DEBUG(hmdMovedValueOri.toString());
}

void ape::FilamentApplicationPlugin::drawSpaghettiSection(const ape::Vector3& startPosition, const ape::NodeSharedPtr& node, std::string& spaghettiSectionName)
{
    std::string spaghettiNodeName = "";
    for(auto spaghettiName : mSpaghettiNodeNames){
        if(spaghettiName.find(node->getName() + "_spaghettiNode") != std::string::npos)
        {
            spaghettiNodeName = spaghettiName;
            break;
        }
    }
    if (auto spaghettiNode = mpSceneManager->getNode(spaghettiNodeName).lock())
    {
        auto currentPosition = node->getDerivedPosition();
        //APE_LOG_DEBUG("startPosition: " << startPosition.toString());
        //APE_LOG_DEBUG("currentPosition: " << currentPosition.toString());
        if (auto spaghettiLineSection = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->getEntity(spaghettiNode->getName()+"_spaghettiEntity").lock()))
        {
            ape::Color color(1, 0, 0);
            auto params = spaghettiLineSection->getParameters();
            auto coords = params.getCoordinates();
            auto indices = params.getIndices();
            
            coords.push_back(currentPosition.x);
            coords.push_back(currentPosition.y);
            coords.push_back(currentPosition.z);
           
            indices[indices.size()-1] = indices[indices.size()-2];
            indices.push_back(indices[indices.size()-1]+1);
            indices.push_back(-1);
            spaghettiLineSection->setParameters(coords, indices, color);
            if (!app.updateinfo.spaghettiVisible)
            {
                spaghettiNode->setChildrenVisibility(false);
                spaghettiNode->setVisible(false);
            }
            else{
                spaghettiNode->setChildrenVisibility(true);
                spaghettiNode->setVisible(true);
            }
            spaghettiSectionName = spaghettiLineSection->getName();
        }
        else if(auto spagetthiLineSection = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity(spaghettiNode->getName()+"_spaghettiEntity", ape::Entity::GEOMETRY_INDEXEDLINESET, true, mpCoreConfig->getNetworkGUID()).lock()))
        { 
            spagetthiLineSection->setParentNode(spaghettiNode);
            ape::GeometryCoordinates coordinates = {
                startPosition.x, startPosition.y, startPosition.z,
                currentPosition.x, currentPosition.y, currentPosition.z };
            ape::GeometryIndices indices = { 0, 1, -1 };
            ape::Color color(1, 0, 0);
            spagetthiLineSection->setParameters(coordinates, indices, color);            
            //mspaghettiLineNames[node->getName()+"_spaghettiEntity"] = spaghettiNode->getName();
            if (!app.updateinfo.spaghettiVisible)
            {
                spaghettiNode->setChildrenVisibility(false);
                spaghettiNode->setVisible(false);
            }
            else{
                spaghettiNode->setChildrenVisibility(true);
                spaghettiNode->setVisible(true);
            }
            mspaghettiLineNames[spagetthiLineSection->getName()]=spaghettiNode->getName();
            spaghettiSectionName = spagetthiLineSection->getName();
            
        }
    }
}

void ape::FilamentApplicationPlugin::playAnimations(double now){
    if(!app.updateinfo.isPlayRunning){
        mSpaghettiNodeNames.clear();
        mspaghettiLineNames.clear();
        mstateNodeNames.clear();
        mstateGeometryNames.clear();
        for (auto animatedNodeName : mAnimatedNodeNames)
        {
            if (auto node = mpSceneManager->getNode(animatedNodeName).lock())
            {
                node->setInitalState();
                node->setOwner(mpCoreConfig->getNetworkGUID());
                std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                if(app.updateinfo.inSinglePlayer)
                if (auto spaghettiNode = mpSceneManager->createNode(animatedNodeName + "_spaghettiNode"+std::to_string(uuid.count()), true, mpCoreConfig->getNetworkGUID()).lock())
                {
                    mSpaghettiNodeNames.insert(spaghettiNode->getName());
                }
            }
        }
    }
    app.updateinfo.isPlayRunning = true;
    
    
    if(app.updateinfo.StartTime >= 0){
        app.updateinfo.pauseTime = (now-app.updateinfo.StartTime)*1000.0;
        for (int i = mPlayedAnimations; i < mParsedAnimations.size(); i++)
        {
            if(app.updateinfo.pauseTime >= mParsedAnimations[i].time){
                if (auto node = mpSceneManager->getNode(mParsedAnimations[i].nodeName).lock())
                {
                    if (mParsedAnimations[i].type == animationQuicktype::EventType::SHOW)
                    {
                        if(!attach2NewAnimationNode(mParsedAnimations[i].parentNodeName, node))
                        {
                            std::string parentsName = "";
                            if(auto parent = node->getParentNode().lock())
                                parentsName = parent->getName();

                            if(parentsName == "")
                                node->detachFromParentNode();
                        }
                           
                        //node->setChildrenVisibility(true);
                        
                       
                        if(mParsedAnimations[i].parentNodeName != ""){
                            if (auto parentNode = mpSceneManager->getNode(mParsedAnimations[i].parentNodeName).lock()){
                                if(node->getParentNode().lock() == parentNode){
                                    node->setPosition(mParsedAnimations[i].position);
                                }
                            }
                        }
                        else{
                            auto nodeAbsolutePos = node->getDerivedPosition();
                            auto nodePos =mParsedAnimations[i].position;
                            auto newPos = Vector3(nodePos.getX()-nodeAbsolutePos.getX(), nodePos.getY()-nodeAbsolutePos.getY(), nodePos.getZ()-nodeAbsolutePos.getZ());
                            node->setPosition(newPos);
                        }
                        node->setOrientation(mParsedAnimations[i].orientation);
                        node->setVisible(true);
                    }
                    else if (mParsedAnimations[i].type == animationQuicktype::EventType::HIDE)
                    {
                        node->setVisible(false);
                    }
                    else if (mParsedAnimations[i].type == animationQuicktype::EventType::ATTACH)
                    {
                        if(mParsedAnimations[i].parentNodeName != ""){
                            if(auto parentNode = mpSceneManager->getNode(mParsedAnimations[i].parentNodeName).lock()){
                                  auto prevPos = node->getDerivedPosition();
                                  node->setParentNode(parentNode);
                                  auto newPos = node->getDerivedPosition();
                                  auto parentPos = parentNode->getDerivedPosition();
                                  node->setPosition(prevPos-parentPos);
                            }
                        }
                    }
                    if (mParsedAnimations[i].type == animationQuicktype::EventType::TRAIL)
                    {
                        if(app.updateinfo.inSinglePlayer)
                            showSpaghetti(node->getName(), mParsedAnimations[i].trail);
                    }
                    if (mParsedAnimations[i].type == animationQuicktype::EventType::LINK)
                    {
                        app.updateinfo.animationLinks[mParsedAnimations[i].nodeName] = mParsedAnimations[i].url;
                        
                    }
                    else if (mParsedAnimations[i].type == animationQuicktype::EventType::STATE)
                    {
                        if (auto stateEntity = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "_StateText").lock()))
                        {
                            std::stringstream posStream;
                            posStream << std::fixed << std::setprecision(1) << (mParsedAnimations[i].time / 1000.0);
                            stateEntity->setCaption(mParsedAnimations[i].descr+" "+ mParsedAnimations[i].nodeName+" "+ posStream.str());
                        }
                        app.updateinfo.stateOfObjects.push_back(mParsedAnimations[i].descr);
                        app.updateinfo.nameOfState.push_back(mParsedAnimations[i].nodeName);
                        app.updateinfo.timeOfState.push_back(mParsedAnimations[i].time/1000.0);
                        if (mParsedAnimations[i].modelName.size())
                        {
                            ;
                            //new IFileGeometry(name_replace_geometry)
                            //setparent(Node)
                            //setfileName
                            //mstateGeometryNames.insert(Ifile)
                            //new Clone(name_Replace)
                            //clone->setsourcegeom
                            //setParent(node)
//                            if(mParsedAnimations[i].descr == "replace"){
//
//                                //geometryClone->setParentNode(ape::NodeWeakPtr());
//                                if(auto fileNode = mpSceneManager->createNode(node->getName()+"_replace", true, mpCoreConfig->getNetworkGUID()).lock())
//                                {
//                                    fileNode->setParentNode(node);
//                                    if (auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(node->getName()+"_replace_geometry",
//                                        ape::Entity::Type::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
//                                    {
//                                        fileGeometry->setFileName(mParsedAnimations[i].modelName);
//                                        fileGeometry->setParentNode(fileNode);
//                                        mstateGeometryNames.insert(fileGeometry->getName());
//                                        mstateNodeNames.insert(fileNode->getName());
//                                        //std::this_thread::sleep_for(std::chrono::milliseconds(200));
//                                    }
//                                    std::this_thread::sleep_for(std::chrono::milliseconds(400));
//                                    if(auto fileGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(node->getName()+"_replace_geometry").lock())){
//                                        if (auto replaceClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity(node->getName()+"_replace", ape::Entity::Type::GEOMETRY_CLONE, true, mpCoreConfig->getNetworkGUID()).lock()))
//                                        {
//
//                                            mstateGeometryNames.insert(replaceClone->getName());
//                                            replaceClone->setSourceGeometry(fileGeometry);
//                                            replaceClone->setParentNode(fileNode);
//                                            node->setChildrenVisibility(false);
//                                            fileNode->setChildrenVisibility(true);
//
//                                        }
//                                    }
//                                }
//                            }
//                            else{
//                                node->setChildrenVisibility(true);
//                                if(auto fileNode = mpSceneManager->getNode(node->getName()+"_replace").lock())
//                                {
//                                    fileNode->detachFromParentNode();
//                                    fileNode->setChildrenVisibility(false);
//                                }
////                                if(mpSceneManager->getEntity(node->getName()+"_replace").lock())
////                                    mpSceneManager->deleteEntity(node->getName()+"_replace");
////
////                                if(mpSceneManager->getEntity(node->getName()+"_replace_geometry").lock())
////                                    mpSceneManager->deleteEntity(node->getName()+"_replace_geometry");
////
////                                if(mpSceneManager->getNode(node->getName()+"_replace").lock())
////                                    mpSceneManager->deleteNode(node->getName()+"_replace");
////
////                                mstateNodeNames.erase(node->getName()+"_replace");
////                                mstateGeometryNames.erase(node->getName()+"_replace_geometry");
////                                mstateGeometryNames.erase(node->getName()+"_replace");
//                            }
                        }
                    }
                    else if (mParsedAnimations[i].type == animationQuicktype::EventType::ANIMATION || mParsedAnimations[i].type == animationQuicktype::EventType::ANIMATION_ADDITIVE)
                    {
                        auto previousPosition = node->getDerivedPosition();
                        if (attach2NewAnimationNode(mParsedAnimations[i].parentNodeName, node))
                        {
                            previousPosition = node->getDerivedPosition();
                        }
                        if (mParsedAnimations[i].type == animationQuicktype::EventType::ANIMATION)
                        {
                            if(mParsedAnimations[i].parentNodeName != ""){
                                if (auto parentNode = mpSceneManager->getNode(mParsedAnimations[i].parentNodeName).lock()){
                                    if(node->getParentNode().lock() == parentNode){
                                        node->setPosition(mParsedAnimations[i].position);
                                    }
                                }
                            }
                            else{
                                auto nodeAbsolutePos = node->getDerivedPosition();
                                auto nodePos =mParsedAnimations[i].position;
                                auto newPos = Vector3(nodePos.getX()-nodeAbsolutePos.getX(), nodePos.getY()-nodeAbsolutePos.getY(), nodePos.getZ()-nodeAbsolutePos.getZ());
                                node->setPosition(newPos);
                            }
                            node->setOrientation(mParsedAnimations[i].orientation);
                            
                        }
                        else if (mParsedAnimations[i].type == animationQuicktype::EventType::ANIMATION_ADDITIVE)
                        {
                            node->rotate(mParsedAnimations[i].rotationAngle.toRadian(), mParsedAnimations[i].rotationAxis, ape::Node::TransformationSpace::PARENT);
                            node->translate(mParsedAnimations[i].translate, ape::Node::TransformationSpace::PARENT);
                        }
                        std::string spaghettiSectionName;
                        if(app.updateinfo.inSinglePlayer)
                            drawSpaghettiSection(previousPosition, node, spaghettiSectionName);
                        //mspaghettiLineNames[spaghettiSectionName]=mParsedAnimations[i].nodeName;
                    }
                    mPlayedAnimations++;
                }
            }
            if (mIsAllSpaghettiVisible != app.updateinfo.spaghettiVisible) {
                mIsAllSpaghettiVisible = app.updateinfo.spaghettiVisible;
                for (auto spg : mSpaghettiNodeNames) {
                    if (auto spgNode = mpSceneManager->getNode(spg).lock()) {
                        spgNode->setChildrenVisibility(app.updateinfo.spaghettiVisible);
                        spgNode->setVisible(app.updateinfo.spaghettiVisible);
                    }
                }
            }
            
        }
        if(mParsedAnimations.size() == mPlayedAnimations){
            for (auto animatedNodeName : mAnimatedNodeNames)
            {
                if (auto node = mpSceneManager->getNode(animatedNodeName).lock())
                {
                    auto prevParent = node->getParentNode();
                    node->revertToInitalState();
                    std::string parentsName = "";
                    if(auto parent = node->getParentNode().lock())
                    {
                        parentsName = parent->getName();
                        node->setParentNode(parent);
                    }
                    if(parentsName == ""){
                        if(prevParent.lock())
                            node->setParentNode(prevParent);
                        node->detachFromParentNode();
                    }
                        
                }
            }
            
            mPlayedAnimations = 0;
            app.updateinfo.isPlayRunning = false;
            app.updateinfo.IsPlayClicked = false;
            app.updateinfo.pauseTime = 0;
            for (auto spaghettiLineName : mspaghettiLineNames)
            {
                if(mpSceneManager->getEntity(spaghettiLineName.first).lock())
                    mpSceneManager->deleteEntity(spaghettiLineName.first);
            }
            for (auto spaghettiNodeName : mSpaghettiNodeNames)
            {
                if(mpSceneManager->getNode(spaghettiNodeName).lock())
                    mpSceneManager->deleteNode(spaghettiNodeName);
            }
            for (auto stateGeometryName : mstateGeometryNames)
            {
                if(mpSceneManager->getEntity(stateGeometryName).lock())
                    mpSceneManager->deleteEntity(stateGeometryName);
            }
            for (auto stateNodeName : mstateNodeNames)
            {
                if(mpSceneManager->getNode(stateNodeName).lock())
                    mpSceneManager->deleteNode(stateNodeName);
            }
            for (auto animatedNodeName : mAnimatedNodeNames)
            {
                if (auto node = mpSceneManager->getNode(animatedNodeName).lock())
                {
                    node->setOwner(node->getCreator());
                   
                }
            }
            mSpaghettiNodeNames.clear();
            mstateNodeNames.clear();
            mstateGeometryNames.clear();
        }
        
    }
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    using namespace Gdiplus;
    UINT  num = 0;
    UINT  size = 0;
    ImageCodecInfo* pImageCodecInfo = NULL;
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;
    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;
    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD wndPid;
    std::cout << "IN ENUM" << std::endl;
    GetWindowThreadProcessId(hwnd, &wndPid);
    std::cout << "WINDOW THREAD" << std::endl;
    int len = 0;
    std::cout << wndPid << " PID" << std::endl;
    len = GetWindowTextLength(hwnd) + 1;
    std::string s;
    s.reserve(len);
    std::cout << len << " WINDOW LENGHT" << std::endl;
    GetWindowText(hwnd, const_cast<char*>(s.c_str()), len - 1);
    std::cout << "WINDOW TEXT" << std::endl;
    if (wndPid == (DWORD)lParam)
    {
        ::PostMessage(hwnd, WM_CLOSE, 0, 0);
        return false;
    }
    else
    {
        return true;
    }
}

void ape::FilamentApplicationPlugin::screenCast()
{
    std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    std::stringstream command;
    command << "/c ffmpeg -f gdigrab -framerate 30 -i desktop ../../screencasts/" << uuid.count() << ".mkv";
    STARTUPINFO info = { sizeof(info) };
    info.cb = sizeof(STARTUPINFO);
    info.wShowWindow = SW_SHOW; 
    memset(&mScreenCastProcessInfo, 0, sizeof(mScreenCastProcessInfo));
    CreateProcess("C:\\windows\\system32\\cmd.exe", LPTSTR((LPCTSTR)command.str().c_str()), NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &info, &mScreenCastProcessInfo);
    
}
#endif

void ape::FilamentApplicationPlugin::takeScreenshot() {
#ifdef __APPLE__
    std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    std::string system_command = "screencapture -x ../../../../screenshots/screenshot" + std::to_string(uuid.count()) + ".jpg";
    system("mkdir ../../../../screenshots");
    system((system_command).c_str());
#else
    using namespace Gdiplus;
    IStream* istream;
    HRESULT res = CreateStreamOnHGlobal(NULL, true, &istream);
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    std::cout << "TAKING SCREENSHOT" << std::endl;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    {
        HDC scrdc, memdc;
        HBITMAP membit;
        scrdc = ::GetDC(0);
        int Height = GetSystemMetrics(SM_CYSCREEN);
        int Width = GetSystemMetrics(SM_CXSCREEN);
        memdc = CreateCompatibleDC(scrdc);
        membit = CreateCompatibleBitmap(scrdc, Width, Height);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
        BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);
        Gdiplus::Bitmap bitmap(membit, NULL);
        CLSID clsid;
        GetEncoderClsid(L"image/jpeg", &clsid);
        std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        auto fileName = "../../screenshots/" + std::to_string(uuid.count()) + ".jpg";
        std::wstring wFileName = std::wstring(fileName.begin(), fileName.end());
        bitmap.Save(wFileName.c_str(), &clsid, NULL);
    }
    GdiplusShutdown(gdiplusToken);
    istream->Release();
#endif
}

void ape::FilamentApplicationPlugin::startScreenCast() {
    app.updateinfo.screenCaptureOn = true;
#ifdef __APPLE__
    auto systemCommand = [this]() {
        std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        mLastScreenCapture = std::to_string(uuid.count()) + ".mov";
        std::string system_command = "screencapture -x -v -V 600 ../../../../screencasts/" + std::to_string(uuid.count()) + ".mov &";
        system("mkdir ../../../../screencasts");
        system(system_command.c_str());
    };
    std::thread systemThread(systemCommand);
    systemThread.detach();
#else
    screenCast();
    //auto screenCastThread = std::thread(&FilamentApplicationPlugin::screenCast, this);
    //screenCastThread.detach();
#endif
}

void ape::FilamentApplicationPlugin::stopScreenCast() {
#ifdef __APPLE__
    system("killall -9 screencapture");
    std::string sysCommand = "mv -v ~/Library/ScreenRecordings/* ../../../../screencasts/";
    system(sysCommand.c_str());
    app.updateinfo.screenCaptureOn = false;
#else
    app.updateinfo.stoppingScreenCast = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "STOP CAST" << std::endl;
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, "ffmpeg.exe") == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 2);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
    CloseHandle(mScreenCastProcessInfo.hProcess);
    CloseHandle(mScreenCastProcessInfo.hThread);
    /*auto succ = TerminateProcess(mScreenCastProcessInfo.hProcess, 2);
    if (succ) {

        const DWORD result = WaitForSingleObject(mScreenCastProcessInfo.hProcess, 100);
        if (result == WAIT_OBJECT_0)
            std::cout << "SUCCESS" << std::endl;
        else
            std::cout << "FAIL" << std::endl;
        ResumeThread(mScreenCastProcessInfo.hThread);
        CloseHandle(mScreenCastProcessInfo.hThread);
        CloseHandle(mScreenCastProcessInfo.hProcess);
        auto succ2 = TerminateProcess(mScreenCastProcessInfo.hProcess, 2);
        if (succ2) {

            const DWORD result = WaitForSingleObject(mScreenCastProcessInfo.hProcess, 100);
            if (result == WAIT_OBJECT_0)
                std::cout << "SUCCESS" << std::endl;
            else
                std::cout << "FAIL" << std::endl;
            CloseHandle(mScreenCastProcessInfo.hProcess);
            CloseHandle(mScreenCastProcessInfo.hThread);

        }
    }
    else
        std::cout << "TERMINATE FAIL" << std::endl;*/
    /*HANDLE ps = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, mScreenCastProcessInfo.dwProcesfsId);

     std::cout << "ps" << std::endl;
     if (ps != INVALID_HANDLE_VALUE) {
         EnumWindows(EnumWindowsProc, mScreenCastProcessInfo.dwProcessId);
         CloseHandle(ps);
         std::cout << "stopped" << std::endl;
     }else
         std::cout << "no process" << std::endl;*/
    app.updateinfo.stoppingScreenCast = false;
    app.updateinfo.screenCaptureOn = false;
#endif
}



void ape::FilamentApplicationPlugin::Step()
{

    APE_LOG_FUNC_ENTER();
    APE_LOG_DEBUG("STEP STARTED");
    app.config.title = "Filament";
    app.config.iblDirectory = FilamentApp::getRootAssetsPath() + DEFAULT_IBL;
    utils::Path filename;
   

//    int num_args = 1;
//    if (num_args >= 1) {
//        filename = "/Users/erik/Documents/ApertusVR/ApertusVR/plugins/scene/photorealisticScene/resources/Conveyor.gltf";
//        if (!filename.exists()) {
//            std::cerr << "file " << filename << " not found!" << std::endl;
//            //return 1;
//        }
//        if (filename.isDirectory()) {
//            auto files = filename.listContents();
//            for (auto file : files) {
//                if (file.getExtension() == "gltf" || file.getExtension() == "glb") {
//                    filename = file;
//                    break;
//                }
//            }
//            if (filename.isDirectory()) {
//                std::cerr << "no glTF file found in " << filename << std::endl;
//                //return 1;
//            }
//        }
//    }

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
        if (filename.getExtension() == "glb") {
            app.logo = app.loader->createAssetFromBinary(buffer.data(), buffer.size());
        } else {
            app.logo = app.loader->createAssetFromJson(buffer.data(), buffer.size());
        }
        buffer.clear();
        buffer.shrink_to_fit();

        if (!app.logo) {
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
        if ( app.resourceLoader->asyncBeginLoad(app.logo))
        {
            //APE_LOG_DEBUG("resources load OK");
            app.mpScene->addEntities(app.logo->getEntities(), app.logo->getEntityCount());
        }
       
        auto ibl = FilamentApp::get().getIBL();
        ibl->~IBL();
      /*  auto ibl = FilamentApp::get().getIBL();
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
        }*/
       

    };

   
    auto setup = [&](Engine* engine, View* view, Scene* scene) {
      
        app.engine = engine;
        app.view = view;
        auto asd = view->getSampleCount();
        view->setSampleCount(4);
        asd = view->getSampleCount();
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
        if (auto cameraNode = mpSceneManager->createNode("Camera_controller", false, mpCoreConfig->getNetworkGUID()).lock()) {
            cameraNode->setPosition(ape::Vector3(0, 0, 0));
        }
        auto mNode = mpSceneManager->createNode("VLFTlogo", false, mpCoreConfig->getNetworkGUID());
        if (auto node = mNode.lock())
        {
            auto logoName = "VLTF_3Dlogo";
            if (auto gltfMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(logoName, ape::Entity::GEOMETRY_FILE, false, mpCoreConfig->getNetworkGUID()).lock()))
            {
                gltfMeshFile->setFileName("../assets/models/logo/VLTF_3Dlogo.gltf");
                gltfMeshFile->setParentNode(node);
            }
            if (auto gltfMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(logoName).lock())){
                if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity("VLFTlogo", ape::Entity::Type::GEOMETRY_CLONE, false, mpCoreConfig->getNetworkGUID()).lock()))
                {
                    geometryClone->setSourceGeometry(gltfMeshFile);
                    geometryClone->setParentNode(node);
                }
            }
            node->rotate(1.5708f, ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::LOCAL);
            node->setScale(ape::Vector3(12, 12, 12));
            node->setPosition(ape::Vector3(2.5, 1.5, -0.4));
            node->setChildrenVisibility(true);
            node->setVisible(true);
        }
        app.setManpipulator = true;
        app.sunLight = utils::EntityManager::get().create();
        LightManager::Builder(LightManager::Type::SUN)
            .color(filament::Color::toLinear<ACCURATE>(sRGBColor(0.95f, 0.95f, 0.95f)))
            .intensity(100000)
            .direction({ app.updateinfo.lightDirection[0], app.updateinfo.lightDirection[1], app.updateinfo.lightDirection[2] })
            .sunAngularRadius(1.9f)
            .castShadows(true)
            .build(*app.engine, app.sunLight);
        app.sunLight2 = utils::EntityManager::get().create();
        LightManager::Builder(LightManager::Type::SUN)
            .color(filament::Color::toLinear<ACCURATE>(sRGBColor(0.95f, 0.95f, 0.95f)))
            .intensity(100000)
            .direction({ -app.updateinfo.lightDirection[0], app.updateinfo.lightDirection[1], -app.updateinfo.lightDirection[2] })
            .sunAngularRadius(1.9f)
            .castShadows(true)
            .build(*app.engine, app.sunLight2);
        app.mpScene->addEntity(app.sunLight);
        //app.mpScene->addEntity(app.sunLight2);
        auto ibl = FilamentApp::get().getIBL();
        auto indirectLight = ibl->getIndirectLight();

        indirectLight->setIntensity(18500);
        
    };

    auto cleanup = [this](Engine* engine, View*, Scene*) {
        for(auto const& x : app.asset){
            try {
                app.loader->destroyAsset(x.second);
            } catch (...) {
                APE_LOG_DEBUG("Asset has already been deleted" << x.first);
            }
        }

        engine->destroy(app.scene.groundPlane);
        engine->destroy(app.colorGrading);

        delete app.materials;
        delete app.names;
        AssetLoader::destroy(&app.loader);
    };

   
    auto resize = [this](Engine* engine, View* view) {
        app.updateinfo.rePositionUI = true;
        const Viewport& vp = view->getViewport();
        
        /*auto mapTM = app.mpTransformManager->getInstance(app.worldMap.playerMap);
        auto mapTransform = app.mpTransformManager->getTransform(mapTM);

        if (false && vp.width > 1300) {
            float wTmp = 1 + float(vp.width) / 30000.0f;
            float hTmp = 1 + float(vp.height) / 35000.0f;
            mapTransform[3][0] = wTmp * wTmp - 1;
            mapTransform[3][1] = hTmp * hTmp - 1;
            mapTransform[3][2] = -0.10015;
            app.mpTransformManager->setTransform(mapTM, mapTransform);
        }
        else {
            mapTransform[3][0] = 0.0515;
            mapTransform[3][1] = 0.0423;
            mapTransform[3][2] = -0.10015;
            app.mpTransformManager->setTransform(mapTM, mapTransform);
        }*/
        filament::Camera& camera = view->getCamera();
        if (&camera == app.mainCamera) {
            // Don't adjut the aspect ratio of the main camera, this is done inside of
            // FilamentApp.cpp
            return;
        }
        //const Viewport& vp = view->getViewport();
        double aspectRatio = (double) vp.width / vp.height;
        camera.setScaling(double4 {1.0 / aspectRatio, 1.0, 1.0, 1.0});
    };

    auto gui = [this](Engine* engine, View* view) {
        //copy and edit apecoreJson to vfgame2

        if (app.updateinfo.lightOn) {
            if (!app.mpScene->hasEntity(app.sunLight)) {
                app.mpScene->addEntity(app.sunLight);
            }
        }
        else {
            if (app.mpScene->hasEntity(app.sunLight)) {
                app.mpScene->remove(app.sunLight);
            }
        }
        if (app.updateinfo.changeLightDir) {
            if (app.mpScene->hasEntity(app.sunLight)) {
                app.mpScene->remove(app.sunLight);
            }
            LightManager::Builder(LightManager::Type::SUN)
                .color(filament::Color::toLinear<ACCURATE>(sRGBColor(0.98f, 0.92f, 0.89f)))
                .intensity(550000)
                .direction({ app.updateinfo.lightDirection[0], app.updateinfo.lightDirection[1], app.updateinfo.lightDirection[2] })
                .sunAngularRadius(1.9f)
                .castShadows(true)
                .build(*app.engine, app.sunLight);
            app.mpScene->addEntity(app.sunLight);
            app.updateinfo.changeLightDir = false;
        }
       /* if (false && app.updateinfo.isMapVisible) {
            if (!app.mpScene->hasEntity(app.worldMap.playerMap)) {
                app.mpScene->addEntity(app.worldMap.playerMap);
                for (auto &ply : app.worldMap.playerTriangles) {
                    if (!app.mpScene->hasEntity(ply.second)) {
                        app.mpScene->addEntity(ply.second);
                    }
                }
            }
        }
        else {
            if (app.mpScene->hasEntity(app.worldMap.playerMap)) {
                app.mpScene->remove(app.worldMap.playerMap);
                for (auto& ply : app.worldMap.playerTriangles) {
                    if (app.mpScene->hasEntity(ply.second)) {
                        app.mpScene->remove(ply.second);
                    }
                }
            }
        }*/
        auto& tm = engine->getTransformManager();
        auto& rm = engine->getRenderableManager();
        auto& lm = engine->getLightManager();
        if(!app.updateinfo.initRun){
            mpVlftImgui->init(&app.updateinfo);
        }
        if (app.updateinfo.takeScreenshot) {
            takeScreenshot();
            app.updateinfo.takeScreenshot = false;
        }
        if (app.updateinfo.screenCast) {
            if (!app.updateinfo.screenCaptureOn) {
                startScreenCast();
            }
            else {
                auto screenCastThread = std::thread(&FilamentApplicationPlugin::stopScreenCast, this);
                screenCastThread.detach();
            }
            app.updateinfo.screenCast = false;
        }
        if(app.updateinfo.checkLogin){
            if(app.firstRun){
                if(app.updateinfo.isAdmin){
                    //check password
                    if(app.updateinfo.password == "apertusvr2020" && app.updateinfo.userName.size() > 0){
                        mUserName = app.updateinfo.userName;
                        app.updateinfo.logedIn = true;
                        app.updateinfo.wrongPassword = false;
                    }
                    else
                        app.updateinfo.wrongPassword = true;
                    app.updateinfo.checkLogin = false;
                }
                else{
                    //check password
                    if(app.updateinfo.userName.size() > 0){
                        mUserName = app.updateinfo.userName;
                        app.updateinfo.logedIn = true;
                        app.updateinfo.wrongPassword = false;
                    }
                    else
                        app.updateinfo.wrongPassword = true;
                    app.updateinfo.checkLogin = false;
                }
                if(app.updateinfo.logedIn){
                    app.firstRun = false;
                }
            }
        }
        if(app.updateinfo.logedIn){
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
               
//                ImGui::Begin("Hierarchy", nullptr);
//                for(auto  const& x: app.asset){
//                    auto instances = x.second->getAssetInstances();
//                    size_t cnt = x.second->getAssetInstanceCount();
//                    mAsset = x.second;
//                    if (ImGui::CollapsingHeader(x.first.c_str())) {
//                        for(size_t i = 0; i < cnt; i++){
//                                ImGui::Indent();
//                                treeNode(instances[i]->getRoot());
//                                ImGui::Unindent();
//                        }
//                    }
//                }
//                ImGui::End();
            }
        }
        if(app.updateinfo.setUpRoom){
            
            std::string locationSceneConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTSceneLoaderPlugin.json";
            auto mApeVLFTSceneLoaderPluginConfigFile = std::fopen(locationSceneConfig.c_str(), "r");
            mSceneJson = nlohmann::json::parse(mApeVLFTSceneLoaderPluginConfigFile);
            std::fclose(mApeVLFTSceneLoaderPluginConfigFile);
            
            for (auto asset : mSceneJson.get_assets())
            {
                std::weak_ptr<std::vector<quicktype::Representation>> representations = asset.get_representations();
                if (representations.lock())
                {
                    for (auto representation : *asset.get_representations())
                    {
                        std::string fileName = representation.get_file();
                        std::string idName = asset.get_id();
                        std::string gltfName = fileName.substr(fileName.find_last_of("#")+1);
                        std::string cloneName = idName.substr(0,idName.find_last_of("."));
                        idGltfMap[idName] = cloneName+"."+gltfName;
                    }
                }
                
            }
            
            if(auto logo = mpSceneManager->getNode("VLFTlogo").lock()){
                logo->setVisible(false);
                logo->setChildrenVisibility(false);
            }
            while((mpCoreConfig->getNetworkGUID() == ""  ||  mPostUserName.find(mpCoreConfig->getNetworkGUID()) != std::string::npos) && !app.updateinfo.inSinglePlayer){
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            if(app.updateinfo.isAdmin)
                mPostUserName = "_vlftTeacher"+ mpCoreConfig->getNetworkGUID();
            else
                mPostUserName = "_vlftStudent"+ mpCoreConfig->getNetworkGUID();
            //auto mCamNode = mpSceneManager->createNode(mUserName + mPostUserName+"_cam", true, mpCoreConfig->getNetworkGUID());
            auto mNode = mpSceneManager->createNode(mUserName + mPostUserName, true, mpCoreConfig->getNetworkGUID());
            //if(auto camNode = mCamNode.lock())
            if (auto node = mNode.lock())
            {
                //camNode->setPosition(ape::Vector3(0.0, 0.0, 0.0));
                //node->setParentNode(camNode);
                if (auto gltfNode = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(mUserName +mPostUserName+ "characterModel", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
                {
                    gltfNode->setFileName("../assets/models/avatar/MC_Char_1.glb");
                    gltfNode->setParentNode(node);
                }
                if (auto gltfNode = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName +mPostUserName+ "characterModel").lock())){
                    if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity(mUserName + mPostUserName, ape::Entity::Type::GEOMETRY_CLONE, true, mpCoreConfig->getNetworkGUID()).lock()))
                    {
                        geometryClone->setSourceGeometry(gltfNode);
                        geometryClone->setParentNode(node);
                        node->setChildrenVisibility(true);
                    }
                }
                node->setPosition(ape::Vector3(0.0, 0.0, 0.0));
            }
            auto userTextNode = mpSceneManager->createNode(mUserName+mPostUserName + "_TextNode", true, mpCoreConfig->getNetworkGUID());
            if (auto textNode = userTextNode.lock()) {
                if (auto textGeometry = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity(mUserName+mPostUserName+"_text", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock())) {
                    textGeometry->setParentNode(userTextNode);
                    textGeometry->setCaption(app.updateinfo.userName+" logged in");
                }
            }

            auto stateTextNode = mpSceneManager->createNode(mUserName + mPostUserName + "_StateNode", true, mpCoreConfig->getNetworkGUID());
            if (auto textNode = stateTextNode.lock()) {
                if (auto textGeometry = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity(mUserName + mPostUserName + "_StateText", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock())) {
                    textGeometry->setParentNode(userTextNode);
                }
            }

           // app.worldMap.mapVertexBuffer = VertexBuffer::Builder()
           //     .vertexCount(16)
           //     .bufferCount(1)
           //     .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3, 0, 16)
           //     .attribute(VertexAttribute::COLOR, 0, VertexBuffer::AttributeType::UBYTE4, 12, 16)
           //     .normalized(VertexAttribute::COLOR)
           //     .build(*engine);
           // app.worldMap.mapVertexBuffer->setBufferAt(*engine, 0,
           //     VertexBuffer::BufferDescriptor(app.worldMap.mapVertices, 256, nullptr));
           // app.worldMap.mapIndexBuffer = IndexBuffer::Builder()
           //     .indexCount(24)
           //     .bufferType(IndexBuffer::IndexType::USHORT)
           //     .build(*engine);
           // app.worldMap.mapIndexBuffer->setBuffer(*engine,
           //     IndexBuffer::BufferDescriptor(app.worldMap.mapIndices, 48, nullptr));
           // app.worldMap.mapMaterial = filament::Material::Builder()
           //     .package(RESOURCES_BAKEDCOLOR_DATA, RESOURCES_BAKEDCOLOR_SIZE)
           //     .build(*engine);
           // if(!app.mpEntityManager->isAlive(app.worldMap.playerMap)){
           //     app.worldMap.playerMap = EntityManager::get().create();
           //     auto mat = app.worldMap.mapMaterial->getDefaultInstance();
           //     double transmissionFact = 0.5;
           //     filament::Material::ParameterInfo parameterInfo;
           //     app.worldMap.mapMaterial->getParameters(&parameterInfo, app.worldMap.mapMaterial->getParameterCount());
           //     //mat->setParameter("transmissionFactor", transmissionFact);
           //     RenderableManager::Builder(1)
           //         .boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
           //         .material(0, mat)
           //         .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, app.worldMap.mapVertexBuffer, app.worldMap.mapIndexBuffer, 0, 24)
           //         .culling(true)
           //         .receiveShadows(false)
           //         .castShadows(false)
           //         .build(*engine, app.worldMap.playerMap);
           // }
           // app.mpScene->addEntity(app.worldMap.playerMap);

           // auto camEntity = app.mainCamera->getEntity();
           // auto camTM = app.mpTransformManager->getInstance(camEntity);
           // auto mapTM = app.mpTransformManager->getInstance(app.worldMap.playerMap);
           // app.mpTransformManager->setParent(mapTM, camTM);
           // auto mapTransform = app.mpTransformManager->getTransform(mapTM);
           // mapTransform[3][0] = 0.0515;
           // mapTransform[3][1] = 0.0423;
           // mapTransform[3][2] = -0.10015;
           // app.mpTransformManager->setTransform(mapTM, mapTransform);


           // app.worldMap.referenceVertexBuffer = VertexBuffer::Builder()
           //     .vertexCount(1)
           //     .bufferCount(1)
           //     .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3, 0, 16)
           //     .attribute(VertexAttribute::COLOR, 0, VertexBuffer::AttributeType::UBYTE4, 12, 16)
           //     .normalized(VertexAttribute::COLOR)
           //     .build(*engine);
           // LineVertex rfPoint[1] = { { {0,0,0}, 0xffff0000u } };
           // app.worldMap.referenceVertexBuffer->setBufferAt(*engine, 0,
           //     VertexBuffer::BufferDescriptor(rfPoint, 16, nullptr));
           // app.worldMap.referenceIndexBuffer = IndexBuffer::Builder()
           //     .indexCount(6)
           //     .bufferType(IndexBuffer::IndexType::USHORT)
           //     .build(*engine);
           // uint16_t rfIndices[1] = { 0 };
           // app.worldMap.referenceIndexBuffer->setBuffer(*engine,
           //     IndexBuffer::BufferDescriptor(rfIndices, 2, nullptr));
           ///* app.worldMap.mapMaterial = filament::Material::Builder()
           //     .package(RESOURCES_BAKEDCOLOR_DATA, RESOURCES_BAKEDCOLOR_SIZE)
           //     .build(*engine);*/

           //app.worldMap.mapReferencePoint = EntityManager::get().create();
           //RenderableManager::Builder(1)
           //     .boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
           //     .material(0, app.worldMap.mapMaterial->getDefaultInstance())
           //     .geometry(0, RenderableManager::PrimitiveType::POINTS, app.worldMap.referenceVertexBuffer, app.worldMap.referenceIndexBuffer, 0, 1)
           //     .culling(true)
           //     .receiveShadows(false)
           //     .castShadows(false)
           //     .build(*engine, app.worldMap.mapReferencePoint);
           // app.mpScene->addEntity(app.worldMap.mapReferencePoint);
           // auto rfTM = app.mpTransformManager->getInstance(app.worldMap.mapReferencePoint);
           // app.mpTransformManager->setParent(rfTM, mapTM);
           // auto rfTransform = app.mpTransformManager->getTransform(rfTM);
           // rfTransform[3][0] = 0.0125;
           // rfTransform[3][1] = -0.0125;
           // rfTransform[3][2] = 0.0001;
           // app.mpTransformManager->setTransform(rfTM, rfTransform);

            initAnimations();
            
            /*app.worldMap.playerVertexBuffer = VertexBuffer::Builder()
                .vertexCount(6)
                .bufferCount(1)
                .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3, 0, 16)
                .attribute(VertexAttribute::COLOR, 0, VertexBuffer::AttributeType::UBYTE4, 12, 16)
                .normalized(VertexAttribute::COLOR)
                .build(*engine);
            app.worldMap.playerVertexBuffer->setBufferAt(*engine, 0,
                VertexBuffer::BufferDescriptor(app.worldMap.playerVertices, 96, nullptr));
            app.worldMap.playerIndexBuffer = IndexBuffer::Builder()
                .indexCount(6)
                .bufferType(IndexBuffer::IndexType::USHORT)
                .build(*engine);
            app.worldMap.playerIndexBuffer->setBuffer(*engine,
                IndexBuffer::BufferDescriptor(app.worldMap.playerIndices, 12, nullptr));*/

            app.updateinfo.setUpRoom = false;
            if(app.updateinfo.inSinglePlayer)
                app.updateinfo.loadingRoom = false;
        }
        FilamentApp::get().setSidebarWidth(0);
        mpVlftImgui->update();
    };

    auto preRender = [this](Engine* engine, View* view, Scene* scene, Renderer* renderer) {
       
       
        auto& rcm = engine->getRenderableManager();
        auto instance = rcm.getInstance(app.scene.groundPlane);
        rcm.setLayerMask(instance,
                0xff, app.viewOptions.groundPlaneEnabled ? 0xff : 0x00);
        filament::Camera& camera = view->getCamera();
        auto viewMatrix =  camera.getModelMatrix();
        if(mIsStudent){
            camera.setModelMatrix(filament::math::mat4f(
                                viewMatrix[0][0],viewMatrix[0][1],viewMatrix[0][2],viewMatrix[0][3],
                                viewMatrix[1][0],viewMatrix[1][1],viewMatrix[1][2],viewMatrix[1][3],
                                viewMatrix[2][0],viewMatrix[2][1],viewMatrix[2][2],viewMatrix[2][3],
                                viewMatrix[3][0], 1.5, viewMatrix[3][2],viewMatrix[3][3]
                                             ));
        }
        if(app.updateinfo.logMovements){
            if(!app.updateinfo.mIsStudentsMovementLogging){
                app.updateinfo.mIsStudentsMovementLogging = true;
                std::stringstream fileName;
                std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
#ifdef __APPLE__
                system("mkdir ../../../../studentsMovementLog");
                fileName << "../../../../studentsMovementLog/" << uuid.count() << ".txt";
#else
                fileName << "../../studentsMovementLog/" << uuid.count() << ".txt";
#endif
                mStudentsMovementLoggingFile.open(fileName.str());
                auto nodes = mpSceneManager->getNodes();
                for (auto node : nodes)
                {
                    if (auto nodeSP = node.second.lock())
                    {
                        if (auto parentNode = nodeSP->getParentNode().lock())
                        {
                            ;
                        }
                        else
                        {
                            std::string nodeName = nodeSP->getName();
                            std::size_t pos = nodeName.find("_vlftStudent");
                            if (pos != std::string::npos)
                            {
                                mStudents.push_back(nodeSP);
                            }
                        }
                    }
                }
            }
            else{
                app.updateinfo.mIsStudentsMovementLogging = false;
                mStudents.clear();
                mStudents.resize(0);
                mStudentsMovementLoggingFile.close();
                
            }
            app.updateinfo.logMovements = false;
        }
        if(app.updateinfo.leftRoom){
            if (app.updateinfo.pickedItem != "")
            {
                if (auto clone = mpSceneManager->getNode(app.rootOfSelected.second).lock()) {
                    clone->detachFromParentNode();
                    clone->revertToInitalState();
                    clone->setOwner(clone->getCreator());
                }
                //            auto parentTm = app.mpTransformManager->getInstance(app.parentOfPicked);
                //            auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
                //            auto tmInstance = app.mpTransformManager->getInstance(instance->getRoot());
                //            auto parentWorldMatrix = app.mpTransformManager->getTransform(tmInstance);
                //            parentWorldMatrix[3][1] += 0.4;
                //            parentWorldMatrix[3][2] += 2.0;
                //            app.mpTransformManager->setTransform(tmInstance, parentWorldMatrix);
                //            app.mpTransformManager->setParent(tmInstance,parentTm);
                app.updateinfo.pickedItem = "";
                app.updateinfo.drop = false;
                scene->remove(app.boxEntity.first);
            }
            mEventDoubleQueue.swap();
           
            while (!mEventDoubleQueue.emptyPop())
                mEventDoubleQueue.pop();
            if (auto node = mpSceneManager->getNode(mUserName + mPostUserName).lock()) {
                if (node->getOwner() != mpCoreConfig->getNetworkGUID()) {
                    node->setOwner(mpCoreConfig->getNetworkGUID());
                }
            }
            if (auto stateNode = mpSceneManager->getNode(mUserName + mPostUserName+"_StateNode").lock()) {
                if (stateNode->getOwner() != mpCoreConfig->getNetworkGUID()) {
                    stateNode->setOwner(mpCoreConfig->getNetworkGUID());
                }
            }
            if (auto textNode = mpSceneManager->getNode(mUserName + mPostUserName+"_TextNode").lock()) {
                if (textNode->getOwner() != mpCoreConfig->getNetworkGUID()) {
                    textNode->setOwner(mpCoreConfig->getNetworkGUID());
                }
            }
            if (auto textNode = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "_text").lock()))
            {
                ;// textNode->setCaption(mUserName + " left the room");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            for (auto attachedUserWP : mAttachedUsers)
            {
                if (auto attachedUser = attachedUserWP.lock())
                {
                    if (attachedUser->getOwner() == mpCoreConfig->getNetworkGUID())
                    {
                        attachedUser->setOwner(attachedUser->getCreator());
                    }
                }
            }
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
            app.updateinfo.leftRoom = false;
            app.updateinfo.callLeave = true;

            
            mpSceneManager->deleteEntity(mUserName+mPostUserName+"characterModel");
            mpSceneManager->deleteEntity(mUserName+mPostUserName);
            mpSceneManager->deleteNode(mUserName+mPostUserName);
            mpSceneManager->deleteEntity(mUserName+mPostUserName + "_text");
            mpSceneManager->deleteEntity(mUserName + mPostUserName + "_StateText");
            mpSceneManager->deleteNode(mUserName + mPostUserName + "_StateNode");
            mpSceneManager->deleteNode(mUserName + mPostUserName + "_TextNode");
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
            //mpSceneManager->deleteNode(mUserName+mPostUserName+"_cam");

            for (auto spa : app.spaghettiLines) {
                app.engine->destroy(spa.second.mat);
                app.engine->destroy(spa.second.lineEntity);
                app.engine->destroy(spa.second.lineIndexBuffer);
                app.engine->destroy(spa.second.lineVertexBuffer);
            }
            app.spaghettiLines.clear();
            if (app.mpScene->hasEntity(app.boxEntity.first)) {
                app.mpScene->remove(app.boxEntity.first);
            }
            if (app.mpEntityManager->isAlive(app.boxEntity.first)) {
                app.engine->destroy(app.boxEntity.first);
                app.mpEntityManager->destroy(app.boxEntity.first);
                if (app.mpTransformManager->hasComponent(app.boxEntity.first))
                    app.mpTransformManager->destroy(app.boxEntity.first);
                app.engine->destroy(app.boxVertexBuffer);
                app.engine->destroy(app.boxIndexBuffer);
            }
            std::vector<std::string> to_erase;
            delete app.resourceLoader;
            app.resourceLoader = nullptr;
            /*if (app.mpEntityManager->isAlive(app.worldMap.playerMap)) {
                app.mpScene->remove(app.worldMap.playerMap);
                for (auto &ply : app.worldMap.playerTriangles) {
                    if (app.mpScene->hasEntity(ply.second)) {
                        app.mpScene->remove(ply.second);
                    }
                    if (app.mpTransformManager->hasComponent(ply.second))
                        app.mpTransformManager->destroy(ply.second);
                    app.mpEntityManager->destroy(ply.second);
                    app.engine->destroy(ply.second);
                }
                app.worldMap.playerTriangles.clear();

                if (app.mpScene->hasEntity(app.worldMap.playerMap))
                    app.mpScene->remove(app.worldMap.playerMap);
                if (app.mpTransformManager->hasComponent(app.worldMap.playerMap))
                    app.mpTransformManager->destroy(app.worldMap.playerMap);
                app.mpEntityManager->destroy(app.worldMap.playerMap);
                app.engine->destroy(app.worldMap.playerMap);
              

                if (app.mpScene->hasEntity(app.worldMap.mapReferencePoint))
                    app.mpScene->remove(app.worldMap.mapReferencePoint);
                if (app.mpTransformManager->hasComponent(app.worldMap.mapReferencePoint))
                    app.mpTransformManager->destroy(app.worldMap.mapReferencePoint);
                app.mpEntityManager->destroy(app.worldMap.mapReferencePoint);
                app.engine->destroy(app.worldMap.mapReferencePoint);

                app.engine->destroy(app.worldMap.mapMaterial);
                app.engine->destroy(app.worldMap.playerIndexBuffer);
                app.engine->destroy(app.worldMap.playerVertexBuffer);
                app.engine->destroy(app.worldMap.mapIndexBuffer);
                app.engine->destroy(app.worldMap.mapVertexBuffer);
                app.engine->destroy(app.worldMap.referenceVertexBuffer);
                app.engine->destroy(app.worldMap.referenceIndexBuffer);

                app.updateinfo.isMapVisible = false;
            }*/
           
            for(auto instanceList: app.instances){
                if (instanceList.first == "default_building" && instanceList.second[0]->getEntityCount() > 0) {
                    instanceList.first;
                    auto entity = instanceList.second[0]->getRoot();
                    if (app.mpScene->hasEntity(entity))
                        app.mpScene->removeEntities(instanceList.second[0]->getEntities(), instanceList.second[0]->getEntityCount());
                }
                else {
                    for (size_t i = 0; i < instanceList.second.size(); i++) {
                        if (instanceList.second[i]->getEntityCount() > 0)
                        {
                            instanceList.first;
                            auto entity = instanceList.second[i]->getRoot();
                            if (app.mpScene->hasEntity(entity))
                                app.mpScene->removeEntities(instanceList.second[i]->getEntities(), instanceList.second[i]->getEntityCount());
                        }
                    }
                }
                to_erase.push_back(instanceList.first);
            }
            for(auto item: to_erase){
                app.instances.erase(item);
            }
            app.instanceCount.clear();
            to_erase.clear();
            app.geometryNameMap.clear();
            
            for(auto instance: app.mpInstancesMap){
                    if(app.asset.find(instance.second.assetName) != app.asset.end()){
                        app.loader->destroyAsset(app.asset[instance.second.assetName]);
                        app.asset.erase(instance.second.assetName);
                    }
                    to_erase.push_back(instance.first);
            }
            for (auto item : to_erase) {
                app.mpInstancesMap.erase(item);
                app.playerNamesToShow.erase(item);
                if (app.updateinfo.playerNamePositions.find(item.substr(0, item.find("_vlft"))) != app.updateinfo.playerNamePositions.end())
                    app.updateinfo.playerNamePositions.erase(item.substr(0, item.find("_vlft")));
            }
            to_erase.clear();
            for (auto asset : app.asset) {
                app.loader->destroyAsset(app.asset[asset.first]);
                to_erase.push_back(asset.first);
            }
            for (auto item : to_erase) {
                app.asset.erase(item);
            }
            to_erase.clear();
            app.mpLoadedAssets.clear();
          
            app.playerAnimations.clear();
            mAnimatedNodeNames.clear();
            mParsedAnimations.clear();
            for (auto entity : app.mpEntities) {
                app.mpTransformManager->destroy(entity.second);
                to_erase.push_back(entity.first);
            }
            for (auto item : to_erase) {
                app.engine->destroy(app.mpEntities[item]);
            }
            to_erase.clear();
            app.mpTransforms.clear();
            app.mpEntities.clear();
            idGltfMap.clear();
            app.updateinfo.leaveWait = true;
            app.updateinfo.leaveTime = app.updateinfo.now;
        }
        if(app.updateinfo.switchOwner){
            for(size_t i = 0; i < mNodeToSwitchOwner.size(); i++){
                if(auto mNode = mpSceneManager->getNode(mNodeToSwitchOwner[i]).lock()){
                    mNode->setOwner(mNode->getCreator());
                    app.updateinfo.switchOwner = false;
                }
            }
            mNodeToSwitchOwner.clear();
        }
        if(app.updateinfo.attachUsers){
            if(!app.updateinfo.usersAttached){
                auto nodes = mpSceneManager->getNodes();
                for (auto node : nodes)
                {
                    if (auto nodeSP = node.second.lock())
                    {
                        if (auto parentNode = nodeSP->getParentNode().lock())
                        {
                            ;
                        }
                        else
                        {
                            std::string nodeName = nodeSP->getName();
                            std::size_t pos = nodeName.find("_vlftStudent");
                            std::size_t pos2 = nodeName.find("_TextNode");
                            if (pos != std::string::npos)
                            {
                                if (pos2 == std::string::npos) {
                                    nodeSP->setInitalState();
                                    nodeSP->setOwner(mpCoreConfig->getNetworkGUID());
                                    if (auto userNode = mpSceneManager->getNode(mUserName + mPostUserName).lock())
                                        nodeSP->setParentNode(userNode);
                                    nodeSP->setPosition(ape::Vector3(0, 0, 0.05));
                                    nodeSP->setOrientation(ape::Quaternion(1, 0, 0, 0));
                                    mAttachedUsers.push_back(nodeSP);
                                }
                            }
                        }
                    }
                }
                app.updateinfo.usersAttached = true;
            }
            else{
                for (auto attachedUserWP : mAttachedUsers)
                {
                    if (auto attachedUser = attachedUserWP.lock())
                    {
                        if (attachedUser->getOwner() == mpCoreConfig->getNetworkGUID())
                        {
                            attachedUser->detachFromParentNode();
                            attachedUser->setPosition(ape::Vector3(0, 1.5, 0));
                            attachedUser->setOrientation(ape::Quaternion(0, 0, 1, 0));
                            app.updateinfo.switchOwner = true;
                            mNodeToSwitchOwner.push_back(attachedUser->getName());
                        }
                    }
                }
                mAttachedUsers.clear();
                app.updateinfo.usersAttached = false;
            }
            app.updateinfo.attachUsers = false;
        }
        if(app.updateinfo.setPosition){
            if(auto clone = mpSceneManager->getNode(app.rootOfSelected.second).lock()){
                auto owner = clone->getOwner();
                clone->setOwner(mpCoreConfig->getNetworkGUID());
                app.updateinfo.setPosition = false;
                app.updateinfo.switchOwner = true;
                mNodeToSwitchOwner.push_back(app.rootOfSelected.second);
                clone->setPosition(app.updateinfo.position);
            }
//            auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
//            auto tmInstance = app.mpTransformManager->getInstance(instance->getRoot());
//            auto transforms = app.mpTransformManager->getTransform(tmInstance);
//            auto worldTm = app.mpTransformManager->getWorldTransform(tmInstance);
//            transforms[3][0] = app.updateinfo.position.getX();
//            transforms[3][1] = app.updateinfo.position.getY();
//            transforms[3][2] = app.updateinfo.position.getZ();
//            app.mpTransformManager->setTransform(tmInstance, transforms);

        }
        if(app.updateinfo.setRotation){
            if(auto clone = mpSceneManager->getNode(app.rootOfSelected.second).lock()){
                auto owner = clone->getOwner();
                clone->setOwner(mpCoreConfig->getNetworkGUID());
                app.updateinfo.setRotation = false;
                app.updateinfo.switchOwner = true;
                mNodeToSwitchOwner.push_back(app.rootOfSelected.second);
                if (app.updateinfo.rotate) {
                    clone->setOrientation(clone->getOrientation() * app.updateinfo.orientation);
                    app.updateinfo.rotate = false;
                }
                else
                    clone->setOrientation(app.updateinfo.orientation);
            }
//            auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
//            auto tmInstance = app.mpTransformManager->getInstance(instance->getRoot());
//            auto transforms = app.mpTransformManager->getTransform(tmInstance);
//            auto setOri = math::details::TQuaternion<float>(app.updateinfo.orientation.getW(),
//                                              app.updateinfo.orientation.getX(),
//                                              app.updateinfo.orientation.getY(),
//                                              app.updateinfo.orientation.getZ());
//            auto orientation = setOri*transforms.toQuaternion();
//            math::mat4f newLocal(orientation);
//            newLocal[3][0] =  transforms[3][0];
//            newLocal[3][1] =  transforms[3][1];
//            newLocal[3][2] =  transforms[3][2];
//            app.mpTransformManager->setTransform(tmInstance, newLocal);

        }
        if(app.updateinfo.deleteSelected){
            if(auto clone = mpSceneManager->getNode(app.rootOfSelected.second).lock()){
                auto owner = clone->getOwner();
                clone->setOwner(mpCoreConfig->getNetworkGUID());
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                clone->setChildrenVisibility(false);
                clone->setVisible(false);
                app.updateinfo.switchOwner = true;
                mNodeToSwitchOwner.push_back(app.rootOfSelected.second);
                if (app.mpScene->hasEntity(app.boxEntity.first)) {
                    app.mpScene->remove(app.boxEntity.first);
                }
                app.boxEntity.second = 1000000.0;

               
            }
            app.updateinfo.deleteSelected = false;
        }
        if(app.updateinfo.drop){
            if(auto clone = mpSceneManager->getNode(app.rootOfSelected.second).lock()){
                clone->detachFromParentNode();
                clone->revertToInitalState();
                clone->setOwner(clone->getCreator());
            }
//            auto parentTm = app.mpTransformManager->getInstance(app.parentOfPicked);
//            auto instance = app.mpInstancesMap[app.rootOfSelected.second].mpInstance;
//            auto tmInstance = app.mpTransformManager->getInstance(instance->getRoot());
//            auto parentWorldMatrix = app.mpTransformManager->getTransform(tmInstance);
//            parentWorldMatrix[3][1] += 0.4;
//            parentWorldMatrix[3][2] += 2.0;
//            app.mpTransformManager->setTransform(tmInstance, parentWorldMatrix);
//            app.mpTransformManager->setParent(tmInstance,parentTm);
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
                    if(auto node = mpSceneManager->getNode(mUserName+mPostUserName).lock()){
                        if(auto clone = mpSceneManager->getNode(app.rootOfSelected.second).lock()){
                            clone->setInitalState();
                            clone->setOwner(mpCoreConfig->getNetworkGUID());
                            clone->setParentNode(node);
                            clone->setPosition(ape::Vector3(0.0,-0.4,-2.0));
                            clone->setOrientation(ape::Quaternion(1, 0, 0, 0));
                        }
                    }
                    app.parentOfPicked = app.mpTransformManager->getParent(tmInstance);
//                    auto name = app.asset[app.mpInstancesMap[app.rootOfSelected.second].assetName]->getName(app.parentOfPicked);
//                    app.mpTransformManager->setParent(tmInstance, camInstance);
//                    parentWorldMatrix = mat4f(1);
//                    parentWorldMatrix[3][1] = -0.4;
//                    parentWorldMatrix[3][2] = -2.0;
//                    app.mpTransformManager->setTransform(tmInstance, parentWorldMatrix);
                    app.updateinfo.pickedItem = app.updateinfo.selectedItem;
                    app.updateinfo.pickUp = false;
                }else if(app.updateinfo.pickUp){
                    if(auto clone = mpSceneManager->getNode(app.rootOfSelected.second).lock()){
                        auto pos = clone->getDerivedPosition();
                        auto ori = clone->getDerivedOrientation();
                        clone->detachFromParentNode();
                        clone->setPosition(pos);
                        clone->setOrientation(ori);
                        clone->setOwner(clone->getCreator());
                    }
                    
//                    auto parentTm = app.mpTransformManager->getInstance(app.parentOfPicked);
//                    parentWorldMatrix = app.mpTransformManager->getWorldTransform(parentTm);
//
//                    auto camWorldMatrix = app.mpTransformManager->getWorldTransform(tmInstance);
//                    app.mpTransformManager->setParent(tmInstance,parentTm);
//                    auto orientation = inverse(parentWorldMatrix.toQuaternion());
//                    auto invTrans = orientation*(camWorldMatrix[3].xyz-parentWorldMatrix[3].xyz);
//                    auto invertedOri = orientation* camWorldMatrix.toQuaternion();
//                    math::mat4f newLocal(invertedOri);
//                    //auto cam = &view->getCamera();
//                    newLocal[3].xyz = invTrans;
//                    app.mpTransformManager->setTransform(tmInstance, newLocal);
                    app.updateinfo.pickedItem = "";
                    app.updateinfo.pickUp = false;
                    scene->remove(app.boxEntity.first);
                }
            }
        }
        if (app.updateinfo.sendMessage) {
            if (auto textNode = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "_text").lock()))
            {
                textNode->setCaption(app.updateinfo.messageToSend);
            }
            app.updateinfo.sendMessage = false;

        }
        
        
        camera.setExposure(
                app.viewOptions.cameraAperture,
                1.0f / app.viewOptions.cameraSpeed,
                app.viewOptions.cameraISO);
       /* auto ibl = FilamentApp::get().getIBL();
        if (ibl) {
            ibl->getSkybox()->setLayerMask(0xff, app.viewOptions.skyboxEnabled ? 0xff : 0x00);
        }*/
        
        view->setColorGrading(nullptr);
        if(!app.updateinfo.leaveWait)
            processEventDoubleQueue();
        else {
            mEventDoubleQueue.swap();
            while (!mEventDoubleQueue.emptyPop())
                mEventDoubleQueue.pop();
        }
        if (!app.vrPlaying) {
            if (mPostUserName.find("_vlftTeacher") != std::string::npos) {
                if (auto node = mpSceneManager->getNode(mUserName + mPostUserName).lock()) {
                    vec3<float> camPos, camTarget, camUp;
                    mCamManipulator->getLookAt(&camPos, &camTarget, &camUp);
                    app.mainCamera->lookAt(camPos, camTarget, camUp);
                }
            }
            else if (auto node = mpSceneManager->getNode(mUserName + mPostUserName).lock()) {
                if (node->getOwner() == mpCoreConfig->getNetworkGUID()) {
                    vec3<float> camPos, camTarget, camUp;
                    mCamManipulator->getLookAt(&camPos, &camTarget, &camUp);
                    app.mainCamera->lookAt(camPos, camTarget, camUp);
                }
            }
        }

        if (app.playVR) {
            app.vrPlaying = true;
            app.playVR = false;
            auto camEntity = app.mainCamera->getEntity();
            auto camTm = app.mpTransformManager->getInstance(camEntity);
            if (app.mpTransforms.find("Camera_controller") != app.mpTransforms.end()) {
                app.mpTransformManager->setParent(camTm, app.mpTransforms["Camera_controller"]);
                auto filaTm = filament::math::mat4f(1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1);
                app.mpTransformManager->setTransform(camTm, filaTm);
            }
            std::string line;
            std::ifstream myfile(mpCoreConfig->getConfigFolderPath()+"/trajectory2.txt");
            if (myfile.is_open())
            {
                while (getline(myfile, line))
                {
                    size_t pos = 0;
                    std::vector<float> lineData = std::vector<float>();
                    std::string num;
                    //APE_LOG_DEBUG("READ " << line);
                    while ((pos = line.find(" ")) != std::string::npos && line.length() > 1) {
                        num = line.substr(0, pos);
                        lineData.push_back(std::stof(num));
                        
                        line.erase(0, pos + 1);
                    }
                    if(line != " ")
                        lineData.push_back(std::stof(line));
                    app.vrPos.push_back(ape::Vector3(lineData[0]+0.3, lineData[1], lineData[2]+3.6));
                    if (lineData.size() == 6) {
                        app.prevRot = ape::Vector3(lineData[5], -lineData[3], lineData[4] + 45);
                        app.prevPos = ape::Vector3(lineData[0] + 0.3, lineData[1], lineData[2] + 3.6);
                        if (app.vrPos.size() % 2 == 1) {
                            ape::Quaternion roll, pitch, yaw;
                            auto data1 = (lineData[5] + app.prevRot.getX())/2;
                            auto data2 = (-lineData[3] + app.prevRot.getY()) / 2;
                            auto data3 = (lineData[4]+45 + app.prevRot.getZ()) / 2;
                            roll.FromAngleAxis(data1 * F_PI / 180, ape::Vector3(0, 0, 1));
                            pitch.FromAngleAxis(data2 * F_PI / 180, ape::Vector3 (1, 0, 0));
                            yaw.FromAngleAxis(data3 * F_PI / 180, ape::Vector3(0, 1, 0));
                            app.vrRot.push_back(roll* yaw* pitch);

                            auto pos1 = (lineData[0]+0.3 + app.prevPos.getX()) / 2;
                            auto pos2 = (lineData[1] + app.prevPos.getY()) / 2;
                            auto pos3 = (lineData[2]+3.6 + app.prevPos.getZ()) / 2;
                            app.vrPos.push_back(ape::Vector3(pos1, pos2, pos3));
                        }
                        ape::Quaternion roll, pitch, yaw;
                        roll.FromAngleAxis(lineData[5] * F_PI / 180, ape::Vector3(0, 0, 1));
                        pitch.FromAngleAxis(-lineData[3] * F_PI / 180, ape::Vector3(1, 0, 0));
                        yaw.FromAngleAxis((lineData[4]+45) * F_PI / 180, ape::Vector3(0, 1, 0));
                        app.vrRot.push_back(roll*yaw*pitch);
                    }
                    else {
                        app.vrRot.push_back(ape::Quaternion(lineData[6], lineData[3], lineData[4],lineData[5]));
                    }
                }
                myfile.close();
            }

        }
      
    };

    auto postRender = [this](Engine* engine, View* view, Scene* scene, Renderer* renderer) {
        ;
    };
    auto userInput = [&](Engine* engine, View* view, Scene* scene, filament::camutils::Manipulator<float>* manipulator,int x, int y, SDL_Event event, int width, int height, double now){
        if(app.setManpipulator){
            manipulator->jumpToBookmark(mCameraBookmark);
            mCamManipulator = manipulator;
            app.setManpipulator = false;
            app.mainCamera = &view->getCamera();
            vec3<float> camPos, camTarget, camUp;
            manipulator->getLookAt(&camPos, &camTarget, &camUp);
            app.mainCamera->lookAt(camPos, camTarget, camUp);
        }
        app.updateinfo.now = now;
        filament::math::vec3<float> origin;
        filament::math::vec3<float> dir;
        filament::math::vec3<float> mtarget;
        filament::math::vec3<float> upward;
        float tnear =  app.mainCamera->getNear()/2;
        float tfar =  app.mainCamera->getCullingFar()/10;
        float fov =  app.mainCamera->getFieldOfViewInDegrees(filament::Camera::Fov::VERTICAL);
        auto keyCode = event.key.keysym.scancode;
        auto a = event.key.keysym.sym;
        auto keyState = SDL_GetModState();
       
        bool moved = false;
        if(mpSceneManager->getNode(mUserName+mPostUserName).lock() || app.updateinfo.inSettings){
            auto node = mpSceneManager->getNode(mUserName + mPostUserName).lock();
            if(app.updateinfo.isAdmin || app.updateinfo.inSettings || node->getOwner() == mpCoreConfig->getNetworkGUID())
                if((!app.updateinfo.inSettings || app.updateinfo.changedKey) || app.updateinfo.inRoom){
                    switch(event.type){
                        case SDL_MOUSEBUTTONDOWN:{
                            if(!app.updateinfo.inSettings){
                                if(event.button.button == SDL_BUTTON_LEFT && app.updateinfo.pickedItem == ""){
                                    auto viewport = view->getViewport();
                                    width = viewport.width;
                                    height = viewport.height;
                                    
                                    manipulator->getLookAt(&origin, &mtarget, &upward);
                                    if(mIsStudent){
                                        float diff = origin.y - 1.5;
                                        origin = math::vec3<float>(origin.x, 1.5, origin.z);
                                        mtarget = math::vec3<float>(mtarget.x, mtarget.y-diff, mtarget.z);
                                    }
                                    math::vec3<float>  gaze = normalize(mtarget - origin);
                                    math::vec3<float> right = normalize(cross(gaze, math::vec3<float>(0,1,0)));
                                    upward = cross(right, gaze);
                                    fov = fov * F_PI / 180.0;
                                    //APE_LOG_DEBUG(x << " x, " << y << " y");
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
                                        
                                        if(asset.first.find("characterModel") == std::string::npos)
                                        {
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
                                                                    .vertexCount(8)
                                                                    .bufferCount(1)
                                                                    .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
                                                                    .build(*engine);

                                                                app.boxIndexBuffer = IndexBuffer::Builder()
                                                                    .indexCount(boxIndCount)
                                                                    .bufferType(IndexBuffer::IndexType::UINT)
                                                                    .build(*engine);

                                                                app.boxVertexBuffer->setBufferAt(*engine, 0, VertexBuffer::BufferDescriptor(
                                                                                    app.boxVerts, 8 * sizeof(float3), nullptr));
                                                                
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
                                                                    auto boxTM = app.mpTransformManager->getInstance(app.boxEntity.first);
                                                                    app.mpTransformManager->setParent(boxTM, tmInstance);
                                                                    auto boxWorld = inverse(app.mpTransformManager->getWorldTransform(boxTM));
                                                                    app.mpTransformManager->setTransform(boxTM, boxWorld);
                                                                }
                                                                   
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
                                                        std::string glftNodeName = idGltfMap[sceneNodeName];
                                                        std::string cloneName = glftNodeName.substr(0,glftNodeName.find_last_of("."));
                                                        std::string nodeName = glftNodeName.substr(glftNodeName.find_last_of(".")+1);
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
                                if (auto userGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "characterModel").lock())) {
                                    userGeometry->playAnimation("3");
                                }
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
                                //bool moved = false;
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
                                if (keyCode == mKeyMap["o"]) {
                                    if(!app.vrPlaying)
                                        app.playVR = true;
                                    else {
                                        app.vrCnt = 10000000;
                                    }
                                }
                                if(moved){
                                    
                                    if(app.animationData.animatedKey){
                                        app.animationData.keyStartTime = now;
                                        app.animationData.animatedKey = false;
                                        app.animationData.keysDown = true;
                                    }
                                    if(keyState == KMOD_RSHIFT || keyState == KMOD_LSHIFT){
                                        if(app.animationData.keyCurrentAnimation == 0){
                                            auto animator = app.instances[mUserName+mPostUserName+"characterModel"][0]->getAnimator();
                                            animator->applyAnimation(0, 0);
                                            if (auto userGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "characterModel").lock())) {
                                                userGeometry->stopAnimation("0");
                                            }
                                        }
                                        app.animationData.keyCurrentAnimation = 1;
                                        if (auto userGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "characterModel").lock())) {
                                            userGeometry->playAnimation("1");
                                        }
                                    }
                                    else{
                                        if(app.animationData.keyCurrentAnimation == 1){
                                            auto animator = app.instances[mUserName+mPostUserName+"characterModel"][0]->getAnimator();
                                            animator->applyAnimation(1, 0);
                                            if (auto userGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "characterModel").lock())) {
                                                userGeometry->stopAnimation("1");
                                            }
                                        }
                                        app.animationData.keyCurrentAnimation = 0;
                                        if (auto userGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "characterModel").lock())) {
                                            userGeometry->playAnimation("0");
                                        }
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
                                if(keyCode == mKeyMap["f2"]){
                                    takeScreenshot();
                                }
                                if(keyCode == mKeyMap["f3"]){
                                    if(!app.updateinfo.screenCaptureOn){
                                        startScreenCast();
                                    }
                                    else {
                                        auto screenCastThread = std::thread(&FilamentApplicationPlugin::stopScreenCast, this);
                                        screenCastThread.detach();
                                    }
                                   
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
        }
        if(!app.vrPlaying){
            if (!app.updateinfo.inSettings && app.updateinfo.inRoom) {
                vec3<float> camPos, camTarget, camUp;
                manipulator->getLookAt(&camPos, &camTarget, &camUp);
                if (!app.updateinfo.isAdmin) {
                    if (auto node = mpSceneManager->getNode(mUserName + mPostUserName).lock()) {
                        if (node->getOwner() == mpCoreConfig->getNetworkGUID()) {
                            //mpUserInputMacro->getUserNode();
                            //node->setPosition(ape::Vector3(camPos.x, camPos.y, camPos.z-1));
                            auto modelMatrix = filament::math::mat4f::lookAt(camPos, camTarget, camUp);
                            modelMatrix[3][0] = modelMatrix[3][1] = modelMatrix[3][2] = 0;
                            auto modelQuat = modelMatrix.toQuaternion();
                            //if(auto camNode = mpSceneManager->getNode(mUserName + mPostUserName+"_cam").lock()){
                            node->setPosition(ape::Vector3(camPos.x, camPos.y, camPos.z));
                            node->setOrientation(ape::Quaternion(modelQuat.w, modelQuat.x, modelQuat.y, modelQuat.z));
                            //}

                            for (auto &x : app.updateinfo.playerMapPositions) {
                                if (auto otherplayer = mpSceneManager->getNode(x.first).lock()) {
                                    auto playerPos = otherplayer->getDerivedPosition();
                                    app.updateinfo.playerMapPositions[x.first][0] = playerPos.getX() - camPos.x;
                                    app.updateinfo.playerMapPositions[x.first][1] = camPos.z - playerPos.getZ();
                                }
                            }

                        }
                    }
                }
                else {
                    if (auto node = mpSceneManager->getNode(mUserName + mPostUserName).lock()) {
                        //node->setPosition(ape::Vector3(camPos.x, camPos.y, camPos.z-1));
                        auto modelMatrix = filament::math::mat4f::lookAt(camPos, camTarget, camUp);
                        modelMatrix[3][0] = modelMatrix[3][1] = modelMatrix[3][2] = 0;
                        auto modelQuat = modelMatrix.toQuaternion();
                        //if(auto camNode = mpSceneManager->getNode(mUserName + mPostUserName+"_cam").lock()){
                        node->setPosition(ape::Vector3(camPos.x, camPos.y, camPos.z));
                        node->setOrientation(ape::Quaternion(modelQuat.w, modelQuat.x, modelQuat.y, modelQuat.z));
                        //}
                        for (auto& x : app.updateinfo.playerMapPositions) {
                            if (auto otherplayer = mpSceneManager->getNode(x.first).lock()) {
                                auto playerPos = otherplayer->getDerivedPosition();
                                app.updateinfo.playerMapPositions[x.first][0] = playerPos.getX() - camPos.x;
                                app.updateinfo.playerMapPositions[x.first][1] = camPos.z - playerPos.getZ();

                            }
                        }
                    }
                }
            }
            std::vector<std::string> toRemove = std::vector<std::string>();
            for (auto playerName : app.updateinfo.playerNamePositions) {
                if (app.playerNamesToShow.find(playerName.first) == app.playerNamesToShow.end())
                    toRemove.push_back(playerName.first);
            }
            for (auto removePlayer : toRemove) {
                app.updateinfo.playerNamePositions.erase(removePlayer);
            }
            for (auto nodeName : app.playerNamesToShow) {

                float tnear = app.mainCamera->getNear() / 2;
                float fov = app.mainCamera->getFieldOfViewInDegrees(filament::Camera::Fov::VERTICAL);
                fov = fov * F_PI / 180.0;
                auto otherV = view->getViewport();
                auto owidth = otherV.width;
                auto oheight = otherV.height;
                auto viewport = app.view->getViewport();
                auto width = viewport.width;
                auto height = viewport.height;

                // Compute the tangent of the field-of-view angle as well as the aspect ratio.
                float tangent = tan(fov / 2.0);
                float aspectRatio = (float)width / height;
                auto& nodeWorldTransforms = app.mpTransformManager->getWorldTransform(app.mpTransforms[nodeName]);
                auto camPos = app.mainCamera->getModelMatrix();

                auto worldToLocalTM = inverse(camPos) * nodeWorldTransforms;
                auto xPrime = worldToLocalTM[3][0];
                auto yPrime = worldToLocalTM[3][1];
                auto zPrime = -worldToLocalTM[3][2];

                auto m = tangent * tnear;
                auto mPrime = tangent * (tnear + zPrime);
                auto w = tangent * tnear * aspectRatio;
                auto wPrime = tangent * (tnear + zPrime) * aspectRatio;

                auto asd = m / w;
                auto x = xPrime * w / wPrime;
                auto wScale = width / w / 2;
                x = x * wScale + width / 2;

                auto y = yPrime * m / mPrime;
                auto hScale = height / m / 2;
                y = -y * hScale + height / 2;

                //APE_LOG_DEBUG(zPrime << " zPrime");
                if (zPrime > 21 || zPrime < 0.65 || x < 0 || y < 0 || x > width || y > height) {
                    if (app.updateinfo.playerNamePositions.find(nodeName.substr(0, nodeName.find("_vlft"))) != app.updateinfo.playerNamePositions.end())
                        app.updateinfo.playerNamePositions.erase(nodeName.substr(0, nodeName.find("_vlft")));
                }
                else {
                    app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][0] = x;
                    float yOffset = 10;
#ifdef __APPLE__
                    yOffset = 35;
#endif
                    app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][1] = y - yOffset - 200 / zPrime;
                    app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][2] = 1.1 + (5 - zPrime) / 5;
                    app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][3] = width;
                    app.updateinfo.playerNamePositions[nodeName.substr(0, nodeName.find("_vlft"))][4] = height;
                }
            }
        }
        
        
    };

    auto animate = [this](Engine* engine, View* view, double now) {
        
        if(now - logoAnimTime > 0.02 && !app.updateinfo.inRoom){
            //float diff = (now - logoAnimTime)/0.02;
            if(auto logo = mpSceneManager->getNode("VLFTlogo").lock()){
                if(logo->getChildrenVisibility())
                    logo->rotate(-0.0034f, ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::LOCAL);
            }
            logoAnimTime = now;
        }

        if (now - app.vrAnimTime > 0.07 && app.vrPlaying) {
            int inc = (int((now - app.vrAnimTime) * 100)) / 7;
            if (app.vrCnt != 0) {
                app.vrCnt += inc;
                app.vrAnimTime = app.vrAnimTime + inc * 0.07;
            }
            else {
                app.vrCnt = 500;
                app.vrAnimTime = now;
            }
            APE_LOG_DEBUG(app.vrCnt);
            if (app.vrCnt < app.vrPos.size()) {
                if (auto camNode = mpSceneManager->getNode("Camera_controller").lock()) {
                    camNode->setPosition(app.vrPos[app.vrCnt]);
                    camNode->setOrientation(app.vrRot[app.vrCnt]);
                }
            }
            


            app.vrCnt++;
            if (app.vrCnt >= app.vrPos.size()) {
                app.vrPlaying = false;
                app.vrCnt = 0;
                app.vrPos.clear();
                app.vrRot.clear();
            }
                
        }
        app.updateinfo.now = now;
        if(app.updateinfo.IsStopClicked && app.updateinfo.pauseTime > 0){
            for (auto animatedNodeName : mAnimatedNodeNames)
            {
                if (auto node = mpSceneManager->getNode(animatedNodeName).lock())
                {
                    auto prevParent = node->getParentNode();
                    node->revertToInitalState();
                    std::string parentsName = "";
                    if(auto parent = node->getParentNode().lock())
                    {
                        parentsName = parent->getName();
                        node->setParentNode(parent);
                    }
                    if(parentsName == ""){
                        if(prevParent.lock())
                            node->setParentNode(prevParent);
                        node->detachFromParentNode();
                    }
                       
                }
            }
            app.updateinfo.IsStopClicked=false;
            mPlayedAnimations = 0;
            app.updateinfo.IsPlayClicked = false;
            app.updateinfo.isPlayRunning = false;
            app.updateinfo.pauseTime = 0;
            for (auto spaghettiLineName : mspaghettiLineNames)
            {
                if(mpSceneManager->getEntity(spaghettiLineName.first).lock())
                    mpSceneManager->deleteEntity(spaghettiLineName.first);
            }
            for (auto spaghettiNodeName : mSpaghettiNodeNames)
            {
                if(mpSceneManager->getNode(spaghettiNodeName).lock())
                    mpSceneManager->deleteNode(spaghettiNodeName);
            }
            for (auto stateGeometryName : mstateGeometryNames)
            {
                if(mpSceneManager->getEntity(stateGeometryName).lock())
                    mpSceneManager->deleteEntity(stateGeometryName);
            }
            for (auto stateNodeName : mstateNodeNames)
            {
                if(mpSceneManager->getNode(stateNodeName).lock())
                    mpSceneManager->deleteNode(stateNodeName);
            }
            for (auto animatedNodeName : mAnimatedNodeNames)
            {
                if (auto node = mpSceneManager->getNode(animatedNodeName).lock())
                {
                    node->setOwner(node->getCreator());
                }
            }
            mSpaghettiNodeNames.clear();
            mstateNodeNames.clear();
            mstateGeometryNames.clear();
        }
        else if(app.updateinfo.IsStopClicked)
            app.updateinfo.IsStopClicked = false;
        if(!app.updateinfo.isPlayRunning && app.updateinfo.IsPlayClicked){
            app.updateinfo.StartTime = now-app.updateinfo.pauseTime/1000;
            mPlayedAnimations = 0;
            playAnimations(now);
        }
        else if(app.updateinfo.IsPlayClicked){
            playAnimations(now);
        }
        for (auto &playerAnim : app.playerAnimations) {
            if (app.updateinfo.inRoom && app.mpInstancesMap.find(playerAnim.first) != app.mpInstancesMap.end()) {
                auto animator = app.mpInstancesMap[playerAnim.first].mpInstance->getAnimator();
                double timeDiff;
                if (!playerAnim.second.animatedClick) {
                    timeDiff = now - playerAnim.second.mouseStartTime;

                    if (timeDiff > animator->getAnimationDuration(3) && playerAnim.second.mouseDown) {
                        while (timeDiff > animator->getAnimationDuration(3)) {
                            playerAnim.second.mouseStartTime += animator->getAnimationDuration(3);
                            timeDiff = now - playerAnim.second.mouseStartTime;
                        }
                    }
                    if (timeDiff <= animator->getAnimationDuration(3)) {
                        animator->applyAnimation(3, timeDiff);
                    }
                    else {
                        playerAnim.second.animatedClick = true;
                        animator->applyAnimation(3, 0);
                    }
                }
                if (!playerAnim.second.animatedKey) {
                    timeDiff = now - playerAnim.second.keyStartTime;
                   /* if (!playerAnim.second.keysDown) {
                        playerAnim.second.animatedKey = true;
                        animator->applyAnimation(3, 0);
                    }*/
                        auto cnt = animator->getAnimationCount();
                        if (playerAnim.second.keyCurrentAnimation <= cnt) {
                            if (timeDiff > animator->getAnimationDuration(playerAnim.second.keyCurrentAnimation) && playerAnim.second.keysDown) {
                                while (timeDiff > animator->getAnimationDuration(playerAnim.second.keyCurrentAnimation)) {
                                    playerAnim.second.keyStartTime += animator->getAnimationDuration(playerAnim.second.keyCurrentAnimation);
                                    timeDiff = now - playerAnim.second.keyStartTime;
                                }
                            }
                            if (timeDiff <= animator->getAnimationDuration(playerAnim.second.keyCurrentAnimation)) {
                                animator->applyAnimation(playerAnim.second.keyCurrentAnimation, timeDiff);
                            }
                            else {
                                playerAnim.second.animatedKey = true;
                                animator->applyAnimation(3, 0);
                            }
                        }

                }
                animator->updateBoneMatrices();
            }
        }
        if(app.updateinfo.inRoom && app.instances.find(mUserName+mPostUserName+"characterModel") != app.instances.end() && app.instanceCount[mUserName + mPostUserName + "characterModel"] > 0){
            auto animator = app.instances[mUserName+mPostUserName+"characterModel"][0]->getAnimator();
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
                    animator->applyAnimation(0, 0);
                    if (auto userGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "characterModel").lock())) {
                        userGeometry->stopAnimation("3");
                    }
                }
            }
            if(!app.animationData.animatedKey){
                timeDiff = now - app.animationData.keyStartTime;
                if(!app.animationData.keysDown){
                    app.animationData.animatedKey = true;
                    animator->applyAnimation(0, 0);
                    if (auto userGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "characterModel").lock())) {
                        std::string anim = std::to_string(app.animationData.keyCurrentAnimation);
                        userGeometry->stopAnimation(anim);
                    }
                   /* if (auto userGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "characterModel").lock())) {
                        userGeometry->stopAnimation("1");
                    }*/
                }
                else{
                    auto cnt = animator->getAnimationCount();
                    if(app.animationData.keyCurrentAnimation <= cnt){
                        if(timeDiff > animator->getAnimationDuration(app.animationData.keyCurrentAnimation) && app.animationData.keysDown){
                            app.animationData.keyStartTime +=  animator->getAnimationDuration(app.animationData.keyCurrentAnimation);
                            timeDiff = now - app.animationData.keyStartTime;
                        }
                        if(timeDiff <= animator->getAnimationDuration(app.animationData.keyCurrentAnimation)){
                            animator->applyAnimation(app.animationData.keyCurrentAnimation, timeDiff);
                        }
                    }
                }
                
            }
            animator->updateBoneMatrices();
        }
        app.currentTime = now;
    };
    
    FilamentApp& filamentApp = FilamentApp::get();
    filamentApp.animate(animate);
    filamentApp.resize(resize);
    filamentApp.setDropHandler([&] (std::string path) {
        loadAsset(path);
        loadResources(path);
    });
    app.config.splitView = false;
    app.config.cameraMode = filament::camutils::Mode::FREE_FLIGHT;
    app.config.title = "VLFT gamification";
    //app.config.iblDirectory = "";  
    filamentApp.run(app.config, setup, cleanup, gui, preRender, postRender, userInput,1280,720);
    
   
    APE_LOG_FUNC_LEAVE();
    //return 0;
}

void ape::FilamentApplicationPlugin::Stop()
{
    APE_LOG_FUNC_ENTER()
    APE_LOG_DEBUG("LEFT ROOM")
    if(app.updateinfo.inRoom){
        if (auto textNode = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->getEntity(mUserName + mPostUserName + "_text").lock()))
        {
            textNode->setCaption(mUserName + " left the room");
        }
        for (auto attachedUserWP : mAttachedUsers)
        {
            if (auto attachedUser = attachedUserWP.lock())
            {
                if (attachedUser->getOwner() == mpCoreConfig->getNetworkGUID())
                {
                    attachedUser->setOwner(attachedUser->getCreator());
                }
            }
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    APE_LOG_FUNC_LEAVE();
}

void ape::FilamentApplicationPlugin::Suspend()
{

}

void ape::FilamentApplicationPlugin::Restart()
{

}
