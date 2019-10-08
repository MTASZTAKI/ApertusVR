#include "ape360StereoImageScenePlugin.h"

ape::StereoImage360ScenePlugin::StereoImage360ScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&StereoImage360ScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&StereoImage360ScenePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

ape::StereoImage360ScenePlugin::~StereoImage360ScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::StereoImage360ScenePlugin::createSphere(std::string cameraName, std::string sphereNodeName, std::string meshName, unsigned int visibility)
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

void ape::StereoImage360ScenePlugin::eventCallBack(const ape::Event & event)
{
	if (event.type == ape::Event::Type::CAMERA_CREATE)
	{
		if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
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

void ape::StereoImage360ScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

void ape::StereoImage360ScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::StereoImage360ScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::StereoImage360ScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::StereoImage360ScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::StereoImage360ScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
