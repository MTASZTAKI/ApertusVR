#include <fstream>
#include "ApeZedPlugin.h"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

Ape::ZedPlugin::ZedPlugin()
{
	LOG_FUNC_ENTER();
	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ZedPlugin::eventCallBack, this, std::placeholders::_1));
	mZed = sl::Camera();
	mZedResolutionWidth = 0;
	mZedResolutionHeight = 0;
	mPointCloudSize = 0;
	mPointCloudBoundingSphereRadius = 100000;
	mApePointCloudPoints = Ape::PointCloudPoints();
	mApePointCloudColors = Ape::PointCloudColors();
	mApePointCloudNode = Ape::NodeWeakPtr();
	mApePointCloud = Ape::PointCloudWeakPtr();
	LOG_FUNC_LEAVE();
}

Ape::ZedPlugin::~ZedPlugin()
{
	LOG_FUNC_ENTER();
	mZedPointCloud.free(sl::MEM_CPU);
	mZed.close();
	LOG_FUNC_LEAVE();
}

void Ape::ZedPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ZedPlugin::Init()
{
	LOG_FUNC_ENTER();
	LOG(LOG_TYPE_DEBUG, "waiting for main window");
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LOG(LOG_TYPE_DEBUG, "main window was found");
	LOG(LOG_TYPE_DEBUG, "try to init Zed");
	sl::InitParameters initParameters;
	initParameters.camera_resolution = sl::RESOLUTION_VGA;
	initParameters.depth_mode = sl::DEPTH_MODE_PERFORMANCE;
	initParameters.coordinate_system = sl::COORDINATE_SYSTEM_RIGHT_HANDED_Y_UP;
	initParameters.coordinate_units = sl::UNIT_CENTIMETER;
	sl::ERROR_CODE err = mZed.open(initParameters);
	if (err != sl::SUCCESS) {
		LOG(LOG_TYPE_DEBUG, "Zed error: " << sl::toString(err));
		mZed.close();
	}
	mZedResolutionWidth = (int)mZed.getResolution().width / 2;
	mZedResolutionHeight = (int)mZed.getResolution().height / 2;
	mPointCloudSize = mZedResolutionWidth * mZedResolutionHeight;
	mZedDepthImage = sl::Mat(mZedResolutionWidth, mZedResolutionHeight, sl::MAT_TYPE_8U_C4);
	mZedImage = sl::Mat(mZedResolutionWidth, mZedResolutionHeight, sl::MAT_TYPE_8U_C4);
	mZedPointCloud = sl::Mat();
	mApePointCloudPoints.clear();
	mApePointCloudColors.clear();
	mApePointCloudPoints.resize(mPointCloudSize * 3);
	mApePointCloudColors.resize(mPointCloudSize * 3);
	LOG(LOG_TYPE_DEBUG, "Zed init was successful, point cloud size: " << mPointCloudSize);
	if (auto apePointCloudNode = mpScene->createNode("ZedPointCloudNode").lock())
	{
		if (auto apePointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("ZedPointCloud", Ape::Entity::Type::POINT_CLOUD).lock()))
		{
			apePointCloud->setParameters(mApePointCloudPoints, mApePointCloudColors, mPointCloudBoundingSphereRadius);
			apePointCloud->setParentNode(apePointCloudNode);
			mApePointCloud = apePointCloud;
		}
		if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("ZedText", Ape::Entity::GEOMETRY_TEXT).lock()))
		{
			text->showOnTop(true);
			text->setCaption("ZedMini");
			text->setParentNode(apePointCloudNode);
		}
		mApePointCloudNode = apePointCloudNode;
	}
	LOG_FUNC_LEAVE();
}

void Ape::ZedPlugin::Run()
{
	LOG_FUNC_ENTER();
	sl::RuntimeParameters runtime_parameters;
	runtime_parameters.sensing_mode = sl::SENSING_MODE_STANDARD;
	while (true)
	{
		if (mZed.grab(runtime_parameters) == sl::SUCCESS)
		{
			//mZed.retrieveImage(mZedImage, sl::VIEW_LEFT, sl::MEM_CPU, mZedResolutionWidth, mZedResolutionHeight);
			//mZed.retrieveImage(mZedDepthImage, sl::VIEW_DEPTH, sl::MEM_CPU, mZedResolutionWidth, mZedResolutionHeight);
			sl::ERROR_CODE zedRetrieveMeasureResult = mZed.retrieveMeasure(mZedPointCloud, sl::MEASURE_XYZRGBA, sl::MEM_CPU, mZedResolutionWidth, mZedResolutionHeight);
			if (zedRetrieveMeasureResult == sl::SUCCESS)
			{
				auto data = mZedPointCloud.getPtr<sl::float4>(sl::MEM_CPU);
				mApePointCloudPoints.clear();
				mApePointCloudColors.clear();
				for (int i = 0; i < mPointCloudSize; i++)
				{
					mApePointCloudPoints.push_back(data[i].x);
					mApePointCloudPoints.push_back(data[i].y);
					mApePointCloudPoints.push_back(data[i].z);
					mApePointCloudColors.push_back(data[i].r);
					mApePointCloudColors.push_back(data[i].g);
					mApePointCloudColors.push_back(data[i].b);
				}
				if (auto apePointCloud = mApePointCloud.lock())
				{
					apePointCloud->updatePoints(mApePointCloudPoints);
					apePointCloud->updateColors(mApePointCloudColors);
				}
			}
			LOG(LOG_TYPE_DEBUG, "Zed retrieveMeasure result: " << zedRetrieveMeasureResult);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ZedPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ZedPlugin::Step()
{

}

void Ape::ZedPlugin::Stop()
{

}

void Ape::ZedPlugin::Suspend()
{

}

void Ape::ZedPlugin::Restart()
{

}