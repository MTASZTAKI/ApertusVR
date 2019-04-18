#include "apeKinectHeadTrackingPlugin.h"

ape::KinectHeadTrackingPlugin::KinectHeadTrackingPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpKinectSensor = NULL;
	mpKinectReader = NULL;
	mpCoordinateMapper = NULL;
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&KinectHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&KinectHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mCameraCount = 0;
	mCameraDoubleQueue = ape::DoubleQueue<ape::CameraWeakPtr>();
	mTrackerConfig = ape::HeadTrackerConfig();
	mDisplayConfigList = ape::HeadTrackerDisplayConfigList();
	mTrackedViewerPosition = ape::Vector3(0, -60, 150);
	mTrackedViewerOrientation = ape::Quaternion();
	mTrackedViewerOrientationYPR = ape::Euler();
	mNearClip = 0.0f;
	mFarClip = 0.0f;
	mC = 0.0f;
	mD = 0.0f;
	mpapeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	APE_LOG_FUNC_LEAVE();
}

ape::KinectHeadTrackingPlugin::~KinectHeadTrackingPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&KinectHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&KinectHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	SafeRelease(mpKinectReader);
	SafeRelease(mpCoordinateMapper);
	if (mpKinectSensor)
	{
		mpKinectSensor->Close();
	}
	SafeRelease(mpKinectSensor);
	APE_LOG_FUNC_LEAVE();
}

void ape::KinectHeadTrackingPlugin::setCameraConfigByName(std::string cameraName, ape::CameraWeakPtr cameraWkPtr)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("cameraName: " << cameraName);
	for (int i = 0; i < mDisplayConfigList.size(); i++)
	{
		if (mDisplayConfigList[i].cameraLeftName == cameraName)
		{
			mDisplayConfigList[i].cameraLeft = cameraWkPtr;
			APE_LOG_DEBUG("mDisplayConfigList[" << i << "].cameraLeftName: " << cameraName << " - found");
			return;
		}
		else if (mDisplayConfigList[i].cameraRightName == cameraName)
		{
			mDisplayConfigList[i].cameraRight = cameraWkPtr;
			APE_LOG_DEBUG("mDisplayConfigList[" << i << "].cameraRightName: " << cameraName << " - found");
			return;
		}
	}
	APE_LOG_FUNC_LEAVE();
}


void ape::KinectHeadTrackingPlugin::eventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::CAMERA_FOVY) //beacuse this is the last event before every property is set up
	{
		if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			APE_LOG_DEBUG("camera: " << camera->getName());
			mCameraDoubleQueue.push(camera);
		}
	}
}

ape::Matrix4 ape::KinectHeadTrackingPlugin::perspectiveOffCenter(float& displayDistanceLeft, float& displayDistanceRight, float& displayDistanceBottom, float& displayDistanceTop)
{
	float x = 2.0f * mNearClip / (displayDistanceRight - displayDistanceLeft);
	float y = 2.0f * mNearClip / (displayDistanceTop - displayDistanceBottom);
	float a = (displayDistanceRight + displayDistanceLeft) / (displayDistanceRight - displayDistanceLeft);
	float b = (displayDistanceTop + displayDistanceBottom) / (displayDistanceTop - displayDistanceBottom);
	ape::Matrix4 m(
		x, 0, a, 0,
		0, y, b, 0,
		0, 0, mC, mD,
		0, 0, -1, 0);
	return m;
}

