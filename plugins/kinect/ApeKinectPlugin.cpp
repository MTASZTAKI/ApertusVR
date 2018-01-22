/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <fstream>
#include "stdafx.h"
#include <strsafe.h>
#include "resource.h"
#include "ApeKinectPlugin.h"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

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
	m_pKinectSensor = NULL;
	m_pCoordinateMapper = NULL;
	reader = NULL;


	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));
	RootNode = mpScene->createNode("KinectRootNode").lock();
}

Ape::KinectPlugin::~KinectPlugin()
{
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

	std::cout << "KinectPlugin destroyed" << std::endl;
}

void Ape::KinectPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::KinectPlugin::Init()
{
	std::cout << "KinectPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "KinectPlugin main window was found" << std::endl;
	InitializeDefaultSensor();
	std::cout << "Sensor init finished" << std::endl;

	CloudSize = (int)(size*pointratio);
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
	FILE* KinectPluginConfigFile = std::fopen(kinectPluginConfigFilePath.str().c_str(), "r");
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
			}
			rapidjson::Value& KOrientation = jsonDocument["sensorOrientation"];
			for (int i = 0; i < 4; i++)
			{
				KRot[i] = jsonDocument["sensorOrientation"].GetArray()[i].GetFloat();
			}
		}
		fclose(KinectPluginConfigFile);
	}

	if (auto rootNode = RootNode.lock())
	{
		rootNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
		rootNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
	}

