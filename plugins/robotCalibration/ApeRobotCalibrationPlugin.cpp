#include <fstream>
#include "ApeRobotCalibrationPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

Ape::ApeRobotCalibrationPlugin::ApeRobotCalibrationPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeRobotCalibrationPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpApeUserInputMacro = Ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = Ape::UserInputMacro::ViewPose();
	mpSceneMakerMacro = new Ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeRobotCalibrationPlugin::~ApeRobotCalibrationPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeRobotCalibrationPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeRobotCalibrationPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeOverlayBrowser();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeCoordinateSystem();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeRobotCalibrationPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
