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
	mEventDoubleQueue.push(event);
}

void ape::FilamentApplicationPlugin::processEventDoubleQueue()
{
	mEventDoubleQueue.swap();
//	while (!mEventDoubleQueue.emptyPop())
//	{
//		ape::Event event = mEventDoubleQueue.front();
//		try
//		{
//		if (event.group == ape::Event::Group::NODE)
//		{
//			if (auto node = mpSceneManager->getNode(event.subjectName).lock())
//			{
//				std::string nodeName = node->getName();
//				if (event.type == ape::Event::Type::NODE_CREATE)
//				{
////					auto filamentEntity = mpFilamentEntityManager->create();
////					mpFilamentTransformManager->create(filamentEntity);
////					auto filamentTransform = mpFilamentTransformManager->getInstance(filamentEntity);
////					mpFilamentTransforms[nodeName] = filamentTransform;
//				}
//				else
//				{
//					switch (event.type)
//					{
//					case ape::Event::Type::NODE_PARENTNODE:
//					{
//						;
//					}
//						break;
//					case ape::Event::Type::NODE_DETACH:
//					{
//						;
//					}
//						break;
//					case ape::Event::Type::NODE_POSITION:
//					{
//						auto nodeTransform = node->getModelMatrix().transpose();
//						auto filamentTransform = filament::math::mat4f(
//							nodeTransform[0][0], nodeTransform[0][1], nodeTransform[0][2], nodeTransform[0][3],
//							nodeTransform[1][0], nodeTransform[1][1], nodeTransform[1][2], nodeTransform[1][3],
//							nodeTransform[2][0], nodeTransform[2][1], nodeTransform[2][2], nodeTransform[2][3],
//							nodeTransform[3][0], nodeTransform[3][1], nodeTransform[3][2], nodeTransform[3][3]);
////						mpFilamentTransformManager->setTransform(mpFilamentTransforms[nodeName], filamentTransform);
//					}
//						break;
//					case ape::Event::Type::NODE_ORIENTATION:
//					{
//						auto nodeTransform = node->getModelMatrix().transpose();
//						auto filamentTransform = filament::math::mat4f(
//							nodeTransform[0][0], nodeTransform[0][1], nodeTransform[0][2], nodeTransform[0][3],
//							nodeTransform[1][0], nodeTransform[1][1], nodeTransform[1][2], nodeTransform[1][3],
//							nodeTransform[2][0], nodeTransform[2][1], nodeTransform[2][2], nodeTransform[2][3],
//							nodeTransform[3][0], nodeTransform[3][1], nodeTransform[3][2], nodeTransform[3][3]);
////						mpFilamentTransformManager->setTransform(mpFilamentTransforms[nodeName], filamentTransform);
//					}
//						break;
//					case ape::Event::Type::NODE_SCALE:
//						;
//						break;
//					case ape::Event::Type::NODE_CHILDVISIBILITY:
//						;
//						break;
//					case ape::Event::Type::NODE_VISIBILITY:
//					{
//						;
//					}
//						break;
//					case ape::Event::Type::NODE_FIXEDYAW:
//						;
//						break;
//					case ape::Event::Type::NODE_INHERITORIENTATION:
//						;
//						break;
//					case ape::Event::Type::NODE_INITIALSTATE:
//						;
//						break;
//					case ape::Event::Type::NODE_SHOWBOUNDINGBOX:
//						;
//						break;
//					case ape::Event::Type::NODE_HIDEBOUNDINGBOX:
//						;
//						break;
//					}
//				}
//			}
//			else if (event.type == ape::Event::Type::NODE_DELETE)
//			{
//				;
//			}
//		}
//		else if (event.group == ape::Event::Group::GEOMETRY_FILE)
//		{
//			if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
//			{
//				std::string geometryName = geometryFile->getName();
//				std::string fileName = geometryFile->getFileName();
//				std::string parentNodeName = "";
//				if (auto parentNode = geometryFile->getParentNode().lock())
//					parentNodeName = parentNode->getName();
//				switch (event.type)
//				{
//				case ape::Event::Type::GEOMETRY_FILE_CREATE:
//					;
//					break;
//				case ape::Event::Type::GEOMETRY_FILE_PARENTNODE:
//				{
////					auto filamentAssetRootEntity = mpFilamentLoadedAssets[fileName]->getRoot();
////					auto filamentAssetRootTransform = mpFilamentTransformManager->getInstance(filamentAssetRootEntity);
////					mpFilamentTransformManager->setParent(filamentAssetRootTransform, mpFilamentTransforms[parentNodeName]);
//				}
//					break;
//				case ape::Event::Type::GEOMETRY_FILE_FILENAME:
//				{
//
//					if (fileName.find_first_of(".") != std::string::npos)
//					{
//						std::string fileExtension = fileName.substr(fileName.find_last_of("."));
//						if (fileExtension == ".mesh")
//						{
//							;
//						}
//						if (fileExtension == ".glb")
//						{
//							;
//						}
//						if (fileExtension == ".gltf")
//						{
//							std::stringstream filePath;
//							std::size_t found = fileName.find(":");
//							if (found != std::string::npos)
//							{
//								filePath << fileName;
//							}
//							else
//							{
//								std::string separator = "../";
//								found = fileName.find(separator);
//								if (found != std::string::npos)
//								{
//									struct stat info;
//									if (stat(fileName.c_str(), &info) == -1)
//									{
//										auto found_it = std::find_end(fileName.begin(), fileName.end(), separator.begin(), separator.end());
//										size_t foundPos = found_it - fileName.begin();
//										std::stringstream resourceLocationPath;
//										resourceLocationPath << APE_SOURCE_DIR << fileName.substr(foundPos + 2);
//										filePath << resourceLocationPath.str();
//									}
//									else
//									{
//										filePath << fileName;
//									}
//								}
//								else
//								{
//									std::stringstream resourceLocationPath;
//									resourceLocationPath << APE_SOURCE_DIR << fileName;
//									filePath << resourceLocationPath.str();
//								}
//							}
//							std::ifstream in(filePath.str().c_str(), std::ifstream::ate | std::ifstream::binary);
//							long contentSize = static_cast<long>(in.tellg());
//							if (contentSize <= 0)
//							{
//								APE_LOG_DEBUG("Unable to open " << filePath.str());
//							}
//							else
//							{
//								APE_LOG_DEBUG(filePath.str() << " was opened");
//							}
//							std::ifstream inBin(filePath.str().c_str(), std::ifstream::binary | std::ifstream::in);
//							std::vector<uint8_t> buffer(static_cast<unsigned long>(contentSize));
//							if (!inBin.read((char*)buffer.data(), contentSize))
//							{
//								APE_LOG_DEBUG("Unable to read " << filePath.str());
//							}
//							else
//							{
//								APE_LOG_DEBUG(filePath.str() << " was read");
//							}
//                            app.asset = app.loader->createAssetFromJson(buffer.data(), buffer.size());
// 							buffer.clear();
//							buffer.shrink_to_fit();
//							if (!app.asset)
//							{
//								APE_LOG_DEBUG("Unable to parse " << filePath.str());
//							}
//							else
//							{
//
//								APE_LOG_DEBUG(filePath.str() << " was parsed");
//                                app.automationSpec = AutomationSpec::generateDefaultTestCases();
//                                app.automationEngine = new AutomationEngine(app.automationSpec, &app.viewer->getSettings());
//                                app.materials = (app.materialSource == GENERATE_SHADERS) ?
//                                        createMaterialGenerator(engine) : createUbershaderLoader(engine);
//                                app.loader = AssetLoader::create({engine, app.materials, app.names });
//								gltfio::ResourceConfiguration resourceConfiguration;
//								resourceConfiguration.engine = mpFilamentEngine;
//								auto resourceLocation = filePath.str();
//								resourceConfiguration.gltfPath = resourceLocation.c_str();
//								resourceConfiguration.normalizeSkinningWeights = true;
//								resourceConfiguration.recomputeBoundingBoxes = false;
//								auto filamentResourceLoader = new gltfio::ResourceLoader(resourceConfiguration);
//								if (filamentResourceLoader->loadResources(asset))
//								{
//									APE_LOG_DEBUG("resources load OK");
//									mpFilamentScene->addEntities(asset->getEntities(), asset->getEntityCount());
//									mpFilamentLoadedAssets[fileName] = asset;
//								}
//								else
//								{
//									APE_LOG_DEBUG("resources load FAILED");
//								}
//								delete filamentResourceLoader;
//							}
//						}
//					}
//				}
//					break;
//				case ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES:
//				{
//					;
//				}
//				break;
//				case ape::Event::Type::GEOMETRY_FILE_EXPORT:
//				{
//					;
//				}
//				break;
//				case ape::Event::Type::GEOMETRY_FILE_MATERIAL:
//				{
//					;
//				}
//				break;
//				case ape::Event::Type::GEOMETRY_FILE_VISIBILITY:
//				{
//					;
//				}
//				break;
//				}
//			}
//			else if (event.type == ape::Event::Type::GEOMETRY_FILE_DELETE)
//			{
//				;
//			}
//		}
//		else if (event.group == ape::Event::Group::LIGHT)
//		{
//			if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->getEntity(event.subjectName).lock()))
//			{
//				if (event.type == ape::Event::Type::LIGHT_CREATE)
//				{
//					;
//				}
//				else
//				{
//					switch (event.type)
//					{
//					case ape::Event::Type::LIGHT_ATTENUATION:
//						break;
//					case ape::Event::Type::LIGHT_DIFFUSE:
//						break;
//					case ape::Event::Type::LIGHT_DIRECTION:
//						break;
//					case ape::Event::Type::LIGHT_POWER:
//						break;
//					case ape::Event::Type::LIGHT_SPECULAR:
//						break;
//					case ape::Event::Type::LIGHT_SPOTRANGE:
//						break;
//					case ape::Event::Type::LIGHT_TYPE:
//						break;
//					case ape::Event::Type::LIGHT_PARENTNODE:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::LIGHT_DELETE:
//						;
//						break;
//					}
//				}
//			}
//		}
//		else if (event.group == ape::Event::Group::CAMERA)
//		{
//			if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
//			{
//				if (event.type == ape::Event::Type::CAMERA_CREATE)
//				{
////					mpFilamentCamera = mpFilamentEngine->createCamera(mpFilamentEntityManager->create());
////					for (int i = 0; i < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList.size(); i++)
////					{
////						for (int j = 0; j < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList.size(); j++)
////						{
////							for (int k = 0; k < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].cameras.size(); k++)
////							{
////								auto cameraSetting = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].cameras[k];
////								if (cameraSetting.name == camera->getName())
////								{
////									float aspectRatio = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].width / mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].height;
////									camera->setWindow(mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].name);
////									camera->setFocalLength(1.0f);
////									camera->setNearClipDistance(cameraSetting.nearClip);
////									camera->setFarClipDistance(cameraSetting.farClip);
////									camera->setFOVy(cameraSetting.fovY.toRadian());
////									//mpFilamentCamera->setProjection(cameraSetting.fovY.degree, aspectRatio, cameraSetting.nearClip, cameraSetting.farClip);
////								}
////							}
////						}
////					}
//				}
//				else
//				{
//					switch (event.type)
//					{
//					case ape::Event::Type::CAMERA_WINDOW:
//                    {
////						for (int i = 0; i < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList.size(); i++)
////						{
////							for (int j = 0; j < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList.size(); j++)
////							{
////								auto renderWindowSetting = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i];
////								auto viewportSetting = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j];
////								for (int k = 0; k < mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].cameras.size(); k++)
////								{
////									auto cameraSetting = mFilamentApplicationPluginConfig.filamentRenderWindowConfigList[i].viewportList[j].cameras[k];
////									if (cameraSetting.name == camera->getName())
////									{
////										int zorder = viewportSetting.zOrder;
////										float width = (float)viewportSetting.width / (float)renderWindowSetting.width;
////										float height = (float)viewportSetting.height / (float)renderWindowSetting.height;
////										float left = (float)viewportSetting.left / (float)renderWindowSetting.width;
////										float top = (float)viewportSetting.top / (float)renderWindowSetting.height;
////
////										if (mpFilamentView)
////										{
////											APE_LOG_DEBUG("filamentViewport: " << "zorder: " << zorder << " left: " << left << " top: " << top << " width: " << width << " height: " << height);
////											mpFilamentView->setCamera(mpFilamentCamera);
////											mpFilamentView->setViewport({ viewportSetting.left, viewportSetting.top, (unsigned int)viewportSetting.width, (unsigned int)viewportSetting.height });}
////											mpFilamentView->setScene(mpFilamentScene);
////											filament::View::AmbientOcclusionOptions ambientOcclusionOptions;
////											ambientOcclusionOptions.upsampling = filament::View::QualityLevel::HIGH;
////											mpFilamentView->setAmbientOcclusionOptions(ambientOcclusionOptions);
////									}
////								}
////							}
////						}
//                        ;
//					}
//					break;
//					case ape::Event::Type::CAMERA_PARENTNODE:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_DELETE:
//						;
//						break;
//					case ape::Event::Type::CAMERA_FOCALLENGTH:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_ASPECTRATIO:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_AUTOASPECTRATIO:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_FOVY:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_FRUSTUMOFFSET:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_FARCLIP:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_NEARCLIP:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_PROJECTION:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_PROJECTIONTYPE:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_ORTHOWINDOWSIZE:
//					{
//						;
//					}
//					break;
//					case ape::Event::Type::CAMERA_VISIBILITY:
//					{
//						;
//					}
//					break;
//					}
//				}
//			}
//		}
//		}
//		catch (std::exception exp)
//		{
//			APE_LOG_DEBUG("");
//		}
//		mEventDoubleQueue.pop();
//	}
}
void ape::FilamentApplicationPlugin::initFilament(){
    App app;
    app.config.title = "Filament";
    app.config.iblDirectory = FilamentApp::getRootAssetsPath() + "default_env";
    
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
//	try
//	{
//
//		while (true)
//		{
//			processEventDoubleQueue();
//			if (mpFilamentRenderer->beginFrame(mpFilamentSwapChain))
//			{
//				mpFilamentRenderer->render(mpFilamentView);
//				//APE_LOG_DEBUG("render");
//				mpFilamentRenderer->endFrame();
//			}
//			std::this_thread::sleep_for(std::chrono::milliseconds(20));
//		}
//
//	}
//	catch (std::exception exp)
//	{
//		APE_LOG_DEBUG("");
//	}
//    while(true)
//        std::this_thread::sleep_for(std::chrono::milliseconds(20));
	APE_LOG_FUNC_LEAVE();
}