#pragma region initBodies
	std::shared_ptr<Ape::IManualMaterial> _0bodyMaterial;
	if (_0bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("0BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto _0bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("0BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
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

		if (auto myNode = mpScene->createNode("0BodyNode" + index).lock())
		{
			_0Body.push_back(myNode);
		}

		if (auto childNode = _0Body[i].lock())
		{
			childNode->setParentNode(RootNode);
		}

		if (auto _0BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("0BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			_0BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			_0BodyGeometry->setParentNode(_0Body[i]);
			_0BodyGeometry->setMaterial(_0bodyMaterial);
		}
	}

	std::shared_ptr<Ape::IManualMaterial> _1bodyMaterial;
	if (_1bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("1BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto _1bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("1BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
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

		if (auto myNode = mpScene->createNode("1BodyNode" + index).lock())
		{
			_1Body.push_back(myNode);
		}

		if (auto childNode = _1Body[i].lock())
		{
			childNode->setParentNode(RootNode);
		}

		if (auto _1BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("1BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			_1BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			_1BodyGeometry->setParentNode(_1Body[i]);
			_1BodyGeometry->setMaterial(_1bodyMaterial);
		}
	}

	std::shared_ptr<Ape::IManualMaterial> _2bodyMaterial;
	if (_2bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("2BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto _2bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("2BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
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

		if (auto myNode = mpScene->createNode("2BodyNode" + index).lock())
		{
			_2Body.push_back(myNode);
		}

		if (auto childNode = _2Body[i].lock())
		{
			childNode->setParentNode(RootNode);
		}

		if (auto _2BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("2BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			_2BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			_2BodyGeometry->setParentNode(_2Body[i]);
			_2BodyGeometry->setMaterial(_2bodyMaterial);
		}
	}

	std::shared_ptr<Ape::IManualMaterial> _3bodyMaterial;
	if (_3bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("3BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto _3bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("3BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
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

		if (auto myNode = mpScene->createNode("3BodyNode" + index).lock())
		{
			_3Body.push_back(myNode);
		}

		if (auto childNode = _3Body[i].lock())
		{
			childNode->setParentNode(RootNode);
		}

		if (auto _3BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("3BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			_3BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			_3BodyGeometry->setParentNode(_3Body[i]);
			_3BodyGeometry->setMaterial(_3bodyMaterial);
		}
	}

	std::shared_ptr<Ape::IManualMaterial> _4bodyMaterial;
	if (_4bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("4BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto _4bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("4BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
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

		if (auto myNode = mpScene->createNode("4BodyNode" + index).lock())
		{
			_4Body.push_back(myNode);
		}

		if (auto childNode = _4Body[i].lock())
		{
			childNode->setParentNode(RootNode);
		}

		if (auto _4BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("4BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			_4BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			_4BodyGeometry->setParentNode(_4Body[i]);
			_4BodyGeometry->setMaterial(_4bodyMaterial);
		}
	}

	std::shared_ptr<Ape::IManualMaterial> _5bodyMaterial;
	if (_5bodyMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("5BodyNodeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto _5bodyMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("5BodyMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
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

		if (auto myNode = mpScene->createNode("5BodyNode" + index).lock())
		{
			_5Body.push_back(myNode);
		}

		if (auto childNode = _5Body[i].lock())
		{
			childNode->setParentNode(RootNode);
		}

		if (auto _5BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("5BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			_5BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			_5BodyGeometry->setParentNode(_5Body[i]);
			_5BodyGeometry->setMaterial(_5bodyMaterial);
		}
	}
#pragma endregion

	std::cout << "KinectPlugin Initialized" << std::endl;
}

void Ape::KinectPlugin::Run()
{
	while (true)
	{
		Update();

#ifdef operatortest
		//Generate the Point Cloud
		if (!pointsGenerated && KPts[3030] != 0.0 && KPts[3030] != -1 * std::numeric_limits<float>::infinity())
		{
			if (auto pointCloudNode = mpScene->createNode("pointCloudNode_Kinect").lock())
			{
				pointCloudNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
				pointCloudNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
				if (auto pointCloudNodeText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("pointCloudNodeText_Kinect", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					pointCloudNodeText->setCaption("Points_Kinect");
					pointCloudNodeText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
					pointCloudNodeText->setParentNode(pointCloudNode);
				}
				if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud_Kinect", Ape::Entity::POINT_CLOUD).lock()))
				{
					pointCloud->setParameters(KPts, KCol, 100000);
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
#endif // operatortest

		//Draw body joints
#pragma region DrawBody
		for (int i = 0; i < 25; i++)
		{
			if (auto bodynode = _0Body[i].lock())
			{
				if (body[0] != NULL)
				{
					if (body[0][i][0]==0 && body[0][i][1]==0 && body[0][i][2]==0)
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
#pragma endregion

		if (_1Detected && !operatorPointsGenerated)
		{
			if (auto pointCloudNode = mpScene->createNode("pointCloudNode_KinectOperator").lock())
			{
				pointCloudNode->setPosition(Ape::Vector3(KPos[0], KPos[1], KPos[2]));
				pointCloudNode->setOrientation(Ape::Quaternion(KRot[0], KRot[1], KRot[2], KRot[3]));
				if (auto pointCloudNodeText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("pointCloudNodeText_KinectOperator", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					pointCloudNodeText->setCaption("Points_KinectOperator");
					pointCloudNodeText->setOffset(Ape::Vector3(0.0f, -1.0f, 0.0f));
					pointCloudNodeText->setParentNode(pointCloudNode);
				}
				if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud_KinectOperator", Ape::Entity::POINT_CLOUD).lock()))
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

		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));
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
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
		std::cout << "No ready Kinect error" << std::endl;
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

		std::cout << "Kinect ready" << std::endl;
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout << "No ready Kinect found!" << std::endl;
		return E_FAIL;
	}

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
		//std::cout << "update" << std::endl;

	if (SUCCEEDED(hr))
	{
/*		indexes.clear();*/
		if (framecount%2==0) //OPT GetBodyIndexes and GetDepthData can only work with different frames //needs too much resources?? 
		{
		GetDepthData(pFrame);
		GetBodyData(pFrame);
		GetRGBData(pFrame);
		}
		else
		{
		GetBodyIndexes(pFrame);
		}

		//GetOperatorPts();
		//GetOperatorColrs();
	}

	SafeRelease(pFrame);
	framecount++;
}

void Ape::KinectPlugin::GetOperatorColrs()
{
	OperatorColors.clear();

	if (indexes.size() > (UINT64)1)
	{
		std::cout << "color start" << std::endl;
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
	std::cout << "color finished" << std::endl;
}

void Ape::KinectPlugin::GetOperatorPts()
{
	OperatorPoints.clear();
	_1Detected = false;

	for (int n = 0; n < BODY_COUNT; n++)
	{
		if (body[n][0][0] != 0 && body[n][0][1] != 0 && body[n][0][2] != 0 && !_1Detected)
		{
			_1Detected = true;
			std::cout << "detected" << std::endl;
			for (int i = 0; i < CloudSize / 3; i++)
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
			std::cout << "finished" << std::endl;
		}
	}

	if (!_1Detected)
		OperatorPoints = {0.0, 0.0, 0.0};
}

float  Ape::KinectPlugin::GetDistance(std::vector<float> joint, std::vector<float> point)
{
	float dist = sqrt(pow((point[0]-joint[0]),2.0)+ pow((point[1] - joint[1]), 2.0) + pow((point[2] - joint[2]), 2.0));

	return dist;
}

void Ape::KinectPlugin::GetRGBData(IMultiSourceFrame* pframe)
{
	//std::cout << "rgb" << std::endl;
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
		//std::cout << "rgb got" << std::endl;
		
		// Get data from frame
		pColorFrame->CopyConvertedFrameDataToArray(colorwidth*colorheight * 4, rgbimage, ColorImageFormat_Rgba);

		Ape::PointCloudColors colorpoints;

		for (int i = 0; i < CloudSize / 3; i++) 
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
				colorpoints.push_back((float)rgbimage[4 * idx + 0] / 255.);
				colorpoints.push_back((float)rgbimage[4 * idx + 1] / 255.);
				colorpoints.push_back((float)rgbimage[4 * idx + 2] / 255.);
			}
		}

		KCol = colorpoints;
	}
	SafeRelease(pColorFrame);
}

void Ape::KinectPlugin::GetBodyIndexes(IMultiSourceFrame* pframe)
{
	//std::cout << "idx" << std::endl;
	IBodyIndexFrame* pIndexFrame = NULL;
	IBodyIndexFrameReference* pIndexFrameRef = NULL;

	HRESULT hr = pframe->get_BodyIndexFrameReference(&pIndexFrameRef);

	if (SUCCEEDED(hr))
	{
		//std::cout << "ref found" << std::endl;
		hr = pIndexFrameRef->AcquireFrame(&pIndexFrame);
	}
	//std::cout << "ref found gd" << std::endl;
		SafeRelease(pIndexFrameRef);
	if (SUCCEEDED(hr))
	{
		//std::cout << "idx got" << std::endl;
		
		hr = pIndexFrame->CopyFrameDataToArray(width*height, bodyIdx);
		
		if (SUCCEEDED(hr))
		{
			//std::cout << "success" << std::endl;
			GetOperator();
		}
	}
	SafeRelease(pIndexFrame);
}

void  Ape::KinectPlugin::GetOperator()
{
	Ape::PointCloudPoints OPoint;
	Ape::PointCloudColors OColor;
	int j = 0;

	//std::cout << std::to_string(bodyIdx[50000]) + "; " + std::to_string(bodyIdx[100000]) + "; " + std::to_string(bodyIdx[150000]) << std::endl;
	for (int i = 0; i < CloudSize/3; i++)
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
			j++;
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

}

void Ape::KinectPlugin::GetDepthData(IMultiSourceFrame* pframe)
{
	//std::cout << "depth" << std::endl;
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
		//std::cout << "depth got" << std::endl;

		// Get data from frame
		unsigned int bsize;
		unsigned short* buf;
		hr = pDepthframe->AccessUnderlyingBuffer(&bsize, &buf);

		if (SUCCEEDED(hr))
		{
			hr = m_pCoordinateMapper->MapDepthFrameToCameraSpace(width*height, buf, width*height, depth2xyz);
			Ape::PointCloudPoints Points;
			for (int i = 0; i < CloudSize / 3; i++)
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
}

///<summary>
///Gets body frame from multiframe reader
///</summary>
void Ape::KinectPlugin::GetBodyData(IMultiSourceFrame* pframe)
{
	//std::cout << "body" << std::endl;
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
		//std::cout << "body got" << std::endl;
		IBody* ppBodies[BODY_COUNT] = { 0 };

		hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);

		if (SUCCEEDED(hr))
		{
			ProcessBody(BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}
	SafeRelease(pBodyFrame);
}

/// <summary>
/// Handle new body data
/// <param name="nTime">timestamp of frame</param>
/// <param name="nBodyCount">body data count</param>
/// <param name="ppBodies">body data in frame</param>
/// </summary>
void Ape::KinectPlugin::ProcessBody(int nBodyCount, IBody** ppBodies)
{
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

					/*pBody->get_HandLeftState(&leftHandState);
					pBody->get_HandRightState(&rightHandState);*/

					hr = pBody->GetJoints(_countof(joints), joints);

					if (SUCCEEDED(hr))
					{
						
						for (int j = 0; j < _countof(joints); ++j)
						{
							if (joints[j].TrackingState==2)
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
		}
		for (int i = 0; i < nBodyCount; i++)
		{
			if (!Operatorfound[i])
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