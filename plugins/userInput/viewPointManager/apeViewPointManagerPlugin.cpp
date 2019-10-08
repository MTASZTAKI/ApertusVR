#include <fstream>
#include "apeViewPointManagerPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::ViewPointManagerPlugin::ViewPointManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mTranslateSpeedFactorKeyboard = 3;
	mRotateSpeedFactorKeyboard = 1;
	mTranslateSpeedFactorMouse = 1.2;
	mRotateSpeedFactorMouse = 0.2;
	mGeneralSpeedFactor = 0;
	mViewPoses = std::vector<ape::UserInputMacro::ViewPose>();
	mViewPosesToggleIndex = 0;
	mIsKeyReleased = false;
	mIsMouseReleased = false;
	mMouseMovedRelValue = ape::Vector2();
	mMouseScrolledValue = 0;
	APE_LOG_FUNC_LEAVE();
}

ape::ViewPointManagerPlugin::~ViewPointManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViewPointManagerPlugin::keyPressedStringEventCallback(const std::string& keyValue)
{
	//APE_LOG_DEBUG("keyPressedStringEventCallback: " << keyValue);
	mIsKeyReleased = false;
	if (keyValue == "r")
	{
		mpUserInputMacro->saveViewPose();
		return;
	}
	else if (keyValue == "t")
	{
		toggleViewPoses(false);
		return;
	}
	else if (keyValue == "i")
	{
		toggleViewPoses(true);
		return;
	}
	std::thread updateViewPoseByKeyBoardThread(&ViewPointManagerPlugin::updateViewPoseByKeyBoard, this, keyValue);
	updateViewPoseByKeyBoardThread.detach();
}

void ape::ViewPointManagerPlugin::keyReleasedStringEventCallback(const std::string & keyValue)
{
	mIsKeyReleased = true;
	//APE_LOG_DEBUG("keyReleasedStringEventCallback: " << keyValue);
}

void ape::ViewPointManagerPlugin::mousePressedStringEventCallback(const std::string & mouseValue)
{
	mIsMouseReleased = false;
	//APE_LOG_DEBUG("mousePressedStringEventCallback: " << mouseValue);
	std::thread updateViewPoseByMouseThread(&ViewPointManagerPlugin::updateViewPoseByMouse, this, mouseValue);
	updateViewPoseByMouseThread.detach();
}

void ape::ViewPointManagerPlugin::mouseReleasedStringEventCallback(const std::string & mouseValue)
{
	mIsMouseReleased = true;
	//APE_LOG_DEBUG("mouseReleasedStringEventCallback: " << mouseValue);
}

void ape::ViewPointManagerPlugin::mouseMovedEventCallback(const ape::Vector2& mouseValueRel, const ape::Vector2& mouseValueAbs)
{
	mMouseMovedRelValue = mouseValueRel;
}

void ape::ViewPointManagerPlugin::controllerMovedEventCallback(ape::Vector3 controllerMovedValuePos, ape::Quaternion controllerMovedValueOri, ape::Vector3 controllerMovedValueScl)
{
	updateViewPoseByController(controllerMovedValuePos, controllerMovedValueOri, controllerMovedValueScl);
}

void ape::ViewPointManagerPlugin::hmdMovedEventCallback(ape::Vector3 hmdMovedValuePos, ape::Quaternion hmdMovedValueOri, ape::Vector3 hmdMovedValueScl)
{
	updateViewPoseByHmd(hmdMovedValuePos, hmdMovedValueOri, hmdMovedValueScl);
}

void ape::ViewPointManagerPlugin::mouseScrolledEventCallback(const int & mouseValue)
{
	mMouseScrolledValue = mouseValue;
}

void ape::ViewPointManagerPlugin::eventCallBack(const ape::Event& event)
{
}