std::ifstream::pos_type ape::FilamentApplicationPlugin::getFileSize(const char* filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

bool ape::FilamentApplicationPlugin::loadSettings(const char* filename, Settings* out) {
    auto contentSize = getFileSize(filename);
    if (contentSize <= 0) {
        return false;
    }
    std::ifstream in(filename, std::ifstream::binary | std::ifstream::in);
    std::vector<char> json(static_cast<unsigned long>(contentSize));
    if (!in.read(json.data(), contentSize)) {
        return false;
    }
    return readJson(json.data(), contentSize, out);
}

void ape::FilamentApplicationPlugin::computeRangePlot(App& app, float* rangePlot) {
    float4& ranges = app.viewer->getSettings().view.colorGrading.ranges;
    ranges.y = clamp(ranges.y, ranges.x + 1e-5f, ranges.w - 1e-5f); // darks
    ranges.z = clamp(ranges.z, ranges.x + 1e-5f, ranges.w - 1e-5f); // lights

    for (size_t i = 0; i < 1024; i++) {
        float x = i / 1024.0f;
        float s = 1.0f - smoothstep(ranges.x, ranges.y, x);
        float h = smoothstep(ranges.z, ranges.w, x);
        rangePlot[i]        = s;
        rangePlot[1024 + i] = 1.0f - s - h;
        rangePlot[2048 + i] = h;
    }
}

void ape::FilamentApplicationPlugin::rangePlotSeriesStart(int series) {
    switch (series) {
        case 0:
            ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4) ImColor::HSV(0.4f, 0.25f, 1.0f));
            break;
        case 1:
            ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4) ImColor::HSV(0.8f, 0.25f, 1.0f));
            break;
        case 2:
            ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4) ImColor::HSV(0.17f, 0.21f, 1.0f));
            break;
    }
}


