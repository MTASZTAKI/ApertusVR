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
#include "ApeKinectPlugin.h"
#include "resource.h"


Ape::KinectPlugin::KinectPlugin()
{
	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&KinectPlugin::eventCallBack, this, std::placeholders::_1));
	mLeapController = Leap::Controller();
	mFingerNames = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
	mBoneNames = {"Metacarpal", "Proximal", "Middle", "Distal"};
	mStateNames = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};
	mPreviousFramePitch = mPreviousFrameYaw = mPreviousFrameRoll = 0;
	mHandOrientationFlag = false;
}

Ape::KinectPlugin::~KinectPlugin()
{
	std::cout << "LeapMotionPlugin dtor" << std::endl;
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
	std::cout << "LeapMotionPlugin::Init" << std::endl;
	std::cout << "LeapMotionPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "LeapMotionPlugin main window was found" << std::endl;
	mLeapController.addListener(*this);
	mLeapController.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
	onFrame(mLeapController);
}

void Ape::KinectPlugin::Run()
{
	while (true)
	{
		Leap::Frame frame;
		frame = mLeapController.frame();
		Leap::Hand rightMost = frame.hands().rightmost();
		if (auto rightHandNode = mRightHandNode.lock())
		{
			rightHandNode->setPosition(Ape::Vector3(rightMost.palmPosition().x / 10, rightMost.palmPosition().z / 10, -rightMost.palmPosition().y / 10));
			//rightHandNode->setPosition(Ape::Vector3(0, 0, -100));
			//std::cout << rightHandNode->getPosition().toString() << std::endl;
		}
		mPreviousFramePitch = rightMost.direction().pitch() * Leap::RAD_TO_DEG;
		mPreviousFrameYaw = rightMost.direction().yaw() * Leap::RAD_TO_DEG;
		mPreviousFrameRoll = rightMost.palmNormal().roll() * Leap::RAD_TO_DEG;
		if (!frame.hands().isEmpty() && !mHandOrientationFlag)
		{
			mHandOrientationFlag = true;
		}
		else if (mHandOrientationFlag && frame.hands().isEmpty())
		{
			mHandOrientationFlag = false;
		}
		if (!frame.hands().isEmpty())
		{
			Leap::Hand rightMost = frame.hands().rightmost();
			float pitchValue = rightMost.direction().pitch() * Leap::RAD_TO_DEG;
			float rollValue = rightMost.palmNormal().roll() * Leap::RAD_TO_DEG;
			float yawValue = rightMost.direction().yaw() * Leap::RAD_TO_DEG;
			mPreviousFramePitch = rightMost.direction().pitch() * Leap::RAD_TO_DEG;
			mPreviousFrameYaw = rightMost.direction().yaw() * Leap::RAD_TO_DEG;
			mPreviousFrameRoll = rightMost.palmNormal().roll() * Leap::RAD_TO_DEG;
			static Leap::FingerList fingers;
			fingers = rightMost.fingers();
			int i = 0;
			int index = 0;
			for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
			{
				static Leap::Finger finger;
				finger = *fl;
				for (int b = 0; b < 4; ++b, ++index)
				{
					static Leap::Bone::Type boneType;
					boneType = static_cast<Leap::Bone::Type>(b);
					static Leap::Bone bone;
					bone = finger.bone(boneType);
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

void Ape::KinectPlugin::onInit(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onInit" << std::endl;
}

void Ape::KinectPlugin::onConnect(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onConnect" << std::endl;
	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
}

void Ape::KinectPlugin::onDisconnect(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onDisconnect" << std::endl;
}

void Ape::KinectPlugin::onExit(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onExit" << std::endl;
}

void Ape::KinectPlugin::onFrame(const Leap::Controller& controller)
{
	//std::cout << "LeapMotionPlugin::onFrame" << std::endl;
	const Leap::Frame frame = controller.frame();
	/*std::cout << "Frame id: " << frame.id() << ", timestamp: " << frame.timestamp() << ", hands: " << frame.hands().count() << ", fingers: " << frame.fingers().count()
		<< ", tools: " << frame.tools().count()	<< ", gestures: " << frame.gestures().count() << std::endl;*/
	Leap::HandList hands = frame.hands();
	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
	{
		const Leap::Hand hand = *hl;
		std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
		/*std::cout << std::string(2, ' ') << handType << ", id: " << hand.id() << ", palm position: " << hand.palmPosition() << std::endl;*/
		const Leap::Vector normal = hand.palmNormal();
		const Leap::Vector direction = hand.direction();
		/*std::cout << std::string(2, ' ') << "pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, " << "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees" << std::endl;*/
		Leap::Arm arm = hand.arm();
		/*std::cout << std::string(2, ' ') << "Arm direction: " << arm.direction() << " wrist position: " << arm.wristPosition() << " elbow position: " << arm.elbowPosition() << std::endl;*/
		const Leap::FingerList fingers = hand.fingers();
		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
		{
			const Leap::Finger finger = *fl;
			/*std::cout << std::string(4, ' ') << mFingerNames[finger.type()] << " finger, id: " << finger.id() << ", length: " << finger.length() << "mm, width: " << finger.width() << std::endl;*/
			for (int b = 0; b < 4; ++b) 
			{
				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
				Leap::Bone bone = finger.bone(boneType);
				/*std::cout << std::string(6, ' ') << mBoneNames[boneType] << " bone, start: " << bone.prevJoint() << ", end: " << bone.nextJoint() << ", direction: " << bone.direction() << std::endl;*/
			}
		}
	}
	const Leap::ToolList tools = frame.tools();
	for (Leap::ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl)
	{
		const Leap::Tool tool = *tl;
		/*std::cout << std::string(2, ' ') << "Tool, id: " << tool.id() << ", position: " << tool.tipPosition() << ", direction: " << tool.direction() << std::endl;*/
	}
	const Leap::GestureList gestures = frame.gestures();
	for (int g = 0; g < gestures.count(); ++g) 
	{
		Leap::Gesture gesture = gestures[g];
		switch (gesture.type()) {
		case Leap::Gesture::TYPE_CIRCLE:
		{
			Leap::CircleGesture circle = gesture;
			std::string clockwiseness;
			if (circle.pointable().direction().angleTo(circle.normal()) <= Leap::PI / 2)
				clockwiseness = "clockwise";
			else 
				clockwiseness = "counterclockwise";
			float sweptAngle = 0;
			if (circle.state() != Leap::Gesture::STATE_START)
			{
				Leap::CircleGesture previousUpdate = Leap::CircleGesture(controller.frame(1).gesture(circle.id()));
				sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * Leap::PI;
			}
			/*std::cout << std::string(2, ' ') << "Circle id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", progress: " << circle.progress()
				<< ", radius: " << circle.radius() << ", angle " << sweptAngle * Leap::RAD_TO_DEG << ", " << clockwiseness << std::endl;*/
			break;
		}
		case Leap::Gesture::TYPE_SWIPE:
		{
			Leap::SwipeGesture swipe = gesture;
			/*std::cout << std::string(2, ' ') << "Swipe id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", direction: " << swipe.direction()
				<< ", speed: " << swipe.speed() << std::endl;*/
			break;
		}
		case Leap::Gesture::TYPE_KEY_TAP:
		{
			Leap::KeyTapGesture tap = gesture;
			/*std::cout << std::string(2, ' ') << "Key Tap id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", position: " << tap.position()
				<< ", direction: " << tap.direction() << std::endl;*/
			break;
		}
		case Leap::Gesture::TYPE_SCREEN_TAP:
		{
			Leap::ScreenTapGesture screentap = gesture;
			/*std::cout << std::string(2, ' ') << "Screen Tap id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", position: " << screentap.position()
				<< ", direction: " << screentap.direction() << std::endl;*/
			break;
		}
		default:
			/*std::cout << std::string(2, ' ') << "Unknown gesture type." << std::endl;*/
			break;
		}
	}
	if (!frame.hands().isEmpty() || !gestures.isEmpty())
		;/*std::cout << std::endl;*/
}

void Ape::KinectPlugin::onFocusGained(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onFocusGained" << std::endl;
}

void Ape::KinectPlugin::onFocusLost(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onFocusLost" << std::endl;
}

void Ape::KinectPlugin::onDeviceChange(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onDeviceChange" << std::endl;
	const Leap::DeviceList devices = controller.devices();
	for (int i = 0; i < devices.count(); ++i) 
	{
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
	}
}

void Ape::KinectPlugin::onServiceConnect(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onServiceConnect" << std::endl;
}

void Ape::KinectPlugin::onServiceDisconnect(const Leap::Controller& controller)
{
	std::cout << "LeapMotionPlugin::onServiceDisconnect" << std::endl;
}
