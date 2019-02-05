#include <iostream>
#include "ApeUserInputMacro.h"

Ape::UserInputMacro::UserInputMacro()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mCameras = std::map<std::string, Ape::CameraWeakPtr>();
	mTranslate = Ape::Vector3();
	mUniqueUserNodeName = mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName;
	if (auto userNode = mpSceneManager->createNode(mUniqueUserNodeName).lock())
	{
		if (auto headNode = mpSceneManager->createNode(mUniqueUserNodeName + "_HeadNode").lock())
		{
			headNode->setParentNode(userNode);
			if (auto userMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity(mUniqueUserNodeName + "_Material", Ape::Entity::MATERIAL_MANUAL).lock()))
			{
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<double> distDouble(0.0, 1.0);
				std::vector<double> randomColors;
				for (int i = 0; i < 3; i++)
					randomColors.push_back(distDouble(gen));
				userMaterial->setDiffuseColor(Ape::Color(randomColors[0], randomColors[1], randomColors[2]));
				userMaterial->setSpecularColor(Ape::Color(randomColors[0], randomColors[1], randomColors[2]));
				mUserMaterial = userMaterial;
			}
			mHeadNode = headNode;
		}
		mUserNode = userNode;
	}
	APE_LOG_FUNC_LEAVE();
}

Ape::UserInputMacro::~UserInputMacro()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::UserInputMacro::updatePose(Pose pose)
{
	if (auto userNode = mUserNode.lock())
	{
		userNode->translate(pose.translate, Ape::Node::TransformationSpace::WORLD);
		userNode->rotate(pose.rotateAngle, pose.rotateAxis, Ape::Node::TransformationSpace::LOCAL);
	}
	if (auto headNode = mHeadNode.lock())
	{
		headNode->setPosition(pose.position);
		headNode->setOrientation(pose.orientation);
	}
}

Ape::CameraWeakPtr Ape::UserInputMacro::createCamera(std::string name)
{
	std::string uniqueName = mUniqueUserNodeName + name;
	if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpSceneManager->createEntity(uniqueName, Ape::Entity::Type::CAMERA).lock()))
	{
		if (auto cameraNode = mpSceneManager->createNode(uniqueName + "_Node").lock())
		{
			cameraNode->setParentNode(mHeadNode);
			if (auto cameraConeNode = mpSceneManager->createNode(uniqueName + "_ConeNode").lock())
			{
				cameraConeNode->setParentNode(cameraNode);
				cameraConeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
				if (auto cameraCone = std::static_pointer_cast<Ape::IConeGeometry>(mpSceneManager->createEntity(uniqueName + "_ConeGeometry", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					cameraCone->setParameters(10.0f, 30.0f, 1.0f, Ape::Vector2(1, 1));
					cameraCone->setParentNode(cameraConeNode);
					cameraCone->setMaterial(mUserMaterial);
				}
			}
			if (auto userNameTextNode = mpSceneManager->createNode(uniqueName + "_TextNode").lock())
			{
				userNameTextNode->setParentNode(cameraNode);
				userNameTextNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
				if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity(uniqueName + "_TextGeometry", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					userNameText->setCaption(uniqueName);
					userNameText->setParentNode(userNameTextNode);
				}
			}
			camera->setParentNode(cameraNode);
		}
		mCameras[uniqueName] = camera;
		return camera;
	}
}

void Ape::UserInputMacro::eventCallBack(const Ape::Event& event)
{
}