#include <fstream>
#include "ApeLeapMotionPlugin.h"

Ape::LeapMotionPlugin::LeapMotionPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&LeapMotionPlugin::eventCallBack, this, std::placeholders::_1));
	mLeapController = Leap::Controller();
	mFingerNames = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
	mBoneNames = {"Metacarpal", "Proximal", "Middle", "Distal"};
	mStateNames = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};
	mPreviousFramePitch = mPreviousFrameYaw = mPreviousFrameRoll = 0;
	mHandOrientationFlag = false;
	APE_LOG_FUNC_LEAVE();
}

Ape::LeapMotionPlugin::~LeapMotionPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&LeapMotionPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::LeapMotionPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mLeapController.addListener(*this);
	mLeapController.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
	onFrame(mLeapController);
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		Leap::Frame frame;
		frame = mLeapController.frame();
		Leap::Hand rightMost = frame.hands().rightmost();
		if (auto rightHandNode = mRightHandNode.lock())
		{
			rightHandNode->setPosition(Ape::Vector3(rightMost.palmPosition().x / 10, rightMost.palmPosition().z / 10, -rightMost.palmPosition().y / 10));
			//rightHandNode->setPosition(Ape::Vector3(0, 0, -100));
			//APE_LOG_DEBUG(rightHandNode->getPosition().toString());
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
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onInit(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onConnect(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onDisconnect(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onExit(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onFrame(const Leap::Controller& controller)
{
	//APE_LOG_DEBUG("LeapMotionPlugin::onFrame");
	const Leap::Frame frame = controller.frame();
	/*APE_LOG_DEBUG("Frame id: " << frame.id() << ", timestamp: " << frame.timestamp() << ", hands: " << frame.hands().count() << ", fingers: " << frame.fingers().count()
		<< ", tools: " << frame.tools().count()	<< ", gestures: " << frame.gestures().count());*/
	Leap::HandList hands = frame.hands();
	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
	{
		const Leap::Hand hand = *hl;
		std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
		/*APE_LOG_DEBUG(std::string(2, ' ') << handType << ", id: " << hand.id() << ", palm position: " << hand.palmPosition());*/
		const Leap::Vector normal = hand.palmNormal();
		const Leap::Vector direction = hand.direction();
		/*APE_LOG_DEBUG(std::string(2, ' ') << "pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, " << "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees");*/
		Leap::Arm arm = hand.arm();
		/*APE_LOG_DEBUG(std::string(2, ' ') << "Arm direction: " << arm.direction() << " wrist position: " << arm.wristPosition() << " elbow position: " << arm.elbowPosition());*/
		const Leap::FingerList fingers = hand.fingers();
		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
		{
			const Leap::Finger finger = *fl;
			/*APE_LOG_DEBUG(std::string(4, ' ') << mFingerNames[finger.type()] << " finger, id: " << finger.id() << ", length: " << finger.length() << "mm, width: " << finger.width());*/
			for (int b = 0; b < 4; ++b) 
			{
				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
				Leap::Bone bone = finger.bone(boneType);
				/*APE_LOG_DEBUG(std::string(6, ' ') << mBoneNames[boneType] << " bone, start: " << bone.prevJoint() << ", end: " << bone.nextJoint() << ", direction: " << bone.direction());*/
			}
		}
	}
	const Leap::ToolList tools = frame.tools();
	for (Leap::ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl)
	{
		const Leap::Tool tool = *tl;
		/*APE_LOG_DEBUG(std::string(2, ' ') << "Tool, id: " << tool.id() << ", position: " << tool.tipPosition() << ", direction: " << tool.direction() <<);*/
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
			/*APE_LOG_DEBUG(std::string(2, ' ') << "Circle id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", progress: " << circle.progress()
				<< ", radius: " << circle.radius() << ", angle " << sweptAngle * Leap::RAD_TO_DEG << ", " << clockwiseness);*/
			break;
		}
		case Leap::Gesture::TYPE_SWIPE:
		{
			Leap::SwipeGesture swipe = gesture;
			/*APE_LOG_DEBUG(std::string(2, ' ') << "Swipe id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", direction: " << swipe.direction()
				<< ", speed: " << swipe.speed());*/
			break;
		}
		case Leap::Gesture::TYPE_KEY_TAP:
		{
			Leap::KeyTapGesture tap = gesture;
			/*APE_LOG_DEBUG(std::string(2, ' ') << "Key Tap id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", position: " << tap.position()
				<< ", direction: " << tap.direction());*/
			break;
		}
		case Leap::Gesture::TYPE_SCREEN_TAP:
		{
			Leap::ScreenTapGesture screentap = gesture;
			/*APE_LOG_DEBUG(std::string(2, ' ') << "Screen Tap id: " << gesture.id() << ", state: " << mStateNames[gesture.state()] << ", position: " << screentap.position()
				<< ", direction: " << screentap.direction());*/
			break;
		}
		default:
			/*APE_LOG_DEBUG(std::string(2, ' ') << "Unknown gesture type.");*/
			break;
		}
	}
	//if (!frame.hands().isEmpty() || !gestures.isEmpty())
	//{
		//APE_LOG_DEBUG("");
	//}
}

void Ape::LeapMotionPlugin::onFocusGained(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onFocusLost(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onDeviceChange(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	const Leap::DeviceList devices = controller.devices();
	for (int i = 0; i < devices.count(); ++i) 
	{
		APE_LOG_DEBUG("id: " << devices[i].toString());
		APE_LOG_DEBUG("  isStreaming: " << (devices[i].isStreaming() ? "true" : "false"));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onServiceConnect(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::LeapMotionPlugin::onServiceDisconnect(const Leap::Controller& controller)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
