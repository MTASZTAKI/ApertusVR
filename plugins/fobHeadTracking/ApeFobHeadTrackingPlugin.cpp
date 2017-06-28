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
	mCameraDoubleQueue = Ape::DoubleQueue<Ape::CameraWeakPtr>();
	mCameras = std::vector<Ape::CameraWeakPtr>();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mTrackerConfig = Ape::FobHeadTrackingTrackerConfig();
	mDisplayConfigList = Ape::FobHeadTrackingDisplayConfigList();
	mUserNode = Ape::NodeWeakPtr();
	mCamerasNode = Ape::NodeWeakPtr();
	mTrackedViewerPosition = Ape::Vector3();
	mTrackedViewerOrientation = Ape::Quaternion();
	mTrackedViewerOrientationYPR = Ape::Euler();
}

ApeFobHeadTrackingPlugin::~ApeFobHeadTrackingPlugin()
{
	std::cout << "ApeFobHeadTrackingPlugin dtor" << std::endl;
}

void ApeFobHeadTrackingPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::CAMERA_CREATE)
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
		{
			camera->setParentNode(mUserNode);
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
				else if (trackerMemberIterator->name == "eyeSeparation")
				{
					mTrackerConfig.eyeSeparationPerEye = trackerMemberIterator->value.GetFloat() / 2;
				}
			}
			rapidjson::Value& displays = jsonDocument["displays"];
			for (auto& display : displays.GetArray())
			{
				Ape::FobHeadTrackingDisplayConfig fobHeadTrackingDisplayConfig;
				for (rapidjson::Value::MemberIterator displayMemberIterator = 
					display.MemberBegin(); displayMemberIterator != display.MemberEnd(); ++displayMemberIterator)
				{
					if (displayMemberIterator->name == "size")
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
				}
				mDisplayConfigList.push_back(fobHeadTrackingDisplayConfig);
			}
		}
		fclose(apeFobHeadTrackingPluginConfigFile);
	}	
}

Ape::Matrix4 ApeFobHeadTrackingPlugin::perspectiveOffCenter(float displayDistanceLeft, float displayDistanceRight, float displayDistanceBottom, float displayDistanceTop, float cameraNearClip, float cameraFarClip)
{
	float x = 2.0f * cameraNearClip / (displayDistanceRight - displayDistanceLeft);
	float y = 2.0f * cameraNearClip / (displayDistanceTop - displayDistanceBottom);
	float a = (displayDistanceRight + displayDistanceLeft) / (displayDistanceRight - displayDistanceLeft);
	float b = (displayDistanceTop + displayDistanceBottom) / (displayDistanceTop - displayDistanceBottom);
	float c = -(cameraFarClip + cameraNearClip) / (cameraFarClip - cameraNearClip);
	float d = -(2.0f * cameraFarClip * cameraNearClip) / (cameraFarClip - cameraNearClip);
	Ape::Matrix4 m(
		x, 0, a, 0,
		0, y, b, 0,
		0, 0, c, d,
		0, 0, -1, 0);
	return m;
}

