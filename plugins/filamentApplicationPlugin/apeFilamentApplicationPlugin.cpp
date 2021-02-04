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
    mpVlftImgui->init(true);
    app.instances.resize(1);
    parseJson();
    initFilament();
	APE_LOG_FUNC_LEAVE();
}

ape::FilamentApplicationPlugin::~FilamentApplicationPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentApplicationPlugin::eventCallBack(const ape::Event& event)
{
    if(event.type == ape::Event::Type::GEOMETRY_FILE_FILENAME)
        std::cout << "geometry file " << event.type  << std::endl;
    else if((event.type == ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRY))
        std::cout << "geometry clone source" << event.type<< std::endl;
    else if((event.type == ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME))
        std::cout << "geometry clone group" << event.type<< std::endl;
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
				if (event.type == ape::Event::Type::NODE_CREATE)
				{
                
					auto filamentEntity = app.mpEntityManager->create();
					app.mpTransformManager->create(filamentEntity);
					auto filamentTransform = app.mpTransformManager->getInstance(filamentEntity);
                    app.mpTransforms[nodeName] = filamentTransform;
				}
				else
				{
					switch (event.type)
					{
					case ape::Event::Type::NODE_PARENTNODE:
					{
						;
					}
						break;
					case ape::Event::Type::NODE_DETACH:
					{
						;
					}
						break;
					case ape::Event::Type::NODE_POSITION:
					{
                            auto nodeTransform = node->getModelMatrix().transpose();
                            auto filamentTransform = filament::math::mat4f(
                                nodeTransform[0][0], nodeTransform[0][1], nodeTransform[0][2], nodeTransform[0][3],
                                nodeTransform[1][0], nodeTransform[1][1], nodeTransform[1][2], nodeTransform[1][3],
                                nodeTransform[2][0], nodeTransform[2][1], nodeTransform[2][2], nodeTransform[2][3],
                                nodeTransform[3][0]/100, nodeTransform[3][1]/100, nodeTransform[3][2]/100, nodeTransform[3][3]);
                            app.mpTransformManager->setTransform(app.mpTransforms[nodeName], filamentTransform);
                        
                       
					}
						break;
					case ape::Event::Type::NODE_ORIENTATION:
					{
                            auto nodeTransform = node->getModelMatrix().transpose();
                            auto filamentTransform = filament::math::mat4f(
                                nodeTransform[0][0], nodeTransform[0][1], nodeTransform[0][2], nodeTransform[0][3],
                                nodeTransform[1][0], nodeTransform[1][1], nodeTransform[1][2], nodeTransform[1][3],
                                nodeTransform[2][0], nodeTransform[2][1], nodeTransform[2][2], nodeTransform[2][3],
                                nodeTransform[3][0]/100, nodeTransform[3][1]/100, nodeTransform[3][2]/100, nodeTransform[3][3]);
                            app.mpTransformManager->setTransform(app.mpTransforms[nodeName], filamentTransform);
					}
						break;
					case ape::Event::Type::NODE_SCALE:
						;
						break;
					case ape::Event::Type::NODE_CHILDVISIBILITY:
						;
						break;
					case ape::Event::Type::NODE_VISIBILITY:
					{
						;
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
					auto filamentAssetRootEntity = app.mpLoadedAssets[fileName]->getRoot();
					auto filamentAssetRootTransform = app.mpTransformManager->getInstance(filamentAssetRootEntity);
					app.mpTransformManager->setParent(filamentAssetRootTransform, app.mpTransforms[parentNodeName]);
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
						if (fileExtension == ".gltf")
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
									std::stringstream resourceLocationPath;
									resourceLocationPath << APE_SOURCE_DIR << fileName;
									filePath << resourceLocationPath.str();
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
                            app.instances.resize(1);
                            app.asset[geometryName] = app.loader->createInstancedAsset(buffer.data(), buffer.size(), app.instances.data(), 1);
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
								resourceConfiguration.recomputeBoundingBoxes = false;
                                if(app.resourceLoader)
                                    delete app.resourceLoader;
								app.resourceLoader = new gltfio::ResourceLoader(resourceConfiguration);
								if (app.resourceLoader->loadResources(app.asset[geometryName]))
								{
									APE_LOG_DEBUG("resources load OK");
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
                        app.mpScene->removeEntities(app.asset[sourceFileName]->getEntities(), app.asset[sourceFileName]->getEntityCount());
                        auto filamentAssetRootEntity = app.instancesMap[event.subjectName]->getRoot();
                        auto filamentAssetRootTransform = app.mpTransformManager->getInstance(filamentAssetRootEntity);
                        app.mpTransformManager->setParent(filamentAssetRootTransform, app.mpTransforms[parentNodeName]);
                        app.mpScene->addEntities(app.asset[sourceFileName]->getEntities(), app.asset[sourceFileName]->getEntityCount());
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRY:
                    {
                        ;
                    }
                    break;
                    case ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME:
                    {
                        if(app.instancesMap.size() == 0){
                            app.instancesMap[event.subjectName] = app.instances[0];
                        }
                        else{
                              FilamentInstance* instance = app.loader->createInstance(app.asset[sourceFileName]);
                              app.instancesMap[event.subjectName] = instance;
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
        app.instances.resize(3);
        if (filename.getExtension() == "glb") {
            app.asset[filename] = app.loader->createAssetFromBinary(buffer.data(), buffer.size());
        } else {
            app.asset[filename] = app.loader->createInstancedAsset(buffer.data(), buffer.size(), app.instances.data(), app.instances.size());
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
        configuration.recomputeBoundingBoxes = app.recomputeAabb;
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
        app.names = new NameComponentManager(EntityManager::get());
        

        app.materials = (app.materialSource == GENERATE_SHADERS) ?
                createMaterialGenerator(engine) : createUbershaderLoader(engine);
        app.loader = AssetLoader::create({engine, app.materials, app.names });
        app.mainCamera = &view->getCamera();
        app.mainCamera->lookAt(math::float3(0,-0.025,-0.05), math::float3(0,0,0));
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

    auto animate = [this](Engine* engine, View* view, double now) {
//        app.resourceLoader->asyncUpdateLoad();
//
//        // Add renderables to the scene as they become ready.
//        app.viewer->populateScene(app.asset, !app.actualSize);
//
//        app.viewer->applyAnimation(now);
        ;
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
        mpVlftImgui->update();
        FilamentApp::get().setSidebarWidth(0);
    };

    auto preRender = [this](Engine* engine, View* view, Scene* scene, Renderer* renderer) {
        auto& rcm = engine->getRenderableManager();
        auto instance = rcm.getInstance(app.scene.groundPlane);
        rcm.setLayerMask(instance,
                0xff, app.viewOptions.groundPlaneEnabled ? 0xff : 0x00);
        
       app.currentCamera = 0;
        filament::Camera& camera = view->getCamera();
        
        camera.setExposure(
                app.viewOptions.cameraAperture,
                1.0f / app.viewOptions.cameraSpeed,
                app.viewOptions.cameraISO);
        auto ibl = FilamentApp::get().getIBL();
        if (ibl) {
            ibl->getSkybox()->setLayerMask(0xff, app.viewOptions.skyboxEnabled ? 0xff : 0x00);
        }
//        if(app.asset)
//            app.mpScene->addEntities(app.asset->getEntities(), app.asset->getEntityCount());

        // we have to clear because the side-bar doesn't have a background, we cannot use
        // a skybox on the ui scene, because the ui view is always full screen.
//        renderer->setClearOptions({
//                .clearColor = { inverseTonemapSRGB(app.viewOptions.backgroundColor), 1.0f },
//                .clear = true
//        });
        view->setColorGrading(nullptr);
        processEventDoubleQueue();
    };

    auto postRender = [this](Engine* engine, View* view, Scene* scene, Renderer* renderer) {
        ;
    };

    FilamentApp& filamentApp = FilamentApp::get();
    //filamentApp.animate(animate);
    filamentApp.resize(resize);

    filamentApp.setDropHandler([&] (std::string path) {
        loadAsset(path);
        loadResources(path);
    });
    app.config.cameraMode = filament::camutils::Mode::FREE_FLIGHT;
    filamentApp.run(app.config, setup, cleanup, gui, preRender, postRender);

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
