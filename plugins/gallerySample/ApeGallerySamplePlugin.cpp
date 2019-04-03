#include "apeGallerySamplePlugin.h"

ape::apeGallerySamplePlugin::apeGallerySamplePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::apeGallerySamplePlugin::~apeGallerySamplePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGallerySamplePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto universeSkyBoxMaterial = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->createEntity("universe", ape::Entity::MATERIAL_FILE).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe.material");
		universeSkyBoxMaterial->setAsSkyBox();
	}
	if (auto meshNode = mpSceneManager->createNode("meshNode").lock())
	{
		meshNode->setPosition(ape::Vector3(-1050, -450, -150));
		if (auto browserGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("websummit_logo.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			browserGeometry->setFileName("websummit_logo.mesh");
			browserGeometry->setParentNode(meshNode);
		}
	}
	if (auto browserNode = mpSceneManager->createNode("websummitNode").lock())
	{
		browserNode->setPosition(ape::Vector3(-900, 0, -300));
		ape::Radian angle(1.57f);
		ape::Vector3 axis(1, 0, 0);
		ape::Quaternion orientation;
		orientation.FromAngleAxis(angle, axis);
		ape::Radian angle2(1.57f);
		ape::Vector3 axis2(0, 1, 0);
		ape::Quaternion orientation2;
		orientation2.FromAngleAxis(angle2, axis2);
		ape::Radian angle3(3.14f);
		ape::Vector3 axis3(1, 0, 0);
		ape::Quaternion orientation3;
		orientation3.FromAngleAxis(angle3, axis3);
		browserNode->setOrientation(orientation * orientation2 * orientation3);
		if (auto browserGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->createEntity("websummitGeometry", ape::Entity::GEOMETRY_PLANE).lock()))
		{
			browserGeometry->setParameters(ape::Vector2(1, 1), ape::Vector2(425, 213), ape::Vector2(1, 1));
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->createEntity("websummitBrowser", ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(2048, 1024);
				browser->setURL("https://websummit.com/");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	if (auto browserNode = mpSceneManager->createNode("video1Node").lock())
	{
		browserNode->setPosition(ape::Vector3(-600, 0, 0));
		if (auto browserGeometry = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			browserGeometry->setFileName("sphere.mesh");
			browserGeometry->setParentNode(browserNode);
			if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->createEntity("video1Browser", ape::Entity::BROWSER).lock()))
			{
				browser->setResoultion(2048, 1024);
				browser->setURL("https://www.youtube.com/embed/oPoRy13M0Q4?vq=hd1080&autoplay=1&loop=1&playlist=oPoRy13M0Q4");
				browser->setGeometry(browserGeometry);
			}
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_imgNode").lock())
	{
		sphereNode->setPosition(ape::Vector3(-1200, 0, 0));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_2Node").lock())
	{
		sphereNode->setPosition(ape::Vector3(-600, -600, 0));
		sphereNode->setOrientation(ape::Quaternion(0.7071, 0, 0, 0.7071));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_2.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_2.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_3Node").lock())
	{
		sphereNode->setPosition(ape::Vector3(-1200, -600, 0));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_3.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_3.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_4Node").lock())
	{
		sphereNode->setPosition(ape::Vector3(-600, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_4.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_4.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_5Node").lock())
	{
		sphereNode->setPosition(ape::Vector3(-1200, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_5.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_5.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_6Node").lock())
	{
		sphereNode->setPosition(ape::Vector3(-600, 0, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_6.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_6.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_7Node").lock())
	{
		sphereNode->setPosition(ape::Vector3(-1200, -600, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_7.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_7.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	if (auto sphereNode = mpSceneManager->createNode("sphere_img_8Node").lock())
	{
		sphereNode->setPosition(ape::Vector3(-600, -600, -600));
		if (auto sphereMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("sphere_img_8.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere_img_8.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGallerySamplePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGallerySamplePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGallerySamplePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGallerySamplePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeGallerySamplePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
