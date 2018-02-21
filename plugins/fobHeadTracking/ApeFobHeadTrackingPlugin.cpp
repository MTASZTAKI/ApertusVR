#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "ApeFobHeadTrackingPlugin.h"

ApeFobHeadTrackingPlugin::ApeFobHeadTrackingPlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeFobHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeFobHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mpFobTracker = nullptr;
	mCameraCount = 0;
	mCameraDoubleQueue = Ape::DoubleQueue<Ape::CameraWeakPtr>();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mTrackerConfig = Ape::FobHeadTrackingTrackerConfig();
	mDisplayConfigList = Ape::FobHeadTrackingDisplayConfigList();
	mCamerasNode = Ape::NodeWeakPtr();
	mTrackedViewerPosition = Ape::Vector3();
	mTrackedViewerOrientation = Ape::Quaternion();
	mTrackedViewerOrientationYPR = Ape::Euler();
	mNearClip = 0.0f;
	mFarClip = 0.0f;
	mC = 0.0f;
	mD = 0.0f;
}

ApeFobHeadTrackingPlugin::~ApeFobHeadTrackingPlugin()
{
	std::cout << "ApeFobHeadTrackingPlugin dtor" << std::endl;
}

void ApeFobHeadTrackingPlugin::setCameraConfigByName(std::string cameraName, Ape::CameraWeakPtr cameraWkPtr)
{
	std::cout << "setCameraConfigByName() cameraName: " << cameraName << std::endl;
	for (int i = 0; i < mDisplayConfigList.size(); i++)
	{
		if (mDisplayConfigList[i].cameraLeftName == cameraName)
		{
			mDisplayConfigList[i].cameraLeft = cameraWkPtr;
			std::cout << "setCameraConfigByName() mDisplayConfigList[" << i << "].cameraLeftName: " << cameraName << " - found" << std::endl;
			return;
		}
		else if (mDisplayConfigList[i].cameraRightName == cameraName)
		{
			mDisplayConfigList[i].cameraRight = cameraWkPtr;
			std::cout << "setCameraConfigByName() mDisplayConfigList[" << i << "].cameraRightName: " << cameraName << " - found" << std::endl;
			return;
		}
	}
	std::cout << "setCameraConfigByName() end - fckn nicht gut!!!!" << std::endl;
}

void ApeFobHeadTrackingPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName)
		mUserNode = mpScene->getNode(event.subjectName);
	else if (event.type == Ape::Event::Type::CAMERA_CREATE)
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
		{
			std::cout << "eventCallBack() camera: " << camera->getName() << std::endl;
			mCameraDoubleQueue.push(camera);
		}
	}
}

