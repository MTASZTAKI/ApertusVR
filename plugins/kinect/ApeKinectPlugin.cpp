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


Ape::KinectPlugin::KinectPlugin()
{
	m_fFreq = 0;
	m_pKinectSensor = NULL;
	m_pCoordinateMapper = NULL;
	m_pBodyFrameReader=NULL;


	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));
}

Ape::KinectPlugin::~KinectPlugin()
{
	// done with body frame reader
	SafeRelease(m_pBodyFrameReader);

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
	std::cout << "KinectPlugin::Init" << std::endl;
	std::cout << "KinectPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "KinectPlugin main window was found" << std::endl;
	std::cout << "test" << std::endl;
	InitializeDefaultSensor();
	std::cout << "finished" << std::endl;

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

		if (auto leftHandGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("0BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			leftHandGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			leftHandGeometry->setParentNode(_0Body[i]);
			leftHandGeometry->setMaterial(_0bodyMaterial);
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

		if (auto leftHandGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("1BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			leftHandGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			leftHandGeometry->setParentNode(_1Body[i]);
			leftHandGeometry->setMaterial(_1bodyMaterial);
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

		if (auto leftHandGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("2BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			leftHandGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			leftHandGeometry->setParentNode(_2Body[i]);
			leftHandGeometry->setMaterial(_2bodyMaterial);
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

		if (auto leftHandGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("3BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			leftHandGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			leftHandGeometry->setParentNode(_3Body[i]);
			leftHandGeometry->setMaterial(_3bodyMaterial);
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

		if (auto leftHandGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("4BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			leftHandGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			leftHandGeometry->setParentNode(_4Body[i]);
			leftHandGeometry->setMaterial(_4bodyMaterial);
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

		if (auto leftHandGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpScene->createEntity("5BodyGeometry" + index, Ape::Entity::GEOMETRY_SPHERE).lock()))
		{
			leftHandGeometry->setParameters(2.0f, Ape::Vector2(1, 1));
			leftHandGeometry->setParentNode(_5Body[i]);
			leftHandGeometry->setMaterial(_5bodyMaterial);
		}
	}
}

void Ape::KinectPlugin::Run()
{
	while (true)
	{
		Update();

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
		IBodyFrameSource* pBodyFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		SafeRelease(pBodyFrameSource);
		std::cout << "Kinect init compleate" << std::endl;
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
	if (!m_pBodyFrameReader)
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;

	HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

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

	SafeRelease(pBodyFrame);
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