#include <fstream>
#include "ape3dRudderControllerPlugin.h"

ape::ape3dRudderControllerPlugin::ape3dRudderControllerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ape3dRudderControllerPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mUserDeadZone = ape::Vector3(0.1, 0.1, 0.1);
	APE_LOG_FUNC_LEAVE();
}

ape::ape3dRudderControllerPlugin::~ape3dRudderControllerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&ape3dRudderControllerPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ape3dRudderControllerPlugin::eventCallBack(const ape::Event& event)
{
}

void ape::ape3dRudderControllerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	APE_LOG_DEBUG("waiting for main window");
	while (mpCoreConfig->getWindowConfig().handle == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("main window was found");
	APE_LOG_FUNC_LEAVE();
}

void ape::ape3dRudderControllerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	bool isInUsePrinted = false;
	mErrCodeLoad = ns3dRudder::LoadSDK(_3DRUDDER_SDK_LAST_COMPATIBLE_VERSION);
	if (mErrCodeLoad == ns3dRudder::ErrorCode::Success)
	{
		mpSdk = ns3dRudder::GetSDK();
		mpSdk->SetEvent(&mEvent);
		mpSdk->Init();
		while (!mpSdk->IsDeviceConnected(0))
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		uint16_t nVersion = mpSdk->GetVersion(0);
		APE_LOG_DEBUG("Firmware Version: " << nVersion);
		uint16_t nSdkVersion = mpSdk->GetSDKVersion();
		APE_LOG_DEBUG("SDK Version: " << nSdkVersion);
		mAxesParamDefault = ns3dRudder::AxesParamDefault();
		mAxesParamNormalizedLinear = ns3dRudder::AxesParamNormalizedLinear();
		mErrCodeGetAxes = ns3dRudder::ErrorCode::NotReady;
	}
	else
	{
		APE_LOG_DEBUG("error: " << ns3dRudder::GetErrorText(mErrCodeLoad));
	}
	if (mErrCodeLoad == ns3dRudder::ErrorCode::Success)
	{
		while (true)
		{
			ns3dRudder::AxesValue lAxis;
			ns3dRudder::AxesValue lAxisUserOffset;
			mErrCodeGetAxes = mpSdk->GetAxes(0, &mAxesParamDefault, &lAxis);
			if (mErrCodeGetAxes != ns3dRudder::NotReady)
			{
				ape::Vector3 position(lAxis.Get(ns3dRudder::Axes::LeftRight), lAxis.Get(ns3dRudder::Axes::ForwardBackward), lAxis.Get(ns3dRudder::Axes::Rotation));
				if (position.length() > mUserDeadZone.length())
				{
					if (auto userNode = mpUserInputMacro->getUserNode().lock())
					{
						userNode->translate(ape::Vector3(lAxis.Get(ns3dRudder::Axes::LeftRight), 0, 0), ape::Node::TransformationSpace::LOCAL);

						userNode->translate(ape::Vector3(lAxis.Get(ns3dRudder::Axes::LeftRight), 0, 0), ape::Node::TransformationSpace::LOCAL);

						userNode->translate(ape::Vector3(0, 0, -lAxis.Get(ns3dRudder::Axes::ForwardBackward)), ape::Node::TransformationSpace::LOCAL);

						userNode->translate(ape::Vector3(0, 0, -lAxis.Get(ns3dRudder::Axes::ForwardBackward)), ape::Node::TransformationSpace::LOCAL);

						userNode->rotate(ape::Degree(-lAxis.Get(ns3dRudder::Axes::Rotation)).toRadian(), ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::WORLD);

						userNode->rotate(ape::Degree(-lAxis.Get(ns3dRudder::Axes::Rotation)).toRadian(), ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::WORLD);

						//APE_LOG_DEBUG("Axes: " << lAxis.Get(ns3dRudder::Axes::LeftRight) << "; " << lAxis.Get(ns3dRudder::Axes::ForwardBackward) << "; " << lAxis.Get(ns3dRudder::Axes::Rotation) << "; " << lAxis.Get(ns3dRudder::Axes::UpDown));
					}
				}
				ns3dRudder::Status status = mpSdk->GetStatus(0);
				switch (status)
				{
					case ns3dRudder::NoFootStayStill:
						APE_LOG_DEBUG("Status : Don't put your Feet !!! Stay still 5s");
						break;
					case ns3dRudder::Initialization:
						APE_LOG_DEBUG("Status : Initialisation ");
						break;
					case ns3dRudder::PutYourFeet:
						APE_LOG_DEBUG("Status : Please put your feet");
						break;
					case ns3dRudder::PutSecondFoot:
						APE_LOG_DEBUG("Status :  Put your second foot");
						break;
					case ns3dRudder::StayStill:
						APE_LOG_DEBUG("Status : Stay still");
						mpSdk->GetUserOffset(0, &lAxisUserOffset);
						break;
					case ns3dRudder::InUse:
						if (!isInUsePrinted)
						{
							APE_LOG_DEBUG("Status : 3DRudder in use");
							isInUsePrinted = true;
						}
						break;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
	ns3dRudder::EndSDK();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape3dRudderControllerPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape3dRudderControllerPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape3dRudderControllerPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape3dRudderControllerPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
