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

const int width = 512;
const int height = 424;
const int colorwidth = 1920;
const int colorheight = 1080;

unsigned char rgbimage[colorwidth*colorheight * 4];
CameraSpacePoint depth2xyz[width*height];
ColorSpacePoint depth2rgb[width*height];


Ape::KinectPlugin::KinectPlugin()
{
	m_fFreq = 0;
	m_pKinectSensor = NULL;
	m_pCoordinateMapper = NULL;
	//m_pBodyFrameReader=NULL;
	reader = NULL;


	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));
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

	std::cout << "KinectPlugin dtor" << std::endl;
}

void Ape::KinectPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName)
		mUserNode = mpScene->getNode(event.subjectName);
	else if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == (mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName + "_rightHandNode"))
		mRightHandNode = mpScene->getNode(event.subjectName);
	else if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == (mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName + "_leftHandNode"))
		mLeftHandNode = mpScene->getNode(event.subjectName);
}

void Ape::KinectPlugin::Init()
{
	std::cout << "KinectPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "KinectPlugin main window was found" << std::endl;
	InitializeDefaultSensor();
	std::cout << "Sensor init finished" << std::endl;

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
		//HeadNode = mNodes[0];

		/*if (auto headnode = mpScene->createNode("MyHeadNode").lock())
			HeadNode = headnode;*/

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

		if (auto _5BodyGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("5BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			_5BodyGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			_5BodyGeometry->setParentNode(_5Body[i]);
			_5BodyGeometry->setMaterial(_5bodyMaterial);
		}
	}
#pragma endregion

	if (auto pointCloudNode = mpScene->createNode("pointCloudNode").lock())
	{
		pointCloudNode->setPosition(Ape::Vector3(0, 0, 0));
		if (auto pointCloudNodeText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("pointCloudNodeText", Ape::Entity::GEOMETRY_TEXT).lock()))
		{
			pointCloudNodeText->setCaption("Points");
			pointCloudNodeText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
			pointCloudNodeText->setParentNode(pointCloudNode);
		}
		if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud", Ape::Entity::POINT_CLOUD).lock()))
		{
			Ape::PointCloudPoints points = {200, 0, 0};
			Ape::PointCloudColors colors = {1, 0, 0};

			pointCloud->setParameters(points, colors, 100000);
			pointCloud->setParentNode(pointCloudNode);
			//mPointCloud = pointCloud;
		}
		//pointCloudNode->setChildrenVisibility(true);
	}

	std::cout << "KinectPlugin Initialized" << std::endl;
}

