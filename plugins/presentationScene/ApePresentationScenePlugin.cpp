#include <iostream>
#include "ApePresentationScenePlugin.h"

ApePresentationScenePlugin::ApePresentationScenePlugin()
{
	mpKeyboard = NULL;
	mpMouse = NULL;
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
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
	mTranslateSpeedFactor = 2;
	mRotateSpeedFactor = 1;
	mCurrentStoryElementIndex = 0;
	mStoryElements = std::vector<StoryElement>();
	mBrowsers = std::map<std::string, Ape::BrowserWeakPtr>();
	mIsFirstSpacePressed = false;
	mActiveMouseTexture = Ape::UnitTextureWeakPtr();
	mRayGeometry = Ape::RayGeometryWeakPtr();
	mRayOverlayNode = Ape::NodeWeakPtr();
	mGeometriesMouseTextures = std::map<std::string, Ape::UnitTextureWeakPtr>();
	mBrowserMouseTextures = std::map<std::string, Ape::BrowserWeakPtr>();
	mOverlayMouseTexture = Ape::UnitTextureWeakPtr();
	mOverlayMouseMaterial = Ape::ManualMaterialWeakPtr();
	mLastLeftClickTime = 0;
	mActiveBrowser = Ape::BrowserWeakPtr();
	mOverlayBrowser = Ape::BrowserWeakPtr();
	mCamera = Ape::CameraWeakPtr();
	mUserNodePositionBeforeFullScreen = Ape::Vector3();
	mUserNodeOrientationBeforeFullScreen = Ape::Quaternion();
	mHumanHeight = 250.0f;
	mHumanXoffset = 0.0f;
	mHumanZoffset = 160.0f;
	mContext = Context::UNKOWN;
}

ApePresentationScenePlugin::~ApePresentationScenePlugin()
{
	std::cout << "ApePresentationScenePlugin dtor" << std::endl;
}

void ApePresentationScenePlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::CAMERA_CREATE)
		mCamera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(event.subjectName).lock());
	else if (event.type == Ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		if (auto rayGeometry = mRayGeometry.lock())
		{
			auto intersections = rayGeometry->getIntersections();
			for (auto intersection : intersections)
			{
				if (auto geometry = intersection.lock())
				{
					if (auto mouseTexture = mGeometriesMouseTextures[geometry->getName()].lock())
					{
						mActiveMouseTexture = mouseTexture;
						if (auto overlayMouseMaterial = mOverlayMouseMaterial.lock())
							overlayMouseMaterial->showOnOverlay(false, 1);
						if (auto browser = mBrowserMouseTextures[mouseTexture->getName()].lock())
							mActiveBrowser = browser;
					}
				}
			}
		}
	}
}