void ApeFobHeadTrackingPlugin::Init()
{
	std::cout << "ApeFobHeadTrackingPlugin::init" << std::endl;
	mCamerasNode = mpScene->createNode("FobHeadTrackingNode");
	std::cout << "ApeFobHeadTrackingPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "ApeFobHeadTrackingPlugin main window was found" << std::endl;
	mpFobTracker = trackdInitTrackerReader(4126);
	
	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "\\ApeFobHeadTrackingPlugin.json";
	FILE* apeFobHeadTrackingPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeFobHeadTrackingPluginConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeFobHeadTrackingPluginConfigFile, readBuffer, sizeof(readBuffer));
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
					Ape::Degree angle;
					Ape::Vector3 axis;
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
					mTrackerConfig.rotation.FromAngleAxis(angle, axis);
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
					mTrackerConfig.leftEyeOffset = Ape::Vector3(trackerMemberIterator->value.GetFloat(), 0, 0);
				}
				else if (trackerMemberIterator->name == "rightEyeOffset")
				{
					mTrackerConfig.rightEyeOffset = Ape::Vector3(trackerMemberIterator->value.GetFloat(), 0, 0);
				}
			}
			rapidjson::Value& displays = jsonDocument["displays"];
			for (auto& display : displays.GetArray())
			{
				Ape::FobHeadTrackingDisplayConfig fobHeadTrackingDisplayConfig;
				for (rapidjson::Value::MemberIterator displayMemberIterator = 
					display.MemberBegin(); displayMemberIterator != display.MemberEnd(); ++displayMemberIterator)
				{
					if (displayMemberIterator->name == "name")
					{
						fobHeadTrackingDisplayConfig.name = displayMemberIterator->value.GetString();
					}
					else if (displayMemberIterator->name == "cameras")
					{
						for (rapidjson::Value::MemberIterator camerasMemberIterator =
							display[displayMemberIterator->name].MemberBegin();
							camerasMemberIterator != display[displayMemberIterator->name].MemberEnd(); ++camerasMemberIterator)
						{
							if (camerasMemberIterator->name == "left")
							{
								fobHeadTrackingDisplayConfig.cameraLeftName = camerasMemberIterator->value.GetString();
								mCameraCount++;
							}
							else if (camerasMemberIterator->name == "right")
							{
								fobHeadTrackingDisplayConfig.cameraRightName = camerasMemberIterator->value.GetString();
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
								fobHeadTrackingDisplayConfig.size.x = sizeMemberIterator->value.GetFloat();
							else if (sizeMemberIterator->name == "height")
								fobHeadTrackingDisplayConfig.size.y = sizeMemberIterator->value.GetFloat();
						}
					}
					else if (displayMemberIterator->name == "position")
					{
						for (rapidjson::Value::MemberIterator positionMemberIterator =
							display[displayMemberIterator->name].MemberBegin();
							positionMemberIterator != display[displayMemberIterator->name].MemberEnd(); ++positionMemberIterator)
						{
							if (positionMemberIterator->name == "x")
								fobHeadTrackingDisplayConfig.position.x = positionMemberIterator->value.GetFloat();
							else if (positionMemberIterator->name == "y")
								fobHeadTrackingDisplayConfig.position.y = positionMemberIterator->value.GetFloat();
							else if (positionMemberIterator->name == "z")
								fobHeadTrackingDisplayConfig.position.z = positionMemberIterator->value.GetFloat();
						}
					}
					else if (displayMemberIterator->name == "orientation")
					{
					    Ape::Degree angle;
						Ape::Vector3 axis;
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
						fobHeadTrackingDisplayConfig.orientation.FromAngleAxis(angle, axis);
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
											fobHeadTrackingDisplayConfig.bottomLeftCorner.x = memberIterator->value.GetFloat();
										else if (memberIterator->name == "y")
											fobHeadTrackingDisplayConfig.bottomLeftCorner.y = memberIterator->value.GetFloat();
										else if (memberIterator->name == "z")
											fobHeadTrackingDisplayConfig.bottomLeftCorner.z = memberIterator->value.GetFloat();
									}
								}
								else if (cornerMemberIterator->name == "bottomRightCorner")
								{
									for (rapidjson::Value::MemberIterator memberIterator =
										corner[cornerMemberIterator->name].MemberBegin();
										memberIterator != corner[cornerMemberIterator->name].MemberEnd(); ++memberIterator)
									{
										if (memberIterator->name == "x")
											fobHeadTrackingDisplayConfig.bottomRightCorner.x = memberIterator->value.GetFloat();
										else if (memberIterator->name == "y")
											fobHeadTrackingDisplayConfig.bottomRightCorner.y = memberIterator->value.GetFloat();
										else if (memberIterator->name == "z")
											fobHeadTrackingDisplayConfig.bottomRightCorner.z = memberIterator->value.GetFloat();
									}
								}
								else if (cornerMemberIterator->name == "topLeftCorner")
								{
									for (rapidjson::Value::MemberIterator memberIterator =
										corner[cornerMemberIterator->name].MemberBegin();
										memberIterator != corner[cornerMemberIterator->name].MemberEnd(); ++memberIterator)
									{
										if (memberIterator->name == "x")
											fobHeadTrackingDisplayConfig.topLeftCorner.x = memberIterator->value.GetFloat();
										else if (memberIterator->name == "y")
											fobHeadTrackingDisplayConfig.topLeftCorner.y = memberIterator->value.GetFloat();
										else if (memberIterator->name == "z")
											fobHeadTrackingDisplayConfig.topLeftCorner.z = memberIterator->value.GetFloat();
									}
								}
							}
						}
					}
					fobHeadTrackingDisplayConfig.width = fobHeadTrackingDisplayConfig.bottomRightCorner - fobHeadTrackingDisplayConfig.bottomLeftCorner;
					fobHeadTrackingDisplayConfig.width.normalise();
					fobHeadTrackingDisplayConfig.height = fobHeadTrackingDisplayConfig.topLeftCorner - fobHeadTrackingDisplayConfig.bottomLeftCorner;
					fobHeadTrackingDisplayConfig.height.normalise();
					fobHeadTrackingDisplayConfig.normal = fobHeadTrackingDisplayConfig.width.crossProduct(fobHeadTrackingDisplayConfig.height);
					fobHeadTrackingDisplayConfig.normal.normalise();

					fobHeadTrackingDisplayConfig.transform = Ape::Matrix4(
						fobHeadTrackingDisplayConfig.width.x, fobHeadTrackingDisplayConfig.width.y, fobHeadTrackingDisplayConfig.width.z, 0,
						fobHeadTrackingDisplayConfig.height.x, fobHeadTrackingDisplayConfig.height.y, fobHeadTrackingDisplayConfig.height.z, 0,
						fobHeadTrackingDisplayConfig.normal.x, fobHeadTrackingDisplayConfig.normal.y, fobHeadTrackingDisplayConfig.normal.z, 0,
						0, 0, 0, 1);
				}
				std::cout << "Init() mDisplayConfigList.push_back(" << fobHeadTrackingDisplayConfig.name << ")" << std::endl;
				mDisplayConfigList.push_back(fobHeadTrackingDisplayConfig);
			}
		}
		fclose(apeFobHeadTrackingPluginConfigFile);
	}
	std::cout << "Init() mCameraCount: " << mCameraCount << std::endl;
}

