#include <fstream>
#include "apeELearningPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "apeELearningPluginConfig.h"

ape::apeELearningPlugin::apeELearningPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&apeELearningPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::apeELearningPlugin::~apeELearningPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::createSphere(std::string cameraName, std::string sphereNodeName, std::string meshName, unsigned int visibility)
{
	if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(cameraName).lock()))
	{
		if (auto sphereNode = mpSceneManager->createNode(sphereNodeName).lock())
		{
			if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(meshName, ape::Entity::GEOMETRY_FILE).lock()))
			{
				sphereMeshFile->setFileName(meshName);
				sphereMeshFile->setParentNode(sphereNode);
				sphereMeshFile->setVisibilityFlag(visibility);
				camera->setVisibilityMask(visibility);
			}
		}
	}
}

void ape::apeELearningPlugin::eventCallBack(const ape::Event & event)
{
	if (event.type == ape::Event::Type::CAMERA_WINDOW)
	{
		std::size_t found = event.subjectName.find("Left");
		if (found != std::string::npos)
		{
			createSphere(event.subjectName, "sphereNodeLeft", "sphere_left.mesh", 1);
		}
		found = event.subjectName.find("Right");
		if (found != std::string::npos)
		{
			createSphere(event.subjectName, "sphereNodeRight", "sphere_right.mesh", 2);
		}
	}
	else if (event.type == ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA)
	{
		if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			if (auto camera = manualTexture->getSourceCamera().lock())
			{
				std::string cameraName = camera->getName();
				std::size_t found = cameraName.find("Left");
				if (found != std::string::npos)
				{
					createSphere(cameraName, "sphereNodeLeft", "sphere_left.mesh", 1);
				}
				found = cameraName.find("Right");
				if (found != std::string::npos)
				{
					createSphere(cameraName, "sphereNodeRight", "sphere_right.mesh", 2);
					mUserInputMacroPose = ape::UserInputMacro::ViewPose();
					mUserInputMacroPose.headPosition = ape::Vector3(0, 0, 0);
					mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
					mpApeUserInputMacro->setHeadNodePositionLock(true);
				}
			}
		}
	}
}

void ape::apeELearningPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeELearningPlugin.json";
	FILE* apeELearningConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	quicktype::Welcome config = nlohmann::json::parse(apeELearningConfigFile);
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeELearningPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