void Ape::KinectPlugin::Run()
{
	while (true)
	{
		Update();
		
		if (!pointsGenerated && depth2xyz[1010].X != 0.0 && depth2xyz[1010].X != -1 * std::numeric_limits<float>::infinity())
		{
			//if (auto pointCloud = mPointCloud.lock())
			//{
			//		double pointratio = 1/6; //point count = 217088 * pointratio
			//		
			//		Ape::PointCloudPoints points;
			//		points.resize(651264 * pointratio);
			//		points = GetPointCloudPTS(depth2xyz, pointratio/*, getrRndIndexes(pointratio)*/);
			//		Ape::PointCloudColors colors;
			//		colors.resize(651264 * pointratio);
			//		colors = GetPointCloudCOL(rgbimage, pointratio);

			//		/*Ape::PointCloudPoints points = { 200, 0, 0 };
			//		Ape::PointCloudColors colors = { 0, 1, 0 };*/

			//		pointCloud->updatePoints(points);
			//		pointCloud->updateColors(colors);	
			//}
			if (auto pointCloudNode = mpScene->createNode("pointCloudNode_debug").lock())
			{
				pointCloudNode->setPosition(Ape::Vector3(0, 0, 0));
				if (auto pointCloudNodeText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("pointCloudNodeText_debug", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					pointCloudNodeText->setCaption("Points_debug");
					pointCloudNodeText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
					pointCloudNodeText->setParentNode(pointCloudNode);
				}
				if (auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(mpScene->createEntity("pointCloud_debug", Ape::Entity::POINT_CLOUD).lock()))
				{
					double pointratio = 1; //point count = 217088 * pointratio

					Ape::PointCloudPoints points = GetPointCloudPTS(depth2xyz, pointratio/*, getrRndIndexes(pointratio)*/);
					Ape::PointCloudColors colors = GetPointCloudCOL(rgbimage, pointratio);

					int num0 = 0;
					for (int i = 0; i<651264 * pointratio; i++)
					{
						if (points[i] == (float)0) num0++;
					}
					std::cout << std::to_string(num0) << std::endl;

					pointCloud->setParameters(points, colors, 100000);
					pointCloud->setParentNode(pointCloudNode);
					mPointCloud = pointCloud;

							std::ostringstream sx;
							sx << points.at(6000);
							std::string x(sx.str());
							std::ostringstream sy;
							sy << points.at(6001);
							std::string y(sy.str());
							std::ostringstream sz;
							sz << points.at(6002);
							std::string z(sz.str());
							std::cout << x+"; "+y+"; "+z << std::endl;
							
				}
			}
			pointsGenerated = true;
		}
		if (auto pointCloud = mPointCloud.lock())
		{
			double pointratio = 1;
			Ape::PointCloudPoints points = GetPointCloudPTS(depth2xyz, pointratio/*, getrRndIndexes(pointratio)*/);
			Ape::PointCloudColors colors = GetPointCloudCOL(rgbimage, pointratio);
			
			pointCloud->updatePoints(points);
			pointCloud->updateColors(colors);
		}

		//std::ostringstream sx;
		//sx << depth2xyz[1010].X*100;
		//std::string x(sx.str());

		//std::ostringstream sy;
		//sy << depth2xyz[1010].Y*100;
		//std::string y(sy.str());

		//std::ostringstream sz;
		//sz << depth2xyz[1010].Z*100;
		//std::string z(sz.str());

		//std::ostringstream sx;
		//sx << (float)rgbimage[1000];
		//std::string x(sx.str());

		//std::ostringstream sy;
		//sy << (float)rgbimage[1001];
		//std::string y(sy.str());

		//std::ostringstream sz;
		//sz << (float)rgbimage[1002];
		//std::string z(sz.str());

		//if (pointcGenerated)
		//{
		//	std::cout << "true; ";
		//}
		//std::cout << "r: " + x + " g: " + y + " b: " + z << std::endl;

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

		std::this_thread::sleep_for (std::chrono::milliseconds(20));
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
		// Initialize the Kinect and get coordinate mapper and the body reader
		//IBodyFrameSource* pBodyFrameSource = NULL;
		/*IMultiSourceFrame* pMultiFramesource = NULL;*/

		hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->Open();
		}

		if (SUCCEEDED(hr))
		{
			hr= m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_Body,
				&reader);
		}

		std::cout << "Kinect ready" << std::endl;
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout<<"No ready Kinect found!"<<std::endl;
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

	pFrame = NULL;
	//m_pDepthFrame = NULL;

	HRESULT hr = reader->AcquireLatestFrame(&pFrame);

	if (SUCCEEDED(hr))
	{
		GetBodyData(pFrame);
		GetDepthData(pFrame);
		GetRGBData(pFrame);

		pFrame->Release();
	}
}

void Ape::KinectPlugin::GetRGBData(IMultiSourceFrame* pframe)
{
	IColorFrame* pColorFrame;
	IColorFrameReference* pColorFrameRef = NULL;

	HRESULT hr = pframe->get_ColorFrameReference(&pColorFrameRef);

	if (SUCCEEDED(hr))
	{
		hr = pColorFrameRef->AcquireFrame(&pColorFrame);
	}

	if (SUCCEEDED(hr))
	{
		if (pColorFrameRef) pColorFrameRef->Release();
		
		// Get data from frame
		pColorFrame->CopyConvertedFrameDataToArray(colorwidth*colorheight * 4, rgbimage, ColorImageFormat_Rgba);	
	}

	if (pColorFrame) pColorFrame->Release();
}

Ape::PointCloudColors Ape::KinectPlugin::GetPointCloudCOL(unsigned char cimage[], double ratio)
{
	int size = 651264;
	int modSize = (int)(size*ratio);
	if (modSize % 3 != 0)
	{
		modSize -= modSize % 3;
	}
	Ape::PointCloudColors colorpoints(modSize);

	for (int i = 0; i < modSize/3; i++) {
		ColorSpacePoint p = depth2rgb[i];
		/* Check if color pixel coordinates are in bounds*/
		if (p.X < 0 || p.Y < 0 || p.X > colorwidth || p.Y > colorheight) {
			colorpoints.push_back(0.);
			colorpoints.push_back(0.);
			colorpoints.push_back(0.);
		}
		else {
			int idx = (int)p.X + colorwidth*(int)p.Y;
			colorpoints.push_back((float)cimage[4 * idx + 0] / 255.);
			colorpoints.push_back((float)cimage[4 * idx + 1] / 255.);
			colorpoints.push_back((float)cimage[4 * idx + 2] / 255.);
		}
	}
	return colorpoints;
}

