#include <iostream>
#include "ApePresentationScenePlugin.h"

ApePresentationScenePlugin::ApePresentationScenePlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mOldXMLFormatTranslateVector = Ape::Vector3(26.809, 637.943, -22.163);
	mOldXMLFormatTranslateVectorCamera = Ape::Vector3(0, 637.943, 0);
	mOldXMLFormatRotationQuaternion = Ape::Quaternion();
	Ape::Radian angle(1.57f);
	Ape::Vector3 axis(0, 0, 1);
	Ape::Quaternion orientation;
	orientation.FromAngleAxis(angle, axis);
	angle = -1.57f;
	axis = Ape::Vector3(1, 0, 0);
	Ape::Quaternion orientation2;
	orientation2.FromAngleAxis(angle, axis);
	mOldXMLFormatRotationQuaternion = orientation * orientation2;
	mKeyCodeMap = std::map<OIS::KeyCode, bool>();
	mTranslateSpeedFactor = 3;
	mRotateSpeedFactor = 1;
	mCurrentStoryElementIndex = 0;
	mStoryElements = std::vector<StoryElement>();
}

ApePresentationScenePlugin::~ApePresentationScenePlugin()
{
	std::cout << "ApePresentationScenePlugin dtor" << std::endl;
}

void ApePresentationScenePlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName)
		mUserNode = mpScene->getNode(event.subjectName);
}

