#include <iostream>
#include "ApePresentationScenePlugin.h"

ApePresentationScenePlugin::ApePresentationScenePlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
	std::string userNodeName = mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName;
	mUserNode = mpScene->getNode(userNodeName);
}

ApePresentationScenePlugin::~ApePresentationScenePlugin()
{
	std::cout << "ApePresentationScenePlugin dtor" << std::endl;
}

void ApePresentationScenePlugin::eventCallBack(const Ape::Event& event)
{
	
}

void ApePresentationScenePlugin::Init()
{
	std::cout << "ApePresentationScenePlugin::init" << std::endl;

	std::cout << "ApePresentationScenePlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "ApePresentationScenePlugin main window was found" << std::endl;

	if (auto node = mpScene->createNode("presentationNode").lock())
	{
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("metalroom.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("metalroom.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("feymannNode").lock())
	{
		node->setPosition(Ape::Vector3(-536, -345, 502));
		Ape::Degree angle1(90);
		Ape::Vector3 axis1(1, 0, 0);
		Ape::Quaternion rotate1;
		rotate1.FromAngleAxis(angle1, axis1);
		Ape::Degree angle2(180);
		Ape::Vector3 axis2(0, 1, 0);
		Ape::Quaternion rotate2;
		rotate2.FromAngleAxis(angle2, axis2);
		node->setOrientation(Ape::Quaternion(0, -0.924, 0, -0.383) * rotate1 * rotate2);
		node->setScale(Ape::Vector3(0.7, 0.7, 0.7));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("feymann.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("feymann.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("univNode").lock())
	{
		node->setPosition(Ape::Vector3(-726, -350, 171));
		Ape::Degree angle1(90);
		Ape::Vector3 axis1(1, 0, 0);
		Ape::Quaternion rotate1;
		rotate1.FromAngleAxis(angle1, axis1);
		Ape::Degree angle2(180);
		Ape::Vector3 axis2(0, 1, 0);
		Ape::Quaternion rotate2;
		rotate2.FromAngleAxis(angle2, axis2);
		node->setOrientation(Ape::Quaternion(0, -0.707, 0, -0.707) * rotate1 * rotate2);
		node->setScale(Ape::Vector3(1.2, 1.2, 1.2));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("univ.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("univ.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("moseleyNode").lock())
	{
		node->setPosition(Ape::Vector3(670, -350, -70));
		Ape::Degree angle1(90);
		Ape::Vector3 axis1(1, 0, 0);
		Ape::Quaternion rotate1;
		rotate1.FromAngleAxis(angle1, axis1);
		Ape::Degree angle2(180);
		Ape::Vector3 axis2(0, 1, 0);
		Ape::Quaternion rotate2;
		rotate2.FromAngleAxis(angle2, axis2);
		node->setOrientation(Ape::Quaternion(-0, 0.707, -0, -0.707) * rotate1 * rotate2);
		node->setScale(Ape::Vector3(0.7, 0.7, 0.7));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("moseley.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("moseley.mesh");
			meshFile->setParentNode(node);
		}
	}
	if (auto node = mpScene->createNode("periodicNode").lock())
	{
		node->setPosition(Ape::Vector3(550, -340, -388));
		Ape::Degree angle1(90);
		Ape::Vector3 axis1(1, 0, 0);
		Ape::Quaternion rotate1;
		rotate1.FromAngleAxis(angle1, axis1);
		Ape::Degree angle2(180);
		Ape::Vector3 axis2(0, 1, 0);
		Ape::Quaternion rotate2;
		rotate2.FromAngleAxis(angle2, axis2);
		Ape::Degree angle3(180);
		Ape::Vector3 axis3(0, 0, 1);
		Ape::Quaternion rotate3;
		rotate3.FromAngleAxis(angle3, axis3);
		node->setOrientation(Ape::Quaternion(-0, 0.383, -0, -0.924) * rotate1 * rotate2 * rotate3);
		node->setScale(Ape::Vector3(1.2, 1.2, 1.2));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("periodic.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("periodic.mesh");
			meshFile->setParentNode(node);
		}
	}
}

void ApePresentationScenePlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
}

void ApePresentationScenePlugin::Step()
{

}

void ApePresentationScenePlugin::Stop()
{

}

void ApePresentationScenePlugin::Suspend()
{

}

void ApePresentationScenePlugin::Restart()
{

}
