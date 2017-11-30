#include <iostream>
#include "ApeGallerySamplePlugin.h"

ApeGallerySamplePlugin::ApeGallerySamplePlugin()
{
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeGallerySamplePlugin::~ApeGallerySamplePlugin()
{
	std::cout << "ApeGallerySamplePlugin dtor" << std::endl;
}

void ApeGallerySamplePlugin::Init()
{
	/*if (auto universeSkyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("universe", Ape::Entity::MATERIAL_FILE).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe.material");
		universeSkyBoxMaterial->setAsSkyBox();
	}*/
	if (auto browserNode = mpScene->createNode("websummitNode").lock())
	{
		browserNode->setPosition(Ape::Vector3(-600, 300, 0));
		browserNode->setOrientation(Ape::Quaternion(1, 0, 0, 0));
		if (auto browserGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("websummitGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			browserGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(240, 120), Ape::Vector2(1, 1));
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
	std::cout << "ApeGallerySamplePlugin::init" << std::endl;
}

void ApeGallerySamplePlugin::Run()
{

}

void ApeGallerySamplePlugin::Step()
{

}

void ApeGallerySamplePlugin::Stop()
{

}

void ApeGallerySamplePlugin::Suspend()
{

}

void ApeGallerySamplePlugin::Restart()
{

}
