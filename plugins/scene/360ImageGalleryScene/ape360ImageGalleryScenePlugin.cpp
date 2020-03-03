#include "ape360ImageGalleryScenePlugin.h"

ape::Image360GalleryScenePlugin::Image360GalleryScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::Image360GalleryScenePlugin::~Image360GalleryScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Image360GalleryScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto universeSkyBoxMaterial = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->createEntity("universe", ape::Entity::MATERIAL_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe");
		universeSkyBoxMaterial->setAsSkyBox();
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_imgNode", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		sphereNode->setPosition(ape::Vector3(-1200, 0, 0));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMeshFile->setFileName("sphere_img.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_2Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		sphereNode->setPosition(ape::Vector3(-600, -600, 0));
		sphereNode->setOrientation(ape::Quaternion(0.7071, 0, 0, 0.7071));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_2.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_2.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_3Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		sphereNode->setPosition(ape::Vector3(-1200, -600, 0));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_3.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_3.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_4Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		sphereNode->setPosition(ape::Vector3(-600, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_4.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_4.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_5Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		sphereNode->setPosition(ape::Vector3(-1200, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_5.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_5.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_6Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		sphereNode->setPosition(ape::Vector3(-600, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_6.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_6.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_7Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		sphereNode->setPosition(ape::Vector3(-1200, -600, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_7.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_7.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_8Node", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		sphereNode->setPosition(ape::Vector3(-600, -600, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_8.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_8.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::Image360GalleryScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Image360GalleryScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Image360GalleryScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Image360GalleryScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::Image360GalleryScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
