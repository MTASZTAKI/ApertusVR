#include "apeFilamentWindowsRenderPlugin.h"

static const filament::math::float2 TRIANGLE_VERTICES[3] = { {1, 0}, {-0.5, 0.866}, {-0.5, -0.866} };
static constexpr uint16_t TRIANGLE_INDICES[3] = { 0, 1, 2 };

ape::FilamentWindowsRenderPlugin::FilamentWindowsRenderPlugin( )
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mEventDoubleQueue = ape::DoubleQueue<Event>();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CLONE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&FilamentWindowsRenderPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::FilamentWindowsRenderPlugin::~FilamentWindowsRenderPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentWindowsRenderPlugin::eventCallBack(const ape::Event& event)
{
	mEventDoubleQueue.push(event);
}

void ape::FilamentWindowsRenderPlugin::processEventDoubleQueue()
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
					;
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
						;
						break;
					case ape::Event::Type::NODE_ORIENTATION:
						;
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
					;
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
							auto asset = mpGltfAssetLoader->createAssetFromJson(buffer.data(), buffer.size());
							buffer.clear();
							buffer.shrink_to_fit();
							if (!asset)
							{
								APE_LOG_DEBUG("Unable to parse " << filePath.str());
							}
							else
							{
								APE_LOG_DEBUG(filePath.str() << " was parsed");
								gltfio::ResourceConfiguration resourceConfiguration;
								resourceConfiguration.engine = mpFilamentEngine;
								auto resourceLocation = filePath.str();
								resourceConfiguration.gltfPath = resourceLocation.c_str();
								resourceConfiguration.normalizeSkinningWeights = true;
								resourceConfiguration.recomputeBoundingBoxes = false;
								auto filamentResourceLoader = new gltfio::ResourceLoader(resourceConfiguration);
								if (filamentResourceLoader->loadResources(asset))
								{
									APE_LOG_DEBUG("resources load OK");

									mpFilamentScene->addEntities(asset->getEntities(), asset->getEntityCount());

									/*auto vb = filament::VertexBuffer::Builder()
										.vertexCount(3).bufferCount(1)
										.attribute(filament::VertexAttribute::POSITION, 0, filament::VertexBuffer::AttributeType::FLOAT2, 0, 8)
										.build(*mpFilamentEngine);
									vb->setBufferAt(*mpFilamentEngine, 0,
										filament::VertexBuffer::BufferDescriptor(TRIANGLE_VERTICES, 24, nullptr));
									auto ib = filament::IndexBuffer::Builder()
										.indexCount(3).bufferType(filament::IndexBuffer::IndexType::USHORT)
										.build(*mpFilamentEngine);
									ib->setBuffer(*mpFilamentEngine, filament::IndexBuffer::BufferDescriptor(TRIANGLE_INDICES, 6, nullptr));
									auto renderable = utils::EntityManager::get().create();
									filament::RenderableManager::Builder(1)
										.boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
										.geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vb, ib, 0, 3)
										.build(*mpFilamentEngine, renderable);
									mpFilamentScene->addEntity(renderable);*/

								}
								else
								{
									APE_LOG_DEBUG("resources load FAILED");
								}
								delete filamentResourceLoader;
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
					;
				}
				else
				{
					switch (event.type)
					{
					case ape::Event::Type::CAMERA_WINDOW:
					{
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

void ape::FilamentWindowsRenderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("waiting for main window");
	while (mpCoreConfig->getWindowConfig().handle == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("main window was found");
	mpFilamentEngine = filament::Engine::create(filament::Engine::Backend::OPENGL);
	mpFilamentSwapChain = mpFilamentEngine->createSwapChain(mpCoreConfig->getWindowConfig().handle);
	mpFilamentRenderer = mpFilamentEngine->createRenderer();
	mpFilamentCamera = mpFilamentEngine->createCamera(utils::EntityManager::get().create());
	mpFilamentView = mpFilamentEngine->createView();
	mpFilamentScene = mpFilamentEngine->createScene();
	mpFilamentView->setCamera(mpFilamentCamera);
	mpFilamentView->setScene(mpFilamentScene);
	mpFilamentMaterialProvider = gltfio::createMaterialGenerator(mpFilamentEngine);
	mpFilamentNameComponentManager = new utils::NameComponentManager(utils::EntityManager::get());
	mFilamentSunlight = utils::EntityManager::get().create();
	mpFilamentLightManagerBuilder = new filament::LightManager::Builder(filament::LightManager::Type::SUN);
	mpFilamentLightManagerBuilder->color(filament::Color::toLinear<filament::ACCURATE>({ 0.98, 0.92, 0.89 }));
	mpFilamentLightManagerBuilder->intensity(100000.0f);
	filament::math::float3 sunlightDirection = { 0.6, -1.0, -0.8 };
	mpFilamentLightManagerBuilder->direction(sunlightDirection);
	mpFilamentLightManagerBuilder->castShadows(true);
	mpFilamentLightManagerBuilder->sunAngularRadius(1.9);
	mpFilamentLightManagerBuilder->sunHaloSize(10.0);
	mpFilamentLightManagerBuilder->sunHaloFalloff(80.0);
	mpFilamentLightManagerBuilder->build(*mpFilamentEngine, mFilamentSunlight);
	mpFilamentScene->addEntity(mFilamentSunlight);
	filament::View::AmbientOcclusionOptions ambientOcclusionOptions;
	ambientOcclusionOptions.upsampling = filament::View::QualityLevel::HIGH;
	mpFilamentView->setAmbientOcclusionOptions(ambientOcclusionOptions);
	mpGltfAssetLoader = gltfio::AssetLoader::create({ mpFilamentEngine, mpFilamentMaterialProvider, mpFilamentNameComponentManager });
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentWindowsRenderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	try
	{
		while (true)
		{
			if (mpFilamentRenderer->beginFrame(mpFilamentSwapChain))
			{
				mpFilamentRenderer->render(mpFilamentView);
				//APE_LOG_DEBUG("render");
				mpFilamentRenderer->endFrame();
			}
			processEventDoubleQueue();
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
	catch (std::exception exp)
	{
		APE_LOG_DEBUG("");
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::FilamentWindowsRenderPlugin::Step()
{
	try
	{
		;
	}
	catch (std::exception exp)
	{
		APE_LOG_ERROR("");
	}
}

void ape::FilamentWindowsRenderPlugin::Stop()
{

}

void ape::FilamentWindowsRenderPlugin::Suspend()
{

}

void ape::FilamentWindowsRenderPlugin::Restart()
{

}