ape::Matrix4 ape::KinectHeadTrackingPlugin::calculateCameraProjection(ape::HeadTrackerDisplayConfig& displayConfig, ape::Vector3& trackedEyePosition)
{
	ape::Vector3 trackedViewerDistanceToDisplayBottomLeftCorner, trackedViewerDistanceToDisplayBottomRightCorner, trackedViewerDistanceToDisplayTopLeftCorner;

	float displayDistanceLeft, displayDistanceRight, displayDistanceBottom, displayDistanceTop, trackedViewerDistanceToDisplay;

	trackedViewerDistanceToDisplayBottomLeftCorner = displayConfig.bottomLeftCorner - trackedEyePosition;
	trackedViewerDistanceToDisplayBottomRightCorner = displayConfig.bottomRightCorner - trackedEyePosition;
	trackedViewerDistanceToDisplayTopLeftCorner = displayConfig.topLeftCorner - trackedEyePosition;

	trackedViewerDistanceToDisplay = -(trackedViewerDistanceToDisplayBottomLeftCorner.dotProduct(displayConfig.normal));

	displayDistanceLeft = (displayConfig.width.dotProduct(trackedViewerDistanceToDisplayBottomLeftCorner) * mNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceRight = (displayConfig.width.dotProduct(trackedViewerDistanceToDisplayBottomRightCorner) * mNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceBottom = (displayConfig.height.dotProduct(trackedViewerDistanceToDisplayBottomLeftCorner) * mNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceTop = (displayConfig.height.dotProduct(trackedViewerDistanceToDisplayTopLeftCorner) * mNearClip) / trackedViewerDistanceToDisplay;

	ape::Matrix4 perspectiveOffCenterProjection = perspectiveOffCenter(displayDistanceLeft, displayDistanceRight, displayDistanceBottom, displayDistanceTop);

	ape::Matrix4 trackedViewerTranslate(
		1, 0, 0, -trackedEyePosition.x,
		0, 1, 0, -trackedEyePosition.y,
		0, 0, 1, -trackedEyePosition.z,
		0, 0, 0, 1);

	ape::Matrix4 cameraProjection = perspectiveOffCenterProjection * displayConfig.transform * trackedViewerTranslate;

	return cameraProjection;
}

void ape::KinectHeadTrackingPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("Sensor init begin");
	InitializeDefaultSensor();
	APE_LOG_DEBUG("Sensor init finished");
	mpapeUserInputMacro->createOverLayText("userPosition");
	ape::UserInputMacro::Pose overLayTextPose;
	overLayTextPose.position = ape::Vector3(0, -100, -50);
	mpapeUserInputMacro->updateOverLayTextPose(overLayTextPose);
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeKinectHeadTrackingPlugin.json";
	FILE* apeKinectHeadTrackingPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeKinectHeadTrackingPluginConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeKinectHeadTrackingPluginConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& tracker = jsonDocument["tracker"];
			for (rapidjson::Value::MemberIterator trackerMemberIterator =
				tracker.MemberBegin(); trackerMemberIterator != tracker.MemberEnd(); ++trackerMemberIterator)
			{
				if (trackerMemberIterator->name == "rotation")
				{
					ape::Degree angle;
					ape::Vector3 axis;
					for (rapidjson::Value::MemberIterator rotationMemberIterator =
						tracker[trackerMemberIterator->name].MemberBegin();
						rotationMemberIterator != tracker[trackerMemberIterator->name].MemberEnd(); ++rotationMemberIterator)
					{
						if (rotationMemberIterator->name == "angle")
							angle = rotationMemberIterator->value.GetFloat();
						else if (rotationMemberIterator->name == "x")
							axis.x = rotationMemberIterator->value.GetFloat();
						else if (rotationMemberIterator->name == "y")
							axis.y = rotationMemberIterator->value.GetFloat();
						else if (rotationMemberIterator->name == "z")
							axis.z = rotationMemberIterator->value.GetFloat();
					}
					mTrackerConfig.rotation.FromAngleAxis(ape::Radian(angle.toRadian()), axis);
				}
				else if (trackerMemberIterator->name == "translate")
				{
					for (rapidjson::Value::MemberIterator translateMemberIterator =
						tracker[trackerMemberIterator->name].MemberBegin();
						translateMemberIterator != tracker[trackerMemberIterator->name].MemberEnd(); ++translateMemberIterator)
					{
						if (translateMemberIterator->name == "x")
							mTrackerConfig.translate.x = translateMemberIterator->value.GetFloat();
						else if (translateMemberIterator->name == "y")
							mTrackerConfig.translate.y = translateMemberIterator->value.GetFloat();
						else if (translateMemberIterator->name == "z")
							mTrackerConfig.translate.z = translateMemberIterator->value.GetFloat();
					}
				}
				else if (trackerMemberIterator->name == "scale")
				{
					for (rapidjson::Value::MemberIterator scaleMemberIterator =
						tracker[trackerMemberIterator->name].MemberBegin();
						scaleMemberIterator != tracker[trackerMemberIterator->name].MemberEnd(); ++scaleMemberIterator)
					{
						if (scaleMemberIterator->name == "x")
							mTrackerConfig.scale.x = scaleMemberIterator->value.GetFloat();
						else if (scaleMemberIterator->name == "y")
							mTrackerConfig.scale.y = scaleMemberIterator->value.GetFloat();
						else if (scaleMemberIterator->name == "z")
							mTrackerConfig.scale.z = scaleMemberIterator->value.GetFloat();
					}
				}
				else if (trackerMemberIterator->name == "leftEyeOffset")
				{
					mTrackerConfig.leftEyeOffset = ape::Vector3(trackerMemberIterator->value.GetFloat(), 0, 0);
				}
				else if (trackerMemberIterator->name == "rightEyeOffset")
				{
					mTrackerConfig.rightEyeOffset = ape::Vector3(trackerMemberIterator->value.GetFloat(), 0, 0);
				}
			}
			rapidjson::Value& displays = jsonDocument["displays"];
			for (auto& display : displays.GetArray())
			{
				ape::HeadTrackerDisplayConfig kinectHeadTrackingDisplayConfig;
				for (rapidjson::Value::MemberIterator displayMemberIterator =
					display.MemberBegin(); displayMemberIterator != display.MemberEnd(); ++displayMemberIterator)
				{
					if (displayMemberIterator->name == "name")
					{
						kinectHeadTrackingDisplayConfig.name = displayMemberIterator->value.GetString();
					}
					else if (displayMemberIterator->name == "cameras")
					{
						for (rapidjson::Value::MemberIterator camerasMemberIterator =
							display[displayMemberIterator->name].MemberBegin();
							camerasMemberIterator != display[displayMemberIterator->name].MemberEnd(); ++camerasMemberIterator)
						{
							if (camerasMemberIterator->name == "left")
							{
								kinectHeadTrackingDisplayConfig.cameraLeftName = camerasMemberIterator->value.GetString();
								mCameraCount++;
							}
							else if (camerasMemberIterator->name == "right")
							{
								kinectHeadTrackingDisplayConfig.cameraRightName = camerasMemberIterator->value.GetString();
								mCameraCount++;
							}
						}
					}
					else if (displayMemberIterator->name == "size")
					{
						for (rapidjson::Value::MemberIterator sizeMemberIterator =
							display[displayMemberIterator->name].MemberBegin();
							sizeMemberIterator != display[displayMemberIterator->name].MemberEnd(); ++sizeMemberIterator)
						{
							if (sizeMemberIterator->name == "width")
								kinectHeadTrackingDisplayConfig.size.x = sizeMemberIterator->value.GetFloat();
							else if (sizeMemberIterator->name == "height")
								kinectHeadTrackingDisplayConfig.size.y = sizeMemberIterator->value.GetFloat();
						}
					}
					else if (displayMemberIterator->name == "position")
					{
						for (rapidjson::Value::MemberIterator positionMemberIterator =
							display[displayMemberIterator->name].MemberBegin();
							positionMemberIterator != display[displayMemberIterator->name].MemberEnd(); ++positionMemberIterator)
						{
							if (positionMemberIterator->name == "x")
								kinectHeadTrackingDisplayConfig.position.x = positionMemberIterator->value.GetFloat();
							else if (positionMemberIterator->name == "y")
								kinectHeadTrackingDisplayConfig.position.y = positionMemberIterator->value.GetFloat();
							else if (positionMemberIterator->name == "z")
								kinectHeadTrackingDisplayConfig.position.z = positionMemberIterator->value.GetFloat();
						}
					}
					else if (displayMemberIterator->name == "orientation")
					{
						ape::Degree angle;
						ape::Vector3 axis;
						for (rapidjson::Value::MemberIterator orientationMemberIterator =
							display[displayMemberIterator->name].MemberBegin();
							orientationMemberIterator != display[displayMemberIterator->name].MemberEnd(); ++orientationMemberIterator)
						{
							if (orientationMemberIterator->name == "angle")
								angle = orientationMemberIterator->value.GetFloat();
							else if (orientationMemberIterator->name == "x")
								axis.x = orientationMemberIterator->value.GetFloat();
							else if (orientationMemberIterator->name == "y")
								axis.y = orientationMemberIterator->value.GetFloat();
							else if (orientationMemberIterator->name == "z")
								axis.z = orientationMemberIterator->value.GetFloat();
						}
						kinectHeadTrackingDisplayConfig.orientation.FromAngleAxis(ape::Radian(angle.toRadian()), axis);
					}
					else if (displayMemberIterator->name == "corners")
					{
						rapidjson::Value& corners = display[displayMemberIterator->name];
						for (auto& corner : corners.GetArray())
						{
							for (rapidjson::Value::MemberIterator cornerMemberIterator =
								corner.MemberBegin(); cornerMemberIterator != corner.MemberEnd(); ++cornerMemberIterator)
							{
								if (cornerMemberIterator->name == "bottomLeftCorner")
								{
									for (rapidjson::Value::MemberIterator memberIterator =
										corner[cornerMemberIterator->name].MemberBegin();
										memberIterator != corner[cornerMemberIterator->name].MemberEnd(); ++memberIterator)
									{
										if (memberIterator->name == "x")
											kinectHeadTrackingDisplayConfig.bottomLeftCorner.x = memberIterator->value.GetFloat();
										else if (memberIterator->name == "y")
											kinectHeadTrackingDisplayConfig.bottomLeftCorner.y = memberIterator->value.GetFloat();
										else if (memberIterator->name == "z")
											kinectHeadTrackingDisplayConfig.bottomLeftCorner.z = memberIterator->value.GetFloat();
									}
								}
								else if (cornerMemberIterator->name == "bottomRightCorner")
								{
									for (rapidjson::Value::MemberIterator memberIterator =
										corner[cornerMemberIterator->name].MemberBegin();
										memberIterator != corner[cornerMemberIterator->name].MemberEnd(); ++memberIterator)
									{
										if (memberIterator->name == "x")
											kinectHeadTrackingDisplayConfig.bottomRightCorner.x = memberIterator->value.GetFloat();
										else if (memberIterator->name == "y")
											kinectHeadTrackingDisplayConfig.bottomRightCorner.y = memberIterator->value.GetFloat();
										else if (memberIterator->name == "z")
											kinectHeadTrackingDisplayConfig.bottomRightCorner.z = memberIterator->value.GetFloat();
									}
								}
								else if (cornerMemberIterator->name == "topLeftCorner")
								{
									for (rapidjson::Value::MemberIterator memberIterator =
										corner[cornerMemberIterator->name].MemberBegin();
										memberIterator != corner[cornerMemberIterator->name].MemberEnd(); ++memberIterator)
									{
										if (memberIterator->name == "x")
											kinectHeadTrackingDisplayConfig.topLeftCorner.x = memberIterator->value.GetFloat();
										else if (memberIterator->name == "y")
											kinectHeadTrackingDisplayConfig.topLeftCorner.y = memberIterator->value.GetFloat();
										else if (memberIterator->name == "z")
											kinectHeadTrackingDisplayConfig.topLeftCorner.z = memberIterator->value.GetFloat();
									}
								}
							}
						}
					}
					kinectHeadTrackingDisplayConfig.width = kinectHeadTrackingDisplayConfig.bottomRightCorner - kinectHeadTrackingDisplayConfig.bottomLeftCorner;
					kinectHeadTrackingDisplayConfig.width.normalise();
					kinectHeadTrackingDisplayConfig.height = kinectHeadTrackingDisplayConfig.topLeftCorner - kinectHeadTrackingDisplayConfig.bottomLeftCorner;
					kinectHeadTrackingDisplayConfig.height.normalise();
					kinectHeadTrackingDisplayConfig.normal = kinectHeadTrackingDisplayConfig.width.crossProduct(kinectHeadTrackingDisplayConfig.height);
					kinectHeadTrackingDisplayConfig.normal.normalise();

					kinectHeadTrackingDisplayConfig.transform = ape::Matrix4(
						kinectHeadTrackingDisplayConfig.width.x, kinectHeadTrackingDisplayConfig.width.y, kinectHeadTrackingDisplayConfig.width.z, 0,
						kinectHeadTrackingDisplayConfig.height.x, kinectHeadTrackingDisplayConfig.height.y, kinectHeadTrackingDisplayConfig.height.z, 0,
						kinectHeadTrackingDisplayConfig.normal.x, kinectHeadTrackingDisplayConfig.normal.y, kinectHeadTrackingDisplayConfig.normal.z, 0,
						0, 0, 0, 1);
				}
				APE_LOG_DEBUG("mDisplayConfigList.push_back(" << kinectHeadTrackingDisplayConfig.name << ")");
				mDisplayConfigList.push_back(kinectHeadTrackingDisplayConfig);
			}
		}
		fclose(apeKinectHeadTrackingPluginConfigFile);
	}
	APE_LOG_DEBUG("mCameraCount: " << mCameraCount);
	APE_LOG_FUNC_LEAVE();
}