void ApePresentationScenePlugin::Init()
{
	std::cout << "ApePresentationScenePlugin::init" << std::endl;

	std::cout << "ApePresentationScenePlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "ApePresentationScenePlugin main window was found" << std::endl;

	std::stringstream hwndStrStream;
	hwndStrStream << mpMainWindow->getHandle();
	std::stringstream windowHndStr;
	windowHndStr << std::stoul(hwndStrStream.str(), nullptr, 16);

	OIS::ParamList pl;
	pl.insert(std::make_pair("WINDOW", windowHndStr.str()));
#ifdef WIN32
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#endif
	OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
	if (inputManager->getNumberOfDevices(OIS::OISKeyboard) > 0)
	{
		OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
		mpKeyboard = keyboard;
		mpKeyboard->setEventCallback(this);
	}

	/*static elements begin*/
	std::string name = "metalroom";
	Ape::Vector3 position = Ape::Vector3();
	Ape::Quaternion orientation = Ape::Quaternion();
	createMesh(name);
	name = "ID10_monitor";
	position = Ape::Vector3(-100, 710, 87) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.981, -0, -0.195, -0);
	createMesh(name, position, orientation);
	name = "ID9_monitortalp";
	position = Ape::Vector3(-186, 633, 56) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.995, -0, -0.098, -0);
	createMesh(name, position, orientation);
	name = "periodicTable";
	std::string url = "http://sciencenotes.org/wp-content/uploads/2015/01/PeriodicTableMuted.png";
	position = Ape::Vector3(442.29, 364.444, -543.149) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.924, -0, 0.383, -0) *  mOldXMLFormatRotationQuaternion;
	int width = 278;
	int height = 157;
	int resolutionVertical = 1024;
	int resolutionHorizontal = 768;
	createBrowser(name, url, position, orientation, width, height);
	name = "feyman";
	url = "https://upload.wikimedia.org/wikipedia/en/4/42/Richard_Feynman_Nobel.jpg";
	position = Ape::Vector3(-551.88, 364.444, 432.824) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.383, -0, -0.924, -0) *  mOldXMLFormatRotationQuaternion;
	width = 278;
	height = 157;
	createBrowser(name, url, position, orientation, width, height);
	name = "universe";
	url = "http://scienceblogs.com/startswithabang/files/2012/08/seqb_0631.jpeg";
	position = Ape::Vector3(-697.103, 364.444, 71.468) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.707, -0, -0.707, -0) *  mOldXMLFormatRotationQuaternion;
	width = 278;
	height = 157;
	createBrowser(name, url, position, orientation, width, height);
	name = "moseley";
	url = "http://www.mhs.ox.ac.uk/moseley/wp-content/uploads/sites/5/2015/05/Moseley_in_his_prime_c1914.jpg";
	position = Ape::Vector3(695.979, 364.444, 71.505) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.707, -0, 0.707, -0) *  mOldXMLFormatRotationQuaternion;
	width = 278;
	height = 157;
	createBrowser(name, url, position, orientation, width, height);
	name = "indigo_first";
	//url = "http://srv.mvv.sztaki.hu/temp/indigo/indigo_first.png";
	url = "http://srv.mvv.sztaki.hu/temp/indigo/bg/index.html";
	position = Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(1, 0, 0, 0) *  mOldXMLFormatRotationQuaternion;
	width = 267;
	height = 150;
	resolutionVertical = 2048;
	resolutionHorizontal = 1024;
	createBrowser(name, url, position, orientation, width, height, resolutionVertical, resolutionHorizontal);
	name = "linkageDesigner";
	url = "http://www.linkagedesigner.com/";
	position = Ape::Vector3(9.504, 364.896, -423) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(0, -0, -1, -0) *  mOldXMLFormatRotationQuaternion;
	width = 143;
	height = 85;
	createBrowser(name, url, position, orientation, width, height);
	name = "t-system";
	url = "https://www.t-systems.com/";
	position = Ape::Vector3(199.758, 677.017, -702.303) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.957, -0, 0.29, -0) *  mOldXMLFormatRotationQuaternion;
	width = 77;
	height = 46;
	createBrowser(name, url, position, orientation, width, height);
	name = "endo2";
	url = "http://srv.mvv.sztaki.hu/temp/endo2";
	position = Ape::Vector3(-74, 727, 736) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(0.098, 0, -0.995, 0) *  mOldXMLFormatRotationQuaternion;
	width = 77;
	height = 46;
	createBrowser(name, url, position, orientation, width, height);
	name = "karsai";
	url = "http://www.karsai.hu";
	position = Ape::Vector3(-706, 727, 51) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.831, 0, -0.556, 0) *  mOldXMLFormatRotationQuaternion;
	width = 77;
	height = 46;
	createBrowser(name, url, position, orientation, width, height);
	name = "motohotwear";
	url = "http://www.motohotwear.com/";
	position = Ape::Vector3(-706, 677, 51) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.831, 0, -0.556, 0) *  mOldXMLFormatRotationQuaternion;
	width = 77;
	height = 46;
	createBrowser(name, url, position, orientation, width, height);
	name = "endo5";
	url = "http://srv.mvv.sztaki.hu/temp/endo5";
	position = Ape::Vector3(188, 727, 708) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	width = 77;
	height = 46;
	createBrowser(name, url, position, orientation, width, height);
	name = "endo";
	url = "https://www.endo-kapszula.hu/";
	position = Ape::Vector3(-201, 723, 692) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	width = 143;
	height = 85;
	createBrowser(name, url, position, orientation, width, height);
	name = "endo1";
	url = "http://srv.mvv.sztaki.hu/temp/endo1";
	position = Ape::Vector3(62, 723, 729) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(0, 0, -1, 0) *  mOldXMLFormatRotationQuaternion;
	width = 143;
	height = 85;
	createBrowser(name, url, position, orientation, width, height);
	name = "endo3";
	url = "http://srv.mvv.sztaki.hu/temp/endo3";
	position = Ape::Vector3(-74, 677, 736) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(0.098, 0, -0.995, 0) *  mOldXMLFormatRotationQuaternion;
	width = 77;
	height = 46;
	createBrowser(name, url, position, orientation, width, height);
	name = "arUserManual";
	url = "https://www.youtube.com/embed/eVV5tUmky6c?vq=hd1080&autoplay=1&loop=1&playlist=eVV5tUmky6c";
	position = Ape::Vector3(-688, 723, 184) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.556, 0, -0.831, 0) *  mOldXMLFormatRotationQuaternion;
	width = 143;
	height = 85;
	createBrowser(name, url, position, orientation, width, height);
	name = "endo4";
	url = "http://srv.mvv.sztaki.hu/temp/endo4";
	position = Ape::Vector3(188, 677, 708) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	width = 77;
	height = 46;
	createBrowser(name, url, position, orientation, width, height);
	name = "ios-android";
	url = "https://nowitzkiblog.files.wordpress.com/2017/03/ios-android-icon_-e1447071162262.jpg?w=580";
	position = Ape::Vector3(199, 727, -702) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.957, 0, 0.29, 0) *  mOldXMLFormatRotationQuaternion;
	width = 77;
	height = 46;
	createBrowser(name, url, position, orientation, width, height);
	name = "bimGeometry";
	url = "http://www.teslacad.ca/blog/wp-content/uploads/2014/04/MEP-Services-Samples-13.jpg";
	position = Ape::Vector3(65, 723, -711) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.995, 0, -0.098, 0) *  mOldXMLFormatRotationQuaternion;
	width = 143;
	height = 85;
	createBrowser(name, url, position, orientation, width, height);
	name = "smartProfile";
	url = "https://www.ogpnet.com/north-america/software/smartprofile/index";
	position = Ape::Vector3(-544, 706, 440) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	width = 278;
	height = 157;
	createBrowser(name, url, position, orientation, width, height);
	name = "qvii";
	url = "https://www.qvii.com/";
	position = Ape::Vector3(-392, 749, 592) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	width = 139;
	height = 78;
	createBrowser(name, url, position, orientation, width, height);
	name = "kotem";
	url = "http://kotem.com/";
	position = Ape::Vector3(-392, 663, 592) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	width = 139;
	height = 78;
	createBrowser(name, url, position, orientation, width, height);
	name = "pridgeonandclay";
	url = "http://www.pridgeonandclay.com/";
	position = Ape::Vector3(542, 706, 441) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	width = 278;
	height = 157;
	createBrowser(name, url, position, orientation, width, height);
	name = "arShopFloor";
	url = "http://thearea.org/wp-content/uploads/2017/03/Augmented-Reality-and-IoT.jpg";
	position = Ape::Vector3(392, 749, 592) - mOldXMLFormatTranslateVector;
	orientation = Ape::Quaternion(0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	width = 139;
	height = 78;
	createBrowser(name, url, position, orientation, width, height);
	/*story begin*/
	mStoryElements.push_back(StoryElement(Ape::Vector3(0, 0, 0) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(1, 0, 0, 0))); //zero
	mStoryElements.push_back(StoryElement(Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(1, 0, 0, 0))); //indigo
	mStoryElements.push_back(StoryElement(Ape::Vector3(13, 359, -52) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(1, 0, 0, 0),
		"github", "https://github.com/MTASZTAKI/ApertusVR", Ape::Vector3(0, 360, -370) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150)); //github
	mStoryElements.push_back(StoryElement(Ape::Vector3(-234, 770, 258) - mOldXMLFormatTranslateVectorCamera, Ape::Euler(Ape::Degree(131).toRadian(), 0, 0).toQuaternion())); //smartProfile
	mStoryElements.push_back(StoryElement(Ape::Vector3(67, 705, 295) - mOldXMLFormatTranslateVectorCamera, Ape::Euler(Ape::Degree(169).toRadian(), 0, 0).toQuaternion())); //endo
	mStoryElements.push_back(StoryElement(Ape::Vector3(36, 717, -451) - mOldXMLFormatTranslateVectorCamera, Ape::Euler(Ape::Degree(-22).toRadian(), 0, 0).toQuaternion())); //t-system
	mStoryElements.push_back(StoryElement(Ape::Vector3(190, 761, 180) - mOldXMLFormatTranslateVectorCamera, Ape::Euler(Ape::Degree(-138).toRadian(), 0, 0).toQuaternion())); //pridgeonandclay
	mStoryElements.push_back(StoryElement(Ape::Vector3(-12, 387, -659) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(0, 0, -1, 0))); //linkageDesigner
	mStoryElements.push_back(StoryElement(Ape::Vector3(13, 359, -52) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(1, 0, 0, 0),
		"symbio-tic", "http://www.symbio-tic.eu", Ape::Vector3(0, 360, -320) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150)); //github
}

