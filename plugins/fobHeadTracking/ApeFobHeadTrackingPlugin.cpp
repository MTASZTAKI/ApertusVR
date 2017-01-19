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
}

ApeFobHeadTrackingPlugin::~ApeFobHeadTrackingPlugin()
{
	std::cout << "ApeFobHeadTrackingPlugin dtor" << std::endl;
}

void ApeFobHeadTrackingPlugin::eventCallBack(const Ape::Event& event)
{
    if (event.type == Ape::Event::Type::CAMERA_PARENTNODE)
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
		{
			mCameraDoubleQueue.push(camera);
			if (!mUserNode.lock())
				mUserNode = camera->getParentNode();
		}
	}
}

void ApeFobHeadTrackingPlugin::Init()
{
	std::cout << "ApeFobHeadTrackingPlugin::init" << std::endl;
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
					mTrackerConfig.scale = trackerMemberIterator->value.GetFloat();
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
					else if (displayMemberIterator->name == "disparity")
					{
						fobHeadTrackingDisplayConfig.disparity = displayMemberIterator->value.GetFloat();
					}
				}
				mDisplayConfigList.push_back(fobHeadTrackingDisplayConfig);
			}
		}
		fclose(apeFobHeadTrackingPluginConfigFile);
	}	
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
			mCameras.push_back(mCameraDoubleQueue.front());
			cameraCount++;
			mCameraDoubleQueue.pop();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	for (int i = 0; i < mDisplayConfigList.size(); i++)
	{
		auto displayConfig = mDisplayConfigList[i];
		Ape::Vector3 viewerPosition = Ape::Vector3(0, 50, 0);
		Ape::Degree angle = 45.0f;
		Ape::Vector3 axis = Ape::Vector3(0, 1, 0);
		Ape::Quaternion viewerOrientation;
		viewerOrientation.FromAngleAxis(angle, axis);
		if (auto cameraLeft = mCameras[i * 2].lock())
		{
			if (auto cameraRight = mCameras[i * 2 + 1].lock())
			{
				cameraLeft->setOrientationOffset(displayConfig.orientation);
				cameraRight->setOrientationOffset(displayConfig.orientation);

				Ape::Vector3 viewerLeftEyeRelativeToDisplay = (displayConfig.orientation * displayConfig.position) + viewerPosition;
				Ape::Vector3 viewerRightEyeRelativeToDisplay = (displayConfig.orientation * displayConfig.position) + viewerPosition;
				cameraLeft->setFocalLength(viewerLeftEyeRelativeToDisplay.z);
				cameraRight->setFocalLength(viewerRightEyeRelativeToDisplay.z);
				cameraLeft->setFrustumOffset(Ape::Vector2(-viewerLeftEyeRelativeToDisplay.x, -viewerLeftEyeRelativeToDisplay.y));
				cameraRight->setFrustumOffset(Ape::Vector2(-viewerRightEyeRelativeToDisplay.x, -viewerRightEyeRelativeToDisplay.y));
				float fovY = 2 * atan((displayConfig.size.y / 2) / cameraLeft->getFocalLength());
				cameraLeft->setFOVy(fovY);
				cameraRight->setFOVy(fovY);
			}
		}
		if (auto userNode = mUserNode.lock())
		{
			userNode->setPosition(viewerPosition);
			userNode->setOrientation(viewerOrientation);
		}
	}

	while (mpFobTracker)
	{
		float positionDataFromTracker[3];
		float orientationDataFromTracker[3];
		if (trackdGetPosition(mpFobTracker, 0, positionDataFromTracker) && trackdGetEulerAngles(mpFobTracker, 0, orientationDataFromTracker))
		{
			for(int i = 0; i < mDisplayConfigList.size(); i++)
			{
				auto displayConfig = mDisplayConfigList[i];
				if (auto cameraLeft = mCameras[i * 2].lock())
				{
					if (auto cameraRight = mCameras[i * 2 + 1].lock())
					{
						;
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
