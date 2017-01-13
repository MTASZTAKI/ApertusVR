#include <iostream>
#include "ApeFobHeadTrackingPlugin.h"

ApeFobHeadTrackingPlugin::ApeFobHeadTrackingPlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeFobHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeFobHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mCameraNode = Ape::NodeWeakPtr();
	mpFobTracker = nullptr;
	mCameraDoubleQueue = Ape::DoubleQueue<Ape::CameraWeakPtr>();
	mCameras = std::vector<Ape::CameraWeakPtr>();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mFobTrackerRotation = Ape::Quaternion();
	mFobTrackerOffset = Ape::Vector3();
	mFobTrackerScale = 0.0f;
}

ApeFobHeadTrackingPlugin::~ApeFobHeadTrackingPlugin()
{
	std::cout << "ApeFobHeadTrackingPlugin dtor" << std::endl;
}

void ApeFobHeadTrackingPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == mpSystemConfig->getSceneSessionConfig().generatedUniqueUserName)
	{
		if (auto node = (mpScene->getNode(event.subjectName).lock()))
			mCameraNode = node;
	}
	else if (event.type == Ape::Event::Type::CAMERA_CREATE)
	{
		if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock()))
		{
			mCameraDoubleQueue.push(camera);
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
	mFobTrackerRotation.FromAngleAxis(Ape::Degree(90.0f), Ape::Vector3(0, -1, 0));
	mFobTrackerOffset = Ape::Vector3(0.0f, -132.5f, 0.0f);
	mFobTrackerScale = 30.48f;
}

void ApeFobHeadTrackingPlugin::Run()
{
	while (mpFobTracker)
	{
		mCameraDoubleQueue.swap();
		while (!mCameraDoubleQueue.emptyPop())
		{
			mCameras.push_back(mCameraDoubleQueue.front());
			mCameraDoubleQueue.pop();
		}
		float positionDataFromTracker[3];
		float orientationDataFromTracker[3];
		if (trackdGetPosition(mpFobTracker, 0, positionDataFromTracker) && trackdGetEulerAngles(mpFobTracker, 0, orientationDataFromTracker))
		{
			Ape::Vector3 viewerPosition = Ape::Vector3(positionDataFromTracker[0], positionDataFromTracker[1], positionDataFromTracker[2]) * mFobTrackerScale + mFobTrackerOffset;
			Ape::Quaternion viewerOrientation = Ape::Euler(orientationDataFromTracker[0], orientationDataFromTracker[1], orientationDataFromTracker[2]).toQuaternion() * mFobTrackerRotation;
			for (auto cameraWeakPtr : mCameras)
			{
				if (auto camera = cameraWeakPtr.lock())
				{
					/*Ape::Vector3 viewerToDisplay = camera->getInitOrientationOffset().Inverse() * (viewerPosition + viewerOrientation * camera->getInitPositionOffset());
					camera->setFocalLength(viewerToDisplay.z);
					camera->setFrustumOffset(Ape::Vector2(-viewerToDisplay.x, -viewerToDisplay.y));
					camera->setFOVy(2 * atan((mpMainWindow->getHeight() / 2) / camera->getFocalLength()));*/
					camera->setFocalLength(viewerPosition.z);
					camera->setFrustumOffset(Ape::Vector2(-viewerPosition.x, -viewerPosition.y));
					camera->setFOVy(2 * atan((mpMainWindow->getHeight() / 2) / camera->getFocalLength()));
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeFobHeadTrackingPlugin::eventCallBack, this, std::placeholders::_1));
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
