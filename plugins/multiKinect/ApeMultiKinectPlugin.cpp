#include "ApeMultiKinectPlugin.h"

int types = 0 | libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth;
libfreenect2::SyncMultiFrameListener listener(types);
libfreenect2::SyncMultiFrameListener listener2(types);
libfreenect2::FrameMap frames;
libfreenect2::FrameMap frames2;
size_t framecount = 0;

Ape::MultiKinectPlugin::MultiKinectPlugin()
{
	APE_LOG_FUNC_ENTER();

	freenect2;
	device1 = 0;
	device2 = 0;
	//pipeline = 0;

	serial = "";

	mpScene = Ape::ISceneManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&MultiKinectPlugin::eventCallBack, this, std::placeholders::_1));
	RootNode = mpScene->createNode("KinectRootNode").lock();
	APE_LOG_FUNC_LEAVE();
}

Ape::MultiKinectPlugin::~MultiKinectPlugin()
{
	APE_LOG_FUNC_ENTER();
	device1->stop();
	if (sensorNum == 2) device2->stop();
	device1->close();
	if (sensorNum == 2) device2->close();
	delete registration;
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
				rapidjson::Value& sNum = jsonDocument["sensorNumber"];
				sensorNum = jsonDocument["sensorNumber"].GetInt();
				APE_LOG_DEBUG("sensorNumber: " << std::to_string(sensorNum));

				rapidjson::Value& KPosition = jsonDocument["sensorPosition"];
				for (int i = 0; i < 3; i++)
				{
					KPos[i] = jsonDocument["sensorPosition"].GetArray()[i].GetFloat();
					APE_LOG_DEBUG("sensorPosition: " << std::to_string(KPos[i]));
				}

				rapidjson::Value& KPosition2 = jsonDocument["sensorPosition2"];
				for (int i = 0; i < 3; i++)
				{
					KPos2[i] = jsonDocument["sensorPosition2"].GetArray()[i].GetFloat();
					APE_LOG_DEBUG("sensorPosition2: " << std::to_string(KPos2[i]));
				}

				rapidjson::Value& KOrientation = jsonDocument["sensorOrientation"];
				for (int i = 0; i < 4; i++)
				{
					KRot[i] = jsonDocument["sensorOrientation"].GetArray()[i].GetFloat();
					APE_LOG_DEBUG("sensorOrientation: " << std::to_string(KRot[i]));
				}

				rapidjson::Value& KOrientation2 = jsonDocument["sensorOrientation2"];
				for (int i = 0; i < 4; i++)
				{
					KRot2[i] = jsonDocument["sensorOrientation2"].GetArray()[i].GetFloat();
					APE_LOG_DEBUG("sensorOrientation2: " << std::to_string(KRot2[i]));
				}
			}
			fclose(MultiKinectPluginConfigFile);
		}
	}
	else
		APE_LOG_DEBUG("Error cannot open config file");

	if (freenect2.enumerateDevices() == 0)
	{
		APE_LOG_ERROR("Connecting to Kinect failed");
	}

	if (serial == "")
	{
		serial = freenect2.getDeviceSerialNumber(0);
		if(sensorNum == 2) serial2 = freenect2.getDeviceSerialNumber(1);
	}

	device1 = freenect2.openDevice(serial);
	if (sensorNum == 2) device2 = freenect2.openDevice(serial2);

	if (device1 == 0)
	{
		APE_LOG_ERROR("Opening Kinect 0 failed");
	}

	if (device2 == 0 && sensorNum == 2)
	{
		APE_LOG_ERROR("Opening Kinect 1 failed");
	}

	device1->setColorFrameListener(&listener);
	device1->setIrAndDepthFrameListener(&listener);

	if (sensorNum == 2)
	{
		device2->setColorFrameListener(&listener2);
		device2->setIrAndDepthFrameListener(&listener2);
	}

	device1->start();
	if (sensorNum == 2) device2->start();

	APE_LOG_DEBUG("device serial: " + device1->getSerialNumber());
	APE_LOG_DEBUG("device firmware: " + device1->getFirmwareVersion());

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

	if (auto rootNode = RootNode.lock())
	{
		rootNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
		rootNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
	}

	KPts.resize(3 * width * height);
	KCol.resize(3 * width * height);

	if (sensorNum == 2)
	{
		KPts2.resize(3 * width * height);
		KCol2.resize(3 * width * height);
	}
	APE_LOG_FUNC_LEAVE();
}
	