void ape::FilamentApplicationPlugin::rangePlotSeriesEnd(int series) {
    if (series < 3) {
        ImGui::PopStyleColor();
    }
}

float ape::FilamentApplicationPlugin::getRangePlotValue(int series, void *data, int index)
{
    return ((float*) data)[series * 1024 + index];
}

float3 ape::FilamentApplicationPlugin::curves(float3 v, float3 shadowGamma, float3 midPoint, float3 highlightScale){
    float3 d = 1.0f / (pow(midPoint, shadowGamma - 1.0f));
    float3 dark = pow(v, shadowGamma) * d;
    float3 light = highlightScale * (v - midPoint) + midPoint;
    return float3{
            v.r <= midPoint.r ? dark.r : light.r,
            v.g <= midPoint.g ? dark.g : light.g,
            v.b <= midPoint.b ? dark.b : light.b,
    };
}

void ape::FilamentApplicationPlugin::computeCurvePlot(App& app, float* curvePlot){
    const auto& colorGradingOptions = app.viewer->getSettings().view.colorGrading;
    for (size_t i = 0; i < 1024; i++) {
        float3 x{i / 1024.0f * 2.0f};
        float3 y = curves(x,
                colorGradingOptions.gamma,
                colorGradingOptions.midPoint,
                colorGradingOptions.scale);
        curvePlot[i]        = y.r;
        curvePlot[1024 + i] = y.g;
        curvePlot[2048 + i] = y.b;
    }
}