void ApePresentationScenePlugin::Init()
{
	std::cout << "ApePresentationScenePlugin::init" << std::endl;

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

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
	if (inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
	{
		OIS::Mouse* mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
		mpMouse = mouse;
		mpMouse->setEventCallback(this);
		const OIS::MouseState &ms = mouse->getMouseState();
		ms.width = mpMainWindow->getWidth();
		ms.height = mpMainWindow->getHeight();
	}
	/*rayGeomtery for rayquery*/
	if (auto rayNode = mpScene->createNode("rayNode").lock())
	{
		if (auto rayGeometry = std::static_pointer_cast<Ape::IRayGeometry>(mpScene->createEntity("rayQuery", Ape::Entity::GEOMETRY_RAY).lock()))
		{
			rayGeometry->setIntersectingEnabled(true);
			rayGeometry->setParentNode(rayNode);
			mRayGeometry = rayGeometry;
		}
		rayNode->setParentNode(mUserNode);
		mRayOverlayNode = rayNode;
	}
	/*overlay begin*/
	//if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("overlay_frame", Ape::Entity::BROWSER).lock()))
	//{
	//	browser->setResoultion(2048, 1024);
	//	browser->setURL("http://srv.mvv.sztaki.hu/temp/indigo/bg/index.html");
	//	browser->showOnOverlay(true, 0);
	//	mOverlayBrowser = browser;
	//	mActiveBrowser = browser;
	//	/*mouse begin*/
	//	if (auto mouseMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("mouseMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	//	{
	//		mouseMaterial->setEmissiveColor(Ape::Color(1.0f, 1.0f, 1.0f));
	//		mouseMaterial->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
	//		mouseMaterial->setLightingEnabled(false);
	//		if (auto mouseTexture = std::static_pointer_cast<Ape::IUnitTexture>(mpScene->createEntity("mouseTexture", Ape::Entity::TEXTURE_UNIT).lock()))
	//		{
	//			mouseTexture->setParameters(mouseMaterial, "browserpointer.png");
	//			mouseTexture->setTextureAddressingMode(Ape::Texture::AddressingMode::CLAMP);
	//			mouseTexture->setTextureFiltering(Ape::Texture::Filtering::POINT, Ape::Texture::Filtering::LINEAR, Ape::Texture::Filtering::F_NONE);
	//			mActiveMouseTexture = mouseTexture;
	//			mOverlayMouseTexture = mouseTexture;
	//			mOverlayMouseMaterial = mouseMaterial;
	//		}
	//		mouseMaterial->showOnOverlay(true, 1);
	//	}
	//}
	/*ApertusVR begin*/
	/*static elements begin*/
	//std::string name = "metalroom";
	//Ape::Vector3 position = Ape::Vector3();
	//Ape::Quaternion orientation = Ape::Quaternion();
	//createMesh(name);
	//name = "ID10_monitor";
	//position = Ape::Vector3(-100, 710, 87) - mOldXMLFormatTranslateVector;
	//orientation = Ape::Quaternion(-0.981, -0, -0.195, -0);
	//createMesh(name, position, orientation);
	//name = "ID9_monitortalp";
	//position = Ape::Vector3(-186, 633, 56) - mOldXMLFormatTranslateVector;
	//orientation = Ape::Quaternion(-0.995, -0, -0.098, -0);
	//createMesh(name, position, orientation);
	//StoryElement storyElement;
	//storyElement.browserName = "periodicTable";
	//storyElement.browserURL = "http://sciencenotes.org/wp-content/uploads/2015/01/PeriodicTableMuted.png";
	//storyElement.browserPosition = Ape::Vector3(442.29, 364.444, -543.149) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.924, -0, 0.383, -0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 278;
	//storyElement.browserHeight = 157;
	//storyElement.browserZoom = 0;
	//storyElement.browserResolutionVertical = 1024;
	//storyElement.browserResolutionHorizontal = 768;
	//manageBrowser(storyElement);
	//storyElement.browserName = "feyman";
	//storyElement.browserURL = "https://upload.wikimedia.org/wikipedia/en/4/42/Richard_Feynman_Nobel.jpg";
	//storyElement.browserPosition = Ape::Vector3(-551.88, 364.444, 432.824) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.383, -0, -0.924, -0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 278;
	//storyElement.browserHeight = 157;
	//manageBrowser(storyElement);
	//storyElement.browserName = "universe";
	//storyElement.browserURL = "http://scienceblogs.com/startswithabang/files/2012/08/seqb_0631.jpeg";
	//storyElement.browserPosition = Ape::Vector3(-697.103, 364.444, 71.468) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.707, -0, -0.707, -0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 278;
	//storyElement.browserHeight = 157;
	//manageBrowser(storyElement);
	//storyElement.browserName = "moseley";
	//storyElement.browserURL = "http://www.mhs.ox.ac.uk/moseley/wp-content/uploads/sites/5/2015/05/Moseley_in_his_prime_c1914.jpg";
	//storyElement.browserPosition = Ape::Vector3(695.979, 364.444, 71.505) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.707, -0, 0.707, -0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 278;
	//storyElement.browserHeight = 157;
	//manageBrowser(storyElement);
	//storyElement.browserName = "first";
	//storyElement.browserURL = "http://google.com";
	//storyElement.browserPosition = Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(1, 0, 0, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 267;
	//storyElement.browserHeight = 150;
	//storyElement.browserResolutionVertical = 2048;
	//storyElement.browserResolutionHorizontal = 1024;
	//manageBrowser(storyElement);
	//storyElement.browserName = "linkageDesigner";
	//storyElement.browserURL = "http://www.linkagedesigner.com/";
	//storyElement.browserPosition = Ape::Vector3(9.504, 364.896, -423) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(0, -0, -1, -0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 143;
	//storyElement.browserHeight = 85;
	//storyElement.browserResolutionVertical = 1024;
	//storyElement.browserResolutionHorizontal = 768;
	//manageBrowser(storyElement);
	//storyElement.browserName = "t-system";
	//storyElement.browserURL = "https://www.t-systems.com/";
	//storyElement.browserPosition = Ape::Vector3(199.758, 677.017, -702.303) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.957, -0, 0.29, -0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 77;
	//storyElement.browserHeight = 46;
	//manageBrowser(storyElement);
	//storyElement.browserName = "endo2";
	//storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo2";
	//storyElement.browserPosition = Ape::Vector3(-74, 727, 726) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(0.098, 0, -0.995, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 77;
	//storyElement.browserHeight = 46;
	//manageBrowser(storyElement);
	//storyElement.browserName = "karsai";
	//storyElement.browserURL = "http://www.karsai.hu";
	//storyElement.browserPosition = Ape::Vector3(-706, 727, 51) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.831, 0, -0.556, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 77;
	//storyElement.browserHeight = 46;
	//manageBrowser(storyElement);
	//storyElement.browserName = "motohotwear";
	//storyElement.browserURL = "http://www.motohotwear.com/";
	//storyElement.browserPosition = Ape::Vector3(-706, 677, 51) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.831, 0, -0.556, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 77;
	//storyElement.browserHeight = 46;
	//manageBrowser(storyElement);
	//storyElement.browserName = "endo5";
	//storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo5";
	//storyElement.browserPosition = Ape::Vector3(188, 727, 708) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 77;
	//storyElement.browserHeight = 46;
	//manageBrowser(storyElement);
	//storyElement.browserName = "endo";
	//storyElement.browserURL = "https://www.endo-kapszula.hu/";
	//storyElement.browserPosition = Ape::Vector3(-201, 723, 692) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 143;
	//storyElement.browserHeight = 85;
	//manageBrowser(storyElement);
	//storyElement.browserName = "endo1";
	//storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo1";
	//storyElement.browserPosition = Ape::Vector3(62, 723, 729) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(0, 0, -1, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 143;
	//storyElement.browserHeight = 85;
	//manageBrowser(storyElement);
	//storyElement.browserName = "endo3";
	//storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo3";
	//storyElement.browserPosition = Ape::Vector3(-74, 677, 726) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(0.098, 0, -0.995, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 77;
	//storyElement.browserHeight = 46;
	//manageBrowser(storyElement);
	//storyElement.browserName = "arUserManual";
	//storyElement.browserURL = "https://www.youtube.com/embed/eVV5tUmky6c?vq=hd480&autoplay=1&loop=1&playlist=eVV5tUmky6c";
	//storyElement.browserPosition = Ape::Vector3(-688, 723, 184) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.556, 0, -0.831, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 143;
	//storyElement.browserHeight = 85;
	//manageBrowser(storyElement);
	//storyElement.browserName = "endo4";
	//storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo4";
	//storyElement.browserPosition = Ape::Vector3(188, 677, 708) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 77;
	//storyElement.browserHeight = 46;
	//manageBrowser(storyElement);
	//storyElement.browserName = "ios-android";
	//storyElement.browserURL = "https://nowitzkiblog.files.wordpress.com/2017/03/ios-android-icon_-e1447071162262.jpg?w=580";
	//storyElement.browserPosition = Ape::Vector3(199, 727, -702) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.957, 0, 0.29, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 77;
	//storyElement.browserHeight = 46;
	//manageBrowser(storyElement);
	//storyElement.browserName = "bimGeometry";
	//storyElement.browserURL = "http://www.teslacad.ca/blog/wp-content/uploads/2014/04/MEP-Services-Samples-13.jpg";
	//storyElement.browserPosition = Ape::Vector3(65, 723, -711) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.995, 0, -0.098, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 143;
	//storyElement.browserHeight = 85;
	//manageBrowser(storyElement);
	//storyElement.browserName = "smartProfile";
	//storyElement.browserURL = "http://kotem.com/Pages/Products.htm";
	//storyElement.browserPosition = Ape::Vector3(-544, 706, 440) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 278;
	//storyElement.browserHeight = 157;
	//manageBrowser(storyElement);
	//storyElement.browserName = "qvii";
	//storyElement.browserURL = "https://www.qvii.com/";
	//storyElement.browserPosition = Ape::Vector3(-392, 749, 592) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 139;
	//storyElement.browserHeight = 78;
	//manageBrowser(storyElement);
	//storyElement.browserName = "kotem";
	//storyElement.browserURL = "http://kotem.com/";
	//storyElement.browserPosition = Ape::Vector3(-392, 663, 592) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 139;
	//storyElement.browserHeight = 78;
	//manageBrowser(storyElement);
	//storyElement.browserName = "pridgeonandclay";
	//storyElement.browserURL = "http://www.pridgeonandclay.com/";
	//storyElement.browserPosition = Ape::Vector3(542, 706, 441) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 278;
	//storyElement.browserHeight = 157;
	//manageBrowser(storyElement);
	//storyElement.browserName = "arShopFloor";
	//storyElement.browserURL = "http://thearea.org/wp-content/uploads/2017/03/Augmented-Reality-and-IoT.jpg";
	//storyElement.browserPosition = Ape::Vector3(392, 749, 592) - mOldXMLFormatTranslateVector;
	//storyElement.browserOrientation = Ape::Quaternion(0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	//storyElement.browserWidth = 139;
	//storyElement.browserHeight = 78;
	//manageBrowser(storyElement);
	///*story begin*/
	//mStoryElements.push_back(StoryElement(Ape::Vector3(0, 0, 0), Ape::Quaternion(1, 0, 0, 0))); //zero
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-12.5, -270.943, 409.5), Ape::Quaternion(1, 0, 0, 0))); //indigo
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-24.5, -273.943, -114), Ape::Quaternion(1, 0, 0, 0),
	//	"github", "https://github.com/MTASZTAKI/ApertusVR", Ape::Vector3(0, 360, -370) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150)); //github
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-30.5, -247.443, -161), Ape::Quaternion(1, 0, 0, 0),
	//	"github", "https://github.com/MTASZTAKI/ApertusVR#about", Ape::Vector3(0, 360, -370) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150)); //github#about
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-27.5, -278.943, -121), Ape::Quaternion(1, 0, 0, 0),
	//	"github", "https://github.com/MTASZTAKI/ApertusVR#apertusvr", Ape::Vector3(0, 360, -370) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150)); //github#swarch
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-312.127, 79.2832, 317.68), Ape::Quaternion(0.383774, -0.00326215, 0.923396, 0.00784904))); //smartProfile1
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-393.724, 78.6628, 287.429), Ape::Quaternion(0.383774, -0.00326215, 0.923396, 0.00784904))); //smartProfile2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(97.6123, 67.057, 422.253), Ape::Quaternion(0.171713, -9.77448e-07, 0.985149, 0.0))); //endo
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-197.586, 100.834, 573.667), Ape::Quaternion(0.116897, -0.00596791, 0.991839, 0.0506278),
	//	"endo", "https://www.youtube.com/embed/MSvpU8bYZiE?vq=hd480&autoplay=1&loop=1&playlist=MSvpU8bYZiE", Ape::Vector3(-201, 723, 692) - mOldXMLFormatTranslateVector, Ape::Quaternion(-0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion, 143, 85));
	//mStoryElements.push_back(StoryElement(Ape::Vector3(253.464, 88.8106, 280.662), Ape::Quaternion(-0.397588, -1.02083e-08, 0.917568, 0.0))); //pridgeonandclay1
	//mStoryElements.push_back(StoryElement(Ape::Vector3(268.471, 112.444, 489.413), Ape::Quaternion(-0.257203, -0.00218629, 0.966327, -0.00821397))); //pridgeonandclay2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-539.024, 82.1165, 121.716), Ape::Quaternion(0.653925, -0.0166771, 0.756182, 0.0192854))); //karsai1
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-590.853, 78.5688, 116.881), Ape::Quaternion(0.808807, -0.0206272, 0.587596, 0.0149859))); //karsai2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-607.718, 94.0923, 162.253), Ape::Quaternion(0.573593, -0.0121893, 0.818928, 0.0174032))); //karsai3
	//mStoryElements.push_back(StoryElement(Ape::Vector3(16.7394, 84.057, -492.757), Ape::Quaternion(0.981627, 0, -0.190809, 0))); //t-system
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-12, 387, -659) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(0, 0, -1, 0))); //linkageDesigner1
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-16.4233, -261.939, -547.675), Ape::Quaternion(0.000344675, 0.0, -0.999992, -0.00424999))); //linkageDesigner2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-16.4233, -261.939, -547.675), Ape::Quaternion(0.000344675, 0.0, -0.999992, -0.00424999),
	//	"linkageDesigner", "https://www.youtube.com/embed/YxcRr_QbPfg?vq=hd480&autoplay=1&loop=1&playlist=YxcRr_QbPfg", Ape::Vector3(9.504, 364.896, -423) - mOldXMLFormatTranslateVector, Ape::Quaternion(0, -0, -1, -0) *  mOldXMLFormatRotationQuaternion, 143, 85)); //linkageDesigner3
	//mStoryElements.push_back(StoryElement(Ape::Vector3(13, 359, -52) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(1, 0, 0, 0),
	//	"symbio-tic", "http://www.symbio-tic.eu/index.php?option=com_content&view=article&id=2&Itemid=25", Ape::Vector3(0, 360, -320) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150, 0, 2048, 1024)); //symbio-tic1
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-25.9053, -304.943, -172.531), Ape::Quaternion(1, 0, -0.000392581, 0))); //symbio-tic2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-12.5, -270.943, 461.5), Ape::Quaternion(1, 0, 0, 0),
	//	"first", "https://techcrunch.com/2016/07/30/tech-trends-that-will-impact-your-home/", Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 267, 150, 0, 2048, 1024)); //thank you for the attention
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-12.5, -270.943, 461.5), Ape::Quaternion(1, 0, 0, 0),
	//	"first", "http://srv.mvv.sztaki.hu/temp/honeywell/tech_arvr.png", Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 267, 150, 0, 2048, 1024)); //thank you for the attention
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-12.5, -270.943, 461.5), Ape::Quaternion(1, 0, 0, 0),
	//	"first", "http://srv.mvv.sztaki.hu/temp/honeywell/tech_iot.png", Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 267, 150, 0, 2048, 1024)); //thank you for the attention
	/*ApertusVR end*/

	/*2017Farewell begin*/
	///*if (auto universeSkyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("universe", Ape::Entity::MATERIAL_FILE).lock()))
	//{
	//	universeSkyBoxMaterial->setFileName("universe.material");
	//	universeSkyBoxMaterial->setAsSkyBox();
	//}*/
	//std::string name = "websummit_logo";
	//Ape::Vector3 position = Ape::Vector3(-1050, -450, -150);
	//Ape::Quaternion orientation = Ape::Quaternion(1, 0, 0, 0);
	//createMesh(name, position, orientation);
	//name = "sphere_img";
	//position = Ape::Vector3(-1200, 0, 0);
	//orientation = Ape::Quaternion(0.7071, 0, 0, 0.7071);
	//createMesh(name, position, orientation);
	//name = "sphere_img_2";
	//position = Ape::Vector3(-600, -600, 0);
	//orientation = Ape::Quaternion(0.7071, 0, 0, 0.7071);
	//createMesh(name, position, orientation);
	///*if (auto browserNode = mpScene->createNode("video2Node").lock())
	//{
	//	browserNode->setPosition(Ape::Vector3(-600, -600, 0));
	//	if (auto browserGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_2.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
	//	{
	//		browserGeometry->setFileName("sphere_2.mesh");
	//		browserGeometry->setParentNode(browserNode);
	//		if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("video2Browser", Ape::Entity::BROWSER).lock()))
	//		{
	//			browser->setResoultion(2048, 1024);
	//			browser->setURL("https://www.youtube.com/embed/JWI2VwOVBFk?vq=hd1080&autoplay=1&loop=1&playlist=JWI2VwOVBFk");
	//			browser->setGeometry(browserGeometry);
	//		}
	//	}
	//}*/
	//name = "sphere_img_3";
	//position = Ape::Vector3(-1200, -600, 0);
	//orientation = Ape::Quaternion(0.7071, 0, 0, 0.7071);
	//createMesh(name, position, orientation);
	//name = "sphere_img_4";
	//position = Ape::Vector3(-600, 0, -600);
	//orientation = Ape::Quaternion(0.7071, 0, 0, 0.7071);
	//createMesh(name, position, orientation);
	//name = "sphere_img_5";
	//position = Ape::Vector3(-1200, 0, -600);
	//orientation = Ape::Quaternion(0.7071, 0, 0, 0.7071);
	//createMesh(name, position, orientation);
	//name = "sphere_img_6";
	//position = Ape::Vector3(-600, 0, 0);
	//orientation = Ape::Quaternion(0.7071, 0, 0, 0.7071);
	//createMesh(name, position, orientation);
	///*if (auto browserNode = mpScene->createNode("video6Node").lock())
	//{
	//	browserNode->setPosition(Ape::Vector3(-600, 0, 0));
	//	if (auto browserGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
	//	{
	//		browserGeometry->setFileName("sphere.mesh");
	//		browserGeometry->setParentNode(browserNode);
	//		if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("video6Browser", Ape::Entity::BROWSER).lock()))
	//		{
	//			browser->setResoultion(2048, 1024);
	//			browser->setURL("https://www.youtube.com/embed/oPoRy13M0Q4?vq=hd1080&autoplay=1&loop=1&playlist=oPoRy13M0Q4");
	//			browser->setGeometry(browserGeometry);
	//		}
	//	}
	//}*/
	///*name = "sphere_img_7";
	//position = Ape::Vector3(-600, -600, -600);
	//orientation = Ape::Quaternion(0.7071, 0, 0, 0.7071);*/
	//if (auto browserNode = mpScene->createNode("videoNode").lock())
	//{
	//	browserNode->setPosition(Ape::Vector3(-600, -600, -600));
	//	if (auto browserGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
	//	{
	//		browserGeometry->setFileName("sphere.mesh");
	//		browserGeometry->setParentNode(browserNode);
	//		if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity("videoBrowser", Ape::Entity::BROWSER).lock()))
	//		{
	//			browser->setResoultion(2048, 1024);
	//			browser->setURL("https://www.youtube.com/embed/2FPma4PLyAI?vq=hd1080&autoplay=1&loop=1&playlist=2FPma4PLyAI");
	//			browser->setGeometry(browserGeometry);
	//		}
	//	}
	//}
	//name = "sphere_img_8";
	//position = Ape::Vector3(-1200, -600, -600);
	//orientation = Ape::Quaternion(0.7071, 0, 0, 0.7071);
	//createMesh(name, position, orientation);

	//name = "sphere_img_10";
	//position = Ape::Vector3(-1200, -600, -6000);
	//orientation = Ape::Quaternion();
	//createMesh(name, position, orientation);

	//Ape::Radian angle0(1.57f);
	//Ape::Vector3 axis0(1, 0, 0);
	//Ape::Quaternion orientation0;
	//orientation0.FromAngleAxis(angle0, axis0);
	//Ape::Radian angle2(1.57f);
	//Ape::Vector3 axis2(0, 1, 0);
	//Ape::Quaternion orientation2;
	//orientation2.FromAngleAxis(angle2, axis2);
	//Ape::Radian angle3(3.14f);
	//Ape::Vector3 axis3(1, 0, 0);
	//Ape::Quaternion orientation3;
	//orientation3.FromAngleAxis(angle3, axis3);
	//Ape::Quaternion defaultOrientation;
	//defaultOrientation = orientation0 * orientation2 * orientation3;
	//if (auto planeNode = mpScene->createNode("planeNode").lock())
	//{
	//	planeNode->setPosition(Ape::Vector3(-900, 0, -310));
	//	planeNode->setOrientation(orientation0 * orientation2);
	//	if (auto plane = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("plane", Ape::Entity::GEOMETRY_PLANE).lock()))
	//	{
	//		plane->setParameters(Ape::Vector2(1, 1), Ape::Vector2(425, 213), Ape::Vector2(1, 1));
	//		plane->setParentNode(planeNode);
	//		if (auto planeMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("planeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	//		{
	//			if (auto planeMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("planeMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
	//			{
	//				planeMaterialManualPass->setShininess(0.0f);
	//				planeMaterialManualPass->setDiffuseColor(Ape::Color());
	//				planeMaterialManualPass->setSpecularColor(Ape::Color());
	//				planeMaterial->setPass(planeMaterialManualPass);
	//				plane->setMaterial(planeMaterial);
	//			}
	//		}
	//	}
	//}
	//StoryElement storyElement;
	//storyElement.browserName = "websummit";
	//storyElement.browserURL = "https://websummit.com/";
	//storyElement.browserPosition = Ape::Vector3(-900, 0, -300);
	//storyElement.browserOrientation = defaultOrientation;
	//storyElement.browserWidth = 425;
	//storyElement.browserHeight = 213;
	//storyElement.browserZoom = 0;
	//storyElement.browserResolutionVertical = 2048;
	//storyElement.browserResolutionHorizontal = 1024;
	//manageBrowser(storyElement);

	//storyElement.browserName = "venue";
	//storyElement.browserURL = "https://www.google.hu/maps/place/Lisszabon,+Portug%C3%A1lia/@38.7646941,-9.0969397,684a,35y,39.25t/data=!3m1!1e3!4m5!3m4!1s0xd19331a61e4f33b:0x400ebbde49036d0!8m2!3d38.7222524!4d-9.1393366";
	//storyElement.browserPosition = Ape::Vector3(-870, 0, -600);
	//storyElement.browserWidth = 400;
	//storyElement.browserHeight = 200;
	//manageBrowser(storyElement);

	//storyElement.browserName = "expo";
	//storyElement.browserURL = "https://en.wikipedia.org/wiki/Expo_%2798";
	//storyElement.browserPosition = Ape::Vector3(-1050, 0, -595);
	//storyElement.browserWidth = 107;
	//storyElement.browserHeight = 213;
	//storyElement.browserResolutionVertical = 512;
	//storyElement.browserResolutionHorizontal = 1024;
	//manageBrowser(storyElement);

	//storyElement.browserName = "kids1";
	//storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/kids/kids1.jpg";
	//storyElement.browserPosition = Ape::Vector3(-1200, -600, -5000);
	//manageBrowser(storyElement);

	//storyElement.browserName = "kids2";
	//storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/kids/kids2.jpg";
	//storyElement.browserPosition = Ape::Vector3(-1200, -600, -4000);
	//manageBrowser(storyElement);

	//mStoryElements.push_back(StoryElement(Ape::Vector3(0, 0, 0), Ape::Quaternion(1, 0, 0, 0))); //zero
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-941.765, 47.9583, 631.489), Ape::Quaternion(0.977507, -0.210903, -4.55392e-07, 2.41213e-07))); //begin
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-895.301, -22.3373, -30.3792), Ape::Quaternion(0.9987, -0.0509765, -4.37447e-07, 3.08579e-07))); //websummit
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-999.878, 22.7379, -416.659), Ape::Quaternion(0.991637, -0.050616, -0.118565, -0.00605159))); //venue
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, -600, 0), Ape::Quaternion(1, 0, 0, 0))); //2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, -600, 0), Ape::Quaternion(0, 0, 0.9, 0))); //2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, -600, 0), Ape::Quaternion(1, 0, -0.1, 0))); //2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, 0), Ape::Quaternion(0.0476064, -0.0128429, 0.867642, 0.234066))); //3
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1208.25, -580.244, -25.0931), Ape::Quaternion(-0.0594213, 0.0160302, 0.866916, 0.23387))); //3
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, 0, -600), Ape::Quaternion(0.99293, 0, 0, 0.11872))); //4
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, 0, -600), Ape::Quaternion(0.729693, -0.122863, 0.617354, 0.267213))); //4
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, 0, -600), Ape::Quaternion(-0.234952, -0.140821, 0.919934, 0.280819))); //4
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, 0, -600), Ape::Quaternion(0.99293, 0, 0, 0.11872))); //5
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, 0, -600), Ape::Quaternion(0.751267, -0.0224898, 0.641798, 0.152713))); //5
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, 0, 0), Ape::Quaternion(1, 0, 0, 0))); //6
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, 0, 0), Ape::Quaternion(0, 0, 0.9, 0))); //6
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-600, 0, 0), Ape::Quaternion(1, 0, -0.1, 0))); //6
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-586.049, -600, -647.3), Ape::Quaternion(0.0110464, 0, 0.999975, 0))); //7
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-553.92, -600, -562.842), Ape::Quaternion(0.960975, 0, 0.276853, 0))); //7
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, -600), Ape::Quaternion(0.994804, 0, 0, 0.101823))); //8
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, -600), Ape::Quaternion(0.883192, -0.0933883, 0.451264, 0.0876624))); //8
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, -600), Ape::Quaternion(-0.177935, -0.0642293, 0.978505, 0.0828286))); //8
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1208.23, -19.4598, -27.9061), Ape::Quaternion(0.0517954, 0.165746, -0.968518, 0.178756))); //1
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-941.765, 47.9583, 631.489), Ape::Quaternion(0.977507, -0.210903, -4.55392e-07, 2.41213e-07))); //endsection
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, -6000), Ape::Quaternion(1, 0, 0, 0))); //10
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, -6000), Ape::Quaternion(0, 0, 0.9, 0))); //10
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, -6000), Ape::Quaternion(1, 0, -0.1, 0))); //10
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, -4900), Ape::Quaternion(1, 0, 0, 0))); //kids1
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-1200, -600, -3900), Ape::Quaternion(1, 0, 0, 0))); //kids2
	//mStoryElements.push_back(StoryElement(Ape::Vector3(-941.765, 47.9583, 631.489), Ape::Quaternion(0.977507, -0.210903, -4.55392e-07, 2.41213e-07))); //end
	/*2017Farewell end*/

	/*endo meeting 2018.02.14 begin*/
	if (auto universeSkyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("universe", Ape::Entity::MATERIAL_FILE).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe.material");
		universeSkyBoxMaterial->setAsSkyBox();
	}
	Ape::Radian angle0(1.57f);
	Ape::Vector3 axis0(1, 0, 0);
	Ape::Quaternion orientation0;
	orientation0.FromAngleAxis(angle0, axis0);
	Ape::Radian angle2(1.57f);
	Ape::Vector3 axis2(0, 1, 0);
	Ape::Quaternion orientation2;
	orientation2.FromAngleAxis(angle2, axis2);
	Ape::Radian angle3(3.14f);
	Ape::Vector3 axis3(1, 0, 0);
	Ape::Quaternion orientation3;
	orientation3.FromAngleAxis(angle3, axis3);
	Ape::Quaternion defaultOrientation;
	defaultOrientation = orientation0 * orientation2 * orientation3;
	StoryElement storyElement;
	storyElement.browserName = "pub1";
	storyElement.browserURL = "https://www.ncbi.nlm.nih.gov/pubmed/28828201";
	storyElement.browserPosition = Ape::Vector3(0, 0, 0);
	storyElement.browserOrientation = defaultOrientation;
	storyElement.browserWidth = 300;
	storyElement.browserHeight = 225;
	storyElement.browserZoom = 0;
	storyElement.browserResolutionVertical = 1024;
	storyElement.browserResolutionHorizontal = 768;
	manageBrowser(storyElement);
	storyElement.browserName = "pub2";
	storyElement.browserURL = "http://gut.bmj.com/content/early/2017/11/09/gutjnl-2017-314547";
	storyElement.browserPosition = Ape::Vector3(0, 0, 400);
	manageBrowser(storyElement);
	storyElement.browserName = "pub3";
	storyElement.browserURL = "http://www.giejournal.org/article/S0016-5107(17)31032-5/pdf";
	storyElement.browserPosition = Ape::Vector3(0, 0, -400);
	manageBrowser(storyElement);
	storyElement.browserName = "endo_roadmap";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo_prezi/endo_roadMap.png";
	storyElement.browserPosition = Ape::Vector3(-325, 0, 0);
	manageBrowser(storyElement);
	storyElement.browserName = "endo_content";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo_prezi/endo_content_2.png";
	storyElement.browserPosition = Ape::Vector3(0, 325, 0);
	manageBrowser(storyElement);
	storyElement.browserName = "endo_result";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo_prezi/endo_result.png";
	storyElement.browserPosition = Ape::Vector3(400, 0, 400);
	manageBrowser(storyElement);
	storyElement.browserName = "endo_result_2";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo_prezi/endo_result_2.png";
	storyElement.browserPosition = Ape::Vector3(400, 0, 0);
	manageBrowser(storyElement);
	storyElement.browserName = "endo_pub";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo_prezi/endo_pub.png";
	storyElement.browserPosition = Ape::Vector3(0, -325, 0);
	manageBrowser(storyElement);
	storyElement.browserName = "endo_pub_cnn";
	storyElement.browserURL = "http://scs.ryerson.ca/~aharley/vis/conv/";
	storyElement.browserPosition = Ape::Vector3(0, -325, 400);
	manageBrowser(storyElement);
	storyElement.browserName = "endo_pub_cnn_2";
	storyElement.browserURL = "http://colah.github.io/posts/2014-10-Visualizing-MNIST/";
	storyElement.browserPosition = Ape::Vector3(0, -325, -400);
	manageBrowser(storyElement);
	storyElement.browserName = "motohotwear";
	storyElement.browserURL = "http://www.motohotwear.com/";
	storyElement.browserPosition = Ape::Vector3(-706, 677, 51) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(-0.831, 0, -0.556, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 77;
	storyElement.browserHeight = 46;
	manageBrowser(storyElement);
	storyElement.browserName = "endo5";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo5";
	storyElement.browserPosition = Ape::Vector3(188, 727, 708) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 77;
	storyElement.browserHeight = 46;
	manageBrowser(storyElement);
	storyElement.browserName = "endo";
	storyElement.browserURL = "https://www.endo-kapszula.hu/";
	storyElement.browserPosition = Ape::Vector3(-201, 723, 692) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(-0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 143;
	storyElement.browserHeight = 85;
	manageBrowser(storyElement);
	storyElement.browserName = "endo1";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo1";
	storyElement.browserPosition = Ape::Vector3(62, 723, 729) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(0, 0, -1, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 143;
	storyElement.browserHeight = 85;
	manageBrowser(storyElement);
	storyElement.browserName = "endo3";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo3";
	storyElement.browserPosition = Ape::Vector3(-74, 677, 726) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(0.098, 0, -0.995, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 77;
	storyElement.browserHeight = 46;
	manageBrowser(storyElement);
	storyElement.browserName = "arUserManual";
	storyElement.browserURL = "https://www.youtube.com/embed/eVV5tUmky6c?vq=hd480&autoplay=1&loop=1&playlist=eVV5tUmky6c";
	storyElement.browserPosition = Ape::Vector3(-688, 723, 184) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(-0.556, 0, -0.831, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 143;
	storyElement.browserHeight = 85;
	manageBrowser(storyElement);
	storyElement.browserName = "endo4";
	storyElement.browserURL = "http://srv.mvv.sztaki.hu/temp/endo4";
	storyElement.browserPosition = Ape::Vector3(188, 677, 708) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 77;
	storyElement.browserHeight = 46;
	manageBrowser(storyElement);
	storyElement.browserName = "ios-android";
	storyElement.browserURL = "https://nowitzkiblog.files.wordpress.com/2017/03/ios-android-icon_-e1447071162262.jpg?w=580";
	storyElement.browserPosition = Ape::Vector3(199, 727, -702) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(-0.957, 0, 0.29, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 77;
	storyElement.browserHeight = 46;
	manageBrowser(storyElement);
	storyElement.browserName = "bimGeometry";
	storyElement.browserURL = "http://www.teslacad.ca/blog/wp-content/uploads/2014/04/MEP-Services-Samples-13.jpg";
	storyElement.browserPosition = Ape::Vector3(65, 723, -711) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(-0.995, 0, -0.098, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 143;
	storyElement.browserHeight = 85;
	manageBrowser(storyElement);
	storyElement.browserName = "smartProfile";
	storyElement.browserURL = "http://kotem.com/Pages/Products.htm";
	storyElement.browserPosition = Ape::Vector3(-544, 706, 440) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 278;
	storyElement.browserHeight = 157;
	manageBrowser(storyElement);
	storyElement.browserName = "qvii";
	storyElement.browserURL = "https://www.qvii.com/";
	storyElement.browserPosition = Ape::Vector3(-392, 749, 592) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 139;
	storyElement.browserHeight = 78;
	manageBrowser(storyElement);
	storyElement.browserName = "kotem";
	storyElement.browserURL = "http://kotem.com/";
	storyElement.browserPosition = Ape::Vector3(-392, 663, 592) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(-0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 139;
	storyElement.browserHeight = 78;
	manageBrowser(storyElement);
	storyElement.browserName = "pridgeonandclay";
	storyElement.browserURL = "http://www.pridgeonandclay.com/";
	storyElement.browserPosition = Ape::Vector3(542, 706, 441) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 278;
	storyElement.browserHeight = 157;
	manageBrowser(storyElement);
	storyElement.browserName = "arShopFloor";
	storyElement.browserURL = "http://thearea.org/wp-content/uploads/2017/03/Augmented-Reality-and-IoT.jpg";
	storyElement.browserPosition = Ape::Vector3(392, 749, 592) - mOldXMLFormatTranslateVector;
	storyElement.browserOrientation = Ape::Quaternion(0.383, 0, -0.924, 0) *  mOldXMLFormatRotationQuaternion;
	storyElement.browserWidth = 139;
	storyElement.browserHeight = 78;
	manageBrowser(storyElement);
	/*story begin*/
	mStoryElements.push_back(StoryElement(Ape::Vector3(0 - mHumanXoffset, 0 - mHumanHeight, 0 - mHumanZoffset), Ape::Quaternion(1, 0, 0, 0))); //zero
	mStoryElements.push_back(StoryElement(Ape::Vector3(-12.5 - mHumanXoffset, -270.943 - mHumanHeight, 409.5 - mHumanZoffset), Ape::Quaternion(1, 0, 0, 0))); //indigo
	mStoryElements.push_back(StoryElement(Ape::Vector3(-24.5 - mHumanXoffset, -273.943 - mHumanHeight, -114 - mHumanZoffset), Ape::Quaternion(1, 0, 0, 0),
		"github", "https://github.com/MTASZTAKI/ApertusVR", Ape::Vector3(0, 360, -370) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150)); //github
	mStoryElements.push_back(StoryElement(Ape::Vector3(-30.5 - mHumanXoffset, -247.443 - mHumanHeight, -161 - mHumanZoffset), Ape::Quaternion(1, 0, 0, 0),
		"github", "https://github.com/MTASZTAKI/ApertusVR#about", Ape::Vector3(0, 360, -370) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150)); //github#about
	mStoryElements.push_back(StoryElement(Ape::Vector3(-27.5 - mHumanXoffset, -278.943 - mHumanHeight, -121 - mHumanZoffset), Ape::Quaternion(1, 0, 0, 0),
		"github", "https://github.com/MTASZTAKI/ApertusVR#apertusvr", Ape::Vector3(0, 360, -370) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150)); //github#swarch
	mStoryElements.push_back(StoryElement(Ape::Vector3(-312.127 - mHumanXoffset, 79.2832 - mHumanHeight, 317.68 - mHumanZoffset), Ape::Quaternion(0.383774, -0.00326215, 0.923396, 0.00784904))); //smartProfile1
	mStoryElements.push_back(StoryElement(Ape::Vector3(-393.724 - mHumanXoffset, 78.6628 - mHumanHeight, 287.429 - mHumanZoffset), Ape::Quaternion(0.383774, -0.00326215, 0.923396, 0.00784904))); //smartProfile2
	mStoryElements.push_back(StoryElement(Ape::Vector3(97.6123 - mHumanXoffset, 67.057 - mHumanHeight, 422.253 - mHumanZoffset), Ape::Quaternion(0.171713, -9.77448e-07, 0.985149, 0.0))); //endo
	mStoryElements.push_back(StoryElement(Ape::Vector3(-197.586 - mHumanXoffset, 100.834 - mHumanHeight, 573.667 - mHumanZoffset), Ape::Quaternion(0.116897, -0.00596791, 0.991839, 0.0506278),
		"endo", "https://www.youtube.com/embed/MSvpU8bYZiE?vq=hd480&autoplay=1&loop=1&playlist=MSvpU8bYZiE", Ape::Vector3(-201, 723, 692) - mOldXMLFormatTranslateVector, Ape::Quaternion(-0.195, 0, -0.981, 0) *  mOldXMLFormatRotationQuaternion, 143, 85));
	mStoryElements.push_back(StoryElement(Ape::Vector3(253.464 - mHumanXoffset, 88.8106 - mHumanHeight, 280.662 - mHumanZoffset), Ape::Quaternion(-0.397588, -1.02083e-08, 0.917568, 0.0))); //pridgeonandclay1
	mStoryElements.push_back(StoryElement(Ape::Vector3(268.471 - mHumanXoffset, 112.444 - mHumanHeight, 489.413 - mHumanZoffset), Ape::Quaternion(-0.257203, -0.00218629, 0.966327, -0.00821397))); //pridgeonandclay2
	mStoryElements.push_back(StoryElement(Ape::Vector3(-539.024 - mHumanXoffset, 82.1165 - mHumanHeight, 121.716 - mHumanZoffset), Ape::Quaternion(0.653925, -0.0166771, 0.756182, 0.0192854))); //karsai1
	mStoryElements.push_back(StoryElement(Ape::Vector3(-590.853 - mHumanXoffset, 78.5688 - mHumanHeight, 116.881 - mHumanZoffset), Ape::Quaternion(0.808807, -0.0206272, 0.587596, 0.0149859))); //karsai2
	mStoryElements.push_back(StoryElement(Ape::Vector3(-607.718 - mHumanXoffset, 94.0923 - mHumanHeight, 162.253 - mHumanZoffset), Ape::Quaternion(0.573593, -0.0121893, 0.818928, 0.0174032))); //karsai3
	mStoryElements.push_back(StoryElement(Ape::Vector3(16.7394 - mHumanXoffset, 84.057 - mHumanHeight, -492.757 - mHumanZoffset), Ape::Quaternion(0.981627, 0, -0.190809, 0))); //t-system
	mStoryElements.push_back(StoryElement(Ape::Vector3(-12 - mHumanXoffset, 387 - mHumanHeight, -659) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(0, 0, -1, 0))); //linkageDesigner1
	mStoryElements.push_back(StoryElement(Ape::Vector3(-16.4233 - mHumanXoffset, -261.939 - mHumanHeight, -547.675 - mHumanZoffset), Ape::Quaternion(0.000344675, 0.0, -0.999992, -0.00424999))); //linkageDesigner2
	mStoryElements.push_back(StoryElement(Ape::Vector3(-16.4233 - mHumanXoffset, -261.939 - mHumanHeight, -547.675 - mHumanZoffset), Ape::Quaternion(0.000344675, 0.0, -0.999992, -0.00424999),
		"linkageDesigner", "https://www.youtube.com/embed/YxcRr_QbPfg?vq=hd480&autoplay=1&loop=1&playlist=YxcRr_QbPfg", Ape::Vector3(9.504, 364.896, -423) - mOldXMLFormatTranslateVector, Ape::Quaternion(0, -0, -1, -0) *  mOldXMLFormatRotationQuaternion, 143, 85)); //linkageDesigner3
	mStoryElements.push_back(StoryElement(Ape::Vector3(13 - mHumanXoffset, 359 - mHumanHeight, -52 - mHumanZoffset) - mOldXMLFormatTranslateVectorCamera, Ape::Quaternion(1, 0, 0, 0),
		"symbio-tic", "http://www.symbio-tic.eu/index.php?option=com_content&view=article&id=2&Itemid=25", Ape::Vector3(0, 360, -320) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 240, 150, 0, 2048, 1024)); //symbio-tic1
	mStoryElements.push_back(StoryElement(Ape::Vector3(-25.9053 - mHumanXoffset, -304.943 - mHumanHeight, -172.531 - mHumanZoffset), Ape::Quaternion(1, 0, -0.000392581, 0))); //symbio-tic2
	mStoryElements.push_back(StoryElement(Ape::Vector3(-12.5 - mHumanXoffset, -270.943 - mHumanHeight, 461.5 - mHumanZoffset), Ape::Quaternion(1, 0, 0, 0),
		"first", "https://techcrunch.com/2016/07/30/tech-trends-that-will-impact-your-home/", Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 267, 150, 0, 2048, 1024)); //thank you for the attention
	mStoryElements.push_back(StoryElement(Ape::Vector3(-12.5 - mHumanXoffset, -270.943 - mHumanHeight, 461.5 - mHumanZoffset), Ape::Quaternion(1, 0, 0, 0),
		"first", "http://srv.mvv.sztaki.hu/temp/honeywell/tech_arvr.png", Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 267, 150, 0, 2048, 1024)); //thank you for the attention
	mStoryElements.push_back(StoryElement(Ape::Vector3(-12.5 - mHumanXoffset, -270.943 - mHumanHeight, 461.5 - mHumanZoffset), Ape::Quaternion(1, 0, 0, 0),
		"first", "http://srv.mvv.sztaki.hu/temp/honeywell/tech_iot.png", Ape::Vector3(15, 367, 206) - mOldXMLFormatTranslateVector, Ape::Quaternion(1, 0, 0, 0) * mOldXMLFormatRotationQuaternion, 267, 150, 0, 2048, 1024)); //thank you for the attention

	mStoryElements.push_back(StoryElement(Ape::Vector3(0, 0, 0), Ape::Quaternion(1, 0, 0, 0))); //zero
	mStoryElements.push_back(StoryElement(Ape::Vector3(496.297, 242.352, 1207.09), Ape::Quaternion(0.976142, -0.125137, 0.176116, 0.0225773))); //begin
	mStoryElements.push_back(StoryElement(Ape::Vector3(244.285, 362.639, 560.672), Ape::Quaternion(0.986104, -0.0755844, 0.147775, 0.0113269)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(5.85465, 10.0267, 707.353), Ape::Quaternion(0.99999, -0.00850004, 1.54037e-07, 3.07597e-08)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(5.85456, 3.26078, 309.411), Ape::Quaternion(0.99999, -0.00850004, 1.54037e-07, 3.07597e-08)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(13.8545, -3.70919, -100.53), Ape::Quaternion(-0.99999, -0.00850004, 1.54037e-07, 3.07597e-08)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(397.854, -4.1349, 719.624), Ape::Quaternion(0.999991, -0.00850005, 1.54037e-07, 3.07597e-08)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(443.9, -10.9689, 318.012), Ape::Quaternion(0.997068, -0.00847519, 0.076425, 0.000649651)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(2.50844, -290.186, 310.75), Ape::Quaternion(0.999985, -0.00849999, -0.00424981, -3.60945e-05)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(-8.69246, -321.79, 687.259), Ape::Quaternion(0.999985, -0.00849999, -0.00424981, -3.60945e-05)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(6.05867, -327.824, -106.902), Ape::Quaternion(1.00002, 9.40636e-08, -0.00424997, 2.96732e-08)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(-495.589, 2.94523, 334.519), Ape::Quaternion(0.977535, 0.00830944, -0.210911, 0.00179282)));
	mStoryElements.push_back(StoryElement(Ape::Vector3(-359.585, -2.15281, 320.593), Ape::Quaternion(0.976142, -0.125137, 0.176116, 0.0225773))); //end
	mStoryElements.push_back(StoryElement(Ape::Vector3(0, 0, 0), Ape::Quaternion(1, 0, 0, 0))); //zero
	/*endo meeting 2018.02.14 end*/
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
			4.0,
			[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
		);
		auto rotateInterpolator = std::make_unique<Ape::Interpolator>(false);
		rotateInterpolator->addSection(
			userNode->getOrientation(),
			storyElement.cameraOrientation,
			4.0,
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
		manageBrowser(storyElement);
}

void ApePresentationScenePlugin::jumpToStoryElement(Ape::NodeSharedPtr userNode)
{
	StoryElement storyElement = mStoryElements[mCurrentStoryElementIndex];
	userNode->setPosition(storyElement.cameraPosition);
	userNode->setOrientation(storyElement.cameraOrientation);
	if (storyElement.browserName.size())
		manageBrowser(storyElement);
}

void ApePresentationScenePlugin::manageBrowser(StoryElement storyElement)
{
	if (!mpScene->getNode(storyElement.browserName).lock())
	{
		if (auto browserNode = mpScene->createNode(storyElement.browserName).lock())
		{
			browserNode->setPosition(storyElement.browserPosition);
			browserNode->setOrientation(storyElement.browserOrientation);
			if (auto browserGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity(storyElement.browserName + "geometry", Ape::Entity::GEOMETRY_PLANE).lock()))
			{
				browserGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(storyElement.browserWidth, storyElement.browserHeight), Ape::Vector2(1, 1));
				browserGeometry->setParentNode(browserNode);
				if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->createEntity(storyElement.browserName + "browser", Ape::Entity::BROWSER).lock()))
				{
					browser->setResoultion(storyElement.browserResolutionVertical, storyElement.browserResolutionHorizontal);
					browser->setURL(storyElement.browserURL);
					browser->setGeometry(browserGeometry);
					mBrowsers[storyElement.browserName] = browser;
					if (auto browserMouseNode = mpScene->createNode(storyElement.browserName + "mouse").lock())
					{
						browserMouseNode->setParentNode(browserNode);
						browserMouseNode->setPosition(Ape::Vector3(0, -1, 0));
						if (auto mouseGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity(storyElement.browserName + "mouseGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
						{
							mouseGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(storyElement.browserWidth, storyElement.browserHeight), Ape::Vector2(1, 1));
							mouseGeometry->setParentNode(browserMouseNode);
							if (auto mouseMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity(storyElement.browserName + "mouseMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
							{
								mouseMaterial->setEmissiveColor(Ape::Color(1.0f, 1.0f, 1.0f));
								mouseMaterial->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
								mouseMaterial->setLightingEnabled(false);
								if (auto mouseTexture = std::static_pointer_cast<Ape::IUnitTexture>(mpScene->createEntity(storyElement.browserName + "mouseTexture", Ape::Entity::TEXTURE_UNIT).lock()))
								{
									mouseTexture->setParameters(mouseMaterial, "browserpointer.png");
									mouseTexture->setTextureAddressingMode(Ape::Texture::AddressingMode::CLAMP);
									mouseTexture->setTextureFiltering(Ape::Texture::Filtering::POINT, Ape::Texture::Filtering::LINEAR, Ape::Texture::Filtering::F_NONE);
									mGeometriesMouseTextures[mouseGeometry->getName()] = mouseTexture;
									mBrowserMouseTextures[mouseTexture->getName()] = browser;
								}
								std::static_pointer_cast<Ape::IPlaneGeometry>(mouseGeometry)->setMaterial(mouseMaterial);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->getEntity(storyElement.browserName + "browser").lock()))
		{
			browser->setURL(storyElement.browserURL);
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
	if (mContext == SPACE)
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
}

void ApePresentationScenePlugin::saveUserNodePose(Ape::NodeSharedPtr userNode)
{
	std::ofstream userNodePoseFile;
	userNodePoseFile.open("userNodePoseFile.txt", std::ios::app);
	userNodePoseFile << userNode->getPosition().x << "," << userNode->getPosition().y << "," << userNode->getPosition().z << " : " <<
		userNode->getOrientation().w << "," << userNode->getOrientation().x << "," << userNode->getOrientation().y << "," << userNode->getOrientation().z << std::endl;
	userNodePoseFile.close();
}

bool ApePresentationScenePlugin::keyPressed(const OIS::KeyEvent& e)
{
	if (auto activeBrowser = mActiveBrowser.lock())
	{
		mContext = Context::BROWSER;
		std::string keyAsString = mpKeyboard->getAsString(e.key);
		std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);
		//std::cout << "ApePresentationScenePlugin::keyPressed " << "keyAsString:" << keyAsString << std::endl;
		std::wstring keyAsWString(keyAsString.begin(), keyAsString.end());
		if (e.key == OIS::KeyCode::KC_BACK)
			keyAsWString = 8;
		else if (e.key == OIS::KeyCode::KC_TAB)
			keyAsWString = 9;
		else if (e.key == OIS::KeyCode::KC_RETURN)
			keyAsWString = 13;
		else if (e.key == OIS::KeyCode::KC_LSHIFT)
			keyAsWString = 14;
		activeBrowser->keyASCIIValue(keyAsWString[0]);
		auto userNode = mUserNode.lock();
		if (userNode)
		{
			if (e.key == OIS::KeyCode::KC_ESCAPE)
			{
				mActiveMouseTexture = mOverlayMouseTexture;
				mActiveBrowser = mOverlayBrowser;
				if (auto overlayMouseMaterial = mOverlayMouseMaterial.lock())
					overlayMouseMaterial->showOnOverlay(true, 1);
				//userNode->setPosition(mUserNodePositionBeforeFullScreen);
				//userNode->setOrientation(mUserNodeOrientationBeforeFullScreen);
				mContext = Context::SPACE;
			}
		}
	}
	else
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
					if (mCurrentStoryElementIndex == mStoryElements.size())
						mCurrentStoryElementIndex = 0;
					animateToStoryElements(userNode);
				}
				if (!mIsFirstSpacePressed)
				{
					/*setting up zoom levels*/
					if (auto browserEndo = mBrowsers["endo"].lock())
						browserEndo->setZoomLevel(4);
					if (auto browserEndo = mBrowsers["smartProfile"].lock())
						browserEndo->setZoomLevel(4);
					if (auto browserEndo = mBrowsers["kotem"].lock())
						browserEndo->setZoomLevel(2);
					if (auto browserEndo = mBrowsers["qvii"].lock())
						browserEndo->setZoomLevel(2);
					mIsFirstSpacePressed = true;
				}
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_M])
			{
				if (mCurrentStoryElementIndex < mStoryElements.size())
				{
					mCurrentStoryElementIndex++;
					if (mCurrentStoryElementIndex == mStoryElements.size())
						mCurrentStoryElementIndex = 0;
					jumpToStoryElement(userNode);
				}
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_N])
			{
				if (mCurrentStoryElementIndex > 0)
				{
					mCurrentStoryElementIndex--;
					if (mCurrentStoryElementIndex < 0)
						mCurrentStoryElementIndex = 0;
					jumpToStoryElement(userNode);
				}
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_R])
			{
				mCurrentStoryElementIndex = 0;
				jumpToStoryElement(userNode);
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_MINUS])
			{
				mTranslateSpeedFactor -= 0.5;
				mRotateSpeedFactor -= 0.5;
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_ADD])
			{
				mTranslateSpeedFactor += 0.5;
				mRotateSpeedFactor += 0.5;
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_C])
			{
				saveUserNodePose(userNode);
			}
		}
		mContext = Context::SPACE;
	}
	return true;
}

bool ApePresentationScenePlugin::keyReleased(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = false;
	return true;
}

bool ApePresentationScenePlugin::mouseMoved(const OIS::MouseEvent & e)
{
	if (auto activeMouseTexture = mActiveMouseTexture.lock())
	{
		if (auto activeBrowser = mActiveBrowser.lock())
		{
			Ape::Vector2 cursorTexturePosition;
			cursorTexturePosition.x = (float)-e.state.X.abs / (float)mpMainWindow->getWidth();
			cursorTexturePosition.y = (float)-e.state.Y.abs / (float)mpMainWindow->getHeight();
			activeMouseTexture->setTextureScroll(cursorTexturePosition.x, cursorTexturePosition.y);
			Ape::Vector2 cursorBrowserPosition;
			cursorBrowserPosition.x = ((float)e.state.X.abs / (float)mpMainWindow->getWidth()) * activeBrowser->getResoultion().x;
			cursorBrowserPosition.y = ((float)e.state.Y.abs / (float)mpMainWindow->getHeight()) * activeBrowser->getResoultion().y;
			activeBrowser->mouseMoved(cursorBrowserPosition);
			activeBrowser->mouseScroll(Ape::Vector2(0, e.state.Z.rel));
			/*std::cout << "ApePresentationScenePlugin::mouseMoved " << "cursorTexturePosition:" << cursorTexturePosition.x << ";" << cursorTexturePosition.y << std::endl;
			std::cout << "ApePresentationScenePlugin::mouseMoved " << "cursorBrowserPosition:" << cursorBrowserPosition.x << ";" << cursorBrowserPosition.y << std::endl;*/
		}
	}
	return true;
}

bool ApePresentationScenePlugin::mousePressed(const OIS::MouseEvent & e, OIS::MouseButtonID id)
{
	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto activeBrowser = mActiveBrowser.lock())
		{
			activeBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, true);
		}
		else
		{
			if (auto rayOverlayNode = mRayOverlayNode.lock())
			{
				rayOverlayNode->setPosition(Ape::Vector3(e.state.X.abs, e.state.Y.abs, 0));
				if (auto rayGeomtery = mRayGeometry.lock())
					rayGeomtery->fireIntersectionQuery();
			}
			clock_t currentLeftClickTime;
			currentLeftClickTime = clock();
			double elapsedTimInMSecs = (currentLeftClickTime - mLastLeftClickTime) / (CLOCKS_PER_SEC / 1000);
			if (elapsedTimInMSecs < 250)
			{
				float planeWidth, planeHeight, windowHeight, windowWidth, planeRatio, windowRatio, cameraDistance = 0.0f;
				if (auto activeBrowser = mActiveBrowser.lock())
				{
					if (auto geometry = std::static_pointer_cast<Ape::IPlaneGeometry>(activeBrowser->getGeometry().lock()))
					{
						planeHeight = geometry->getParameters().size.x;
						planeWidth = geometry->getParameters().size.y;
						windowWidth = mpMainWindow->getWidth();
						windowHeight = mpMainWindow->getHeight();
						planeRatio = planeWidth / planeHeight;
						windowRatio = windowWidth / windowHeight;
						if (auto camera = mCamera.lock())
						{
							if (windowRatio < planeRatio)
							{
								float fieldOfViewX = 2 * atan(tan(camera->getFOVy().getRadian() * 0.5) * windowRatio);
								cameraDistance = (planeWidth / 2) / (tan(fieldOfViewX / 2));
							}
							else
								cameraDistance = (planeHeight / 2) / (tan(camera->getFOVy().getRadian() / 2));
						}
						if (auto userNode = mUserNode.lock())
						{
							if (auto geometryNode = geometry->getParentNode().lock())
							{
								Ape::Vector3 position = geometryNode->getDerivedOrientation() * Ape::Vector3(0, -cameraDistance, 0) + geometryNode->getDerivedPosition();
								Ape::Quaternion orientation = geometryNode->getDerivedOrientation();
								mUserNodePositionBeforeFullScreen = userNode->getPosition();
								mUserNodeOrientationBeforeFullScreen = userNode->getOrientation();
								userNode->setPosition(position);
								userNode->setOrientation(orientation * mOldXMLFormatRotationQuaternion.Inverse());
							}
						}
					}
				}
			}
			mLastLeftClickTime = currentLeftClickTime;
		}
	}
	else if (id == OIS::MouseButtonID::MB_Right)
	{
		if (auto activeBrowser = mActiveBrowser.lock())
			activeBrowser->mouseClick(Ape::Browser::MouseClick::RIGHT, e.state.buttonDown(id));
	}
	else if (id == OIS::MouseButtonID::MB_Middle)
	{
		if (auto activeBrowser = mActiveBrowser.lock())
			activeBrowser->mouseClick(Ape::Browser::MouseClick::MIDDLE, e.state.buttonDown(id));
	}
	return true;
}

bool ApePresentationScenePlugin::mouseReleased(const OIS::MouseEvent & e, OIS::MouseButtonID id)
{
	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto activeBrowser = mActiveBrowser.lock())
			activeBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, false);
	}
	return true;
}

void ApePresentationScenePlugin::Run()
{
	while (true)
	{
		if (mpKeyboard)
			mpKeyboard->capture();
		if (mpMouse)
			mpMouse->capture();
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
