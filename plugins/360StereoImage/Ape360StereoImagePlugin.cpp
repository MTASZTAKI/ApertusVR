#include "ape360StereoImagePlugin.h"

ape::ape360StereoImagePlugin::ape360StereoImagePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&ape360StereoImagePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&ape360StereoImagePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

ape::ape360StereoImagePlugin::~ape360StereoImagePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360StereoImagePlugin::createSphere(std::string cameraName, std::string sphereNodeName, std::string meshName, unsigned int visibility)
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

void ape::ape360StereoImagePlugin::eventCallBack(const ape::Event & event)
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
				}
			}
		}
	}
}

void ape::ape360StereoImagePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360StereoImagePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360StereoImagePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360StereoImagePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360StereoImagePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360StereoImagePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