void Ape::MultiKinectPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	registration = new libfreenect2::Registration(device1->getIrCameraParams(), device1->getColorCameraParams());
	libfreenect2::Frame undistorted(width, height, 4), registered(width, height, 4);

	while (true)
	{
		if (!listener.waitForNewFrame(frames, 10 * 1000)) // 10 sconds
		{
			APE_LOG_ERROR("Kinect timeout");
		}

		libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
		libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
		libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

		registration->apply(rgb, depth, &undistorted, &registered);

		for (unsigned int row = 0; row < width; row++)
		{
			for (unsigned int col = 0; col < height; col++)
			{
				float rgb;
				registration->getPointXYZRGB(&undistorted, &registered, row, col, KPts[3 * (col * width + row)], KPts[3 * (col * width + row) + 1], KPts[3 * (col * width + row) + 2], rgb);
				const uint8_t *p = reinterpret_cast<uint8_t*>(&rgb);
				KCol[3 * (col * width + row)] = (float)p[2] / 255;
				KCol[3 * (col * width + row) + 1] = (float)p[1] / 255;
				KCol[3 * (col * width + row) + 2] = (float)p[0] / 255;
			}
		}

		if (sensorNum == 2)
		{
			if (!listener2.waitForNewFrame(frames2, 10 * 1000)) // 10 sconds
			{
				APE_LOG_ERROR("Kinect 2 timeout");
			}

			rgb = frames2[libfreenect2::Frame::Color];
			ir = frames2[libfreenect2::Frame::Ir];
			depth = frames2[libfreenect2::Frame::Depth];

			registration->apply(rgb, depth, &undistorted, &registered);

			for (unsigned int row = 0; row < width; row++)
			{
				for (unsigned int col = 0; col < height; col++)
				{
					float rgb;
					registration->getPointXYZRGB(&undistorted, &registered, row, col, KPts2[3 * (col * width + row)], KPts2[3 * (col * width + row) + 1], KPts2[3 * (col * width + row) + 2], rgb);
					const uint8_t *p = reinterpret_cast<uint8_t*>(&rgb);
					KCol2[3 * (col * width + row)] = (float)p[2] / 255;
					KCol2[3 * (col * width + row) + 1] = (float)p[1] / 255;
					KCol2[3 * (col * width + row) + 2] = (float)p[0] / 255;
				}
			}
		}
		//for each (float coordinate in KPts)
		//{
		//	//coordinate *= 100;
		//}

		//if (sensorNum == 2)
		//{
		//	for each (float coordinate in KPts2)
		//	{
		//		//coordinate *= 100;
		//	}
		//}

		//Generate the Point Cloud
		if (!pointsGenerated/* && KPts[3030] != 0.0 && KPts[3030] != -1 * std::numeric_limits<float>::infinity()*/)
		{
			if (auto pointCloudNode = mpScene->createNode("pointCloudNode_Kinect").lock())
			{
				pointCloudNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
				pointCloudNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
				pointCloudNode->setScale(Ape::Vector3(100,100,100));
				pointCloudNode->showBoundingBox(true);
				if (auto textNode = mpScene->createNode("pointCloudNode_Kinect_Text_Node").lock())
				{
					textNode->setParentNode(pointCloudNode);
					//textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
					if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("pointCloudNode_Kinect_Text", Ape::Entity::GEOMETRY_TEXT).lock()))
					{
						text->setCaption("Kinect");
						text->setParentNode(textNode);
					}
				}
				if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud_Kinect", Ape::Entity::POINT_CLOUD).lock()))
				{
					pointCloud->setParameters(KPts, KCol, 0.01);
					pointCloud->setParentNode(pointCloudNode);
					mPointCloud = pointCloud;
				}
			}

			if (sensorNum == 2)
			{
				if (auto pointCloudNode = mpScene->createNode("pointCloudNode_Kinect2").lock())
				{
					pointCloudNode->setPosition(Ape::Vector3(KPos2[0], KPos2[1], KPos2[2]));
					pointCloudNode->setOrientation(Ape::Quaternion(KRot2[0], KRot2[1], KRot2[2], KRot2[3]));
					pointCloudNode->setScale(Ape::Vector3(100, 100, 100));
					pointCloudNode->showBoundingBox(true);
					if (auto textNode = mpScene->createNode("pointCloudNode_Kinect_Text_Node2").lock())
					{
						textNode->setParentNode(pointCloudNode);
						//textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("pointCloudNode_Kinect_Text2", Ape::Entity::GEOMETRY_TEXT).lock()))
						{
							text->setCaption("Kinect2");
							text->setParentNode(textNode);
						}
					}
					if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud_Kinect2", Ape::Entity::POINT_CLOUD).lock()))
					{
						pointCloud->setParameters(KPts2, KCol2, 0.01);
						pointCloud->setParentNode(pointCloudNode);
						mPointCloud2 = pointCloud;
					}
				}
			}

			pointsGenerated = true;
		}

		//Refresh the Point Cloud
		if (auto pointCloud = mPointCloud.lock())
		{
			pointCloud->updatePoints(KPts);
			pointCloud->updateColors(KCol);
		}

		if (sensorNum == 2)
		{
			if (auto pointCloud = mPointCloud2.lock())
			{
				pointCloud->updatePoints(KPts2);
				pointCloud->updateColors(KCol2);
			}
		}

		framecount++;
		//std::cout << "\n" + std::to_string(framecount) + "\n";
		if (framecount % 100 == 0)
			APE_LOG_DEBUG("got " + std::to_string(framecount) + " frames");
		
		listener.release(frames);
		if (sensorNum == 2) listener2.release(frames2);
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