void Ape::KinectPlugin::GetDepthData(IMultiSourceFrame* pframe)
{
	IDepthFrame* pDepthframe;
	IDepthFrameReference* pDepthFrameRef = NULL;

	HRESULT hr = pframe->get_DepthFrameReference(&pDepthFrameRef);

	if (SUCCEEDED(hr))
	{
		hr = pDepthFrameRef->AcquireFrame(&pDepthframe);
	}

	if (SUCCEEDED(hr))
	{
		if (pDepthFrameRef) pDepthFrameRef->Release();

		// Get data from frame
		unsigned int size;
		unsigned short* buf;
		hr = pDepthframe->AccessUnderlyingBuffer(&size, &buf);

		if (SUCCEEDED(hr))
		{
			hr = m_pCoordinateMapper->MapDepthFrameToCameraSpace(width*height, buf, width*height, depth2xyz);
		}
	m_pCoordinateMapper->MapDepthFrameToColorSpace(width*height, buf, width*height, depth2rgb);
	}
	if (pDepthframe) pDepthframe->Release();
}

std::vector<int> Ape::KinectPlugin::getrRndIndexes(double ratio)
{
	int size = 217088;
	int maxR = (int)(size*ratio);
	if (maxR % 3 != 0)
	{
		maxR -= maxR % 3;
	}
	std::vector<int> indexes(maxR);
	for (int i = 0; i < maxR; i++)
	{
		int temp = rand()%static_cast<int>(maxR);

		bool cont = false;
		for (int i = 0; i <maxR; i++)
		{
			if (indexes[i] == temp)
			{
				cont = true;
			}
		}

		if (!cont)
		{
			indexes.push_back(temp);
		}
	}
	return indexes;
}

Ape::PointCloudPoints Ape::KinectPlugin::GetPointCloudPTS(CameraSpacePoint depthPts[], double ratio/*, std::vector<int> indexes*/)
{
	int size = 651264;
	int modSize = (int)(size*ratio);
	if (modSize%3!=0)
	{
		modSize -= modSize % 3;
	}
	Ape::PointCloudPoints point(modSize);
	for (int i = 0; i < modSize/3; i++)
	{
		//if (ratio>0 && ratio<1)
		//{
		//	point.push_back(depthPts[indexes[i]].X * 100);
		//	point.push_back(depthPts[indexes[i]].Y * 100);
		//	point.push_back(depthPts[indexes[i]].Z * 100);
		//}
		//if (ratio >= 1)
		//{
			point.push_back(depthPts[i].X * 100);
			point.push_back(depthPts[i].Y * 100);
			point.push_back(depthPts[i].Z * 100);
		//}
	}
	return point;
}

///<summary>
///Gets body frame from multiframe reader
///</summary>
void Ape::KinectPlugin::GetBodyData(IMultiSourceFrame* pframe)
{
	IBodyFrame* pBodyFrame;
	IBodyFrameReference* pBodyFrameRef = NULL;

	HRESULT hr = pframe->get_BodyFrameReference(&pBodyFrameRef);

	if (SUCCEEDED(hr))
	{
		hr = pBodyFrameRef->AcquireFrame(&pBodyFrame);
		if (pBodyFrameRef) pBodyFrameRef->Release();
	}

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);

		IBody* ppBodies[BODY_COUNT] = { 0 };

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			ProcessBody(nTime, BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}
	if (pBodyFrame) pBodyFrame->Release();
}

/// <summary>
/// Handle new body data
/// <param name="nTime">timestamp of frame</param>
/// <param name="nBodyCount">body data count</param>
/// <param name="ppBodies">body data in frame</param>
/// </summary>
void Ape::KinectPlugin::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
	HRESULT hr;
	if (m_pCoordinateMapper)
	{
		//DOUBLE dist = 0.0;
		
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

					if (SUCCEEDED(hr)/* && (joints[0].Position.Z<dist || dist==0.0)*/)
					{
						//dist = joints[0].Position.Z;
						
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