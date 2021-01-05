#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "apeKinectPlugin.h"

//#define HALF
const int width = 512;
const int height = 424;
const int colorwidth = 1920;
const int colorheight = 1080;

unsigned char rgbimage[colorwidth*colorheight * 4];
CameraSpacePoint depth2xyz[width*height];
ColorSpacePoint depth2rgb[width*height];
BYTE bodyIdx[width*height];

ape::KinectPlugin::KinectPlugin()
{
	APE_LOG_FUNC_ENTER();
	m_pKinectSensor = NULL;
	m_pCoordinateMapper = NULL;
	reader = NULL;
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));
	RootNode = mpSceneManager->createNode("KinectRootNode", true, mpCoreConfig->getNetworkGUID()).lock();
	APE_LOG_FUNC_LEAVE();
}

ape::KinectPlugin::~KinectPlugin()
{
	APE_LOG_FUNC_ENTER();

	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));

	// done with body frame reader
	SafeRelease(reader);

	// done with coordinate mapper
	SafeRelease(m_pCoordinateMapper);

	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);

	APE_LOG_FUNC_LEAVE();
}

void ape::KinectPlugin::eventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::NODE_CREATE)
	{
		if (event.subjectName == "clothNode")
		{
			mClothNode = mpSceneManager->getNode(event.subjectName);
		}
	}
}