void ape::KinectHeadTrackingPlugin::getHeadPositionFromBodyData(IBody* pBody)
{
	APE_LOG_FUNC_ENTER();
	HRESULT hr;
	if (mpCoordinateMapper)
	{
		if (pBody)
		{
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);
			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count];
				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					if (joints[JointType_Head].TrackingState == 2)
					{
						ape::Vector3 positionFromSensor(joints[JointType_Head].Position.X, joints[JointType_Head].Position.Y, joints[JointType_Head].Position.Z);
						mTrackedViewerPosition = ((mTrackerConfig.rotation * positionFromSensor) * mTrackerConfig.scale) + mTrackerConfig.translate;
						mpapeUserInputMacro->updateOverLayText(mTrackedViewerPosition.toString());
					}
				}
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::KinectHeadTrackingPlugin::getBodyDataFromSensor(IMultiSourceFrame* pframe)
{
	APE_LOG_FUNC_ENTER();
	IBodyFrame* pBodyFrame = NULL;
	IBodyFrameReference* pBodyFrameRef = NULL;
	HRESULT hr = pframe->get_BodyFrameReference(&pBodyFrameRef);
	if (SUCCEEDED(hr))
	{
		hr = pBodyFrameRef->AcquireFrame(&pBodyFrame);
	}
	else
	{
		APE_LOG_DEBUG("get_BodyFrameReference Error: " << HRESULT_CODE(hr));
	}
	SafeRelease(pBodyFrameRef);

	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };
		hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < BODY_COUNT; ++i)
			{
				IBody* pBody = ppBodies[i];
				if (pBody)
				{
					getHeadPositionFromBodyData(pBody);
				}
			}
		}
		else
		{
			APE_LOG_DEBUG("GetAndRefreshBodyData Error: " << HRESULT_CODE(hr));
		}
		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}
	SafeRelease(pBodyFrame);
	APE_LOG_FUNC_LEAVE();
}

