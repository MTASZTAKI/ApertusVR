#include <iostream>
#include "ApePointCloudRecorderPlugin.h"

Ape::ApePointCloudRecorderPlugin::ApePointCloudRecorderPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&ApePointCloudRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mPointCloud = Ape::PointCloudWeakPtr();
	mPointCloudName = "";
	mIsRecorder = true;
	mIsPlayer = false;
	mIsLooping = false;
	mFileName = "";
	mPointCloudPosition = Ape::Vector3();
	mPointCloudOrinetation = Ape::Quaternion();
	mPointCloudNode = Ape::NodeWeakPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApePointCloudRecorderPlugin::~ApePointCloudRecorderPlugin()
{
	LOG_FUNC_ENTER();
	if (mFileStreamOut.is_open())
	{
		mFileStreamOut.close();
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::readFrame()
{
	Ape::PointCloudPoints points;
	Ape::PointCloudColors colors;
	long size;
	mFileStreamIn.read(reinterpret_cast<char*>(&size), sizeof(long));
	points.resize(size);
	colors.resize(size);
	mFileStreamIn.read(reinterpret_cast<char*>(&points[0]), points.size() * sizeof(float));
	mFileStreamIn.read(reinterpret_cast<char*>(&colors[0]), colors.size() * sizeof(float));
	if (auto pointCloud = mPointCloud.lock())
	{
		//pointCloud->updatePoints(points);
		//pointCloud->updateColors(colors);
	}
}

void Ape::ApePointCloudRecorderPlugin::writeFrame()
{
	if (auto pointCloud = mPointCloud.lock())
	{
		auto points = pointCloud->getCurrentPoints();
		auto colors = pointCloud->getCurrentColors();
		long size = points.size();
		mFileStreamOut.write(reinterpret_cast<char*>(&size), sizeof(long));
		mFileStreamOut.write(reinterpret_cast<char*>(&points[0]), points.size() * sizeof(float));
		mFileStreamOut.write(reinterpret_cast<char*>(&colors[0]), colors.size() * sizeof(float));
	}
}

void Ape::ApePointCloudRecorderPlugin::eventCallBack(const Ape::Event& event)
{
	if (mIsRecorder)
	{
		if (event.type == Ape::Event::Type::POINT_CLOUD_CREATE)
		{
			if (event.subjectName == mPointCloudName)
			{
				if (auto entity = mpScene->getEntity(event.subjectName).lock())
				{
					mPointCloud = std::static_pointer_cast<Ape::IPointCloud>(entity);
				}
			}
		}
		else if (event.type == Ape::Event::Type::POINT_CLOUD_COLORS)
		{
			if (event.subjectName == mPointCloudName)
			{
				writeFrame();
			}
		}
	}
}

void Ape::ApePointCloudRecorderPlugin::Init()
{
	LOG_FUNC_ENTER();
	mPointCloudName = "pointCloud_Kinect";
	mIsRecorder = false;
	mIsPlayer = true;
	mIsLooping = false;
	mFileName = "pointCloud.bin";
	mPointCloudPosition = Ape::Vector3(0, 0, 0);
	mPointCloudOrinetation = Ape::Quaternion(1, 0, 0, 0);
	if (mIsRecorder)
		mFileStreamOut.open(mFileName, std::ios::out | std::ios::binary);
	else if (mIsPlayer)
	{
		mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
		if (auto pointCloudNode = mpScene->createNode("pointCloudNode_Player").lock())
		{
			pointCloudNode->setPosition(mPointCloudPosition);
			pointCloudNode->setOrientation(mPointCloudOrinetation);
			if (auto textNode = mpScene->createNode("pointCloudNode_Player_Text_Node").lock())
			{
				textNode->setParentNode(pointCloudNode);
				textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
				if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("pointCloudNode_Player_Text", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					text->setCaption("Player");
					text->setParentNode(textNode);
				}
			}
			if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud_Player", Ape::Entity::POINT_CLOUD).lock()))
			{
				//pointCloud->setParameters(Ape::PointCloudPoints(), Ape::PointCloudColors(), 100000);
				//pointCloud->setParentNode(pointCloudNode);
				mPointCloud = pointCloud;
			}
			mPointCloudNode = pointCloudNode;
		}
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		if (mIsRecorder)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else if (mIsPlayer)
		{
			readFrame();
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&ApePointCloudRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApePointCloudRecorderPlugin::Step()
{

}

void Ape::ApePointCloudRecorderPlugin::Stop()
{

}

void Ape::ApePointCloudRecorderPlugin::Suspend()
{

}

void Ape::ApePointCloudRecorderPlugin::Restart()
{

}
