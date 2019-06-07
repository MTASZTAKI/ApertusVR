#include "ape360StereoImagePlugin.h"

ape::ape360StereoImagePlugin::ape360StereoImagePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&ape360StereoImagePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

ape::ape360StereoImagePlugin::~ape360StereoImagePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ape360StereoImagePlugin::eventCallBack(const ape::Event & event)
{
	if (event.type == ape::Event::Type::CAMERA_PARENTNODE)
	{
		//TODO_ape360StereoImagePlugin somehow not rendering (camera ignore list) the sphere of the other eye
		std::size_t found = event.subjectName.find("Left");
		if (found != std::string::npos)
		{
			if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				if (auto sphereNode = mpSceneManager->createNode("sphereNodeLeft").lock())
				{
					if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_left.mesh", ape::Entity::GEOMETRY_FILE).lock()))
					{
						if (auto cameraNode = camera->getParentNode().lock())
						{
							sphereMeshFile->setFileName("sphere_left.mesh");
							sphereMeshFile->setParentNode(sphereNode);
						}
					}
				}
			}
		}
		found = event.subjectName.find("Right");
		if (found != std::string::npos)
		{
			if (auto camera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				if (auto sphereNode = mpSceneManager->createNode("sphereNodeRight").lock())
				{
					if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_right.mesh", ape::Entity::GEOMETRY_FILE).lock()))
					{
						if (auto cameraNode = camera->getParentNode().lock())
						{
							sphereMeshFile->setFileName("sphere_right.mesh");
							sphereMeshFile->setParentNode(sphereNode);
						}
					}
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