void ape::KinectHeadTrackingPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	int cameraCount = 0;
	while (cameraCount < mCameraCount)
	{
		mCameraDoubleQueue.swap();
		while (!mCameraDoubleQueue.emptyPop())
		{
			if (auto camera = mCameraDoubleQueue.front().lock())
			{
				setCameraConfigByName(camera->getName(), camera);
				mNearClip = camera->getNearClipDistance();
				mFarClip = camera->getFarClipDistance();
				mC = -(mFarClip + mNearClip) / (mFarClip - mNearClip);
				mD = -(2.0f * mFarClip * mNearClip) / (mFarClip - mNearClip);
				APE_LOG_DEBUG("camera: " << camera->getName() << " nearClip: " << mNearClip << " farClip: " << mFarClip);
				cameraCount++;
			}
			mCameraDoubleQueue.pop();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	while (true)
	{
		IMultiSourceFrame* pFrame = NULL;
		float positionDataFromTracker[3];
		if (mpKinectReader)
		{
			HRESULT hr = mpKinectReader->AcquireLatestFrame(&pFrame);
			if (SUCCEEDED(hr))
			{
				getBodyDataFromSensor(pFrame);
				for (int i = 0; i < mDisplayConfigList.size(); i++)
				{
					auto displayConfig = mDisplayConfigList[i];
					if (auto cameraLeft = displayConfig.cameraLeft.lock())
					{
						cameraLeft->setProjection(calculateCameraProjection(displayConfig, mTrackedViewerPosition));
					}
				}
			}
			SafeRelease(pFrame);
		}
	}
	APE_LOG_FUNC_LEAVE();
}

HRESULT ape::KinectHeadTrackingPlugin::InitializeDefaultSensor()
{
	APE_LOG_FUNC_ENTER();
	HRESULT hr;
	hr = GetDefaultKinectSensor(&mpKinectSensor);
	if (FAILED(hr))
	{
		return hr;
		APE_LOG_ERROR("Connecting to Kinect failed");
	}
	if (mpKinectSensor)
	{
		hr = mpKinectSensor->get_CoordinateMapper(&mpCoordinateMapper);
		if (SUCCEEDED(hr))
		{
			hr = mpKinectSensor->Open();
		}
		if (SUCCEEDED(hr))
		{
			hr = mpKinectSensor->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Body, &mpKinectReader);
		}
		APE_LOG_DEBUG("Connected to Kinect");
	}
	if (!mpKinectSensor || FAILED(hr))
	{
		APE_LOG_ERROR("Kinect has not found");
		return E_FAIL;
	}
	APE_LOG_FUNC_LEAVE();
	return hr;
}

void ape::KinectHeadTrackingPlugin::Step()
{

}

void ape::KinectHeadTrackingPlugin::Stop()
{

}

void ape::KinectHeadTrackingPlugin::Suspend()
{

}

void ape::KinectHeadTrackingPlugin::Restart()
{

}

