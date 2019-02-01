#include <iostream>
#include "ApePointCloudRecorderPlugin.h"

Ape::ApePointCloudRecorderPlugin::ApePointCloudRecorderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&ApePointCloudRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mPointCloud = Ape::PointCloudWeakPtr();
	mRecordedPointCloudName = "";
	mIsRecorder = false;
	mIsPlayer = false;
	mIsLooping = false;
	mFileName = "";
	mPointCloudPosition = Ape::Vector3();
	mPointCloudOrinetation = Ape::Quaternion();
	mPointCloudNode = Ape::NodeWeakPtr();
	mPointCloudSize = 0;
	mCurrentPoints = Ape::PointCloudPoints();
	mCurrentColors = Ape::PointCloudColors();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApePointCloudRecorderPlugin::~ApePointCloudRecorderPlugin()
{
	APE_LOG_FUNC_ENTER();
	if (mFileStreamOut.is_open())
	{
		mFileStreamOut.close();
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::readFrame()
{
	mFileStreamIn.read(reinterpret_cast<char*>(&mCurrentPoints[0]), mPointCloudSize * sizeof(float));
	mFileStreamIn.read(reinterpret_cast<char*>(&mCurrentColors[0]), mPointCloudSize * sizeof(float));
}

void Ape::ApePointCloudRecorderPlugin::writeFrame()
{
	//TODO maybe write timestamp for timing?
	mFileStreamOut.write(reinterpret_cast<char*>(&mCurrentPoints[0]), mPointCloudSize * sizeof(float));
	mFileStreamOut.write(reinterpret_cast<char*>(&mCurrentColors[0]), mPointCloudSize * sizeof(float));
}

void Ape::ApePointCloudRecorderPlugin::eventCallBack(const Ape::Event& event)
{
	if (mIsRecorder)
	{
		if (event.type == Ape::Event::Type::POINT_CLOUD_CREATE)
		{
			if (event.subjectName == mRecordedPointCloudName)
			{
				if (auto entity = mpSceneManager->getEntity(event.subjectName).lock())
				{
					mPointCloud = std::static_pointer_cast<Ape::IPointCloud>(entity);
				}
			}
		}
		else if (event.type == Ape::Event::Type::POINT_CLOUD_PARAMETERS)
		{
			if (event.subjectName == mRecordedPointCloudName)
			{
				if (auto pointCloud = mPointCloud.lock())
				{
					auto parameters = pointCloud->getParameters();
					mPointCloudSize = parameters.points.size();
					mFileStreamOut.write(reinterpret_cast<char*>(&mPointCloudSize), sizeof(long));
					mCurrentPoints = parameters.points;
					mCurrentColors = parameters.colors;
					writeFrame();
				}
			}
		}
		else if (event.type == Ape::Event::Type::POINT_CLOUD_COLORS)
		{
			if (event.subjectName == mRecordedPointCloudName)
			{
				if (auto pointCloud = mPointCloud.lock())
				{
					mCurrentPoints = pointCloud->getCurrentPoints();
					mCurrentColors = pointCloud->getCurrentColors();
					writeFrame();
				}
			}
		}
	}
}

void Ape::ApePointCloudRecorderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mRecordedPointCloudName = "pointCloud_Kinect";
	mIsRecorder = false;
	mIsPlayer = true;
	mIsLooping = true;
	mFileName = "pointCloud.bin";
	//TODO get the pose information from the file like pointCloudSize
	mPointCloudPosition = Ape::Vector3(0.0, 170.0, -250.0);
	mPointCloudOrinetation = Ape::Quaternion(0.707, 0.0, 0.707, 0.0);
	if (mIsRecorder)
		mFileStreamOut.open(mFileName, std::ios::out | std::ios::binary);
	else if (mIsPlayer)
	{
		mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
		if (auto pointCloudNode = mpSceneManager->createNode("pointCloudNode_Player").lock())
		{
			pointCloudNode->setPosition(mPointCloudPosition);
			pointCloudNode->setOrientation(mPointCloudOrinetation);
			if (auto textNode = mpSceneManager->createNode("pointCloudNode_Player_Text_Node").lock())
			{
				textNode->setParentNode(pointCloudNode);
				textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
				if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("pointCloudNode_Player_Text", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					text->setCaption("Player");
					text->setParentNode(textNode);
				}
			}
			if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpSceneManager->createEntity("pointCloud_Player", Ape::Entity::POINT_CLOUD).lock()))
			{
				mFileStreamIn.read(reinterpret_cast<char*>(&mPointCloudSize), sizeof(long));
				mCurrentPoints.resize(mPointCloudSize);
				mCurrentColors.resize(mPointCloudSize);
				readFrame();
				pointCloud->setParameters(mCurrentPoints, mCurrentColors, 100000, -400.0f, 500.0f);
				pointCloud->setParentNode(pointCloudNode);
				mPointCloud = pointCloud;
			}
			mPointCloudNode = pointCloudNode;
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		if (mIsRecorder)
		{
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else if (mIsPlayer)
		{
			readFrame();
			if (auto pointCloud = mPointCloud.lock())
			{
				pointCloud->updatePoints(mCurrentPoints);
				pointCloud->updateColors(mCurrentColors);
			}
			if (!mFileStreamIn.good() && mIsLooping) 
			{
				mFileStreamIn.close();
				mFileStreamIn.clear();
				mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
				mFileStreamIn.read(reinterpret_cast<char*>(&mPointCloudSize), sizeof(long));
			}
			//TODO maybe timig by reading timestamps from the file?
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&ApePointCloudRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
