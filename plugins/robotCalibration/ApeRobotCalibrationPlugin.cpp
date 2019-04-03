#include <fstream>
#include "apeRobotCalibrationPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::apeRobotCalibrationPlugin::apeRobotCalibrationPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeRobotCalibrationPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpapeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apeRobotCalibrationPlugin::~apeRobotCalibrationPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeRobotCalibrationPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeRobotCalibrationPlugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeRobotCalibrationPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeOverlayBrowser();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeCoordinateSystem();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeRobotCalibrationPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeRobotCalibrationPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeRobotCalibrationPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeRobotCalibrationPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeRobotCalibrationPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