void ape::FilamentApplicationPlugin::tooltipFloat(float value){
    if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%.2f", value);
    }
}

void ape::FilamentApplicationPlugin::pushSliderColors(float hue){
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4) ImColor::HSV(hue, 0.5f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4) ImColor::HSV(hue, 0.6f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4) ImColor::HSV(hue, 0.7f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4) ImColor::HSV(hue, 0.9f, 0.9f));
}

void ape::FilamentApplicationPlugin::colorGradingUI(App& app){
    const static ImVec2 verticalSliderSize(18.0f, 160.0f);
    const static ImVec2 plotLinesSize(260.0f, 160.0f);
    const static ImVec2 plotLinesWideSize(350.0f, 120.0f);

    if (ImGui::CollapsingHeader("Color grading")) {
        ColorGradingSettings& colorGrading = app.viewer->getSettings().view.colorGrading;

        ImGui::Indent();
        ImGui::Checkbox("Enabled##colorGrading", &colorGrading.enabled);

        int quality = (int) colorGrading.quality;
        ImGui::Combo("Quality##colorGradingQuality", &quality, "Low\0Medium\0High\0Ultra\0\0");
        colorGrading.quality = (decltype(colorGrading.quality)) quality;

        int toneMapping = (int) colorGrading.toneMapping;
        ImGui::Combo("Tone-mapping", &toneMapping,
                "Linear\0ACES (legacy)\0ACES\0Filmic\0Uchimura\0Reinhard\0Display Range\0\0");
        colorGrading.toneMapping = (decltype(colorGrading.toneMapping)) toneMapping;

        if (ImGui::CollapsingHeader("White balance")) {
            int temperature = colorGrading.temperature * 100.0f;
            int tint = colorGrading.tint * 100.0f;
            ImGui::SliderInt("Temperature", &temperature, -100, 100);
            ImGui::SliderInt("Tint", &tint, -100, 100);
            colorGrading.temperature = temperature / 100.0f;
            colorGrading.tint = tint / 100.0f;
        }

        if (ImGui::CollapsingHeader("Channel mixer")) {
            pushSliderColors(0.0f / 7.0f);
            ImGui::VSliderFloat("##outRed.r", verticalSliderSize, &colorGrading.outRed.r, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outRed.r);
            ImGui::SameLine();
            ImGui::VSliderFloat("##outRed.g", verticalSliderSize, &colorGrading.outRed.g, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outRed.g);
            ImGui::SameLine();
            ImGui::VSliderFloat("##outRed.b", verticalSliderSize, &colorGrading.outRed.b, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outRed.b);
            ImGui::SameLine(0.0f, 18.0f);
            popSliderColors();

            pushSliderColors(2.0f / 7.0f);
            ImGui::VSliderFloat("##outGreen.r", verticalSliderSize, &colorGrading.outGreen.r, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outGreen.r);
            ImGui::SameLine();
            ImGui::VSliderFloat("##outGreen.g", verticalSliderSize, &colorGrading.outGreen.g, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outGreen.g);
            ImGui::SameLine();
            ImGui::VSliderFloat("##outGreen.b", verticalSliderSize, &colorGrading.outGreen.b, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outGreen.b);
            ImGui::SameLine(0.0f, 18.0f);
            popSliderColors();

            pushSliderColors(4.0f / 7.0f);
            ImGui::VSliderFloat("##outBlue.r", verticalSliderSize, &colorGrading.outBlue.r, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outBlue.r);
            ImGui::SameLine();
            ImGui::VSliderFloat("##outBlue.g", verticalSliderSize, &colorGrading.outBlue.g, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outBlue.g);
            ImGui::SameLine();
            ImGui::VSliderFloat("##outBlue.b", verticalSliderSize, &colorGrading.outBlue.b, -2.0f, 2.0f, "");
            tooltipFloat(colorGrading.outBlue.b);
            popSliderColors();
        }
        if (ImGui::CollapsingHeader("Tonal ranges")) {
            ImGui::ColorEdit3("Shadows", &colorGrading.shadows.x);
            ImGui::SliderFloat("Weight##shadowsWeight", &colorGrading.shadows.w, -2.0f, 2.0f);
            ImGui::ColorEdit3("Mid-tones", &colorGrading.midtones.x);
            ImGui::SliderFloat("Weight##midTonesWeight", &colorGrading.midtones.w, -2.0f, 2.0f);
            ImGui::ColorEdit3("Highlights", &colorGrading.highlights.x);
            ImGui::SliderFloat("Weight##highlightsWeight", &colorGrading.highlights.w, -2.0f, 2.0f);
            ImGui::SliderFloat4("Ranges", &colorGrading.ranges.x, 0.0f, 1.0f);
            computeRangePlot(app, app.rangePlot);
        }
        if (ImGui::CollapsingHeader("Color decision list")) {
            ImGui::SliderFloat3("Slope", &colorGrading.slope.x, 0.0f, 2.0f);
            ImGui::SliderFloat3("Offset", &colorGrading.offset.x, -0.5f, 0.5f);
            ImGui::SliderFloat3("Power", &colorGrading.power.x, 0.0f, 2.0f);
        }
        if (ImGui::CollapsingHeader("Adjustments")) {
            ImGui::SliderFloat("Contrast", &colorGrading.contrast, 0.0f, 2.0f);
            ImGui::SliderFloat("Vibrance", &colorGrading.vibrance, 0.0f, 2.0f);
            ImGui::SliderFloat("Saturation", &colorGrading.saturation, 0.0f, 2.0f);
        }
        if (ImGui::CollapsingHeader("Curves")) {
            ImGui::Checkbox("Linked curves", &colorGrading.linkedCurves);

            computeCurvePlot(app, app.curvePlot);

            if (!colorGrading.linkedCurves) {
                pushSliderColors(0.0f / 7.0f);
                ImGui::VSliderFloat("##curveGamma.r", verticalSliderSize, &colorGrading.gamma.r, 0.0f, 4.0f, "");
                tooltipFloat(colorGrading.gamma.r);
                ImGui::SameLine();
                ImGui::VSliderFloat("##curveMid.r", verticalSliderSize, &colorGrading.midPoint.r, 0.0f, 2.0f, "");
                tooltipFloat(colorGrading.midPoint.r);
                ImGui::SameLine();
                ImGui::VSliderFloat("##curveScale.r", verticalSliderSize, &colorGrading.scale.r, 0.0f, 4.0f, "");
                tooltipFloat(colorGrading.scale.r);
                ImGui::SameLine(0.0f, 18.0f);
                popSliderColors();

                ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4) ImColor::HSV(0.0f, 0.7f, 0.8f));
                ImGui::PlotLines("", app.curvePlot, 1024, 0, "Red", 0.0f, 2.0f, plotLinesSize);
                ImGui::PopStyleColor();

                pushSliderColors(2.0f / 7.0f);
                ImGui::VSliderFloat("##curveGamma.g", verticalSliderSize, &colorGrading.gamma.g, 0.0f, 4.0f, "");
                tooltipFloat(colorGrading.gamma.g);
                ImGui::SameLine();
                ImGui::VSliderFloat("##curveMid.g", verticalSliderSize, &colorGrading.midPoint.g, 0.0f, 2.0f, "");
                tooltipFloat(colorGrading.midPoint.g);
                ImGui::SameLine();
                ImGui::VSliderFloat("##curveScale.g", verticalSliderSize, &colorGrading.scale.g, 0.0f, 4.0f, "");
                tooltipFloat(colorGrading.scale.g);
                ImGui::SameLine(0.0f, 18.0f);
                popSliderColors();

                ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4) ImColor::HSV(0.3f, 0.7f, 0.8f));
                ImGui::PlotLines("", app.curvePlot + 1024, 1024, 0, "Green", 0.0f, 2.0f, plotLinesSize);
                ImGui::PopStyleColor();

                pushSliderColors(4.0f / 7.0f);
                ImGui::VSliderFloat("##curveGamma.b", verticalSliderSize, &colorGrading.gamma.b, 0.0f, 4.0f, "");
                tooltipFloat(colorGrading.gamma.b);
                ImGui::SameLine();
                ImGui::VSliderFloat("##curveMid.b", verticalSliderSize, &colorGrading.midPoint.b, 0.0f, 2.0f, "");
                tooltipFloat(colorGrading.midPoint.b);
                ImGui::SameLine();
                ImGui::VSliderFloat("##curveScale.b", verticalSliderSize, &colorGrading.scale.b, 0.0f, 4.0f, "");
                tooltipFloat(colorGrading.scale.b);
                ImGui::SameLine(0.0f, 18.0f);
                popSliderColors();

                ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4) ImColor::HSV(0.6f, 0.7f, 0.8f));
                ImGui::PlotLines("", app.curvePlot + 2048, 1024, 0, "Blue", 0.0f, 2.0f, plotLinesSize);
                ImGui::PopStyleColor();
            } else {
                ImGui::VSliderFloat("##curveGamma", verticalSliderSize, &colorGrading.gamma.r, 0.0f, 4.0f, "");
                tooltipFloat(colorGrading.gamma.r);
                ImGui::SameLine();
                ImGui::VSliderFloat("##curveMid", verticalSliderSize, &colorGrading.midPoint.r, 0.0f, 2.0f, "");
                tooltipFloat(colorGrading.midPoint.r);
                ImGui::SameLine();
                ImGui::VSliderFloat("##curveScale", verticalSliderSize, &colorGrading.scale.r, 0.0f, 4.0f, "");
                tooltipFloat(colorGrading.scale.r);
                ImGui::SameLine(0.0f, 18.0f);

                colorGrading.gamma = float3{colorGrading.gamma.r};
                colorGrading.midPoint = float3{colorGrading.midPoint.r};
                colorGrading.scale = float3{colorGrading.scale.r};

                ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4) ImColor::HSV(0.17f, 0.21f, 0.9f));
                ImGui::PlotLines("", app.curvePlot, 1024, 0, "RGB", 0.0f, 2.0f, plotLinesSize);
                ImGui::PopStyleColor();
            }
        }
        ImGui::Unindent();
    }
}


