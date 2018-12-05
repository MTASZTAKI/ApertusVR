#include "ApeKinectPlugin.h"

//#define HALF
const int width = 512;
const int height = 424;
const int colorwidth = 1920;
const int colorheight = 1080;

unsigned char rgbimage[colorwidth*colorheight * 4];
CameraSpacePoint depth2xyz[width*height];
ColorSpacePoint depth2rgb[width*height];
BYTE bodyIdx[width*height];

Ape::KinectPlugin::KinectPlugin()
{
	APE_LOG_FUNC_ENTER();
	m_pKinectSensor = NULL;
	m_pCoordinateMapper = NULL;
	reader = NULL;

	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));
	RootNode = mpSceneManager->createNode("KinectRootNode").lock();
	APE_LOG_FUNC_LEAVE();
}

Ape::KinectPlugin::~KinectPlugin()
{
	APE_LOG_FUNC_ENTER();

	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));

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

void Ape::KinectPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE)
	{
		if (event.subjectName == "clothNode")
		{
			mClothNode = mpSceneManager->getNode(event.subjectName);
		}
	}
}

void Ape::KinectPlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	InitializeDefaultSensor();
	APE_LOG_DEBUG("Sensor init finished");

	if (auto userNode = mpSceneManager->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

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
	kinectPluginConfigFilePath << mpSystemConfig->getFolderPath() << "\\ApeKinectPlugin.json";
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

				rapidjson::Value& KBRemoval = jsonDocument["backgroundRemoval"];
				backgroundRemoval = jsonDocument["backgroundRemoval"].GetBool();
				APE_LOG_DEBUG("backgroundRemoval: " << std::to_string(backgroundRemoval));

				rapidjson::Value& KMFPS = jsonDocument["maxFPS"];
				maxFPS = jsonDocument["maxFPS"].GetBool();
				APE_LOG_DEBUG("maxFPS: " << std::to_string(maxFPS));

				rapidjson::Value& KM3DS = jsonDocument["3dScan"];
				_3dScan = jsonDocument["3dScan"].GetBool();
				APE_LOG_DEBUG("3dScan: " << std::to_string(_3dScan));
			}
			fclose(KinectPluginConfigFile);
		}
	}
	else
		APE_LOG_DEBUG("Error cannot open config file");
	if (_3dScan)
	{
		if (auto mClothNode = mpSceneManager->createNode("clothNode").lock())
		{
			mClothNode->setScale(Ape::Vector3(1.0, 1.0, 1.2));
			if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->createEntity("T-Shirt.3DS.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
			{
				meshFile->setFileName("T-Shirt.3DS.mesh");
				meshFile->setParentNode(mClothNode);
			}
		}
	}

	if (auto rootNode = RootNode.lock())
	{
		rootNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
		rootNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
	}

	//init bodies
	if (showSkeleton)
	{
		std::shared_ptr<Ape::IManualMaterial> _0bodyMaterial;
		if (_0bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("0BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			if (auto _0bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("0BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
			{
				_0bodyMaterialManualPass->setShininess(15.0f);
				_0bodyMaterialManualPass->setDiffuseColor(Ape::Color(0.0f, 1.0f, 0.0f));
				_0bodyMaterialManualPass->setSpecularColor(Ape::Color(0.0f, 1.0f, 0.0f));
				_0bodyMaterial->setPass(_0bodyMaterialManualPass);
			}
		}

		for (int i = 0; i < 25; i++)
		{
			std::string index = std::to_string(i);

			if (auto myNode = mpSceneManager->createNode("0BodyNode" + index).lock())
			{
				_0Body.push_back(myNode);
			}

			if (auto childNode = _0Body[i].lock())
			{
				childNode->setParentNode(RootNode);
			}

			if (auto _0BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->createEntity("0BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
			{
				_0BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
				_0BodyGeometry->setParentNode(_0Body[i]);
				_0BodyGeometry->setMaterial(_0bodyMaterial);
			}
		}

		std::shared_ptr<Ape::IManualMaterial> _1bodyMaterial;
		if (_1bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("1BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			if (auto _1bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("1BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
			{
				_1bodyMaterialManualPass->setShininess(15.0f);
				_1bodyMaterialManualPass->setDiffuseColor(Ape::Color(1.0f, 0.0f, 0.0f));
				_1bodyMaterialManualPass->setSpecularColor(Ape::Color(1.0f, 0.0f, 0.0f));
				_1bodyMaterial->setPass(_1bodyMaterialManualPass);
			}
		}

		for (int i = 0; i < 25; i++)
		{
			std::string index = std::to_string(i);

			if (auto myNode = mpSceneManager->createNode("1BodyNode" + index).lock())
			{
				_1Body.push_back(myNode);
			}

			if (auto childNode = _1Body[i].lock())
			{
				childNode->setParentNode(RootNode);
			}

			if (auto _1BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->createEntity("1BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
			{
				_1BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
				_1BodyGeometry->setParentNode(_1Body[i]);
				_1BodyGeometry->setMaterial(_1bodyMaterial);
			}
		}

		std::shared_ptr<Ape::IManualMaterial> _2bodyMaterial;
		if (_2bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("2BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			if (auto _2bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("2BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
			{
				_2bodyMaterialManualPass->setShininess(15.0f);
				_2bodyMaterialManualPass->setDiffuseColor(Ape::Color(0.0f, 0.0f, 1.0f));
				_2bodyMaterialManualPass->setSpecularColor(Ape::Color(0.0f, 0.0f, 1.0f));
				_2bodyMaterial->setPass(_2bodyMaterialManualPass);
			}
		}

		for (int i = 0; i < 25; i++)
		{
			std::string index = std::to_string(i);

			if (auto myNode = mpSceneManager->createNode("2BodyNode" + index).lock())
			{
				_2Body.push_back(myNode);
			}

			if (auto childNode = _2Body[i].lock())
			{
				childNode->setParentNode(RootNode);
			}

			if (auto _2BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->createEntity("2BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
			{
				_2BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
				_2BodyGeometry->setParentNode(_2Body[i]);
				_2BodyGeometry->setMaterial(_2bodyMaterial);
			}
		}

		std::shared_ptr<Ape::IManualMaterial> _3bodyMaterial;
		if (_3bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("3BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			if (auto _3bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("3BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
			{
				_3bodyMaterialManualPass->setShininess(15.0f);
				_3bodyMaterialManualPass->setDiffuseColor(Ape::Color(1.0f, 1.0f, 0.0f));
				_3bodyMaterialManualPass->setSpecularColor(Ape::Color(1.0f, 1.0f, 0.0f));
				_3bodyMaterial->setPass(_3bodyMaterialManualPass);
			}
		}

		for (int i = 0; i < 25; i++)
		{
			std::string index = std::to_string(i);

			if (auto myNode = mpSceneManager->createNode("3BodyNode" + index).lock())
			{
				_3Body.push_back(myNode);
			}

			if (auto childNode = _3Body[i].lock())
			{
				childNode->setParentNode(RootNode);
			}

			if (auto _3BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->createEntity("3BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
			{
				_3BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
				_3BodyGeometry->setParentNode(_3Body[i]);
				_3BodyGeometry->setMaterial(_3bodyMaterial);
			}
		}

		std::shared_ptr<Ape::IManualMaterial> _4bodyMaterial;
		if (_4bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("4BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			if (auto _4bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("4BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
			{
				_4bodyMaterialManualPass->setShininess(15.0f);
				_4bodyMaterialManualPass->setDiffuseColor(Ape::Color(1.0f, 0.0f, 1.0f));
				_4bodyMaterialManualPass->setSpecularColor(Ape::Color(1.0f, 0.0f, 1.0f));
				_4bodyMaterial->setPass(_4bodyMaterialManualPass);
			}
		}

		for (int i = 0; i < 25; i++)
		{
			std::string index = std::to_string(i);

			if (auto myNode = mpSceneManager->createNode("4BodyNode" + index).lock())
			{
				_4Body.push_back(myNode);
			}

			if (auto childNode = _4Body[i].lock())
			{
				childNode->setParentNode(RootNode);
			}

			if (auto _4BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->createEntity("4BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
			{
				_4BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
				_4BodyGeometry->setParentNode(_4Body[i]);
				_4BodyGeometry->setMaterial(_4bodyMaterial);
			}
		}

		std::shared_ptr<Ape::IManualMaterial> _5bodyMaterial;
		if (_5bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("5BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
		{
			if (auto _5bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("5BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
			{
				_5bodyMaterialManualPass->setShininess(15.0f);
				_5bodyMaterialManualPass->setDiffuseColor(Ape::Color(0.0f, 1.0f, 1.0f));
				_5bodyMaterialManualPass->setSpecularColor(Ape::Color(0.0f, 1.0f, 1.0f));
				_5bodyMaterial->setPass(_5bodyMaterialManualPass);
			}
		}

		for (int i = 0; i < 25; i++)
		{
			std::string index = std::to_string(i);

			if (auto myNode = mpSceneManager->createNode("5BodyNode" + index).lock())
			{
				_5Body.push_back(myNode);
			}

			if (auto childNode = _5Body[i].lock())
			{
				childNode->setParentNode(RootNode);
			}

			if (auto _5BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->createEntity("5BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
			{
				_5BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
				_5BodyGeometry->setParentNode(_5Body[i]);
				_5BodyGeometry->setMaterial(_5bodyMaterial);
			}
		}
	}

	APE_LOG_FUNC_LEAVE();
}

void Ape::KinectPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		Update();

		if (sstate != 2)
		{
			if (!backgroundRemoval)
			{
				//Generate the Point Cloud
				if (!pointsGenerated && KPts[3030] != 0.0 && KPts[3030] != -1 * std::numeric_limits<float>::infinity())
				{
					if (auto pointCloudNode = mpSceneManager->createNode("pointCloudNode_Kinect").lock())
					{
						pointCloudNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
						pointCloudNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
						if (auto textNode = mpSceneManager->createNode("pointCloudNode_Kinect_Text_Node").lock())
						{
							textNode->setParentNode(pointCloudNode);
							textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
							if (auto text = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("pointCloudNode_Kinect_Text", Ape::Entity::GEOMETRY_TEXT).lock()))
							{
								text->setCaption("Kinect");
								text->setParentNode(textNode);
							}
						}
						if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpSceneManager->createEntity("pointCloud_Kinect", Ape::Entity::POINT_CLOUD).lock()))
						{
							pointCloud->setParameters(KPts, KCol, 10000);
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
					if (auto pointCloudNode = mpSceneManager->createNode("pointCloudNode_KinectOperator").lock())
					{
						pointCloudNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
						pointCloudNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
						if (auto textNode = mpSceneManager->createNode("pointCloudNodeText_KinectOperator_Node").lock())
						{
							textNode->setParentNode(pointCloudNode);
							textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
							if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("pointCloudNodeText_KinectOperator", Ape::Entity::GEOMETRY_TEXT).lock()))
							{
								userNameText->setCaption("pointCloudNodeText_KinectOperator");
								userNameText->setParentNode(textNode);
							}
						}
						if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpSceneManager->createEntity("pointCloud_KinectOperator", Ape::Entity::POINT_CLOUD).lock()))
						{
							pointCloud->setParameters(OperatorPoints, OperatorColors, 100000);
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

					if (auto pointCloudNode = mpScene->createNode("pointCloudNode_KinectHalfScan").lock())
					{
						pointCloudNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
						pointCloudNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
						if (auto textNode = mpScene->createNode("pointCloudNodeText_KinectHalfScan_Node").lock())
						{
							textNode->setParentNode(pointCloudNode);
							textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
							if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("pointCloudNodeText_KinectHalfScan", Ape::Entity::GEOMETRY_TEXT).lock()))
							{
								userNameText->setCaption("pointCloudNodeText_KinectHalfScan");
								userNameText->setParentNode(textNode);
							}
						}
						if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud_KinectHalfScan", Ape::Entity::POINT_CLOUD).lock()))
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
				if (auto pointCloudNode = mpSceneManager->createNode("pointCloudNode_KinectScanner").lock())
				{
					pointCloudNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
					pointCloudNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
					if (auto textNode = mpSceneManager->createNode("pointCloudNodeText_KinectScanner_Node").lock())
					{
						textNode->setParentNode(pointCloudNode);
						textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
						if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("pointCloudNodeText_KinectScanner", Ape::Entity::GEOMETRY_TEXT).lock()))
						{
							userNameText->setCaption("pointCloudNodeText_KinectScanner");
							userNameText->setParentNode(textNode);
						}
					}
					if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpSceneManager->createEntity("pointCloud_KinectScanner", Ape::Entity::POINT_CLOUD).lock()))
					{
						pointCloud->setParameters(ScannedPoints, ScannedColors, 100000);
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

					if (auto userNode = mUserNode.lock())
					{
						auto moveInterpolator = std::make_unique<Ape::Interpolator>(false);
						moveInterpolator->addSection(
							userNode->getPosition(),
							Ape::Vector3(-0.931241, 31.6337, 110.023),
							5.0,
							[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
						);
						auto rotateInterpolator = std::make_unique<Ape::Interpolator>(false);
						rotateInterpolator->addSection(
							userNode->getOrientation(),
							Ape::Quaternion(-0.993851, 0.0846812, 0.0710891, 0.00605706),
							5.0,
							[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
						);
						while (!moveInterpolator->isQueueEmpty() && !rotateInterpolator->isQueueEmpty())
						{
							if (!moveInterpolator->isQueueEmpty())
								moveInterpolator->iterateTopSection();
							if (!rotateInterpolator->isQueueEmpty())
								rotateInterpolator->iterateTopSection();
						}
						auto moveInterpolator2 = std::make_unique<Ape::Interpolator>(false);
						moveInterpolator2->addSection(
							userNode->getPosition(),
							Ape::Vector3(-406.561, 45.8449, -250.643),
							15.0,
							[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
						);
						auto rotateInterpolator2 = std::make_unique<Ape::Interpolator>(false);
						rotateInterpolator2->addSection(
							userNode->getOrientation(),
							Ape::Quaternion(-0.710511, 0.0605394, 0.698546, 0.0595197),
							15.0,
							[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
						);
						while (!moveInterpolator2->isQueueEmpty() && !rotateInterpolator2->isQueueEmpty())
						{
							if (!moveInterpolator2->isQueueEmpty())
								moveInterpolator2->iterateTopSection();
							if (!rotateInterpolator2->isQueueEmpty())
								rotateInterpolator2->iterateTopSection();
						}
						auto moveInterpolator3 = std::make_unique<Ape::Interpolator>(false);
						moveInterpolator3->addSection(
							userNode->getPosition(),
							Ape::Vector3(11.0208, 51.4279, -695.983),
							15.0,
							[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
						);
						auto rotateInterpolator3 = std::make_unique<Ape::Interpolator>(false);
						rotateInterpolator3->addSection(
							userNode->getOrientation(),
							Ape::Quaternion(0.0135394, -0.00138574, 0.994711, 0.101814),
							15.0,
							[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
						);
						while (!moveInterpolator3->isQueueEmpty() && !rotateInterpolator3->isQueueEmpty())
						{
							if (!moveInterpolator3->isQueueEmpty())
								moveInterpolator3->iterateTopSection();
							if (!rotateInterpolator3->isQueueEmpty())
								rotateInterpolator3->iterateTopSection();
						}
						auto moveInterpolator4 = std::make_unique<Ape::Interpolator>(false);
						moveInterpolator4->addSection(
							userNode->getPosition(),
							Ape::Vector3(460.696, 62.134, -252.101),
							15.0,
							[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
						);
						auto rotateInterpolator4 = std::make_unique<Ape::Interpolator>(false);
						rotateInterpolator4->addSection(
							userNode->getOrientation(),
							Ape::Quaternion(0.704635, -0.0721229, 0.702226, 0.0718766),
							15.0,
							[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
						);
						while (!moveInterpolator4->isQueueEmpty() && !rotateInterpolator4->isQueueEmpty())
						{
							if (!moveInterpolator4->isQueueEmpty())
								moveInterpolator4->iterateTopSection();
							if (!rotateInterpolator4->isQueueEmpty())
								rotateInterpolator4->iterateTopSection();
						}
						auto moveInterpolator5 = std::make_unique<Ape::Interpolator>(false);
						moveInterpolator5->addSection(
							userNode->getPosition(),
							Ape::Vector3(-0.931241, 31.6337, 110.023),
							15.0,
							[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
						);
						auto rotateInterpolator5 = std::make_unique<Ape::Interpolator>(false);
						rotateInterpolator5->addSection(
							userNode->getOrientation(),
							Ape::Quaternion(-0.993851, 0.0846812, 0.0710891, 0.00605706),
							15.0,
							[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
						);
						while (!moveInterpolator5->isQueueEmpty() && !rotateInterpolator5->isQueueEmpty())
						{
							if (!moveInterpolator5->isQueueEmpty())
								moveInterpolator5->iterateTopSection();
							if (!rotateInterpolator5->isQueueEmpty())
								rotateInterpolator5->iterateTopSection();
						}
					}
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
							bodynode->setPosition(Ape::Vector3(body[0][i][0] * 100, body[0][i][1] * 100, body[0][i][2] * 100));
						}
					}
				}
			}

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
							bodynode->setPosition(Ape::Vector3(body[1][i][0] * 100, body[1][i][1] * 100, body[1][i][2] * 100));
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
							bodynode->setPosition(Ape::Vector3(body[2][i][0] * 100, body[2][i][1] * 100, body[2][i][2] * 100));
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
							bodynode->setPosition(Ape::Vector3(body[3][i][0] * 100, body[3][i][1] * 100, body[3][i][2] * 100));
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
							bodynode->setPosition(Ape::Vector3(body[4][i][0] * 100, body[4][i][1] * 100, body[4][i][2] * 100));
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
							bodynode->setPosition(Ape::Vector3(body[5][i][0] * 100, body[5][i][1] * 100, body[5][i][2] * 100));
						}
					}
				}
			}
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::KinectPlugin::Step()
{

}

void Ape::KinectPlugin::Stop()
{

}

void Ape::KinectPlugin::Suspend()
{

}

void Ape::KinectPlugin::Restart()
{

}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT Ape::KinectPlugin::InitializeDefaultSensor()
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
void Ape::KinectPlugin::Update()
{
	if (!reader)
	{
		return;
	}
	IMultiSourceFrame* pFrame = NULL;
	HRESULT hr = reader->AcquireLatestFrame(&pFrame);
	//APE_LOG_DEBUG("update");
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
void Ape::KinectPlugin::ScanOperator()
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
bool Ape::KinectPlugin::CheckAngles(int index)
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
float Ape::KinectPlugin::AngleBetween(Vector3 p1, Vector3 p2)
{
	p1.normalise();
	p2.normalise();
	Radian angle = acos(p1.dotProduct(p2) / sqrt(p1.squaredLength() * p2.squaredLength()));

	return angle.toDegree();
}

bool Ape::KinectPlugin::CheckDifference(float data, float ref, float perc)
{
	if (data > ref * (1 - perc) && data < ref * (1 + perc))
		return true;
	else
		return false;
}

bool Ape::KinectPlugin::CheckMaxDifference(float data, float ref, float maxdev)
{
	return (data > (ref - maxdev) && data < (ref + maxdev));
}

//Legacy
void Ape::KinectPlugin::GetOperatorColrs()
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

void Ape::KinectPlugin::GetOperatorPts()
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

float Ape::KinectPlugin::GetDistance(std::vector<float> joint, std::vector<float> point)
{
	return sqrtf(powf((point[0]-joint[0]),2.0)+ powf((point[1] - joint[1]), 2.0) + powf((point[2] - joint[2]), 2.0));
}

void Ape::KinectPlugin::GetRGBData(IMultiSourceFrame* pframe)
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

		Ape::PointCloudColors colorpoints;
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

void Ape::KinectPlugin::GetBodyIndexes(IMultiSourceFrame* pframe)
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

void  Ape::KinectPlugin::GetOperator()
{
	APE_LOG_FUNC_ENTER();
	Ape::PointCloudPoints OPoint;
	Ape::PointCloudColors OColor;

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

void Ape::KinectPlugin::GetDepthData(IMultiSourceFrame* pframe)
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
			Ape::PointCloudPoints Points;
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
void Ape::KinectPlugin::GetBodyData(IMultiSourceFrame* pframe)
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
void Ape::KinectPlugin::ProcessBody(int nBodyCount, IBody** ppBodies)
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
				else
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
