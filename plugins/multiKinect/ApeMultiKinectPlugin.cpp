#include "ApeMultiKinectPlugin.h"

Ape::MultiKinectPlugin::MultiKinectPlugin()
{
	APE_LOG_FUNC_ENTER();
	mFreenect2;
	mpScene = Ape::ISceneManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&MultiKinectPlugin::eventCallBack, this, std::placeholders::_1));
	mSensors = std::vector<Sensor>();
	APE_LOG_FUNC_LEAVE();
}

Ape::MultiKinectPlugin::~MultiKinectPlugin()
{
	APE_LOG_FUNC_ENTER();
	for (auto sensor : mSensors)
	{
		if (sensor.device)
		{
			sensor.device->stop();
			sensor.device->close();
			delete sensor.registration;
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::MultiKinectPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::MultiKinectPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("MultiKinectPlugin waiting for main window");
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("MultiKinectPlugin main window was found");
	std::stringstream MultiKinectPluginConfigFilePath;
	MultiKinectPluginConfigFilePath << mpSystemConfig->getFolderPath() << "\\ApeMultiKinectPlugin.json";
	APE_LOG_DEBUG("MultiKinectPluginConfigFilePath: " << MultiKinectPluginConfigFilePath.str());
	FILE* MultiKinectPluginConfigFile;
	if (errno_t err = fopen_s(&MultiKinectPluginConfigFile, MultiKinectPluginConfigFilePath.str().c_str(), "r") == 0)
	{
		char readBuffer[65536];
		if (MultiKinectPluginConfigFile)
		{
			rapidjson::FileReadStream jsonFileReaderStream(MultiKinectPluginConfigFile, readBuffer, sizeof(readBuffer));
			rapidjson::Document jsonDocument;
			jsonDocument.ParseStream(jsonFileReaderStream);
			if (jsonDocument.IsObject())
			{
				rapidjson::Value& sensors = jsonDocument["sensors"];
				for (auto& sensorData : sensors.GetArray())
				{
					Sensor sensor;
					sensor.id = sensorData["ID"].GetInt();
				    sensor.position.x = sensorData["position"].GetArray()[0].GetFloat();
					sensor.position.y = sensorData["position"].GetArray()[1].GetFloat();
					sensor.position.z = sensorData["position"].GetArray()[2].GetFloat();
					sensor.orientation.w = sensorData["orientation"].GetArray()[0].GetFloat();
					sensor.orientation.x = sensorData["orientation"].GetArray()[1].GetFloat();
					sensor.orientation.y = sensorData["orientation"].GetArray()[2].GetFloat();
					sensor.orientation.z = sensorData["orientation"].GetArray()[3].GetFloat();
					sensor.maxDepth = sensorData["maxDepth"].GetFloat() / 100.f;
					sensor.pointScaleOffset = sensorData["pointScaleOffset"].GetFloat();
					sensor.unitScaleDistance = sensorData["unitScaleDistance"].GetFloat();
					mSensors.push_back(sensor);
				}
			}
			fclose(MultiKinectPluginConfigFile);
		}
	}
	else
		APE_LOG_DEBUG("Error cannot open config file");
	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;
	int foundSensorCount = mFreenect2.enumerateDevices();
	if (foundSensorCount == 0)
	{
		APE_LOG_ERROR("No Kinect found");
		mSensors.clear();
		mSensors.resize(0);
	}
	else
	{
		auto sensorIt = mSensors.begin();
		while (sensorIt != mSensors.end())
		{
			auto& sensor = *sensorIt;
			sensor.serial = mFreenect2.getDeviceSerialNumber(sensor.id);
			if (sensor.serial.size() == 0)
			{
				APE_LOG_DEBUG("Kinect device was not found, ID: " << sensor.id);
				mSensors.erase(sensorIt);
				continue;
			}
			else
			{
				++sensorIt;
			}
			sensor.packePipeline = new libfreenect2::OpenCLPacketPipeline();
			libfreenect2::DepthPacketProcessor::Config depthPacketProcessorConfig;
			depthPacketProcessorConfig.MaxDepth = sensor.maxDepth;
			sensor.packePipeline->getDepthPacketProcessor()->setConfiguration(depthPacketProcessorConfig);
			sensor.device = mFreenect2.openDevice(sensor.serial, sensor.packePipeline);
			while (!sensor.device)
			{
				APE_LOG_DEBUG("Wait 5 secs until try again to open kinect device: " << sensor.serial);
				std::this_thread::sleep_for(std::chrono::milliseconds(5000));
				sensor.device = mFreenect2.openDevice(sensor.serial);
			}
			sensor.listener = new libfreenect2::SyncMultiFrameListener(0 | libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
			sensor.device->setColorFrameListener(sensor.listener);
			sensor.device->setIrAndDepthFrameListener(sensor.listener);
			while (!sensor.device->start())
			{
				APE_LOG_DEBUG("Wait 5 secs until try again to start kinect device: " << sensor.serial);
				std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			}
			APE_LOG_DEBUG("device serial: " + sensor.device->getSerialNumber());
			APE_LOG_DEBUG("device firmware: " + sensor.device->getFirmwareVersion());
			sensor.registration = new libfreenect2::Registration(sensor.device->getIrCameraParams(), sensor.device->getColorCameraParams());
			sensor.undistorted = new libfreenect2::Frame(mWidth, mHeight, 4);
			sensor.registered = new libfreenect2::Frame(mWidth, mHeight, 4);
			sensor.points.resize(3 * mWidth * mHeight);
			sensor.colors.resize(3 * mWidth * mHeight);
			if (auto pointCloudNode = mpScene->createNode("MultiKinectPointCloudNode" + std::to_string(sensor.id)).lock())
			{
				pointCloudNode->setPosition(sensor.position);
				pointCloudNode->setOrientation(sensor.orientation);
				pointCloudNode->setScale(Ape::Vector3(100, 100, 100));
				sensor.pointCloudNode = pointCloudNode;
				if (auto textNode = mpScene->createNode(pointCloudNode->getName() + "TextNode").lock())
				{
					textNode->setParentNode(pointCloudNode);
					if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity(textNode->getName() + "Text", Ape::Entity::GEOMETRY_TEXT).lock()))
					{
						text->setCaption(pointCloudNode->getName());
						text->setParentNode(textNode);
					}
				}
			}
			if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("MultiKinectPointCloud" + sensor.id, Ape::Entity::POINT_CLOUD).lock()))
			{
				pointCloud->setParameters(sensor.points, sensor.colors, 10000, sensor.pointScaleOffset, sensor.unitScaleDistance);
				pointCloud->setParentNode(sensor.pointCloudNode);
				sensor.pointCloud = pointCloud;
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}
	

void Ape::MultiKinectPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		for (auto& sensor : mSensors)
		{
			if (sensor.listener->waitForNewFrame(sensor.frames, 20))
			{
				libfreenect2::Frame *rgb = sensor.frames[libfreenect2::Frame::Color];
				libfreenect2::Frame *ir = sensor.frames[libfreenect2::Frame::Ir];
				libfreenect2::Frame *depth = sensor.frames[libfreenect2::Frame::Depth];
				sensor.registration->apply(rgb, depth, sensor.undistorted, sensor.registered);
				for (unsigned int row = 0; row < mWidth; row++)
				{
					for (unsigned int col = 0; col < mHeight; col++)
					{
						float rgb;
						sensor.registration->getPointXYZRGB(sensor.undistorted, sensor.registered, row, col, sensor.points[3 * (col * mWidth + row)],
							sensor.points[3 * (col * mWidth + row) + 1], sensor.points[3 * (col * mWidth + row) + 2], rgb);
						const uint8_t *p = reinterpret_cast<uint8_t*>(&rgb);
						sensor.colors[3 * (col * mWidth + row)] = (float)p[2] / 255;
						sensor.colors[3 * (col * mWidth + row) + 1] = (float)p[1] / 255;
						sensor.colors[3 * (col * mWidth + row) + 2] = (float)p[0] / 255;
					}
				}
				if (auto pointCloud = sensor.pointCloud.lock())
				{
					pointCloud->updatePoints(sensor.points);
					pointCloud->updateColors(sensor.colors);
				}
				sensor.listener->release(sensor.frames);
			}
			else
			{
				APE_LOG_ERROR("Cannot get frame from Kinect");
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::MultiKinectPlugin::Step()
{

}

void Ape::MultiKinectPlugin::Stop()
{

}

void Ape::MultiKinectPlugin::Suspend()
{

}

void Ape::MultiKinectPlugin::Restart()
{

}