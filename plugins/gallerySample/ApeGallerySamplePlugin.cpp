#include <iostream>
#include "ApeGallerySamplePlugin.h"

Ape::ApeGallerySamplePlugin::ApeGallerySamplePlugin()
{
	LOG_FUNC_ENTER();
	mpScene = Ape::IScene::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeGallerySamplePlugin::~ApeGallerySamplePlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeGallerySamplePlugin::Init()
{
	LOG_FUNC_ENTER();
	/*if (auto universeSkyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("universe", Ape::Entity::MATERIAL_FILE).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe.material");
		universeSkyBoxMaterial->setAsSkyBox();
	}*/
	if (auto meshNode = mpScene->createNode("meshNode").lock())
	{
		meshNode->setPosition(Ape::Vector3(-1050, -450, -150));
		if (auto browserGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("websummit_logo.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			browserGeometry->setFileName("websummit_logo.mesh");
			browserGeometry->setParentNode(meshNode);
		}
	}
	if (auto browserNode = mpScene->createNode("websummitNode").lock())
	{
		browserNode->setPosition(Ape::Vector3(-900, 0, -300));
		Ape::Radian angle(1.57f);
		Ape::Vector3 axis(1, 0, 0);
		Ape::Quaternion orientation;
		orientation.FromAngleAxis(angle, axis);
		Ape::Radian angle2(1.57f);
		Ape::Vector3 axis2(0, 1, 0);
		Ape::Quaternion orientation2;
		orientation2.FromAngleAxis(angle2, axis2);
		Ape::Radian angle3(3.14f);
		Ape::Vector3 axis3(1, 0, 0);
		Ape::Quaternion orientation3;
		orientation3.FromAngleAxis(angle3, axis3);
		browserNode->setOrientation(orientation * orientation2 * orientation3);
		if (auto browserGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("websummitGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			browserGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(425, 213), Ape::Vector2(1, 1));
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("websummitBrowser", Ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(2048, 1024);
				browser->setURL("https://websummit.com/");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	if (auto browserNode = mpScene->createNode("video1Node").lock())
	{
		browserNode->setPosition(Ape::Vector3(-600, 0, 0));
		if (auto browserGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			browserGeometry->setFileName("sphere.mesh");
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("video1Browser", Ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(2048, 1024);
				browser->setURL("https://www.youtube.com/embed/oPoRy13M0Q4?vq=hd1080&autoplay=1&loop=1&playlist=oPoRy13M0Q4");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	if (auto sphereNode = mpScene->createNode("sphere_imgNode").lock())
	{
		sphereNode->setPosition(Ape::Vector3(-1200, 0, 0));
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_img.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpScene->createNode("sphere_img_2Node").lock())
	{
		sphereNode->setPosition(Ape::Vector3(-600, -600, 0));
		sphereNode->setOrientation(Ape::Quaternion(0.7071, 0, 0, 0.7071));
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_img_2.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_2.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpScene->createNode("sphere_img_3Node").lock())
	{
		sphereNode->setPosition(Ape::Vector3(-1200, -600, 0));
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_img_3.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_3.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpScene->createNode("sphere_img_4Node").lock())
	{
		sphereNode->setPosition(Ape::Vector3(-600, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_img_4.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_4.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpScene->createNode("sphere_img_5Node").lock())
	{
		sphereNode->setPosition(Ape::Vector3(-1200, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_img_5.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_5.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpScene->createNode("sphere_img_6Node").lock())
	{
		sphereNode->setPosition(Ape::Vector3(-600, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_img_6.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_6.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpScene->createNode("sphere_img_7Node").lock())
	{
		sphereNode->setPosition(Ape::Vector3(-1200, -600, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_img_7.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_7.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpScene->createNode("sphere_img_8Node").lock())
	{
		sphereNode->setPosition(Ape::Vector3(-600, -600, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_img_8.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_8.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeGallerySamplePlugin::Run()
{

}

void Ape::ApeGallerySamplePlugin::Step()
{

}

void Ape::ApeGallerySamplePlugin::Stop()
{

}

void Ape::ApeGallerySamplePlugin::Suspend()
{

}

void Ape::ApeGallerySamplePlugin::Restart()
{

}