void ape::KinectPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpapeUserInputMacro = ape::UserInputMacro::getSingletonPtr();

	InitializeDefaultSensor();
	APE_LOG_DEBUG("Sensor init finished");

	CloudSize = (unsigned int)(size*pointratio);
	if (CloudSize % 3 != 0)
	{
		CloudSize -= CloudSize % 3;
	}
	KPts.resize(CloudSize);
	KCol.resize(CloudSize);
	OperatorPoints.resize(CloudSize);
	OperatorColors.resize(CloudSize);

	std::stringstream kinectPluginConfigFilePath;
	kinectPluginConfigFilePath << mpCoreConfig->getConfigFolderPath() << "\\apeKinectPlugin.json";
	APE_LOG_DEBUG("kinectPluginConfigFilePath: " << kinectPluginConfigFilePath.str());
	FILE* KinectPluginConfigFile;
	if (errno_t err = fopen_s(&KinectPluginConfigFile, kinectPluginConfigFilePath.str().c_str(), "r") == 0)
	{
		char readBuffer[65536];
		if (KinectPluginConfigFile)
		{
			rapidjson::FileReadStream jsonFileReaderStream(KinectPluginConfigFile, readBuffer, sizeof(readBuffer));
			rapidjson::Document jsonDocument;
			jsonDocument.ParseStream(jsonFileReaderStream);
			if (jsonDocument.IsObject())
			{
				rapidjson::Value& KPosition = jsonDocument["sensorPosition"];
				for (int i = 0; i < 3; i++)
				{
					KPos[i] = jsonDocument["sensorPosition"].GetArray()[i].GetFloat();
					APE_LOG_DEBUG("sensorPosition: " << std::to_string(KPos[i]));
				}

				rapidjson::Value& KOrientation = jsonDocument["sensorOrientation"];
				for (int i = 0; i < 4; i++)
				{
					KRot[i] = jsonDocument["sensorOrientation"].GetArray()[i].GetFloat();
					APE_LOG_DEBUG("sensorOrientation: " << std::to_string(KRot[i]));
				}

				rapidjson::Value& KSSkeleton = jsonDocument["showSkeleton"];
				showSkeleton = jsonDocument["showSkeleton"].GetBool();
				APE_LOG_DEBUG("showSkeleton: " << std::to_string(showSkeleton));

				rapidjson::Value& KSPointCloud = jsonDocument["showPointCloud"];
				showPointCloud = jsonDocument["showPointCloud"].GetBool();
				APE_LOG_DEBUG("showPointCloud: " << std::to_string(showPointCloud));

				rapidjson::Value& KBRemoval = jsonDocument["backgroundRemoval"];
				backgroundRemoval = jsonDocument["backgroundRemoval"].GetBool();
				APE_LOG_DEBUG("backgroundRemoval: " << std::to_string(backgroundRemoval));

				rapidjson::Value& KMFPS = jsonDocument["maxFPS"];
				maxFPS = jsonDocument["maxFPS"].GetBool();
				APE_LOG_DEBUG("maxFPS: " << std::to_string(maxFPS));

				rapidjson::Value& KM3DS = jsonDocument["3dScan"];
				_3dScan = jsonDocument["3dScan"].GetBool();
				APE_LOG_DEBUG("3dScan: " << std::to_string(_3dScan));

				rapidjson::Value& KSForceOneSkeleton = jsonDocument["forceOneSkeleton"];
				forceOneSkeleton = jsonDocument["forceOneSkeleton"].GetBool();
				APE_LOG_DEBUG("forceOneSkeleton: " << std::to_string(forceOneSkeleton));
			}
			fclose(KinectPluginConfigFile);
		}
	}
	else
		APE_LOG_DEBUG("Error cannot open config file");
	if (_3dScan)
	{
		if (auto mClothNode = mpSceneManager->createNode("clothNode", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			mClothNode->setScale(ape::Vector3(1.0, 1.0, 1.2));
			if (auto meshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("T-Shirt.3DS.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				meshFile->setFileName("T-Shirt.3DS.mesh");
				meshFile->setParentNode(mClothNode);
			}
		}
	}

	if (auto rootNode = RootNode.lock())
	{
		rootNode->setPosition(ape::Vector3(KPos[0], KPos[1], KPos[2]));
		rootNode->setOrientation(ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
	}

	//init bodies
	if (showSkeleton)
	{
		std::shared_ptr<ape::IManualMaterial> _0bodyMaterial;
		if (_0bodyMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("0BodyNodeMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			_0bodyMaterial->setDiffuseColor(ape::Color(0.0f, 1.0f, 0.0f));
			_0bodyMaterial->setSpecularColor(ape::Color(0.0f, 1.0f, 0.0f));
		}

		for (int i = 0; i < 25; i++)
		{
			std::string index = std::to_string(i);

			if (auto myNode = mpSceneManager->createNode("0BodyNode" + index, true, mpCoreConfig->getNetworkGUID()).lock())
			{
				myNode->setPosition(ape::Vector3(0, i * 100, 0));
				_0Body.push_back(myNode);
			}

			if (auto childNode = _0Body[i].lock())
			{
				childNode->setParentNode(RootNode);
			}

			if (auto _0BodyGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("0BodyGeometry" + index, ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				_0BodyGeometry->setParameters(2.0f, ape::Vector2(1, 1));
				_0BodyGeometry->setParentNode(_0Body[i]);
				_0BodyGeometry->setMaterial(_0bodyMaterial);

				ape::RigidBodySharedPtr rigidBody;
				if (rigidBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("0BodyNode" + index + "_rigidBody", ape::Entity::RIGIDBODY, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					rigidBody->setGeometry(_0BodyGeometry);
					rigidBody->setToStatic();
					rigidBody->setParentNode(_0Body[i]);
				}
			}
		}

		if (!forceOneSkeleton)
		{
			std::shared_ptr<ape::IManualMaterial> _1bodyMaterial;
			if (_1bodyMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("1BodyNodeMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				_1bodyMaterial->setDiffuseColor(ape::Color(1.0f, 0.0f, 0.0f));
				_1bodyMaterial->setSpecularColor(ape::Color(1.0f, 0.0f, 0.0f));
			}

			for (int i = 0; i < 25; i++)
			{
				std::string index = std::to_string(i);

				if (auto myNode = mpSceneManager->createNode("1BodyNode" + index, true, mpCoreConfig->getNetworkGUID()).lock())
				{
					_1Body.push_back(myNode);
				}

				if (auto childNode = _1Body[i].lock())
				{
					childNode->setParentNode(RootNode);
				}

				if (auto _1BodyGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("1BodyGeometry" + index, ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					_1BodyGeometry->setParameters(2.0f, ape::Vector2(1, 1));
					_1BodyGeometry->setParentNode(_1Body[i]);
					_1BodyGeometry->setMaterial(_1bodyMaterial);

					/*ape::RigidBodySharedPtr rigidBody;
					if (rigidBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("1BodyNode" + index + "_rigidBody", ape::Entity::RIGIDBODY, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						rigidBody->setGeometry(_1BodyGeometry);
						rigidBody->setToStatic();
						rigidBody->setParentNode(_1Body[i]);
					}*/
				}
			}

			std::shared_ptr<ape::IManualMaterial> _2bodyMaterial;
			if (_2bodyMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("2BodyNodeMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				_2bodyMaterial->setDiffuseColor(ape::Color(0.0f, 0.0f, 1.0f));
				_2bodyMaterial->setSpecularColor(ape::Color(0.0f, 0.0f, 1.0f));
			}

			for (int i = 0; i < 25; i++)
			{
				std::string index = std::to_string(i);

				if (auto myNode = mpSceneManager->createNode("2BodyNode" + index, true, mpCoreConfig->getNetworkGUID()).lock())
				{
					_2Body.push_back(myNode);
				}

				if (auto childNode = _2Body[i].lock())
				{
					childNode->setParentNode(RootNode);
				}

				if (auto _2BodyGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("2BodyGeometry" + index, ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					_2BodyGeometry->setParameters(2.0f, ape::Vector2(1, 1));
					_2BodyGeometry->setParentNode(_2Body[i]);
					_2BodyGeometry->setMaterial(_2bodyMaterial);

					/*ape::RigidBodySharedPtr rigidBody;
					if (rigidBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("2BodyNode" + index + "_rigidBody", ape::Entity::RIGIDBODY, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						rigidBody->setGeometry(_2BodyGeometry);
						rigidBody->setToStatic();
						rigidBody->setParentNode(_2Body[i]);
					}*/
				}
			}

			std::shared_ptr<ape::IManualMaterial> _3bodyMaterial;
			if (_3bodyMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("3BodyNodeMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				_3bodyMaterial->setDiffuseColor(ape::Color(0.0f, 0.0f, 1.0f));
				_3bodyMaterial->setSpecularColor(ape::Color(0.0f, 0.0f, 1.0f));
			}

			for (int i = 0; i < 25; i++)
			{
				std::string index = std::to_string(i);

				if (auto myNode = mpSceneManager->createNode("3BodyNode" + index, true, mpCoreConfig->getNetworkGUID()).lock())
				{
					_3Body.push_back(myNode);
				}

				if (auto childNode = _3Body[i].lock())
				{
					childNode->setParentNode(RootNode);
				}

				if (auto _3BodyGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("3BodyGeometry" + index, ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					_3BodyGeometry->setParameters(2.0f, ape::Vector2(1, 1));
					_3BodyGeometry->setParentNode(_3Body[i]);
					_3BodyGeometry->setMaterial(_3bodyMaterial);

					/*ape::RigidBodySharedPtr rigidBody;
					if (rigidBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("3BodyNode" + index + "_rigidBody", ape::Entity::RIGIDBODY, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						rigidBody->setGeometry(_3BodyGeometry);
						rigidBody->setToStatic();
						rigidBody->setParentNode(_3Body[i]);
					}*/
				}
			}

			std::shared_ptr<ape::IManualMaterial> _4bodyMaterial;
			if (_4bodyMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("4BodyNodeMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				_4bodyMaterial->setDiffuseColor(ape::Color(0.0f, 0.0f, 1.0f));
				_4bodyMaterial->setSpecularColor(ape::Color(0.0f, 0.0f, 1.0f));
			}

			for (int i = 0; i < 25; i++)
			{
				std::string index = std::to_string(i);

				if (auto myNode = mpSceneManager->createNode("4BodyNode" + index, true, mpCoreConfig->getNetworkGUID()).lock())
				{
					_4Body.push_back(myNode);
				}

				if (auto childNode = _4Body[i].lock())
				{
					childNode->setParentNode(RootNode);
				}

				if (auto _4BodyGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("4BodyGeometry" + index, ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					_4BodyGeometry->setParameters(2.0f, ape::Vector2(1, 1));
					_4BodyGeometry->setParentNode(_4Body[i]);
					_4BodyGeometry->setMaterial(_4bodyMaterial);

					/*ape::RigidBodySharedPtr rigidBody;
					if (rigidBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("4BodyNode" + index + "_rigidBody", ape::Entity::RIGIDBODY, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						rigidBody->setGeometry(_4BodyGeometry);
						rigidBody->setToStatic();
						rigidBody->setParentNode(_4Body[i]);
					}*/
				}
			}

			std::shared_ptr<ape::IManualMaterial> _5bodyMaterial;
			if (_5bodyMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("5BodyNodeMaterial", ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				_5bodyMaterial->setDiffuseColor(ape::Color(0.0f, 0.0f, 1.0f));
				_5bodyMaterial->setSpecularColor(ape::Color(0.0f, 0.0f, 1.0f));
			}

			for (int i = 0; i < 25; i++)
			{
				std::string index = std::to_string(i);

				if (auto myNode = mpSceneManager->createNode("5BodyNode" + index, true, mpCoreConfig->getNetworkGUID()).lock())
				{
					_5Body.push_back(myNode);
				}

				if (auto childNode = _5Body[i].lock())
				{
					childNode->setParentNode(RootNode);
				}

				if (auto _5BodyGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("5BodyGeometry" + index, ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					_5BodyGeometry->setParameters(2.0f, ape::Vector2(1, 1));
					_5BodyGeometry->setParentNode(_5Body[i]);
					_5BodyGeometry->setMaterial(_5bodyMaterial);

					/*ape::RigidBodySharedPtr rigidBody;
					if (rigidBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->createEntity("5BodyNode" + index + "_rigidBody", ape::Entity::RIGIDBODY, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						rigidBody->setGeometry(_5BodyGeometry);
						rigidBody->setToStatic();
						rigidBody->setParentNode(_5Body[i]);
					}*/
				}
			}
		}
	}

	APE_LOG_FUNC_LEAVE();
}

void ape::KinectPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		Update();

		if (sstate != 2 && showPointCloud)
		{
			if (!backgroundRemoval)
			{
				//Generate the Point Cloud
				if (!pointsGenerated && KPts[3030] != 0.0 && KPts[3030] != -1 * std::numeric_limits<float>::infinity())
				{
					if (auto pointCloudNode = mpSceneManager->createNode("pointCloudNode_Kinect", true, mpCoreConfig->getNetworkGUID()).lock())
					{
						pointCloudNode->setPosition(ape::Vector3(KPos[0], KPos[1], KPos[2]));
						pointCloudNode->setOrientation(ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
						if (auto textNode = mpSceneManager->createNode("pointCloudNode_Kinect_Text_Node", true, mpCoreConfig->getNetworkGUID()).lock())
						{
							textNode->setParentNode(pointCloudNode);
							textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
							if (auto text = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("pointCloudNode_Kinect_Text", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
							{
								text->setCaption("Kinect");
								text->setParentNode(textNode);
							}
						}
						if (auto pointCloud = std::static_pointer_cast<ape::IPointCloud>(mpSceneManager->createEntity("pointCloud_Kinect", ape::Entity::POINT_CLOUD, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							pointCloud->setParameters(KPts, KCol, 10000, 1.0f, true, 500.0f, 500.0f, 3.0f);
							pointCloud->setParentNode(pointCloudNode);
							mPointCloud = pointCloud;
						}
					}
					pointsGenerated = true;
				}

				//Refresh the Point Cloud
				if (auto pointCloud = mPointCloud.lock())
				{
					pointCloud->updatePoints(KPts);
					pointCloud->updateColors(KCol);
				}
			}
			else
			{
				if (_1Detected && !operatorPointsGenerated)
				{
					if (auto pointCloudNode = mpSceneManager->createNode("pointCloudNode_KinectOperator", true, mpCoreConfig->getNetworkGUID()).lock())
					{
						pointCloudNode->setPosition(ape::Vector3(KPos[0], KPos[1], KPos[2]));
						pointCloudNode->setOrientation(ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
						if (auto textNode = mpSceneManager->createNode("pointCloudNodeText_KinectOperator_Node", true, mpCoreConfig->getNetworkGUID()).lock())
						{
							textNode->setParentNode(pointCloudNode);
							textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
							if (auto userNameText = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("pointCloudNodeText_KinectOperator", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
							{
								userNameText->setCaption("pointCloudNodeText_KinectOperator");
								userNameText->setParentNode(textNode);
							}
						}
						if (auto pointCloud = std::static_pointer_cast<ape::IPointCloud>(mpSceneManager->createEntity("pointCloud_KinectOperator", ape::Entity::POINT_CLOUD, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							pointCloud->setParameters(OperatorPoints, OperatorColors, 100000, 1.0f, true, 500.0f, 500.0f, 3.0f);
							pointCloud->setParentNode(pointCloudNode);
							mOperatorPointCloud = pointCloud;
						}
					}

					operatorPointsGenerated = true;
				}

				if (auto pointCloud = mOperatorPointCloud.lock())
				{
					pointCloud->updatePoints(OperatorPoints);
					pointCloud->updateColors(OperatorColors);
				}
#ifdef HALF
				//show the scanned body so far
				if (!halfscan && sstate == 1 && ScannedPoints.size() != 0)
				{
					std::cout << ScannedPoints.size() << std::endl;
					std::cout << ScannedColors.size() << std::endl;

					if (auto pointCloudNode = mpScene->createNode("pointCloudNode_KinectHalfScan", true, mpCoreConfig->getNetworkGUID()).lock())
					{
						pointCloudNode->setPosition(ape::Vector3(KPos[0], KPos[1], KPos[2]));
						pointCloudNode->setOrientation(ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
						if (auto textNode = mpScene->createNode("pointCloudNodeText_KinectHalfScan_Node", true, mpCoreConfig->getNetworkGUID()).lock())
						{
							textNode->setParentNode(pointCloudNode);
							textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
							if (auto userNameText = std::static_pointer_cast<ape::ITextGeometry>(mpScene->createEntity("pointCloudNodeText_KinectHalfScan", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
							{
								userNameText->setCaption("pointCloudNodeText_KinectHalfScan");
								userNameText->setParentNode(textNode);
							}
						}
						if (auto pointCloud = std::static_pointer_cast<ape::IPointCloud>(mpScene->createEntity("pointCloud_KinectHalfScan", ape::Entity::POINT_CLOUD, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							pointCloud->setParameters(ScannedPoints, ScannedColors, 100000);
							pointCloud->setParentNode(pointCloudNode);
							//mOperatorPointCloud = pointCloud;
						}
					}
					halfscan = true;
				}
#endif
			}
		}
		else
		{
			if (!operatorPointsGenerated)
			{
				if (auto pointCloudNode = mpSceneManager->getNode("pointCloudNode_KinectOperator").lock())//hide point cloud
				{
					pointCloudNode->setChildrenVisibility(false);
				}
#ifdef HALF
				if (auto pointCloudNode = mpScene->getNode("pointCloudNode_KinectHalfScan").lock())//hide halfscan point cloud
				{
					pointCloudNode->setChildrenVisibility(false);
				}
#endif
				//show scanned point cloud
				if (auto pointCloudNode = mpSceneManager->createNode("pointCloudNode_KinectScanner", true, mpCoreConfig->getNetworkGUID()).lock())
				{
					pointCloudNode->setPosition(ape::Vector3(KPos[0], KPos[1], KPos[2]));
					pointCloudNode->setOrientation(ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
					if (auto textNode = mpSceneManager->createNode("pointCloudNodeText_KinectScanner_Node", true, mpCoreConfig->getNetworkGUID()).lock())
					{
						textNode->setParentNode(pointCloudNode);
						textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto userNameText = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("pointCloudNodeText_KinectScanner", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
						{
							userNameText->setCaption("pointCloudNodeText_KinectScanner");
							userNameText->setParentNode(textNode);
						}
					}
					if (auto pointCloud = std::static_pointer_cast<ape::IPointCloud>(mpSceneManager->createEntity("pointCloud_KinectScanner", ape::Entity::POINT_CLOUD, true, mpCoreConfig->getNetworkGUID()).lock()))
					{
						pointCloud->setParameters(ScannedPoints, ScannedColors, 100000, 1.0f, true, 500.0f, 500.0f, 3.0f);
						pointCloud->setParentNode(pointCloudNode);
						mOperatorPointCloud = pointCloud;
					}
				}

				//Affix cloth node to scanned point cloud
				if (auto rootClothNode = mpSceneManager->getNode("clothNode").lock())
				{
					std::cout << "locked" << std::endl;
					rootClothNode->setPosition(anchor);
					std::cout << rootClothNode->getPosition().toString() << std::endl;
					/*mUserInputMacroPose.userPosition = ape::Vector3(-0.931241, 31.6337, 110.023);
					mUserInputMacroPose.userOrientation = ape::Quaternion(-0.993851, 0.0846812, 0.0710891, 0.00605706);
					mpapeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
					mUserInputMacroPose.userPosition = ape::Vector3(-406.561, 45.8449, -250.643);
					mUserInputMacroPose.userOrientation = ape::Quaternion(-0.710511, 0.0605394, 0.698546, 0.0595197);
					mpapeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);
					mUserInputMacroPose.userPosition = ape::Vector3(11.0208, 51.4279, -695.983);
					mUserInputMacroPose.userOrientation = ape::Quaternion(0.0135394, -0.00138574, 0.994711, 0.101814);
					mpapeUserInputMacro->interpolateViewPose(mUserInputMacroPose, 5000);*/
				}
				operatorPointsGenerated = true;
			}
		}

		//Draw body joints
		if (showSkeleton)
		{
			for (int i = 0; i < 25; i++)
			{
				if (auto bodynode = _0Body[i].lock())
				{
					if (body[0] != NULL)
					{
						if (body[0][i][0] == 0 && body[0][i][1] == 0 && body[0][i][2] == 0)
						{
							bodynode->setChildrenVisibility(false);
						}
						else
						{
							bodynode->setChildrenVisibility(true);
							bodynode->setPosition(ape::Vector3(body[0][i][0] * 100, body[0][i][1] * 100, body[0][i][2] * 100));
						}
					}
				}
			}
			if (!forceOneSkeleton)
			{
				for (int i = 0; i < 25; i++)
				{
					if (auto bodynode = _1Body[i].lock())
					{
						if (body[1] != NULL)
						{
							if (body[1][i][0] == 0 && body[1][i][1] == 0 && body[1][i][2] == 0)
							{
								bodynode->setChildrenVisibility(false);
							}
							else
							{
								bodynode->setChildrenVisibility(true);
								bodynode->setPosition(ape::Vector3(body[1][i][0] * 100, body[1][i][1] * 100, body[1][i][2] * 100));
							}

						}
					}
				}

				for (int i = 0; i < 25; i++)
				{
					if (auto bodynode = _2Body[i].lock())
					{
						if (body[2] != NULL)
						{
							if (body[2][i][0] == 0 && body[2][i][1] == 0 && body[2][i][2] == 0)
							{
								bodynode->setChildrenVisibility(false);
							}
							else
							{
								bodynode->setChildrenVisibility(true);
								bodynode->setPosition(ape::Vector3(body[2][i][0] * 100, body[2][i][1] * 100, body[2][i][2] * 100));
							}
						}
					}
				}

				for (int i = 0; i < 25; i++)
				{
					if (auto bodynode = _3Body[i].lock())
					{
						if (body[3] != NULL)
						{
							if (body[3][i][0] == 0 && body[3][i][1] == 0 && body[3][i][2] == 0)
							{
								bodynode->setChildrenVisibility(false);
							}
							else
							{
								bodynode->setChildrenVisibility(true);
								bodynode->setPosition(ape::Vector3(body[3][i][0] * 100, body[3][i][1] * 100, body[3][i][2] * 100));
							}
						}
					}
				}

				for (int i = 0; i < 25; i++)
				{
					if (auto bodynode = _4Body[i].lock())
					{
						if (body[4] != NULL)
						{
							if (body[4][i][0] == 0 && body[4][i][1] == 0 && body[4][i][2] == 0)
							{
								bodynode->setChildrenVisibility(false);
							}
							else
							{
								bodynode->setChildrenVisibility(true);
								bodynode->setPosition(ape::Vector3(body[4][i][0] * 100, body[4][i][1] * 100, body[4][i][2] * 100));
							}
						}
					}
				}

				for (int i = 0; i < 25; i++)
				{
					if (auto bodynode = _5Body[i].lock())
					{
						if (body[5] != NULL)
						{
							if (body[5][i][0] == 0 && body[5][i][1] == 0 && body[5][i][2] == 0)
							{
								bodynode->setChildrenVisibility(false);
							}
							else
							{
								bodynode->setChildrenVisibility(true);
								bodynode->setPosition(ape::Vector3(body[5][i][0] * 100, body[5][i][1] * 100, body[5][i][2] * 100));
							}
						}
					}
				}
			}
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::KinectPlugin::Step()
{

}

void ape::KinectPlugin::Stop()
{

}

void ape::KinectPlugin::Suspend()
{

}

void ape::KinectPlugin::Restart()
{

}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT ape::KinectPlugin::InitializeDefaultSensor()
{
	APE_LOG_FUNC_ENTER();
	HRESULT hr;
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
		APE_LOG_ERROR("Connecting to Kinect failed");
	}
	if (m_pKinectSensor)
	{
		hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->Open();
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Body ,
				&reader);
		}
		APE_LOG_DEBUG("Connected to Kinect");
	}
	if (!m_pKinectSensor || FAILED(hr))
	{
		APE_LOG_ERROR("Kinect has not found");
		return E_FAIL;
	}
	APE_LOG_FUNC_LEAVE();
	return hr;
}

/// <summary>
/// Main processing function
/// </summary>
void ape::KinectPlugin::Update()
{
	if (!reader)
	{
		return;
	}
	IMultiSourceFrame* pFrame = NULL;
	HRESULT hr = reader->AcquireLatestFrame(&pFrame);
	if (backgroundRemoval)
	{
		if (maxFPS)
		{
			if (SUCCEEDED(hr))
			{
				//indexes.clear(); //operator function specific
				GetDepthData(pFrame);
				if (showSkeleton)
				{
					GetBodyData(pFrame);
				}
				GetRGBData(pFrame);
				//GetOperatorPts();
				//GetOperatorColrs();
			}
			SafeRelease(pFrame);
			IMultiSourceFrame* pFrame2 = NULL;
			HRESULT hr2 = reader->AcquireLatestFrame(&pFrame2);
			if (SUCCEEDED(hr2))
			{
				GetBodyIndexes(pFrame2);
			}
			SafeRelease(pFrame2);
		}
		else
		{
			if (SUCCEEDED(hr))
			{
				if (framecount % 2 == 0) //OPT GetBodyIndexes and GetDepthData can only work with different frames //needs too much resources?? 
				{
					GetDepthData(pFrame);
					if (showSkeleton || _3dScan)
					{
						GetBodyData(pFrame);
					}
					GetRGBData(pFrame);
				}
				else
				{
					GetBodyIndexes(pFrame);

					if (_3dScan && sstate!=2)
					{
						ScanOperator();
					}
				}
			}
			SafeRelease(pFrame);
			framecount++;
		}
	}
	else
	{
		if (SUCCEEDED(hr))
		{
			GetDepthData(pFrame);
			GetRGBData(pFrame);
			if (showSkeleton)
			{
				GetBodyData(pFrame);
			}
		}
		SafeRelease(pFrame);
	}
}

/// <summary>
/// Record point cloud if the pose is correct
/// </summary>
void ape::KinectPlugin::ScanOperator()
{
	bool onefound = false;
	for (int i = 0; i < cBodyCount; ++i)
	{
		if (Operatorfound[i] && !onefound && (framecount > (scanframe + 100)))
		{
			onefound = true;
			if (CheckAngles(i))
			{
				std::cout << "true" << std::endl;
				if (sstate == 0)
				{
					spoint1 = Vector3(body[i][0][0] * 100, body[i][0][1] * 100, body[i][0][2] * 100);
					anchor = Vector3(-body[i][0][0] * 100, body[i][0][1] * 100 - 131, -body[i][0][2] * 100 + 4.4);
				}
				else
				{
					spoint2 = Vector3(body[i][0][0] * 100, body[i][0][1] * 100, body[i][0][2] * 100);				
				}

				for (unsigned int j = 0; j < CloudSize / 3; ++j)
				{
					if (bodyIdx[j] != 0xff && (spoint1.z - OperatorPoints[3 * j + 2]) < 40.0f)
					{
						if (sstate == 1)
						{
							Vector3 point = Vector3(OperatorPoints[3 * j], OperatorPoints[3 * j + 1], OperatorPoints[3 * j + 2]);
							point -= spoint2;
							point = turnaraound * point;
							point = point + spoint1 * 0.97f;

							ScannedPoints.push_back(point.x);
							ScannedPoints.push_back(point.y);
							ScannedPoints.push_back(point.z);
						}
						else
						{
							ScannedPoints.push_back(OperatorPoints[3 * j]);
							ScannedPoints.push_back(OperatorPoints[3 * j + 1]);
							ScannedPoints.push_back(OperatorPoints[3 * j + 2]);
						}

						ScannedColors.push_back(OperatorColors[3 * j]);
						ScannedColors.push_back(OperatorColors[3 * j + 1]);
						ScannedColors.push_back(OperatorColors[3 * j + 2]);
					}
				}
				if (sstate == WAITING)
				{
					sstate = FIRST_DONE;
					scanframe = framecount;
				}
				else
				{				
					sstate = SECOND_DONE;
					operatorPointsGenerated = false;
					std::cout << ScannedPoints.size() << std::endl;
				}
			}
		}
	}
}

/// <summary>
/// Check body angles for correct pose recognition
/// </summary>
/// <param name='index'> index of operator's body </param>
/// <returns> True if pose is correct </returns>
bool ape::KinectPlugin::CheckAngles(int index)
{
	Vector3 spine(body[index][0][0] - body[index][20][0], body[index][0][1] - body[index][20][1], body[index][0][2] - body[index][20][2]);
	Vector3 head(body[index][20][0] - body[index][3][0], body[index][20][1] - body[index][3][1], body[index][20][2] - body[index][3][2]);
	Vector3 rhand(body[index][10][0] - body[index][8][0], body[index][10][1] - body[index][8][1], body[index][10][2] - body[index][8][2]);
	Vector3 lhand(body[index][6][0] - body[index][4][0], body[index][6][1] - body[index][4][1], body[index][6][2] - body[index][4][2]);
	Vector3 rleg(body[index][18][0] - body[index][16][0], body[index][18][1] - body[index][16][1], body[index][18][2] - body[index][16][2]);
	Vector3 lleg(body[index][14][0] - body[index][12][0], body[index][14][1] - body[index][12][1], body[index][14][2] - body[index][12][2]);

	float sang = AngleBetween(spine, Vector3(0.0f, -1.0f, 0.0f));
	float hang = AngleBetween(head, Vector3(0.0f, -1.0f, 0.0f));
	float rhang = AngleBetween(rhand, Vector3(1.0f, -1.0f, 0.0f));
	float lhang = AngleBetween(lhand, Vector3(-1.0f, -1.0f, 0.0f));
	float rlang = AngleBetween(rleg, Vector3(0.2f, -1.0f, 0.0f));
	float llang = AngleBetween(lleg, Vector3(-0.2f, -1.0f, 0.0f));

	std::cout << rhang;
	std::cout << "  ";
	std::cout << lhang;
	std::cout << "  ";
	std::cout << rlang;
	std::cout << "  ";
	std::cout << llang;
	std::cout << "  ";
	std::cout << sang;
	std::cout << "  ";
	std::cout << hang << std::endl;

	return (CheckMaxDifference(rhang, 0.0f, 8.5f) && CheckMaxDifference(lhang, 0.0f, 8.5f) && CheckMaxDifference(rlang, 0.0f, 8.5f) && CheckMaxDifference(llang, 0.0f, 8.5f) &&
		CheckMaxDifference(sang, 0.0f, 4.0f) && CheckMaxDifference(hang, 0.0f, 8.5f));
}

/// <summary>
/// Calculate the angle between two vectors
/// </summary>
/// <param name='p1'> first vector </param>
/// <param name='p2'> second vector </param>
/// <returns> the angle value in degrees </returns>
float ape::KinectPlugin::AngleBetween(Vector3 p1, Vector3 p2)
{
	p1.normalise();
	p2.normalise();
	Radian angle = acos(p1.dotProduct(p2) / sqrt(p1.squaredLength() * p2.squaredLength()));

	return angle.toDegree();
}

bool ape::KinectPlugin::CheckDifference(float data, float ref, float perc)
{
	if (data > ref * (1 - perc) && data < ref * (1 + perc))
		return true;
	else
		return false;
}

bool ape::KinectPlugin::CheckMaxDifference(float data, float ref, float maxdev)
{
	return (data > (ref - maxdev) && data < (ref + maxdev));
}

//Legacy
void ape::KinectPlugin::GetOperatorColrs()
{
	OperatorColors.clear();
	if (indexes.size() > (UINT64)1)
	{
		APE_LOG_DEBUG("color start");
		for (UINT64 i = 0; i < indexes.size(); i++)
		{
			OperatorColors.push_back(KCol[3 * indexes[i]]);
			OperatorColors.push_back(KCol[3 * indexes[i] + 1]);
			OperatorColors.push_back(KCol[3 * indexes[i] + 2]);
		}
	}
	else
	{
		OperatorColors = {0.0, 0.0, 0.0};
	}
	APE_LOG_DEBUG("color finished");
}

void ape::KinectPlugin::GetOperatorPts()
{
	OperatorPoints.clear();
	_1Detected = false;
	for (int n = 0; n < cBodyCount; n++)
	{
		if (body[n][0][0] != 0 && body[n][0][1] != 0 && body[n][0][2] != 0 && !_1Detected)
		{
			_1Detected = true;
			APE_LOG_DEBUG("detected");
			for (unsigned int i = 0; i < CloudSize / 3; i++)
			{
				for (int j = 0; j < 25; j++)
				{
					std::vector<float> Point = { KPts[3 * i], KPts[3 * i + 1], KPts[3 * i + 2] };
					std::vector<float> Joint = {100 * body[n][j][0], 100 * body[n][j][1], 100 * body[n][j][2] };
					float distance = GetDistance(Joint, Point);
					if (j == 0 && distance < 80.0 && distance > 0.0)
					{
						indexes.push_back(i);
						OperatorPoints.push_back(Point[0]);
						OperatorPoints.push_back(Point[1]);
						OperatorPoints.push_back(Point[2]);
					}
				}
			}
			APE_LOG_DEBUG("finished");
		}
	}
	if (!_1Detected)
		OperatorPoints = {0.0, 0.0, 0.0};
}

float ape::KinectPlugin::GetDistance(std::vector<float> joint, std::vector<float> point)
{
	return sqrtf(powf((point[0]-joint[0]),2.0)+ powf((point[1] - joint[1]), 2.0) + powf((point[2] - joint[2]), 2.0));
}

void ape::KinectPlugin::GetRGBData(IMultiSourceFrame* pframe)
{
	//APE_LOG_FUNC_ENTER();
	IColorFrame* pColorFrame = NULL;
	IColorFrameReference* pColorFrameRef = NULL;
	HRESULT hr = pframe->get_ColorFrameReference(&pColorFrameRef);
	if (SUCCEEDED(hr))
	{
		hr = pColorFrameRef->AcquireFrame(&pColorFrame);
	}
	SafeRelease(pColorFrameRef);
	if (SUCCEEDED(hr))
	{
		// Get data from frame
		pColorFrame->CopyConvertedFrameDataToArray(colorwidth*colorheight * 4, rgbimage, ColorImageFormat_Rgba);

		ape::PointCloudColors colorpoints;
		for (unsigned int i = 0; i < CloudSize / 3; i++) 
		{
			ColorSpacePoint p = depth2rgb[i];
			/* Check if color pixel coordinates are in bounds*/
			if (p.X < 0 || p.Y < 0 || p.X > colorwidth || p.Y > colorheight) {
				colorpoints.push_back(0.);
				colorpoints.push_back(0.);
				colorpoints.push_back(0.);
			}
			else {
				int idx = (int)p.X + colorwidth * (int)p.Y;
				colorpoints.push_back((float)rgbimage[4 * idx + 0] / 255.0f);
				colorpoints.push_back((float)rgbimage[4 * idx + 1] / 255.0f);
				colorpoints.push_back((float)rgbimage[4 * idx + 2] / 255.0f);
			}
		}
		KCol = colorpoints;
	}
	SafeRelease(pColorFrame);
}

void ape::KinectPlugin::GetBodyIndexes(IMultiSourceFrame* pframe)
{
	APE_LOG_FUNC_ENTER();
	IBodyIndexFrame* pIndexFrame = NULL;
	IBodyIndexFrameReference* pIndexFrameRef = NULL;
	HRESULT hr = pframe->get_BodyIndexFrameReference(&pIndexFrameRef);
	if (SUCCEEDED(hr))
	{
		APE_LOG_TRACE("ref found");
		hr = pIndexFrameRef->AcquireFrame(&pIndexFrame);
	}
	APE_LOG_TRACE("ref found gd");
	SafeRelease(pIndexFrameRef);
	if (SUCCEEDED(hr))
	{
		APE_LOG_TRACE("idx got");
		hr = pIndexFrame->CopyFrameDataToArray(width*height, bodyIdx);
		if (SUCCEEDED(hr))
		{
			APE_LOG_TRACE("success");
			GetOperator();
		}
	}
	SafeRelease(pIndexFrame);
	APE_LOG_FUNC_LEAVE();
}

void  ape::KinectPlugin::GetOperator()
{
	APE_LOG_FUNC_ENTER();
	ape::PointCloudPoints OPoint;
	ape::PointCloudColors OColor;

	//APE_LOG_DEBUG(std::to_string(bodyIdx[50000]) + "; " + std::to_string(bodyIdx[100000]) + "; " + std::to_string(bodyIdx[150000]));
	for (unsigned int i = 0; i < CloudSize/3; i++)
	{
		if (bodyIdx[i] != 0xff)
		{
			_1Detected = true;

			OPoint.push_back(KPts[3 * i]);
			OPoint.push_back(KPts[3 * i + 1]);
			OPoint.push_back(KPts[3 * i + 2]);

			OColor.push_back(KCol[3 * i]);
			OColor.push_back(KCol[3 * i + 1]);
			OColor.push_back(KCol[3 * i + 2]);
		}
		else
		{
			OPoint.push_back(0.0);
			OPoint.push_back(0.0);
			OPoint.push_back(0.0);

			OColor.push_back(0.0);
			OColor.push_back(0.0);
			OColor.push_back(0.0);
		}
	}

	OperatorPoints = OPoint;
	OperatorColors = OColor;
	APE_LOG_FUNC_LEAVE();
}

void ape::KinectPlugin::GetDepthData(IMultiSourceFrame* pframe)
{
	APE_LOG_FUNC_ENTER();
	IDepthFrame* pDepthframe = NULL;
	IDepthFrameReference* pDepthFrameRef = NULL;
	HRESULT hr = pframe->get_DepthFrameReference(&pDepthFrameRef);
	if (SUCCEEDED(hr))
	{
		hr = pDepthFrameRef->AcquireFrame(&pDepthframe);
	}
	SafeRelease(pDepthFrameRef);
	if (SUCCEEDED(hr))
	{
		// Get data from frame
		unsigned int bsize;
		unsigned short* buf;
		hr = pDepthframe->AccessUnderlyingBuffer(&bsize, &buf);
		if (SUCCEEDED(hr))
		{
			hr = m_pCoordinateMapper->MapDepthFrameToCameraSpace(width*height, buf, width*height, depth2xyz);
			ape::PointCloudPoints Points;
			for (unsigned int i = 0; i < CloudSize / 3; i++)
			{
				Points.push_back(depth2xyz[i].X * 100);
				Points.push_back(depth2xyz[i].Y * 100);
				Points.push_back(depth2xyz[i].Z * 100);
			}
			KPts = Points;
		}
		m_pCoordinateMapper->MapDepthFrameToColorSpace(width*height, buf, width*height, depth2rgb);
	}
	SafeRelease(pDepthframe);
	APE_LOG_FUNC_LEAVE();
}

///<summary>
///Gets body frame from multiframe reader
///</summary>
void ape::KinectPlugin::GetBodyData(IMultiSourceFrame* pframe)
{
	APE_LOG_FUNC_ENTER();
	IBodyFrame* pBodyFrame = NULL;
	IBodyFrameReference* pBodyFrameRef = NULL;
	HRESULT hr = pframe->get_BodyFrameReference(&pBodyFrameRef);
	if (SUCCEEDED(hr))
	{
		hr = pBodyFrameRef->AcquireFrame(&pBodyFrame);
	}
	SafeRelease(pBodyFrameRef);

	if (SUCCEEDED(hr))
	{
		APE_LOG_TRACE("body got");
		IBody* ppBodies[cBodyCount] = { 0 };
		hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		if (SUCCEEDED(hr))
		{
			ProcessBody(cBodyCount, ppBodies);
		}
		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}
	SafeRelease(pBodyFrame);
	APE_LOG_FUNC_LEAVE();
}

/// <summary>
/// Handle new body data
/// <param name="nTime">timestamp of frame</param>
/// <param name="nBodyCount">body data count</param>
/// <param name="ppBodies">body data in frame</param>
/// </summary>
void ape::KinectPlugin::ProcessBody(int nBodyCount, IBody** ppBodies)
{
	APE_LOG_FUNC_ENTER();
	HRESULT hr;
	if (m_pCoordinateMapper)
	{
		for (int i = 0; i < nBodyCount; ++i)
		{
			Operatorfound[i] = false;
			IBody* pBody = ppBodies[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				hr = pBody->get_IsTracked(&bTracked);
				if (SUCCEEDED(hr) && bTracked)
				{
					Operatorfound[i] = true;
					Joint joints[JointType_Count];
					//HandState leftHandState = HandState_Unknown;
					//HandState rightHandState = HandState_Unknown;		

					//pBody->get_HandLeftState(&leftHandState);
					//pBody->get_HandRightState(&rightHandState);

					hr = pBody->GetJoints(_countof(joints), joints);
					if (SUCCEEDED(hr))
					{
						for (int j = 0; j < _countof(joints); ++j)
						{
							if (forceOneSkeleton)
							{
								if (joints[j].TrackingState == 2)
								{
									body[0][j][0] = joints[j].Position.X;
									body[0][j][1] = joints[j].Position.Y;
									body[0][j][2] = joints[j].Position.Z;
								}
								else
								{
									body[0][j][0] = 0;
									body[0][j][1] = j * 100;
									body[0][j][2] = 0;
								}
							}
							else
							{
								if (joints[j].TrackingState == 2)
								{
									body[i][j][0] = joints[j].Position.X;
									body[i][j][1] = joints[j].Position.Y;
									body[i][j][2] = joints[j].Position.Z;
								}
								else
								{
									body[i][j][0] = 0;
									body[i][j][1] = 0;
									body[i][j][2] = 0;
								}
							}
						}
					}
				}
				else if (!forceOneSkeleton)
				{
					for (int j = 0; j < 25; j++)
					{
						body[i][j][0] = 0;
						body[i][j][1] = 0;
						body[i][j][2] = 0;
					}
				}
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}