LinearColor ape::FilamentApplicationPlugin::inverseTonemapSRGB(sRGBColor x) {
    return (x * -0.155) / (x - 1.019);
}

void ape::FilamentApplicationPlugin::Step()
{
    App app;

    app.config.title = "Filament";
    app.config.iblDirectory = FilamentApp::getRootAssetsPath() + DEFAULT_IBL;

    utils::Path filename;
    int num_args = 1;
    if (num_args >= 1) {
        filename = "c:/ApertusVR/plugins/scene/photorealisticScene/resources/damagedHelmet.gltf";
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

    auto loadAsset = [&app, this](utils::Path filename) {
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
            app.asset = app.loader->createAssetFromBinary(buffer.data(), buffer.size());
        } else {
            app.asset = app.loader->createAssetFromJson(buffer.data(), buffer.size());
        }
        buffer.clear();
        buffer.shrink_to_fit();

        if (!app.asset) {
            std::cerr << "Unable to parse " << filename << std::endl;
            exit(1);
        }
    };

    auto loadResources = [&app] (utils::Path filename) {
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
        app.resourceLoader->asyncBeginLoad(app.asset);

        // Load animation data then free the source hierarchy.
        app.asset->getAnimator();
        app.asset->releaseSourceData();

        auto ibl = FilamentApp::get().getIBL();
        if (ibl) {
            app.viewer->setIndirectLight(ibl->getIndirectLight(), ibl->getSphericalHarmonics());
        }
    };

    auto setup = [&](Engine* engine, View* view, Scene* scene) {
        app.engine = engine;
        app.names = new NameComponentManager(EntityManager::get());
        app.viewer = new SimpleViewer(engine, scene, view, 410);

        const bool batchMode = !app.batchFile.empty();

        // First check if a custom automation spec has been provided. If it fails to load, the app
        // must be closed since it could be invoked from a script.
        if (batchMode && app.batchFile != "default") {
            auto size = getFileSize(app.batchFile.c_str());
            if (size > 0) {
                std::ifstream in(app.batchFile, std::ifstream::binary | std::ifstream::in);
                std::vector<char> json(static_cast<unsigned long>(size));
                in.read(json.data(), size);
                app.automationSpec = AutomationSpec::generate(json.data(), size);
                if (!app.automationSpec) {
                    std::cerr << "Unable to parse automation spec: " << app.batchFile << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Unable to load automation spec: " << app.batchFile << std::endl;
                exit(1);
            }
        }

        // If no custom spec has been provided, or if in interactive mode, load the default spec.
        if (!app.automationSpec) {
            app.automationSpec = AutomationSpec::generateDefaultTestCases();
        }

        app.automationEngine = new AutomationEngine(app.automationSpec, &app.viewer->getSettings());

        if (batchMode) {
            app.automationEngine->startBatchMode();
            auto options = app.automationEngine->getOptions();
            options.sleepDuration = 0.0;
            options.exportScreenshots = true;
            options.exportSettings = true;
            app.automationEngine->setOptions(options);
            app.viewer->stopAnimation();
        }

        if (app.settingsFile.size() > 0) {
            bool success = loadSettings(app.settingsFile.c_str(), &app.viewer->getSettings());
            if (success) {
                std::cout << "Loaded settings from " << app.settingsFile << std::endl;
            } else {
                std::cerr << "Failed to load settings from " << app.settingsFile << std::endl;
            }
        }

        app.materials = (app.materialSource == GENERATE_SHADERS) ?
                createMaterialGenerator(engine) : createUbershaderLoader(engine);
        app.loader = AssetLoader::create({engine, app.materials, app.names });
        app.mainCamera = &view->getCamera();
        loadAsset(filename);

        loadResources(filename);

        app.viewer->setUiCallback([&app, scene, view, engine, this] () {
            auto& automation = *app.automationEngine;

            float progress = app.resourceLoader->asyncGetLoadProgress();
            if (progress < 1.0) {
                ImGui::ProgressBar(progress);
            } else {
                // The model is now fully loaded, so let automation know.
                automation.signalBatchMode();
            }

            // The screenshots do not include the UI, but we auto-open the Automation UI group
            // when in batch mode. This is useful when a human is observing progress.
            const int flags = automation.isBatchModeEnabled() ? ImGuiTreeNodeFlags_DefaultOpen : 0;

            if (ImGui::CollapsingHeader("Automation", flags)) {
                ImGui::Indent();

                const ImVec4 yellow(1.0f,1.0f,0.0f,1.0f);
                if (automation.isRunning()) {
                    ImGui::TextColored(yellow, "Test case %zu / %zu",
                            automation.currentTest(), automation.testCount());
                } else {
                    ImGui::TextColored(yellow, "%zu test cases", automation.testCount());
                }

                auto options = automation.getOptions();

                ImGui::PushItemWidth(150);
                ImGui::SliderFloat("Sleep (seconds)", &options.sleepDuration, 0.0, 5.0);
                ImGui::PopItemWidth();

                // Hide the tooltip during automation to avoid photobombing the screenshot.
                if (ImGui::IsItemHovered() && !automation.isRunning()) {
                    ImGui::SetTooltip("Specifies the amount of time to sleep between test cases.");
                }

                ImGui::Checkbox("Export screenshot for each test", &options.exportScreenshots);
                ImGui::Checkbox("Export settings JSON for each test", &options.exportSettings);

                automation.setOptions(options);

                if (automation.isRunning()) {
                    if (ImGui::Button("Stop batch test")) {
                        automation.stopRunning();
                    }
                } else if (ImGui::Button("Run batch test")) {
                    automation.startRunning();
                }

                if (ImGui::Button("Export view settings")) {
                    automation.exportSettings(app.viewer->getSettings(), "settings.json");
                    app.messageBoxText = automation.getStatusMessage();
                    ImGui::OpenPopup("MessageBox");
                }
                ImGui::Unindent();
            }

            if (ImGui::CollapsingHeader("Stats")) {
                ImGui::Indent();
                ImGui::Text("%zu entities in the asset", app.asset->getEntityCount());
                ImGui::Text("%zu renderables (excluding UI)", scene->getRenderableCount());
                ImGui::Text("%zu skipped frames", FilamentApp::get().getSkippedFrameCount());
                ImGui::Unindent();
            }

            if (ImGui::CollapsingHeader("Scene")) {
                ImGui::Indent();
                ImGui::Checkbox("Show skybox", &app.viewOptions.skyboxEnabled);
                ImGui::ColorEdit3("Background color", &app.viewOptions.backgroundColor.r);
                ImGui::Checkbox("Ground shadow", &app.viewOptions.groundPlaneEnabled);
                ImGui::Indent();
                ImGui::SliderFloat("Strength", &app.viewOptions.groundShadowStrength, 0.0f, 1.0f);
                ImGui::Unindent();
                ImGui::Unindent();
            }

            if (ImGui::CollapsingHeader("Camera")) {
                ViewSettings& settings = app.viewer->getSettings().view;

                ImGui::Indent();
                ImGui::SliderFloat("Focal length (mm)", &FilamentApp::get().getCameraFocalLength(), 16.0f, 90.0f);
                ImGui::SliderFloat("Aperture", &app.viewOptions.cameraAperture, 1.0f, 32.0f);
                ImGui::SliderFloat("Speed (1/s)", &app.viewOptions.cameraSpeed, 1000.0f, 1.0f);
                ImGui::SliderFloat("ISO", &app.viewOptions.cameraISO, 25.0f, 6400.0f);
                ImGui::Checkbox("DoF", &settings.dof.enabled);
                ImGui::SliderFloat("Focus distance", &settings.dof.focusDistance, 0.0f, 30.0f);
                ImGui::SliderFloat("Blur scale", &settings.dof.cocScale, 0.1f, 10.0f);

                if (ImGui::CollapsingHeader("Vignette")) {
                    ImGui::Checkbox("Enabled##vignetteEnabled", &settings.vignette.enabled);
                    ImGui::SliderFloat("Mid point", &settings.vignette.midPoint, 0.0f, 1.0f);
                    ImGui::SliderFloat("Roundness", &settings.vignette.roundness, 0.0f, 1.0f);
                    ImGui::SliderFloat("Feather", &settings.vignette.feather, 0.0f, 1.0f);
                    ImGui::ColorEdit3("Color##vignetteColor", &settings.vignette.color.r);
                }

                const utils::Entity* cameras = app.asset->getCameraEntities();
                const size_t cameraCount = app.asset->getCameraEntityCount();

                std::vector<std::string> names;
                names.reserve(cameraCount + 1);
                names.push_back("Free camera");
                int c = 0;
                for (size_t i = 0; i < cameraCount; i++) {
                    const char* n = app.asset->getName(cameras[i]);
                    if (n) {
                        names.emplace_back(n);
                    } else {
                        char buf[32];
                        sprintf(buf, "Unnamed camera %d", c++);
                        names.emplace_back(buf);
                    }
                }

                std::vector<const char*> cstrings;
                cstrings.reserve(names.size());
                for (size_t i = 0; i < names.size(); i++) {
                    cstrings.push_back(names[i].c_str());
                }

                ImGui::ListBox("Cameras", &app.currentCamera, cstrings.data(), cstrings.size());
                ImGui::Unindent();
            }

            colorGradingUI(app);

            if (ImGui::CollapsingHeader("Debug")) {
                if (ImGui::Button("Capture frame")) {
                    auto& debug = engine->getDebugRegistry();
                    bool* captureFrame =
                        debug.getPropertyAddress<bool>("d.renderer.doFrameCapture");
                    *captureFrame = true;
                }
            }

            if (ImGui::BeginPopupModal("MessageBox", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("%s", app.messageBoxText.c_str());
                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        });
    };

    auto cleanup = [&app](Engine* engine, View*, Scene*) {
        app.automationEngine->terminate();
        app.loader->destroyAsset(app.asset);
        app.materials->destroyMaterials();

        engine->destroy(app.scene.groundPlane);
        engine->destroy(app.scene.groundVertexBuffer);
        engine->destroy(app.scene.groundIndexBuffer);
        engine->destroy(app.scene.groundMaterial);
        engine->destroy(app.colorGrading);

        delete app.viewer;
        delete app.materials;
        delete app.names;

        AssetLoader::destroy(&app.loader);
    };

    auto animate = [&app](Engine* engine, View* view, double now) {
        app.resourceLoader->asyncUpdateLoad();

        // Add renderables to the scene as they become ready.
        app.viewer->populateScene(app.asset, !app.actualSize);

        app.viewer->applyAnimation(now);
    };

    auto resize = [&app](Engine* engine, View* view) {
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

    auto gui = [&app](Engine* engine, View* view) {
        app.viewer->updateUserInterface();

        FilamentApp::get().setSidebarWidth(app.viewer->getSidebarWidth());
    };

    auto preRender = [&app,this](Engine* engine, View* view, Scene* scene, Renderer* renderer) {
        auto& rcm = engine->getRenderableManager();
        auto instance = rcm.getInstance(app.scene.groundPlane);
        rcm.setLayerMask(instance,
                0xff, app.viewOptions.groundPlaneEnabled ? 0xff : 0x00);

        const size_t cameraCount = app.asset->getCameraEntityCount();
        view->setCamera(app.mainCamera);
        if (app.currentCamera > 0) {
            const int gltfCamera = app.currentCamera - 1;
            if (gltfCamera < cameraCount) {
                const utils::Entity* cameras = app.asset->getCameraEntities();
                filament::Camera* c = engine->getCameraComponent(cameras[gltfCamera]);
                assert(c);
                view->setCamera(c);

                // Override the aspect ratio in the glTF file and adjust the aspect ratio of this
                // camera to the viewport.
                const Viewport& vp = view->getViewport();
                double aspectRatio = (double) vp.width / vp.height;
                c->setScaling(double4 {1.0 / aspectRatio, 1.0, 1.0, 1.0});
            } else {
                // gltfCamera is out of bounds. Reset camera selection to main camera.
                app.currentCamera = 0;
            }
        }

        filament::Camera& camera = view->getCamera();
        camera.setExposure(
                app.viewOptions.cameraAperture,
                1.0f / app.viewOptions.cameraSpeed,
                app.viewOptions.cameraISO);

//        app.scene.groundMaterial->setDefaultParameter(
//                "strength", app.viewOptions.groundShadowStrength);

        auto ibl = FilamentApp::get().getIBL();
        if (ibl) {
            ibl->getSkybox()->setLayerMask(0xff, app.viewOptions.skyboxEnabled ? 0xff : 0x00);
        }

        // we have to clear because the side-bar doesn't have a background, we cannot use
        // a skybox on the ui scene, because the ui view is always full screen.
        renderer->setClearOptions({
                .clearColor = { inverseTonemapSRGB(app.viewOptions.backgroundColor), 1.0f },
                .clear = true
        });

        ColorGradingSettings& options = app.viewer->getSettings().view.colorGrading;
        if (options.enabled) {
            // An inefficient but simple way of detecting change is to serialize to JSON, then
            // do a string comparison.
            if (writeJson(options) != writeJson(app.lastColorGradingOptions)) {
                ColorGrading *colorGrading = createColorGrading(options, engine);
                engine->destroy(app.colorGrading);
                app.colorGrading = colorGrading;
                app.lastColorGradingOptions = options;
            }
            view->setColorGrading(app.colorGrading);
        } else {
            view->setColorGrading(nullptr);
        }
    };

    auto postRender = [&app](Engine* engine, View* view, Scene* scene, Renderer* renderer) {
        if (app.automationEngine->shouldClose()) {
            FilamentApp::get().close();
            return;
        }
        Settings* settings = &app.viewer->getSettings();
        MaterialInstance* const* materials = app.asset->getMaterialInstances();
        size_t materialCount = app.asset->getMaterialInstanceCount();
        app.automationEngine->tick(view, materials, materialCount, renderer,
                ImGui::GetIO().DeltaTime);
    };

    FilamentApp& filamentApp = FilamentApp::get();
    filamentApp.animate(animate);
    filamentApp.resize(resize);

    filamentApp.setDropHandler([&] (std::string path) {
        app.viewer->removeAsset();
        app.loader->destroyAsset(app.asset);
        loadAsset(path);
        loadResources(path);
    });

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