void ape::ViewPointManagerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mpUserInputMacro->registerCallbackForKeyPressedStringValue(std::bind(&ViewPointManagerPlugin::keyPressedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForKeyReleasedStringValue(std::bind(&ViewPointManagerPlugin::keyReleasedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForMousePressedStringValue(std::bind(&ViewPointManagerPlugin::mousePressedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForMouseReleasedStringValue(std::bind(&ViewPointManagerPlugin::mouseReleasedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForMouseMovedValue(std::bind(&ViewPointManagerPlugin::mouseMovedEventCallback, this, std::placeholders::_1, std::placeholders::_2));
	mpUserInputMacro->registerCallbackForMouseScrolledValue(std::bind(&ViewPointManagerPlugin::mouseScrolledEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForControllerMovedValue(std::bind(&ViewPointManagerPlugin::controllerMovedEventCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	mpUserInputMacro->registerCallbackForHmdMovedValue(std::bind(&ViewPointManagerPlugin::hmdMovedEventCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeViewPointManagerPlugin.json";
	FILE* apeViewPointManagerPlugintConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeViewPointManagerPlugintConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeViewPointManagerPlugintConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			if (jsonDocument.HasMember("cameraPoses"))
			{
				rapidjson::Value& cameraPoses = jsonDocument["cameraPoses"];
				if (cameraPoses.IsArray())
				{
					for (auto& cameraPose : cameraPoses.GetArray())
					{
						ape::Vector3 position(cameraPose[0].GetFloat(), cameraPose[1].GetFloat(), cameraPose[2].GetFloat());
						ape::Quaternion orientation(cameraPose[3].GetFloat(), cameraPose[4].GetFloat(), cameraPose[5].GetFloat(), cameraPose[6].GetFloat());
						ape::UserInputMacro::ViewPose userInputMacroPose;
						userInputMacroPose.userPosition = position;
						userInputMacroPose.userOrientation = orientation;
						mViewPoses.push_back(userInputMacroPose);
					}
				}
			}
			if (jsonDocument.HasMember("cameraNames"))
			{
				rapidjson::Value& cameraNames = jsonDocument["cameraNames"];
				if (cameraNames.IsArray())
				{
					for (auto& cameraName : cameraNames.GetArray())
					{
						std::string cameraNameSTR = cameraName.GetString();
						mpUserInputMacro->createCamera(cameraNameSTR);
					}
				}
			}
		}
		fclose(apeViewPointManagerPlugintConfigFile);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ViewPointManagerPlugin::toggleViewPoses(bool isInterpolated)
{
	if (mViewPoses.size() > 0 && mViewPosesToggleIndex < mViewPoses.size())
	{
		if (!isInterpolated)
		{
			mpUserInputMacro->getUserNode().lock()->setPosition(mViewPoses[mViewPosesToggleIndex].userPosition);
			mpUserInputMacro->getUserNode().lock()->setOrientation(mViewPoses[mViewPosesToggleIndex].userOrientation);
		}
		else
		{
			mpUserInputMacro->interpolateViewPose(mViewPoses[mViewPosesToggleIndex], 5000);
		}
		APE_LOG_DEBUG("View pose is toggled: " << mViewPoses[mViewPosesToggleIndex].userPosition.toString() << " | " << mViewPoses[mViewPosesToggleIndex].userOrientation.toString());
		mViewPosesToggleIndex++;
		if (mViewPoses.size() == mViewPosesToggleIndex)
			mViewPosesToggleIndex = 0;
	}
}

void ape::ViewPointManagerPlugin::updateViewPoseByKeyBoard(const std::string& keyValue)
{
	while (!mIsKeyReleased)
	{
		if (auto userNode = mpUserInputMacro->getUserNode().lock())
		{
			if (keyValue == "space")
			{
				mGeneralSpeedFactor += 3;
			}
			int transScalar = mTranslateSpeedFactorKeyboard + mGeneralSpeedFactor;
			if (keyValue == "page up")
			{
				userNode->translate(ape::Vector3(0, +transScalar, 0), ape::Node::TransformationSpace::LOCAL);
			}
			if (keyValue == "page down")
			{
				userNode->translate(ape::Vector3(0, -transScalar, 0), ape::Node::TransformationSpace::LOCAL);
			}
			if (keyValue == "d")
			{
				userNode->translate(ape::Vector3(+transScalar, 0, 0), ape::Node::TransformationSpace::LOCAL);
			}
			if (keyValue == "a")
			{
				userNode->translate(ape::Vector3(-transScalar, 0, 0), ape::Node::TransformationSpace::LOCAL);
			}
			if (keyValue == "w")
			{
				userNode->translate(ape::Vector3(0, 0, -transScalar), ape::Node::TransformationSpace::LOCAL);
			}
			if (keyValue == "s")
			{
				userNode->translate(ape::Vector3(0, 0, +transScalar), ape::Node::TransformationSpace::LOCAL);
			}
			if (keyValue == "left")
			{
				userNode->rotate(ape::Radian(0.017f * mRotateSpeedFactorKeyboard), ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::WORLD);
			}
			if (keyValue == "right")
			{
				userNode->rotate(ape::Radian(-0.017f * mRotateSpeedFactorKeyboard), ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::WORLD);
			}
			if (keyValue == "up")
			{
				userNode->rotate(ape::Radian(0.017f * mRotateSpeedFactorKeyboard), ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::LOCAL);
			}
			if (keyValue == "down")
			{
				userNode->rotate(ape::Radian(-0.017f * mRotateSpeedFactorKeyboard), ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::LOCAL);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void ape::ViewPointManagerPlugin::updateViewPoseByMouse(const std::string& mouseValue)
{
	while (!mIsMouseReleased)
	{
		if (auto userNode = mpUserInputMacro->getUserNode().lock())
		{
			if (mouseValue == "right")
			{
				userNode->rotate(ape::Degree(-mMouseMovedRelValue.x).toRadian() * mRotateSpeedFactorMouse, ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::WORLD);
				userNode->rotate(ape::Degree(-mMouseMovedRelValue.y).toRadian() * mRotateSpeedFactorMouse, ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::LOCAL);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		//APE_LOG_DEBUG("updateViewPoseByMouse: " << mouseValue << " mIsMouseReleased: " << mIsMouseReleased);
	}
}

void ape::ViewPointManagerPlugin::updateViewPoseByController(ape::Vector3 controllerMovedValuePos, ape::Quaternion controllerMovedValueOri, ape::Vector3 controllerMovedValueScl)
{
	if (auto userNode = mpUserInputMacro->getUserNode().lock())
	{
		//userNode->translate(controllerMovedValueOri * ape::Vector3(0, 0, 3 * -controllerMovedValuePos.y), ape::Node::TransformationSpace::WORLD);
		//APE_LOG_DEBUG("updateViewPoseByController: ");
	}
}

void ape::ViewPointManagerPlugin::updateViewPoseByHmd(ape::Vector3 hmdMovedValuePos, ape::Quaternion hmdMovedValueOri, ape::Vector3 hmdMovedValueScl)
{
	if (auto headNode = mpUserInputMacro->getHeadNode().lock())
	{
		headNode->setPosition(hmdMovedValuePos * hmdMovedValueScl);
		headNode->setOrientation(hmdMovedValueOri);
		//APE_LOG_DEBUG("updateViewPoseByHmd: " << headNode->getPosition().toString() << " ; " << headNode->getOrientation().toString());
	}
}

void ape::ViewPointManagerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	toggleViewPoses(false);
	while (true)
	{
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ViewPointManagerPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViewPointManagerPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViewPointManagerPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ViewPointManagerPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