void ApePresentationScenePlugin::animateToStoryElements(Ape::NodeSharedPtr userNode)
{
	StoryElement storyElement = mStoryElements[mCurrentStoryElementIndex];
	if (auto userNode = mUserNode.lock())
	{
		auto moveInterpolator = std::make_unique<Ape::Interpolator>(false);
		moveInterpolator->addSection(
			userNode->getPosition(),
			storyElement.cameraPosition,
			6.0,
			[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
		);
		auto rotateInterpolator = std::make_unique<Ape::Interpolator>(false);
		rotateInterpolator->addSection(
			userNode->getOrientation(),
			storyElement.cameraOrientation,
			6.0,
			[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
		);
		while (!moveInterpolator->isQueueEmpty() && !rotateInterpolator->isQueueEmpty())
		{
			if (!moveInterpolator->isQueueEmpty())
				moveInterpolator->iterateTopSection();
			if (!rotateInterpolator->isQueueEmpty())
				rotateInterpolator->iterateTopSection();
		}
	}
	if (storyElement.browserName.size())
	{
		std::string name = storyElement.browserName;
		std::string url = storyElement.browserURL;
		Ape::Vector3 position = storyElement.browserPosition;
		Ape::Quaternion orientation = storyElement.browserOrientation;
		int width = storyElement.browserWidth;
		int height = storyElement.browserHeight;
		createBrowser(name, url, position, orientation, width, height);
	}
}

void ApePresentationScenePlugin::jumpToStoryElement(Ape::NodeSharedPtr userNode)
{
	StoryElement storyElement = mStoryElements[mCurrentStoryElementIndex];
	userNode->setPosition(storyElement.cameraPosition);
	userNode->setOrientation(storyElement.cameraOrientation);
	if (storyElement.browserName.size())
	{
		std::string name = storyElement.browserName;
		std::string url = storyElement.browserURL;
		Ape::Vector3 position = storyElement.browserPosition;
		Ape::Quaternion orientation = storyElement.browserOrientation;
		int width = storyElement.browserWidth;
		int height = storyElement.browserHeight;
		createBrowser(name, url, position, orientation, width, height);
	}
}

void ApePresentationScenePlugin::createBrowser(std::string name, std::string url, Ape::Vector3 position, Ape::Quaternion orientation, int width, int height, int resolutionVertical, int resolutionHorizontal)
{
	if (!mpScene->getNode(name).lock())
	{
		if (auto browserNode = mpScene->createNode(name).lock())
		{
			browserNode->setPosition(position);
			browserNode->setOrientation(orientation);
			if (auto browserGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity(name + "geometry", Ape::Entity::GEOMETRY_PLANE).lock()))
			{
				browserGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(width, height), Ape::Vector2(1, 1));
				browserGeometry->setParentNode(browserNode);
				if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity(name + "browser", Ape::Entity::BROWSER).lock()))
				{
					browser->setResoultion(resolutionVertical, resolutionHorizontal);
					browser->setURL(url);
					browser->setGeometry(browserGeometry);
				}
			}
		}
	}
}

