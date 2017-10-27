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
	
	//LARGE_INTEGER qpf = { 0 };
	//if (QueryPerformanceFrequency(&qpf))
	//{
	//	m_fFreq = double(qpf.QuadPart);
	//}

	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));
	/*mLeapController = Leap::Controller();*/
	/*mFingerNames = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
	mBoneNames = {"Metacarpal", "Proximal", "Middle", "Distal"};
	mStateNames = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};*/
	//mPreviousFramePitch = mPreviousFrameYaw = mPreviousFrameRoll = 0;
	//mHandOrientationFlag = false;
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

	//mLeapController.addListener(*this);
	//mLeapController.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
	//onFrame(mLeapController);

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

				//rightHandNode->setPosition(Ape::Vector3(0, 0, -100));
				//std::cout << headnode->getPosition().toString() << std::endl;
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
		//Leap::Frame frame;
		//frame = mLeapController.frame();
		//Leap::Hand rightMost = frame.hands().rightmost();
		//if (auto rightHandNode = mRightHandNode.lock())
		//{
		//	rightHandNode->setPosition(Ape::Vector3(rightMost.palmPosition().x / 10, rightMost.palmPosition().z / 10, -rightMost.palmPosition().y / 10));
		//	//rightHandNode->setPosition(Ape::Vector3(0, 0, -100));
		//	//std::cout << rightHandNode->getPosition().toString() << std::endl;
		//}
		//mPreviousFramePitch = rightMost.direction().pitch() * Leap::RAD_TO_DEG;
		//mPreviousFrameYaw = rightMost.direction().yaw() * Leap::RAD_TO_DEG;
		//mPreviousFrameRoll = rightMost.palmNormal().roll() * Leap::RAD_TO_DEG;
		//if (!frame.hands().isEmpty() && !mHandOrientationFlag)
		//{
		//	mHandOrientationFlag = true;
		//}
		//else if (mHandOrientationFlag && frame.hands().isEmpty())
		//{
		//	mHandOrientationFlag = false;
		//}
		//if (!frame.hands().isEmpty())
		//{
		//	Leap::Hand rightMost = frame.hands().rightmost();
		//	float pitchValue = rightMost.direction().pitch() * Leap::RAD_TO_DEG;
		//	float rollValue = rightMost.palmNormal().roll() * Leap::RAD_TO_DEG;
		//	float yawValue = rightMost.direction().yaw() * Leap::RAD_TO_DEG;
		//	mPreviousFramePitch = rightMost.direction().pitch() * Leap::RAD_TO_DEG;
		//	mPreviousFrameYaw = rightMost.direction().yaw() * Leap::RAD_TO_DEG;
		//	mPreviousFrameRoll = rightMost.palmNormal().roll() * Leap::RAD_TO_DEG;
		//	static Leap::FingerList fingers;
		//	fingers = rightMost.fingers();
		//	int i = 0;
		//	int index = 0;
		//	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
		//	{
		//		static Leap::Finger finger;
		//		finger = *fl;
		//		for (int b = 0; b < 4; ++b, ++index)
		//		{
		//			static Leap::Bone::Type boneType;
		//			boneType = static_cast<Leap::Bone::Type>(b);
		//			static Leap::Bone bone;
		//			bone = finger.bone(boneType);
		//		}
		//	}
		//}
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
				/*std::ostringstream strs;
				strs << body[3][2];
				std::string str = strs.str();
				std::cout << str << std::endl;*/
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