Ape::Matrix4 ApeFobHeadTrackingPlugin::calculateCameraProjection(Ape::Vector3 displayBottomLeftCorner, Ape::Vector3 displayBottomRightCorner, Ape::Vector3 displayTopLeftCorner,
	Ape::Vector3 trackedViewerPosition, float cameraNearClip, float cameraFarClip)
{
	Ape::Vector3 trackedViewerDistanceToDisplayBottomLeftCorner, trackedViewerDistanceToDisplayBottomRightCorner, trackedViewerDistanceToDisplayTopLeftCorner;
	Ape::Vector3 displayWidth, displayHeight, displayNormal;

	float displayDistanceLeft, displayDistanceRight, displayDistanceBottom, displayDistanceTop, trackedViewerDistanceToDisplay;

	displayWidth = displayBottomRightCorner - displayBottomLeftCorner;
	displayWidth.normalise();
	displayHeight = displayTopLeftCorner - displayBottomLeftCorner;
	displayHeight.normalise();
	displayNormal = displayWidth.crossProduct(displayHeight);
	displayNormal.normalise();

	trackedViewerDistanceToDisplayBottomLeftCorner = displayBottomLeftCorner - trackedViewerPosition;
	trackedViewerDistanceToDisplayBottomRightCorner = displayBottomRightCorner - trackedViewerPosition;
	trackedViewerDistanceToDisplayTopLeftCorner = displayTopLeftCorner - trackedViewerPosition;

	trackedViewerDistanceToDisplay = -(trackedViewerDistanceToDisplayBottomLeftCorner.dotProduct(displayNormal));

	displayDistanceLeft = (displayWidth.dotProduct(trackedViewerDistanceToDisplayBottomLeftCorner) * cameraNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceRight = (displayWidth.dotProduct(trackedViewerDistanceToDisplayBottomRightCorner) * cameraNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceBottom = (displayHeight.dotProduct(trackedViewerDistanceToDisplayBottomLeftCorner) * cameraNearClip) / trackedViewerDistanceToDisplay;
	displayDistanceTop = (displayHeight.dotProduct(trackedViewerDistanceToDisplayTopLeftCorner) * cameraNearClip) / trackedViewerDistanceToDisplay;

	Ape::Matrix4 perspectiveOffCenterProjection = perspectiveOffCenter(displayDistanceLeft, displayDistanceRight, displayDistanceBottom, displayDistanceTop, cameraNearClip, cameraFarClip);

	Ape::Matrix4 transform(
		displayWidth.x,  displayWidth.y,  displayWidth.z, 0,
		displayHeight.x, displayHeight.y, displayHeight.z, 0,
		displayNormal.x, displayNormal.y, displayNormal.z, 0,
		0, 0, 0, 1);

	Ape::Matrix4 trackedViewerTranslate(
		1, 0, 0, -trackedViewerPosition.x,
		0, 1, 0, -trackedViewerPosition.y,
		0, 0, 1, -trackedViewerPosition.z,
		0, 0, 0, 1);

	Ape::Matrix4 cameraProjection = perspectiveOffCenterProjection * transform * trackedViewerTranslate;

	return cameraProjection;
}

void ApeFobHeadTrackingPlugin::Run()
{
    int excpectedCameraCount = mDisplayConfigList.size() * 2;
	int cameraCount = 0;
    while (cameraCount < excpectedCameraCount)
	{
		mCameraDoubleQueue.swap();
		while (!mCameraDoubleQueue.emptyPop())
		{
			if (auto camera = mCameraDoubleQueue.front().lock())
			{
				mCameras.push_back(camera);
				if (!mUserNode.lock())
				{
					mUserNode = camera->getParentNode();
					mCamerasNode.lock()->setParentNode(mUserNode);
				}
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
		std::vector<Ape::Vector3> positions;
		std::vector<Ape::Quaternion> orientations;
		int cycleCount = 1000;
		int minMaxCount = 200;
		while (positions.size() < cycleCount && orientations.size() < cycleCount)
		{
			if (positions.size() < cycleCount && trackdGetPosition(mpFobTracker, 0, positionDataFromTracker))
				positions.push_back((Ape::Vector3(positionDataFromTracker[0], positionDataFromTracker[1], positionDataFromTracker[2])
					* mTrackerConfig.scale) + mTrackerConfig.translate);

			if (orientations.size() < cycleCount && trackdGetEulerAngles(mpFobTracker, 0, orientationDataFromTracker))
			{
				mTrackedViewerOrientationYPR = Ape::Euler(Ape::Degree(orientationDataFromTracker[1]).toRadian(),
					Ape::Degree(orientationDataFromTracker[2]).toRadian() - 1.57f,
					Ape::Degree(orientationDataFromTracker[0]).toRadian());
				orientations.push_back(mTrackedViewerOrientationYPR.toQuaternion() * mTrackerConfig.rotation);
			}
		}
		for (int i = 0; i < minMaxCount; i++)
		{
			positions.erase(std::min_element(std::begin(positions), std::end(positions)));
			positions.erase(std::max_element(std::begin(positions), std::end(positions)));
		}
		Ape::Vector3 positionTemp;
		for (auto const& position : positions)
			positionTemp = positionTemp + position;
		mTrackedViewerPosition = (positionTemp / positions.size());

		for (int i = 0; i < minMaxCount; i++)
		{
			orientations.erase(std::min_element(std::begin(orientations), std::end(orientations)));
			orientations.erase(std::max_element(std::begin(orientations), std::end(orientations)));
		}
		Ape::Quaternion orientationTemp;
		for (auto const& orientation : orientations)
			orientationTemp = orientationTemp + orientation;
		mTrackedViewerOrientation = (orientationTemp / orientations.size());

		if (auto camerasNode = mCamerasNode.lock())
			camerasNode->setPosition(mTrackedViewerPosition);

		for (int i = 0; i < mDisplayConfigList.size(); i++)
		{
			auto displayConfig = mDisplayConfigList[i];
			if (auto cameraLeft = mCameras[i * 2].lock())
			{
				if (auto cameraRight = mCameras[i * 2 + 1].lock())
				{
					Ape::Vector3 trackedViewerLeftEyePosition = mTrackedViewerPosition +
						(mTrackedViewerOrientation * Ape::Vector3(-mTrackerConfig.eyeSeparationPerEye, 0, 0));
					Ape::Vector3 trackedViewerRightEyePosition = mTrackedViewerPosition +
						(mTrackedViewerOrientation * Ape::Vector3(mTrackerConfig.eyeSeparationPerEye, 0, 0));
					cameraLeft->setProjection(calculateCameraProjection(displayConfig.bottomLeftCorner, displayConfig.bottomRightCorner, displayConfig.topLeftCorner,
						trackedViewerLeftEyePosition, cameraLeft->getNearClipDistance(), cameraLeft->getFarClipDistance()));
					cameraRight->setProjection(calculateCameraProjection(displayConfig.bottomLeftCorner, displayConfig.bottomRightCorner, displayConfig.topLeftCorner,
						trackedViewerRightEyePosition, cameraRight->getNearClipDistance(), cameraRight->getFarClipDistance()));
				}
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