void ApePresentationScenePlugin::createMesh(std::string name, Ape::Vector3 position, Ape::Quaternion orientation)
{
	if (auto node = mpScene->createNode(name).lock())
	{
		node->setPosition(position);
		node->setOrientation(orientation);
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity(name + ".mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName(name + ".mesh");
			meshFile->setParentNode(node);
		}
	}
}

void ApePresentationScenePlugin::moveUserNode()
{
	auto userNode = mUserNode.lock();
	if (userNode)
	{
		if (mKeyCodeMap[OIS::KeyCode::KC_PGUP])
			userNode->translate(Ape::Vector3(0, 1 * mTranslateSpeedFactor, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_PGDOWN])
			userNode->translate(Ape::Vector3(0, -1 * mTranslateSpeedFactor, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_D])
			userNode->translate(Ape::Vector3(1 * mTranslateSpeedFactor, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_A])
			userNode->translate(Ape::Vector3(-1 * mTranslateSpeedFactor, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_W])
			userNode->translate(Ape::Vector3(0, 0, -1 * mTranslateSpeedFactor), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_S])
			userNode->translate(Ape::Vector3(0, 0, 1 * mTranslateSpeedFactor), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_LEFT])
			userNode->rotate(0.017f * mRotateSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
		if (mKeyCodeMap[OIS::KeyCode::KC_RIGHT])
			userNode->rotate(-0.017f * mRotateSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
		if (mKeyCodeMap[OIS::KeyCode::KC_UP])
			userNode->rotate(0.017f * mRotateSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_DOWN])
			userNode->rotate(-0.017f * mRotateSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
		if (mKeyCodeMap[OIS::KeyCode::KC_NUMPAD4])
			userNode->rotate(0.017f * mRotateSpeedFactor, Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
		if (mKeyCodeMap[OIS::KeyCode::KC_NUMPAD6])
			userNode->rotate(-0.017f * mRotateSpeedFactor, Ape::Vector3(0, 0, 1), Ape::Node::TransformationSpace::WORLD);
	}
}

bool ApePresentationScenePlugin::keyPressed(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = true;
	auto userNode = mUserNode.lock();
	if (userNode)
	{
		if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
		{
			if (mCurrentStoryElementIndex < mStoryElements.size())
			{
				mCurrentStoryElementIndex++;
				animateToStoryElements(userNode);
			}
		}
		if (mKeyCodeMap[OIS::KeyCode::KC_M])
		{
			if (mCurrentStoryElementIndex < mStoryElements.size())
			{
				mCurrentStoryElementIndex++;
				jumpToStoryElement(userNode);
			}
		}
		if (mKeyCodeMap[OIS::KeyCode::KC_N])
		{
			if (mCurrentStoryElementIndex > 0)
			{
				mCurrentStoryElementIndex--;
				jumpToStoryElement(userNode);
			}
		}
		if (mKeyCodeMap[OIS::KeyCode::KC_R])
		{
			mCurrentStoryElementIndex = 0;
			jumpToStoryElement(userNode);
		}
	}
	return true;
}

bool ApePresentationScenePlugin::keyReleased(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = false;
	return true;
}

void ApePresentationScenePlugin::Run()
{
	while (true)
	{
		if (mpKeyboard)
			mpKeyboard->capture();
		moveUserNode();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
}

void ApePresentationScenePlugin::Step()
{

}

void ApePresentationScenePlugin::Stop()
{

}

void ApePresentationScenePlugin::Suspend()
{

}

void ApePresentationScenePlugin::Restart()
{

}