//void Ape::KinectPlugin::onInit(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onInit" << std::endl;
//}
//
//void Ape::KinectPlugin::onConnect(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onConnect" << std::endl;
//	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
//	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
//	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
//	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
//}
//
//void Ape::KinectPlugin::onDisconnect(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onDisconnect" << std::endl;
//}
//
//void Ape::KinectPlugin::onExit(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onExit" << std::endl;
//}
//
//void Ape::KinectPlugin::onFrame(const Leap::Controller& controller)
//{
//	//std::cout << "KinectPlugin::onFrame" << std::endl;
//	const Leap::Frame frame = controller.frame();
//	/*std::cout << "Frame id: " << frame.id() << ", timestamp: " << frame.timestamp() << ", hands: " << frame.hands().count() << ", fingers: " << frame.fingers().count()
//		<< ", tools: " << frame.tools().count()	<< ", gestures: " << frame.gestures().count() << std::endl;*/
//	Leap::HandList hands = frame.hands();
//	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
//	{
//		const Leap::Hand hand = *hl;
//		std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
//		/*std::cout << std::string(2, ' ') << handType << ", id: " << hand.id() << ", palm position: " << hand.palmPosition() << std::endl;*/
//		const Leap::Vector normal = hand.palmNormal();
//		const Leap::Vector direction = hand.direction();
//		/*std::cout << std::string(2, ' ') << "pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, " << "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
//			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees" << std::endl;*/
//		Leap::Arm arm = hand.arm();
//		/*std::cout << std::string(2, ' ') << "Arm direction: " << arm.direction() << " wrist position: " << arm.wristPosition() << " elbow position: " << arm.elbowPosition() << std::endl;*/
//		const Leap::FingerList fingers = hand.fingers();
//		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
//		{
//			const Leap::Finger finger = *fl;
//			/*std::cout << std::string(4, ' ') << mFingerNames[finger.type()] << " finger, id: " << finger.id() << ", length: " << finger.length() << "mm, width: " << finger.width() << std::endl;*/
//			for (int b = 0; b < 4; ++b) 
//			{
//				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
//				Leap::Bone bone = finger.bone(boneType);
//				/*std::cout << std::string(6, ' ') << mBoneNames[boneType] << " bone, start: " << bone.prevJoint() << ", end: " << bone.nextJoint() << ", direction: " << bone.direction() << std::endl;*/
//			}
//		}
//	}
//	const Leap::ToolList tools = frame.tools();
//	for (Leap::ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl)
//	{
//		const Leap::Tool tool = *tl;
//		/*std::cout << std::string(2, ' ') << "Tool, id: " << tool.id() << ", position: " << tool.tipPosition() << ", direction: " << tool.direction() << std::endl;*/
//	}
//	const Leap::GestureList gestures = frame.gestures();
//	for (int g = 0; g < gestures.count(); ++g) 
//	{
//		Leap::Gesture gesture = gestures[g];
//		switch (gesture.type()) {
//		case Leap::Gesture::TYPE_CIRCLE:
//		{
//			Leap::CircleGesture circle = gesture;
//			std::string clockwiseness;
//			if (circle.pointable().direction().angleTo(circle.normal()) <= Leap::PI / 2)
//				clockwiseness = "clockwise";
//			else 
//				clockwiseness = "counterclockwise";
//			float sweptAngle = 0;
//			if (circle.state() != Leap::Gesture::STATE_START)
//			{
//				Leap::CircleGesture previousUpdate = Leap::CircleGesture(controller.frame(1).gesture(circle.id()));
//				sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * Leap::PI;
//			}
//			/*std::cout << std::string(2, ' ') << "Circle id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", progress: " << circle.progress()
//				<< ", radius: " << circle.radius() << ", angle " << sweptAngle * Leap::RAD_TO_DEG << ", " << clockwiseness << std::endl;*/
//			break;
//		}
//		case Leap::Gesture::TYPE_SWIPE:
//		{
//			Leap::SwipeGesture swipe = gesture;
//			/*std::cout << std::string(2, ' ') << "Swipe id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", direction: " << swipe.direction()
//				<< ", speed: " << swipe.speed() << std::endl;*/
//			break;
//		}
//		case Leap::Gesture::TYPE_KEY_TAP:
//		{
//			Leap::KeyTapGesture tap = gesture;
//			/*std::cout << std::string(2, ' ') << "Key Tap id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", position: " << tap.position()
//				<< ", direction: " << tap.direction() << std::endl;*/
//			break;
//		}
//		case Leap::Gesture::TYPE_SCREEN_TAP:
//		{
//			Leap::ScreenTapGesture screentap = gesture;
//			/*std::cout << std::string(2, ' ') << "Screen Tap id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", position: " << screentap.position()
//				<< ", direction: " << screentap.direction() << std::endl;*/
//			break;
//		}
//		default:
//			/*std::cout << std::string(2, ' ') << "Unknown gesture type." << std::endl;*/
//			break;
//		}
//	}
//	if (!frame.hands().isEmpty() || !gestures.isEmpty())
//		;/*std::cout << std::endl;*/
//}
//
//void Ape::KinectPlugin::onFocusGained(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onFocusGained" << std::endl;
//}
//
//void Ape::KinectPlugin::onFocusLost(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onFocusLost" << std::endl;
//}
//
//void Ape::KinectPlugin::onDeviceChange(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onDeviceChange" << std::endl;
//	const Leap::DeviceList devices = controller.devices();
//	for (int i = 0; i < devices.count(); ++i) 
//	{
//		std::cout << "id: " << devices[i].toString() << std::endl;
//		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
//	}
//}

//void Ape::KinectPlugin::onServiceConnect(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onServiceConnect" << std::endl;
//}
//
//void Ape::KinectPlugin::onServiceDisconnect(const Leap::Controller& controller)
//{
//	std::cout << "KinectPlugin::onServiceDisconnect" << std::endl;
//}