Ape::Matrix4 ApeFobHeadTrackingPlugin::perspectiveOffCenter(float& displayDistanceLeft, float& displayDistanceRight, float& displayDistanceBottom, float& displayDistanceTop)
{
	float x = 2.0f * mNearClip / (displayDistanceRight - displayDistanceLeft);
	float y = 2.0f * mNearClip / (displayDistanceTop - displayDistanceBottom);
	float a = (displayDistanceRight + displayDistanceLeft) / (displayDistanceRight - displayDistanceLeft);
	float b = (displayDistanceTop + displayDistanceBottom) / (displayDistanceTop - displayDistanceBottom);
	Ape::Matrix4 m(
		x, 0, a, 0,
		0, y, b, 0,
		0, 0, mC, mD,
		0, 0, -1, 0);
	return m;
}

Ape::Matrix4 ApeFobHeadTrackingPlugin::calculateCameraProjection(Ape::FobHeadTrackingDisplayConfig& displayConfig)
{
	Ape::Vector3 trackedViewerDistanceToDisplayBottomLeftCorner, trackedViewerDistanceToDisplayBottomRightCorner, trackedViewerDistanceToDisplayTopLeftCorner;

	float displayDistanceLeft, displayDistanceRight, displayDistanceBottom, displayDistanceTop, trackedViewerDistanceToDisplay;

	trackedViewerDistanceToDisplayBottomLeftCorner = displayConfig.bottomLeftCorner - mTrackedViewerPosition;
	trackedViewerDistanceToDisplayBottomRightCorner = displayConfig.bottomRightCorner - mTrackedViewerPosition;
	trackedViewerDistanceToDisplayTopLeftCorner = displayConfig.topLeftCorner - mTrackedViewerPosition;

	trackedViewerDistanceToDisplay = -(trackedViewerDistanceToDisplayBottomLeftCorner.dotProduct(displayConfig.normal));

	displayDistanceLeft = (displayConfig.width.dotProduct(trackedViewerDistanceToDisplayBottomLeftCorner) * mNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceRight = (displayConfig.width.dotProduct(trackedViewerDistanceToDisplayBottomRightCorner) * mNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceBottom = (displayConfig.height.dotProduct(trackedViewerDistanceToDisplayBottomLeftCorner) * mNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceTop = (displayConfig.height.dotProduct(trackedViewerDistanceToDisplayTopLeftCorner) * mNearClip) / trackedViewerDistanceToDisplay;

	Ape::Matrix4 perspectiveOffCenterProjection = perspectiveOffCenter(displayDistanceLeft, displayDistanceRight, displayDistanceBottom, displayDistanceTop);

	Ape::Matrix4 trackedViewerTranslate(
		1, 0, 0, -mTrackedViewerPosition.x,
		0, 1, 0, -mTrackedViewerPosition.y,
		0, 0, 1, -mTrackedViewerPosition.z,
		0, 0, 0, 1);

	Ape::Matrix4 cameraProjection = perspectiveOffCenterProjection * displayConfig.transform * trackedViewerTranslate;

	return cameraProjection;
}

void ApeFobHeadTrackingPlugin::Run()
{
	int cameraCount = 0;
    while (cameraCount < mCameraCount)
	{
		mCameraDoubleQueue.swap();
		while (!mCameraDoubleQueue.emptyPop())
		{
			if (auto camera = mCameraDoubleQueue.front().lock())
			{
				std::cout << "ApeFobHeadTrackingPlugin::Run() camera: " << camera->getName() << std::endl;
				setCameraConfigByName(camera->getName(), camera);
				mCamerasNode.lock()->setParentNode(mUserNode);
				mNearClip = camera->getNearClipDistance();
				mFarClip = camera->getFarClipDistance();
				mC = -(mFarClip + mNearClip) / (mFarClip - mNearClip);
				mD = -(2.0f * mFarClip * mNearClip) / (mFarClip - mNearClip);
				camera->setParentNode(mCamerasNode);
				cameraCount++;
			}
			mCameraDoubleQueue.pop();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	while (mpFobTracker)
	{
		float positionDataFromTracker[3];
		float orientationDataFromTracker[3];
		if (trackdGetPosition(mpFobTracker, 0, positionDataFromTracker))
			mTrackedViewerPosition = (Ape::Vector3(positionDataFromTracker[0], positionDataFromTracker[1], positionDataFromTracker[2]) * mTrackerConfig.scale) + mTrackerConfig.translate;
		if (trackdGetEulerAngles(mpFobTracker, 0, orientationDataFromTracker))
		{
			mTrackedViewerOrientationYPR = Ape::Euler(Ape::Degree(orientationDataFromTracker[1]).toRadian(), Ape::Degree(orientationDataFromTracker[2]).toRadian() - 1.57f, Ape::Degree(orientationDataFromTracker[0]).toRadian());
			mTrackedViewerOrientation = mTrackedViewerOrientationYPR.toQuaternion() * mTrackerConfig.rotation;
		}
		if (auto camerasNode = mCamerasNode.lock())
			camerasNode->setPosition(mTrackedViewerPosition);
		for (int i = 0; i < mDisplayConfigList.size(); i++)
		{
			auto displayConfig = mDisplayConfigList[i];
			if (auto cameraLeft = displayConfig.cameraLeft.lock())
			{
				Ape::Vector3 trackedViewerLeftEyePosition = mTrackedViewerPosition + (mTrackedViewerOrientation * mTrackerConfig.leftEyeOffset);
				cameraLeft->setProjection(calculateCameraProjection(displayConfig));
			}
			if (auto cameraRight = displayConfig.cameraRight.lock())
			{
				Ape::Vector3 trackedViewerRightEyePosition = mTrackedViewerPosition + (mTrackedViewerOrientation * mTrackerConfig.rightEyeOffset);
				cameraRight->setProjection(calculateCameraProjection(displayConfig));
			}
		}
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeFobHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeFobHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
}

void ApeFobHeadTrackingPlugin::Step()
{

}

void ApeFobHeadTrackingPlugin::Stop()
{

}

void ApeFobHeadTrackingPlugin::Suspend()
{

}

void ApeFobHeadTrackingPlugin::Restart()
{

}
